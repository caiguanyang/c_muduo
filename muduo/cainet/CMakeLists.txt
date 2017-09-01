set(net_SRCS
  EventLoop.cc
  Channel.cc
  Poller.cc
  InetAddress.cc
  TimerQueue.cc
  SocketsOps.cc
  InetAddress.cc
  )

add_library(muduo_cainet ${net_SRCS})
target_link_libraries(muduo_cainet muduo_base)
