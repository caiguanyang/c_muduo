/***************************************************************************
 * 
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file Poller.h
 * @author caiguanyang(com@baidu.com)
 * @date 2017/08/09 22:55:24
 * @brief 
 *  
 **/

#ifndef  __POLLER_H_
#define  __POLLER_H_

#include <boost/noncopyable.hpp>
#include "muduo/base/Timestamp.h"
#include <vector>
#include <map>

struct pollfd;

namespace cai {
class Channel;
class EventLoop;

class Poller : boost::noncopyable {
public:
    typedef std::vector<Channel *> TChannelList;
    typedef std::map<int, Channel*> TChannelMap;
    typedef std::vector<pollfd> TPollFdList;

    Poller(EventLoop *);
    ~Poller();

    // 获取活跃事件
    muduo::Timestamp poll(int timeoutMS, TChannelList* activeChannels);
    // add && update channel
    void updateChannel(Channel *channel);

    void fillActiveChannels(int numEvent, TChannelList *activeChannels);
private:
    EventLoop *ownerLoop_;
    TPollFdList pollfds_;
    TChannelMap channels_;
};

}

#endif  //__POLLER_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
