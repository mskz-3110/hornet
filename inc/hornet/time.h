// [cc:namespace] Hornet
// [cc:class_name] Time
#ifndef __HORNET_TIME_H__
#define __HORNET_TIME_H__

C_API_BEGIN
  // [cc:api] double hornet_time_now()
  C_API double hornet_time_now();
  
  // [cc:api] void hornet_sleep( int msec )
  C_API void hornet_sleep( int32 msec );
C_API_END

#endif
