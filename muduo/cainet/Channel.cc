/***************************************************************************
 * 
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file Channel.cc
 * @author caiguanyang(com@baidu.com)
 * @date 2017/08/09 22:33:14
 * @brief 
 *  
 **/

#include "muduo/cainet/Channel.h"
#include "muduo/cainet/EventLoop.h"
#include "muduo/base/Logging.h"
#include <sys/poll.h>

namespace cai {
    const int Channel::kNoneEvent = 0;
    const int Channel::kReadEvent = POLLIN | POLLPRI;
    const int Channel::kWriteEvent = POLLOUT | POLLWRBAND;

    Channel::Channel (EventLoop *loop, int fd) :
        loop_(loop),
        fd_(fd),
        events_(0),
        revents_(0),
        index_(-1) {
    }

    void Channel::update() {
        loop_->updateChannel(this);
    }

    void Channel::handleEvent() {
        if (revents_ & POLLNVAL) {
            LOG_WARN << "Channel:: pollnval";
        }
        if (revents_ & (POLLERR | POLLNVAL)) {
            if (errorCallback_) {
                errorCallback_();
            }
        }
        if (revents_ & Channel::kReadEvent) {
            if (readCallback_) {
                readCallback_();
            }
        }
        if (revents_ & Channel::kWriteEvent) {
            if (writeCallback_) {
                writeCallback_();
            }
        }
    }

}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
