/* -------------------------------------------------------------------------
//    FileName		:	d:\yx_code\yx\yx\_sockaddr.h
//    Creator		  : (zc) <zcnet4@gmail.com>
//    CreateTime	:	2016-12-27 22:06
//    Description	:    
//
// -----------------------------------------------------------------------*/
#ifndef _SOCKADDR_H_
#define _SOCKADDR_H_
#include "uv.h"
// -------------------------------------------------------------------------

union sockaddr_all {
  struct sockaddr s;
  struct sockaddr_in v4;
  struct sockaddr_in6 v6;
};

struct sockaddr_all_functor
{
  bool operator()(const sockaddr_all& l, const sockaddr_all& r) const;
};

void split_host_port(const char* addr, char host[128], int* port);
int parse_hostaddr(const char* host_addr, sockaddr_all* addr);
void to_hostaddr(const struct sockaddr* addr, char host_addr[64]);

// -------------------------------------------------------------------------
#endif /* _SOCKADDR_H_ */