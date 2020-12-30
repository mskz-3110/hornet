#ifndef __C_H__
#define __C_H__

#ifndef C_PLATFORM_WINDOWS
  #ifdef _MSC_VER
    #define C_PLATFORM_WINDOWS
  #endif
#endif

#ifndef C_PLATFORM_ANDROID
  #ifdef __ANDROID__
    #define C_PLATFORM_ANDROID
  #endif
#endif

#ifndef C_PLATFORM_APPLE
  #ifdef __APPLE__
    #define C_PLATFORM_APPLE
  #endif
#endif

#if ! defined( C_PLATFORM_WINDOWS ) && ! defined( C_PLATFORM_ANDROID ) && ! defined( C_PLATFORM_APPLE )
  #define C_PLATFORM_LINUX
#endif

#ifndef C_API_BEGIN
  #ifdef __cplusplus
    #define C_API_BEGIN extern "C" {
  #else
    #define C_API_BEGIN
  #endif
#endif

#ifndef C_API_END
  #ifdef __cplusplus
    #define C_API_END }
  #else
    #define C_API_END
  #endif
#endif

#ifndef null
  #define null NULL
#endif

#ifndef C_MEMORY_ALLOCATE
  #define C_MEMORY_ALLOCATE(...) malloc( __VA_ARGS__ )
#endif

#ifndef C_MEMORY_REALLOCATE
  #define C_MEMORY_REALLOCATE(...) realloc( __VA_ARGS__ )
#endif

#ifndef C_MEMORY_FREE
  #define C_MEMORY_FREE(...) free( __VA_ARGS__ )
#endif

#ifdef C_PLATFORM_WINDOWS
  #ifndef _CRTDBG_MAP_ALLOC
    #ifndef NDEBUG
      #define _CRTDBG_MAP_ALLOC
      #include <crtdbg.h>
      // _CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF );
      // _CrtSetBreakAlloc(  );
    #endif
  #endif
  
  #ifndef C_API
    #ifdef C_API_EXPORT
      #define C_API __declspec(dllexport)
    #else
      #define C_API
    #endif
    #define C_UNUSED_VAR( _var ) UNREFERENCED_PARAMETER( _var )
    #include <windows.h>
  #endif
#else
  #ifndef C_API
    #define C_UNUSED_VAR( _var )
    #define C_API
    #include <unistd.h>
  #endif
#endif

#ifndef C_TYPES
  #define C_TYPES
  
  #include <stdio.h>
  #include <stdint.h>
  #include <stdbool.h>
  #include <float.h>
  #include <stdlib.h>
  #include <stdarg.h>
  #include <string.h>
  #include <errno.h>
  
  typedef int8_t             int8;
  typedef uint8_t            uint8;
  typedef int16_t            int16;
  typedef uint16_t           uint16;
  typedef int32_t            int32;
  typedef uint32_t           uint32;
  typedef long long          int64;
  typedef unsigned long long uint64;
  typedef const char*        string;
  typedef const char*        String;
#endif

#endif
