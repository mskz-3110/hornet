// [cc:namespace] Hornet
// [cc:class_name] Store
#ifndef __HORNET_STORE_H__
#define __HORNET_STORE_H__

// [cc:callback] void DeleteValueCallbackType( IntPtr value )
typedef void (*HornetStoreDeleteValueCallbackType)( void* value );

typedef struct {
  int32                            alignment_count;
  int32                            value_size;
  int32                            allocated_count;
  int32                            count;
  int32                            head_index;
  int32                            tail_index;
  uint8*                           bytes;
  HornetStoreDeleteValueCallbackType delete_value_callback;
} HornetStore;

// [cc:callback] bool EachCallbackType( IntPtr store, IntPtr arg, uint key, IntPtr value )
typedef bool (*HornetStoreEachCallbackType)( HornetStore* store, void* arg, uint32 key, void* value );

C_API_BEGIN
  // [cc:api] IntPtr hornet_store_new( int alignment_count, int value_size, DeleteValueCallbackType delete_value_callback )
  C_API HornetStore* hornet_store_new( int32 alignment_count, int32 value_size, HornetStoreDeleteValueCallbackType delete_value_callback );
  
  // [cc:api] void hornet_store_delete( IntPtr store )
  C_API void hornet_store_delete( HornetStore* store );
  
  // [cc:api] int hornet_store_get_count( IntPtr store )
  C_API int32 hornet_store_get_count( HornetStore* store );
  
  // [cc:api] IntPtr hornet_store_get_value( IntPtr store, uint key )
  C_API void* hornet_store_get_value( HornetStore* store, uint32 key );
  
  // [cc:api] void hornet_store_clear( IntPtr store )
  C_API void hornet_store_clear( HornetStore* store );
  
  // [cc:api] bool hornet_store_lock( IntPtr store, uint key, bool is_unlock_check )
  C_API bool hornet_store_lock( HornetStore* store, uint32 key, bool is_unlock_check );
  
  // [cc:api] void hornet_store_unlock( IntPtr store, uint key )
  C_API void hornet_store_unlock( HornetStore* store, uint32 key );
  
  // [cc:api] void hornet_store_each( IntPtr store, EachCallbackType callback, IntPtr arg )
  C_API void hornet_store_each( HornetStore* store, HornetStoreEachCallbackType callback, void* arg );
  
  // [cc:api] void hornet_store_reach( IntPtr store, EachCallbackType callback, IntPtr arg )
  C_API void hornet_store_reach( HornetStore* store, HornetStoreEachCallbackType callback, void* arg );
C_API_END

#endif
