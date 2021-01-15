// [cc:namespace] Hornet
// [cc:class_name] Arist
#ifndef __HORNET_ARIST_H__
#define __HORNET_ARIST_H__

typedef struct {
  int32  alignment_count;
  int32  value_size;
  int32  allocated_count;
  int32  count;
  int32  empty_id;
  int32  head_id;
  int32  tail_id;
  uint8* bytes;
} HornetArist;

// [cc:callback] bool EachCallbackType( IntPtr arist, IntPtr arg, int id, IntPtr value )
typedef bool (*HornetAristEachCallbackType)( HornetArist* arist, void* arg, int32 id, void* value );

C_API_BEGIN
  // [cc:api] IntPtr hornet_arist_new( int alignment_count, int value_size )
  C_API HornetArist* hornet_arist_new( int32 alignment_count, int32 value_size );
  
  // [cc:api] void hornet_arist_delete( IntPtr arist )
  C_API void hornet_arist_delete( HornetArist* arist );
  
  // [cc:api] int hornet_arist_get_count( IntPtr arist )
  C_API int32 hornet_arist_get_count( HornetArist* arist );
  
  // [cc:api] int hornet_arist_get_head_id( IntPtr arist )
  C_API int32 hornet_arist_get_head_id( HornetArist* arist );
  
  // [cc:api] int hornet_arist_get_tail_id( IntPtr arist )
  C_API int32 hornet_arist_get_tail_id( HornetArist* arist );
  
  // [cc:api] IntPtr hornet_arist_get_value( IntPtr arist, int id )
  C_API void* hornet_arist_get_value( HornetArist* arist, int32 id );
  
  // [cc:api] int hornet_arist_get_id( IntPtr arist, IntPtr value )
  C_API int32 hornet_arist_get_id( HornetArist* arist, void* value );
  
  // [cc:api] int hornet_arist_get_prev_id( IntPtr arist, int id )
  C_API int32 hornet_arist_get_prev_id( HornetArist* arist, int32 id );
  
  // [cc:api] int hornet_arist_get_next_id( IntPtr arist, int id )
  C_API int32 hornet_arist_get_next_id( HornetArist* arist, int32 id );
  
  // [cc:api] int hornet_arist_lock( IntPtr arist )
  C_API int32 hornet_arist_lock( HornetArist* arist );
  
  // [cc:api] void hornet_arist_unlock( IntPtr arist, int id )
  C_API void hornet_arist_unlock( HornetArist* arist, int32 id );
  
  // [cc:api] void hornet_arist_each( IntPtr arist, EachCallbackType callback, IntPtr arg )
  C_API void hornet_arist_each( HornetArist* arist, HornetAristEachCallbackType callback, void* arg );
  
  // [cc:api] void hornet_arist_reach( IntPtr arist, EachCallbackType callback, IntPtr arg )
  C_API void hornet_arist_reach( HornetArist* arist, HornetAristEachCallbackType callback, void* arg );
C_API_END

#endif
