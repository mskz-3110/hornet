// [cc:namespace] Hornet
// [cc:class_name] Socket
#ifndef __HORNET_SOCKET_H__
#define __HORNET_SOCKET_H__

#include <hornet/poll.h>

struct HornetSocket;

// [cc:callback] void EventCallback( IntPtr socket, int events )
typedef void (*HornetSocketEventCallback)( struct HornetSocket* socket, int32 events );

typedef struct HornetSocket {
  HornetPoll*               poll;
  HornetSocketAddress*      address;
  HornetSocketAddress*      peer_address;
  HornetString*             errmsg;
  HornetIO                  io;
  HornetSocketEventCallback event_callback;
  void*                     data;
} HornetSocket;

// [cc:enum_name] Event
typedef enum {
  HORNET_SOCKET_EVENT_NONE         = 0,  // [cc:enum_value] NONE = 0
  HORNET_SOCKET_EVENT_READABLE     = 1,  // [cc:enum_value] READABLE = 1
  HORNET_SOCKET_EVENT_WRITABLE     = 2,  // [cc:enum_value] WRITABLE = 2
  HORNET_SOCKET_EVENT_ACCEPTABLE   = 4,  // [cc:enum_value] ACCEPTABLE = 4
  HORNET_SOCKET_EVENT_CONNECTED    = 8,  // [cc:enum_value] CONNECTED = 8
  HORNET_SOCKET_EVENT_DISCONNECTED = 16, // [cc:enum_value] DISCONNECTED = 16
} HornetSocketEvent;

C_API_BEGIN
  // [cc:api] IntPtr hornet_socket_new( IntPtr poll, IntPtr address )
  C_API HornetSocket* hornet_socket_new( HornetPoll* poll, HornetSocketAddress* address );
  
  // [cc:api] void hornet_socket_delete( IntPtr socket )
  C_API void hornet_socket_delete( HornetSocket* socket );
  
  // [cc:api] void hornet_socket_set_peer_address( IntPtr socket, IntPtr address )
  C_API void hornet_socket_set_peer_address( HornetSocket* socket, HornetSocketAddress* address );
  
  // [cc:api] IntPtr hornet_socket_get_peer_address( IntPtr socket )
  C_API HornetSocketAddress* hornet_socket_get_peer_address( HornetSocket* socket );
  
  // [cc:api] string hornet_socket_get_errmsg( IntPtr socket )
  C_API string hornet_socket_get_errmsg( HornetSocket* socket );
  
  // [cc:api] IntPtr hornet_socket_get_io( IntPtr socket )
  C_API HornetIO* hornet_socket_get_io( HornetSocket* socket );
  
  // [cc:api] void hornet_socket_set_event_callback( IntPtr socket, EventCallback event_callback )
  C_API void hornet_socket_set_event_callback( HornetSocket* socket, HornetSocketEventCallback event_callback );
  
  // [cc:api] void hornet_socket_call_event( IntPtr socket, int events )
  C_API void hornet_socket_call_event( HornetSocket* socket, int32 events );
  
  // [cc:api] void hornet_socket_set_data( IntPtr socket, IntPtr data )
  C_API void hornet_socket_set_data( HornetSocket* socket, void* data );
  
  // [cc:api] IntPtr hornet_socket_get_data( IntPtr socket )
  C_API void* hornet_socket_get_data( HornetSocket* socket );
  
  // [cc:api] bool hornet_socket_standalone( IntPtr socket )
  C_API bool hornet_socket_standalone( HornetSocket* socket );
  
  // [cc:api] bool hornet_socket_standby( IntPtr socket )
  C_API bool hornet_socket_standby( HornetSocket* socket );
  
  // [cc:api] bool hornet_socket_connect( IntPtr socket, float timeout )
  C_API bool hornet_socket_connect( HornetSocket* socket, float timeout );
  
  // [cc:api] IntPtr hornet_socket_accept( IntPtr socket )
  C_API HornetSocket* hornet_socket_accept( HornetSocket* socket );
  
  // [cc:api] bool hornet_socket_set_write_buffer_size( IntPtr socket, int write_buffer_size )
  C_API bool hornet_socket_set_write_buffer_size( HornetSocket* socket, int32 write_buffer_size );
  
  // [cc:api] int hornet_socket_get_write_buffer_size( IntPtr socket )
  C_API int32 hornet_socket_get_write_buffer_size( HornetSocket* socket );
  
  // [cc:api] bool hornet_socket_set_read_buffer_size( IntPtr socket, int read_buffer_size )
  C_API bool hornet_socket_set_read_buffer_size( HornetSocket* socket, int32 read_buffer_size );
  
  // [cc:api] int hornet_socket_get_read_buffer_size( IntPtr socket )
  C_API int32 hornet_socket_get_read_buffer_size( HornetSocket* socket );
  
  // [cc:api] bool hornet_socket_set_is_broadcast( IntPtr socket, bool is_broadcast )
  C_API bool hornet_socket_set_is_broadcast( HornetSocket* socket, bool is_broadcast );
  
  // [cc:api] bool hornet_socket_get_is_broadcast( IntPtr socket )
  C_API bool hornet_socket_get_is_broadcast( HornetSocket* socket );
  
  // [cc:api] bool hornet_socket_write( IntPtr socket, byte[] data, int data_size )
  C_API bool hornet_socket_write( HornetSocket* socket, void* data, int32 data_size );
  
  // [cc:api] int hornet_socket_read( IntPtr socket, byte[] data, int data_size )
  C_API int32 hornet_socket_read( HornetSocket* socket, void* data, int32 data_size );
  
  // [cc:api] void hornet_socket_to_string( IntPtr _string, IntPtr socket )
  C_API void hornet_socket_to_string( HornetString* string, HornetSocket* socket );
C_API_END

#endif
