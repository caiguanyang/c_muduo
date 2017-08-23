/***************************************************************************
 * 
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file Callbacks.h
 * @author caiguanyang(com@baidu.com)
 * @date 2017/08/14 12:43:21
 * @brief 
 * 定义用到的 callback 类型 
 **/

#ifndef  __CALLBACKS_H_
#define  __CALLBACKS_H_

namespace cai {
    // timer相关的 callback
    typedef boost::function<void ()> TTimerCallBack;

}

#endif  //__CALLBACKS_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
