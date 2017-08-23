/***************************************************************************
 * 
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file TimerId.h
 * @author caiguanyang(com@baidu.com)
 * @date 2017/08/13 22:41:57
 * @brief 
 * 定时器标识类，包含 Timer 信息
 **/

#ifndef  __TIMERID_H_
#define  __TIMERID_H_

namespace cai {

class Timer;

class TimerId {
public:
    TimerId() :
        timer_(NULL),
        sequence_(0) {
    }
    TimerId(Timer *t, int64_t seq) :
        timer_(t),
        sequence_(seq) {
    }
private:
    Timer *timer_;
    int64_t sequence_;
};

}

#endif  //__TIMERID_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
