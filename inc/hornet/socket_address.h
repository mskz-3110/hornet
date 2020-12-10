// [cc:namespace] Hornet
// [cc:class_name] SocketAddress
#ifndef __HORNET_SOCKET_ADDRESS_H__
#define __HORNET_SOCKET_ADDRESS_H__

#include <hornet/string.h>
#include <netinet/in.h>

// [cc:enum_name] Type
typedef enum {
  HORNET_SOCKET_ADDRESS_TYPE_NONE     = 0, // [cc:enum_value] NONE = 0
  HORNET_SOCKET_ADDRESS_TYPE_DATAGRAM = 1, // [cc:enum_value] DATAGRAM = 1
  HORNET_SOCKET_ADDRESS_TYPE_STREAM   = 2, // [cc:enum_value] STREAM = 2
} HornetSocketAddressType;

typedef struct {
  HornetSocketAddressType type;
  union {
    struct sockaddr     base;
    struct sockaddr_in  ipv4;
    struct sockaddr_in6 ipv6;
  } address;
  int32                   address_size;
  HornetString*           string;
} HornetSocketAddress;

typedef struct {
  HornetString*         protocol;
  HornetString*         path;
  int32                 addresses_count;
  HornetSocketAddress** addresses;
  HornetString*         errmsg;
} HornetSocketAddressResolveResult;

// [cc:callback] void ResolveResultCallback( IntPtr data, IntPtr result )
typedef void (*HornetSocketAddressResolveResultCallback)( void* data, HornetSocketAddressResolveResult* result );

C_API_BEGIN
  // [cc:api] IntPtr hornet_socket_address_clone( IntPtr address )
  C_API HornetSocketAddress* hornet_socket_address_clone( HornetSocketAddress* address );
  
  // [cc:api] void hornet_socket_address_delete( IntPtr address )
  C_API void hornet_socket_address_delete( HornetSocketAddress* address );
  
  // [cc:api] void hornet_socket_address_set( IntPtr _address, Type type, IntPtr address, int address_size )
  C_API void hornet_socket_address_set( HornetSocketAddress* _address, HornetSocketAddressType type, void* address, int32 address_size );
  
  // [cc:api] Type hornet_socket_address_get_type( IntPtr address )
  C_API HornetSocketAddressType hornet_socket_address_get_type( HornetSocketAddress* address );
  
  // [cc:api] IntPtr hornet_socket_address_get_address( IntPtr address )
  C_API void* hornet_socket_address_get_address( HornetSocketAddress* address );
  
  // [cc:api] int hornet_socket_address_get_address_size( IntPtr address )
  C_API int32 hornet_socket_address_get_address_size( HornetSocketAddress* address );
  
  // [cc:api] void hornet_socket_address_resolve( string url, ResolveResultCallback callback, IntPtr data )
  C_API void hornet_socket_address_resolve( string url, HornetSocketAddressResolveResultCallback callback, void* data );
  
  // [cc:api] string hornet_socket_address_resolve_result_get_protocol( IntPtr result )
  C_API string hornet_socket_address_resolve_result_get_protocol( HornetSocketAddressResolveResult* result );
  
  // [cc:api] string hornet_socket_address_resolve_result_get_path( IntPtr result )
  C_API string hornet_socket_address_resolve_result_get_path( HornetSocketAddressResolveResult* result );
  
  // [cc:api] int hornet_socket_address_resolve_result_get_addresses_count( IntPtr result )
  C_API int32 hornet_socket_address_resolve_result_get_addresses_count( HornetSocketAddressResolveResult* result );
  
  // [cc:api] IntPtr hornet_socket_address_resolve_result_get_address( IntPtr result, int index )
  C_API HornetSocketAddress* hornet_socket_address_resolve_result_get_address( HornetSocketAddressResolveResult* result, int32 index );
  
  // [cc:api] string hornet_socket_address_resolve_result_get_errmsg( IntPtr result )
  C_API string hornet_socket_address_resolve_result_get_errmsg( HornetSocketAddressResolveResult* result );
C_API_END

#endif
