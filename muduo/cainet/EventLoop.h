/***************************************************************************
 * 
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file EventLoop.h
 * @author caiguanyang(com@baidu.com)
 * @date 2017/08/05 23:03:07
 * @brief 
 *  
 **/


#ifndef  __EVENTLOOP_H_
#define  __EVENTLOOP_H_
#include "muduo/base/Mutex.h"

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <bits/types.h>
#include <vector>

namespace cai {

class Channel;
class Poller;
class TimerQueue;

class EventLoop : boost::noncopyable {
public:
    typedef std::vector<Channel*> TChannelList;
    typedef boost::function<void ()> TFunctor;

    EventLoop();
    ~EventLoop();

    void loop();
    // 更新 channel 状态
    void updateChannel(Channel *ch);
    // 退出 loop
    void quit();

    void assertInLoopThread() {
        if (!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }

    void runInLoop(const TFunctor &cb);

    TimerId runAfter(double delay, const TTimerCallback &cb);

private:
    // 降低头文件依赖，在 cc文件中实现
    bool isInLoopThread() const;
    void abortNotInLoopThread();
    // 添加回调函数到队列中
    void queueInLoop(const TFunctor &cb);
    void doPendingFunc();

private:
    // 标示是否在 loop 中
    bool looping_;
    // 创建此示例的线程 ID
    const pid_t threadId_;
    bool quit_;
    TChannelList activeChannels_;
    boost::scoped_ptr<Poller> poller_;
    // poll超时时间，单位毫秒
    int timeoutMs_;

    boost::scoped_ptr<TimerQueue> timerQueue_;

    // 待执行函数队列
    bool callingPendingFunctor_;
    muduo::MutexLock mutex_;
    std::vector<TFunctor> pendingFunctos_;
};

}


#endif  //__EVENTLOOP_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
