#include <hornet.h>

HornetSocketAddress* hornet_socket_address_clone( HornetSocketAddress* address ){
  if ( null == address ) return null;
  HornetSocketAddress* cloned_address = (HornetSocketAddress*)C_MEMORY_ALLOCATE( sizeof( HornetSocketAddress ) );
  do {
    if ( null == cloned_address ) break;
    
    cloned_address->type = address->type;
    memcpy( &(cloned_address->address), &(address->address), address->address_size );
    cloned_address->address_size = address->address_size;
    cloned_address->string = hornet_string_clone( address->string );
    
    return cloned_address;
  }while ( 0 );
  hornet_socket_address_delete( cloned_address );
  return null;
}

void hornet_socket_address_delete( HornetSocketAddress* address ){
  if ( null != address ){
    if ( null != address->string ){
      hornet_string_delete( address->string );
      address->string = null;
    }
  }
}

void hornet_socket_address_set( HornetSocketAddress* _address, HornetSocketAddressType type, void* address, int32 address_size ){
  _address->type = type;
  if ( sizeof( _address->address ) < address_size ){
    address_size = sizeof( _address->address );
  }else if ( address_size < 0 ){
    address_size = 0;
  }
  if ( 0 < address_size ) memcpy( &(_address->address), address, address_size );
}

HornetSocketAddressType hornet_socket_address_get_type( HornetSocketAddress* address ){
  return address->type;
}

void* hornet_socket_address_get_address( HornetSocketAddress* address ){
  return &(address->address);
}

int32 hornet_socket_address_get_address_size( HornetSocketAddress* address ){
  return address->address_size;
}

void hornet_socket_address_resolve( string url, HornetSocketAddressResolveResultCallback callback, void* data ){
  // TODO
}

string hornet_socket_address_resolve_result_get_protocol( HornetSocketAddressResolveResult* result ){
  return hornet_string_get_string( result->protocol );
}

string hornet_socket_address_resolve_result_get_path( HornetSocketAddressResolveResult* result ){
  return hornet_string_get_string( result->path );
}

int32 hornet_socket_address_resolve_result_get_addresses_count( HornetSocketAddressResolveResult* result ){
  return result->addresses_count;
}

HornetSocketAddress* hornet_socket_address_resolve_result_get_address( HornetSocketAddressResolveResult* result, int32 index ){
  return ( index < result->addresses_count ) ? result->addresses[ index ] : null;
}

string hornet_socket_address_resolve_result_get_errmsg( HornetSocketAddressResolveResult* result ){
  return hornet_string_get_string( result->errmsg );
}
