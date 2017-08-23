/***************************************************************************
 * 
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file t2.cpp
 * @author caiguanyang(com@baidu.com)
 * @date 2017/08/05 23:06:20
 * @brief 
 *  
 **/

#include <muduo/cainet/EventLoop.h>
#include <muduo/base/Thread.h>
#include <muduo/base/CurrentThread.h>
#include <muduo/base/Logging.h>
#include <unistd.h>

cai::EventLoop *g_objLoop;

void threadFunc() {
    LOG_TRACE << "run thread:" << muduo::CurrentThread::tid();
    cai::EventLoop objLoop;
    objLoop.loop();
}

void threadFunc_2() {
    LOG_TRACE << "run thread:" << muduo::CurrentThread::tid();
    g_objLoop->loop();
}

int main() {
    // 设置日志等级
    muduo::Logger::setLogLevel(muduo::Logger::TRACE);

    muduo::Thread t(threadFunc);
    t.start();
    t.join();

    cai::EventLoop tmpLoop;
    g_objLoop = &tmpLoop;
    muduo::Thread t2(threadFunc_2);
    t2.start();
    t2.join();

    LOG_TRACE << "sleep 5 secs ...";
    sleep(5);
}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
