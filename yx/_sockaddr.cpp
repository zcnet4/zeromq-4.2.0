/* -------------------------------------------------------------------------
//    FileName		:	D:\yx_code\yx\yx\_sockaddr.cpp
//    Creator		  : (zc) <zcnet4@gmail.com>
//    CreateTime	:	2016-12-27 22:24
//    Description	:    
//
// -----------------------------------------------------------------------*/
#include "_sockaddr.h"
#include <stdlib.h>
#include <memory.h>
#include <string.h>
// -------------------------------------------------------------------------

bool sockaddr_all_functor::operator()(const sockaddr_all& l, const sockaddr_all& r) const {
  if (l.s.sa_family == AF_INET && r.s.sa_family == AF_INET) {
    return memcmp(&l.v4, &r.v4, sizeof(l.v4)) < 0;
  }
  else if (l.s.sa_family == AF_INET6 && r.s.sa_family == AF_INET6) {
    if (l.v6.sin6_port < r.v6.sin6_port)
      return true;
    return memcmp(&l.v6.sin6_addr, &r.v6.sin6_addr, sizeof(l.v6.sin6_addr)) < 0;
  }
  //
  return false;
}

void split_host_port(const char* addr, char host[128], int* port) {
  strcpy(host, addr);
  char* p = strrchr(host, ':');
  if (p) {
    *p++ = '\0';
    *port = atoi(p);
  }
  else {
    *port = 0;
  }
}

int parse_hostaddr(const char* host_addr, sockaddr_all* addr) {
  int port = 0;
  char host[128] = { 0 };
  split_host_port(host_addr, host, &port);
  //
  int err = uv_ip6_addr(host, port, &addr->v6);
  if (0 != err) {
    memset(addr, 0, sizeof(sockaddr_all));
    err = uv_ip4_addr(host, port, &addr->v4);
  }
  //
  return err;
}

void to_hostaddr(const struct sockaddr* addr, char host_addr[64]) {
  if (AF_INET == addr->sa_family) {
    const sockaddr_in* a = reinterpret_cast<const struct sockaddr_in*>(addr);
    uv_ip4_name(a, host_addr, 64);
    sprintf(host_addr + strlen(host_addr), ":%u", a->sin_port);
  }
  else {
    const sockaddr_in6* a = reinterpret_cast<const struct sockaddr_in6*>(addr);
    uv_ip6_name(a, host_addr, 64);
    sprintf(host_addr + strlen(host_addr), ":%u", a->sin6_port);
  }
}

// -------------------------------------------------------------------------