/***************************************************************************
 * 
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file TimerQueue.h
 * @author caiguanyang(com@baidu.com)
 * @date 2017/08/13 23:08:42
 * @brief 
 * 定时器队列
 **/

#ifndef  __TIMERQUEUE_H_
#define  __TIMERQUEUE_H_

#include "muduo/cainet/Callbacks.h"
#include "muduo/base/Timestamp.h"
#include "muduo/cainet/Channel.h"

#include <boost/noncopyable.hpp>
#include <map>
#include <vector>
#include <set>

namespace cai {
  
class Timer;
class TimerId;
class EventLoop;

class TimerQueue : boost::noncopyable {
public:
    TimerQueue(EventLoop *loop);
    ~TimerQueue();
    TimerId addTimer(const TTimerCallback &cb, muduo::Timestamp, double interval);

private:
    void addTimerInLoop(Timer *timer);
    bool insert(Timer *timer);
    
private:
    typedef std::pair<Timestamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;
    typedef std::pair<Timer*, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerList;

    // 获取到期的 timer
    std::vector<Entry> getExpired(muduo::Timestamp now);

    // 处理读事件
    void handleRead();

    EventLoop *loop_;
    int timerfd_;
    Channel timerfdChannel_;

    TimerList timers_;
    ActiveTimerList activeTimers_;
};
}

#endif  //__TIMERQUEUE_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
