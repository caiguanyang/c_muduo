/***************************************************************************
 * 
 * Copyright (c) 2017 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file SocketsOps.h
 * @author caiguanyang(com@baidu.com)
 * @date 2017/09/01 23:25:47
 * @brief 
 *  
 **/

#ifndef  __SOCKETSOPS_H_
#define  __SOCKETSOPS_H_

#include <arpa/inet.h>

namespace cai {
namespace sockets {

void toIpPort(char* buf, size_t size,
              const struct sockaddr* addr);
void toIp(char* buf, size_t size,
          const struct sockaddr* addr);

void fromIpPort(const char* ip, uint16_t port,
                struct sockaddr_in* addr);
void fromIpPort(const char* ip, uint16_t port,
                struct sockaddr_in6* addr);


}
}
#endif  //__SOCKETSOPS_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
