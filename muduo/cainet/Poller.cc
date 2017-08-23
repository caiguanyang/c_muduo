/***************************************************************************
 * 
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file Poller.cc
 * @author caiguanyang(com@baidu.com)
 * @date 2017/08/09 23:08:18
 * @brief 
 *  
 **/
#include "muduo/cainet/Poller.h"
#include "muduo/cainet/EventLoop.h"
#include "muduo/cainet/Channel.h"
#include "muduo/base/Logging.h"

#include <poll.h>

namespace cai {

Poller::Poller (EventLoop *loop) :
    ownerLoop_(loop) {
    
}

Poller::~Poller () {

}

muduo::Timestamp Poller::poll(int timeoutMs, TChannelList *activeChannels) {
    int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
    muduo::Timestamp now(muduo::Timestamp::now());
    if (numEvents > 0) {
        LOG_TRACE << numEvents << " events happended";
        fillActiveChannels(numEvents, activeChannels);
    } else if (numEvents == 0) {
        LOG_TRACE << "nothing happended";
    } else {
        LOG_SYSERR << "Poller::poll() error";
    }
    return now;
}

void Poller::fillActiveChannels(int numEvents, TChannelList *activeChannels) {
    TPollFdList::iterator it = pollfds_.begin();
    for (; it != pollfds_.end() && numEvents > 0; it++) {
        if (it->revents > 0) {
            // 有事件准备好
            int fd = it->fd;
            TChannelMap::const_iterator ch = channels_.find(fd);
            assert(ch != channels_.end());
            Channel *ach = ch->second;
            assert(ach->fd() == fd);
            ach->set_revents(it->revents);
            activeChannels->push_back(ach);
        } 
    }
}

void Poller::updateChannel(Channel *channel) {
    if (channel->index() == -1) {
        // add
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = static_cast<short>(channel->revents());
        pollfds_.push_back(pfd);
        channel->set_index(pollfds_.size() - 1);
        channels_.insert(std::pair<int, Channel*>(pfd.fd, channel));
    } else {
        // update
        int idx = channel->index();
        assert(idx >=0 && idx < static_cast<int>(pollfds_.size()));
        struct pollfd &tmpPfd = pollfds_[idx];
        tmpPfd.events = static_cast<short>(channel->events());
        tmpPfd.revents = 0;
        if (channel->isNoneEvents()) {
            tmpPfd.fd = -1;
        }
    }
}

}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
