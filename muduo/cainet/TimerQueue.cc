/***************************************************************************
 * 
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file TimerQueue.cc
 * @author caiguanyang(com@baidu.com)
 * @date 2017/08/14 12:56:56
 * @brief 
 * 注意：定时器的实现目前只支持执行一次，不能重复 
 **/

#include "muduo/cainet/TimerQueue.h"
#include "muduo/cainet/TimerId.h"
#include "muduo/cainet/Timer.h"
#include "muduo/base/Logging.h"

#include <boost/bind.hpp>
#include <sys/timerfd.h>
#include <cstdint>
#include <vector>

namespace cai {
namespace detail{
    int createTimerfd() {
        int fd = ::timerfd_create(::CLOCK_REALTIME, ::TFD_NONBLOCK); 
        if (fd < 0) {
            LOG_SYSERR << "create timerfd failed!";
            return -1;
        }
        return fd;
    } 

    struct timespec howMuchTimeFromNow(Timestamp when)
    {
        int64_t microseconds = when.microSecondsSinceEpoch()
            - Timestamp::now().microSecondsSinceEpoch();
        if (microseconds < 100)
        {
            microseconds = 100;
        }
        struct timespec ts;
        ts.tv_sec = static_cast<time_t>(
                microseconds / Timestamp::kMicroSecondsPerSecond);
        ts.tv_nsec = static_cast<long>(
                (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
        return ts;
    }
    /**
     * 重置 timer 的超时时间
     * @return
     */
    void resetTimerfd(int tfd, Timestamp when) {
        struct itimerspec newVal;
        struct itimerspec oldVal;
        memset(&newVal, 0, sizeof newVal);
        memset(&newVal, 0, sizeof oldVal);

        newVal.it_value = howMuchTimeFromNow(when);
        int ret = ::timerfd_settime(tfd, 0, &newVal, &oldVal);
        if (ret) {
            LOG_SYSERR << "timer set time failed!";
        }
    }
}
}

namespace cai {
    TimerQueue::TimerQueue(EventLoop *loop) :
        loop_(loop),
        timerfd_(detail::createTimerfd()),
        timerfdChannel_(loop, timerfd_) {

        timerfdChannel_.setReadCallback(
            boost::bind(&TimerQueue::handleRead, this));
        timerfdChannel_.enableReading();
    }

    /**
     * 获取到期的 timer 列表，并执行他们的回调函数
     * @return
     */
    void TimerQueue::handleRead() {
        LOG_TRACE << "handle read timerfd...";
        muduo::Timestamp now(Timestamp::now());
        std::vector<Entry> expires = getExpired(now);
        std::vector<Entry>::iterator it = expires.begin();
        for (it; it != expires.end(); it++) {
            it->second->run();
        }
        
        // !!!如果需要重复 Timer，需要重置
    }

    /**
     * 获取过期的 timer 列表
     * @param Timestamp now
     * @return vector
     */
    std::vector<Entry> TimerQueue::getExpired(muduo::Timestamp now) {
        assert(timers_.size() == activeTimers_.size());
        std::vector<Entry> expires;
        Entry sentry(now, reinterpret_cast<Timer*>(UNITPTR_MAX));
        TimerList::iterator it = timers_.lower_bound(sentry);
        assert(it == timers_.end() || it < timers_.begin());
        std::copy(timers_.begin(), it, expires);
        std::erase(timers_begin(), it);

        // 清除 activeTimers 中的
        std::vector<Entry> it_e = expires.begin();
        for (it_e; it_e != expires.end(); it_e++) {
            ActiveTimer tmp(it_e->second, it_e->second->sequence());
            size_t n = activeTimers_.erase(tmp);
            assert(n == 1);
        }

        assert(timers_.size() == activeTimers_.size());
        return expires; 
    }

    /**
     * 添加定时器
     * @param TTimerCallback cb
     * @return TimerId
     */
    TimerId TimerQueue::addTimer(const TTimerCallback &cb, muduo::Timestamp when, double interval) {
        Timer *timer = new Timer(cb, exp, interval); 
        loop_->runInLoop(boost::bind(&TimerQueue::addTimerInLoop, this, timer));
        return TimerId(timer, timer->sequence());
    }

    /**
     * 在 IO 线程中添加 timer，不需要考虑多线程的问题
     * @return
     */
    void TimerQueue::addTimerInLoop(Timer *timer) {
        loop_->assertInLoopThread();
        bool earlier = insert(timer);
        if (earlier) {
            detail::resetTimerfd(timerfd_, timer->expiration());
        }
    }

    /**
     * 插入 timer 到 Set 中，并返回是否是最近的超时时间
     *    用于决定是否需要重置 timer 的时间
     * @return bool
     */
    bool insert(Timer *timer) {
        bool earlier = false;
        TimerList::iterator it = timers_.begin();
        Timestamp when = timer->expiration();
        if (it == timers_.end() || it->first > when){
            earlier = true;
        }

        {
            std::pair<TimerList::iterator, bool> rst = 
                timers_.insert(Entry(when, timer));
            assert(rst.second);
        }

        {
            std::pair<ActiveTimerList::iterator, bool> rst =
                activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
            assert(rst.second);
        }

        assert(timers_.size() == activeTimers_.size());
        return earlier; 
    }

}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
