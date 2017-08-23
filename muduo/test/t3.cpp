/***************************************************************************
 * 
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file t3.cpp
 * @author caiguanyang(com@baidu.com)
 * @date 2017/08/10 23:34:46
 * @brief 
 * test EventLoop, Channel, Poller 
 **/

#include <sys/timerfd.h>
#include "muduo/cainet/EventLoop.h"
#include "muduo/cainet/Poller.h"
#include "muduo/cainet/Channel.h"
#include "muduo/base/Logging.h"

cai::EventLoop *g_loop;

void timeout() {
    LOG_INFO << "timeout() called";
    g_loop->quit();
}

int main() {
    // 设置日志等级
    muduo::Logger::setLogLevel(muduo::Logger::TRACE);

    cai::EventLoop loop;
    g_loop = &loop;

    // 创建 timer 文件描述符
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    cai::Channel channel(&loop, timerfd);
    channel.setReadCallback(timeout);
    channel.enableReading();

    struct itimerspec howlong;
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    loop.loop();

    ::close(timerfd);
    return 0;
}


















/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
