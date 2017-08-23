/***************************************************************************
 * 
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file t1.cpp
 * @author caiguanyang(com@baidu.com)
 * @date 2017/08/05 23:06:20
 * @brief 
 *  
 **/

#include <muduo/cainet/EventLoop.h>
#include <muduo/base/Logging.h>

int main() {
    // 设置日志等级
    muduo::Logger::setLogLevel(muduo::Logger::TRACE);

    cai::EventLoop objLoop;
    objLoop.loop();
    return 0;
}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
