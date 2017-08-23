/***************************************************************************
 * 
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file Timer.h
 * @author caiguanyang(com@baidu.com)
 * @date 2017/08/13 22:46:36
 * @brief 
 * 暂不支持重复 
 **/

#ifndef  __TIMER_H_
#define  __TIMER_H_

#include "muduo/base/Timestamp.h"
#include "muduo/base/Atomic.h"
#include "muduo/cainet/Callbacks.h"

namespace cai {
class Timer {
public:
    Timer(TTimerCallBack &cb, Timestam when, double interval) :
        callback_(cb),
        expiration_(when),
        interval_(interval),
        repeat_(interval > 0.0),
        sequence_(s_numCreated_.incrementAndGet()) {
    }
    void run() {
        callback_();
    }
    // 重启定时器
    //void restart();

    int64_t sequence() {
        return sequence_;
    }

    Timestamp expiration() {
        return expiration_;
    } 

private:
    // 定时器到期的回调函数
    TTimerCallBack callback_;
    Timestamp expiration_;
    // 定时器的时间间隔, 单位 s
    const double interval_;
    // 是否可以重复
    const bool repeat_;
    const int64_t sequence_;

    static AtomicInt64 s_numCreated_;
};

}

#endif  //__TIMER_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
