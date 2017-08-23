/***************************************************************************
 * 
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file Channel.h
 * @author caiguanyang(com@baidu.com)
 * @date 2017/08/08 22:36:45
 * @brief 
 *  
 **/

#ifndef  __CHANNEL_H_
#define  __CHANNEL_H_

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace cai {
// 前置声明，不需要引用头文件
class EventLoop;

class Channel : boost::noncopyable {
public:
    typedef boost::function<void()> EventCallback;

    Channel (EventLoop *loop, int fd);

    // 处理文件描述符准备就绪的事件
    void handleEvent ();
    // 更新 channel 的状态
    void update();

    inline int fd() {
        return fd_;
    }

    // for poller
    inline int index() {
        return index_;
    }

    EventLoop* ownerLoop() {
        return loop_;
    }

    void set_index(int idx) {
        index_ = idx;
    } 

    int events() const {
        return events_;
    }

    int revents() const {
        return revents_;
    }

    void set_revents(int re) {
        revents_ = re;
    }

    // channel 关注事件的设置
    void setReadCallback (const EventCallback &cb) {
        readCallback_ = cb;
    }
    void enableReading() {
        events_ |= Channel::kReadEvent;
        update();
    }
    void disableReading() {
        events_ &= ~Channel::kReadEvent;
        update();
    }

    void setWriteCallback (const EventCallback &cb) {
        writeCallback_ = cb;
    }
    void enableWriting() {
        events_ |= Channel::kWriteEvent;
        update();
    }
    void disableWriting() {
        events_ &= ~Channel::kWriteEvent;
        update();
    }

    void setErrorCallback (const EventCallback &cb) {
        errorCallback_ = cb;
    }

    void disableAll() {
        events_ = Channel::kNoneEvent;
        update();
    }

    bool isNoneEvents() {
        return events_ == Channel::kNoneEvent;
    }
private:
    // 读写标示常量
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_;
    const int fd_;
    // 关注的事件
    int events_;
    // 当前活跃的事件
    int revents_;
    // channel在 poller 中 pollfds 数组中的索引，便于查找使用
    int index_;
    // 事件回调函数
    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;

};

}

#endif  //__CHANNEL_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
