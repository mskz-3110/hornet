// [cc:namespace] Hornet
// [cc:class_name] Hornet
#ifndef __HORNET_H__
#define __HORNET_H__

#include <c.h>
#include <hornet/io.h>
#include <hornet/memory.h>
#include <hornet/string.h>
#include <hornet/socket_address.h>
#include <hornet/socket.h>
#include <hornet/poll.h>
#include <hornet/time.h>
#include <hornet/arist.h>
#include <hornet/list.h>
#include <hornet/store.h>

C_API_BEGIN
  // [cc:api] bool hornet_init()
  C_API bool hornet_init();
  
  // [cc:api] void hornet_exit()
  C_API void hornet_exit();
  
  // [cc:api] void hornet_lock()
  C_API void hornet_lock();
  
  // [cc:api] void hornet_unlock()
  C_API void hornet_unlock();
C_API_END

#endif
