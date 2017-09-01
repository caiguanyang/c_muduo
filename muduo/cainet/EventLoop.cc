/***************************************************************************
 * 
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file EventLoop.cc
 * @author caiguanyang(com@baidu.com)
 * @date 2017/08/07 22:39:47
 * @brief 
 *  
 **/

#include "muduo/cainet/EventLoop.h"
#include "muduo/cainet/TimerQueue.h"
#include "muduo/cainet/Channel.h"
#include "muduo/cainet/Poller.h"
#include "muduo/base/CurrentThread.h"
#include "muduo/base/Logging.h"

#include <poll.h>
#include <sys/eventfd.h>
#include <boost/bind.hpp>

namespace cai {

__thread EventLoop *t_loopInThisThread = 0;

/******************************************************/
int createEventFd() {
    int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (fd < 0) {
        LOG_SYSERR << "create event fd failed!";
        abort();
    }
    return fd;
}

/******************************************************/

EventLoop::EventLoop()
    : looping_(false),
    threadId_(muduo::CurrentThread::tid()),
    poller_(new Poller(this)),
    quit_(false),
    timeoutMs_(5000),
    timerQueue_(new TimerQueue(this)),
    wakeupFd_(createEventFd()),
    wakeupChannel_(new Channel(this, wakeupFd_)){
 
    LOG_TRACE << "EventLoop created " << this << " in thread " << threadId_;
    if (t_loopInThisThread) {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread
            << " exists in this thread " << threadId_;
    } else {
        t_loopInThisThread = this;
    }

    wakeupChannel_->setReadCallback(
            boost::bind(&EventLoop::handleEventFdRD, this)); 
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop() {
    assert(!looping_);
    t_loopInThisThread = NULL;
}

void EventLoop::quit() {
    quit_ = true;
    if (!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::loop() {
    assert(!looping_);
    assertInLoopThread();

    looping_ = true;
    quit_ = false;
    
    while (!quit_) {
        activeChannels_.clear();
        poller_->poll(timeoutMs_, &activeChannels_);
        for (TChannelList::iterator it = activeChannels_.begin();
                it != activeChannels_.end(); it++) {
            (*it)->handleEvent();
        }

        // 处理需要执行的函数列表
        doPendingFunc();
    }

    LOG_TRACE << "stop looping";
    looping_ = false;
}

bool EventLoop::isInLoopThread() const {
    return threadId_ == muduo::CurrentThread::tid();
}

void EventLoop::handleEventFdRD() {
    uint64_t one = 1;
    ssize_t n = 2;
    //ssize_t n = sockets::read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR << "EventLoop read data from wakeup_fd failed, read " << n << "bytes instead of 8";
    }
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = 2;
    //ssize_t n = sockets::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR << "EventLoop write data to wakeup_fd failed, write" << n << "bytes instead of 8";
    }
}

void EventLoop::abortNotInLoopThread() {
    LOG_FATAL << "EventLoop obj is created in thread[" << threadId_
        << "], but curr thread is[" << muduo::CurrentThread::tid() << "]";
}

TimerId EventLoop::runAfter(double delay, const TTimerCallback &cb) {
    muduo::Timestamp when = muduo::addTime(muduo::Timestamp::now(), delay);
    return timerQueue_->addTimer(cb, when, 0.0);
}

void EventLoop::updateChannel(Channel *ch) {
    assert(ch->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(ch);
}

void EventLoop::runInLoop(const TFunctor &cb) {
    if (isInLoopThread()) {
        cb();
    } else {
        // 添加到待执行队列中
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const TFunctor &cb) {
    {
        muduo::MutexLockGuard guard(mutex_);
        pendingFunctors_.push_back(cb);
    } 
    // !!!!如果非 IO 线程，可以采用唤醒机制, 及时执行执行函数
    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

void EventLoop::doPendingFunc() {
    LOG_TRACE << "do pending functons ...";
    if (!isInLoopThread()) {
        LOG_ERROR << "not in Loop thread";
        return;
    }

    std::vector<TFunctor> funcs;
    callingPendingFunctors_ = true;
    {
        muduo::MutexLockGuard guard(mutex_);
        funcs.swap(pendingFunctors_);
    }

    for (std::vector<TFunctor>::iterator it = funcs.begin();
            it != funcs.end(); it++) {
        (*it)();
    }

    callingPendingFunctors_ = false;
}


}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
