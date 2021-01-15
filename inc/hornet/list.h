// [cc:namespace] Hornet
// [cc:class_name] List
#ifndef __HORNET_LIST_H__
#define __HORNET_LIST_H__

typedef struct __HornetListNode__ {
  struct __HornetListNode__* prev;
  struct __HornetListNode__* next;
  void*                    value;
} HornetListNode;

// [cc:callback] void DeleteValueCallbackType( IntPtr value )
typedef void (*HornetListDeleteValueCallbackType)( void* value );

typedef struct {
  HornetListDeleteValueCallbackType delete_value_callback;
  int32                           count;
  HornetListNode                    root;
} HornetList;

// [cc:callback] bool EachCallbackType( IntPtr list, IntPtr arg, IntPtr node, IntPtr value )
typedef bool (*HornetListEachCallbackType)( HornetList* list, void* arg, HornetListNode* node, void* value );

C_API_BEGIN
  // [cc:api] IntPtr hornet_list_new( DeleteValueCallbackType delete_value_callback )
  C_API HornetList* hornet_list_new( HornetListDeleteValueCallbackType delete_value_callback );
  
  // [cc:api] void hornet_list_delete( IntPtr list )
  C_API void hornet_list_delete( HornetList* list );
  
  // [cc:api] int hornet_list_get_count( IntPtr list )
  C_API int32 hornet_list_get_count( HornetList* list );
  
  // [cc:api] IntPtr hornet_list_get_head_node( IntPtr list )
  C_API HornetListNode* hornet_list_get_head_node( HornetList* list );
  
  // [cc:api] IntPtr hornet_list_get_tail_node( IntPtr list )
  C_API HornetListNode* hornet_list_get_tail_node( HornetList* list );
  
  // [cc:api] IntPtr hornet_list_get_next_node( IntPtr list, IntPtr node )
  C_API HornetListNode* hornet_list_get_next_node( HornetList* list, HornetListNode* node );
  
  // [cc:api] IntPtr hornet_list_get_prev_node( IntPtr list, IntPtr node )
  C_API HornetListNode* hornet_list_get_prev_node( HornetList* list, HornetListNode* node );
  
  // [cc:api] void hornet_list_clear( IntPtr list )
  C_API void hornet_list_clear( HornetList* list );
  
  // [cc:api] IntPtr hornet_list_add( IntPtr list, IntPtr node, IntPtr value )
  C_API HornetListNode* hornet_list_add( HornetList* list, HornetListNode* node, void* value );
  
  // [cc:api] void hornet_list_remove( IntPtr list, IntPtr node )
  C_API void hornet_list_remove( HornetList* list, HornetListNode* node );
  
  // [cc:api] IntPtr hornet_list_head_push( IntPtr list, IntPtr value )
  C_API HornetListNode* hornet_list_head_push( HornetList* list, void* value );
  
  // [cc:api] IntPtr hornet_list_tail_push( IntPtr list, IntPtr value )
  C_API HornetListNode* hornet_list_tail_push( HornetList* list, void* value );
  
  // [cc:api] void hornet_list_head_pop( IntPtr list )
  C_API void hornet_list_head_pop( HornetList* list );
  
  // [cc:api] void hornet_list_tail_pop( IntPtr list )
  C_API void hornet_list_tail_pop( HornetList* list );
  
  // [cc:api] IntPtr hornet_list_get_value( IntPtr list, IntPtr node )
  C_API void* hornet_list_get_value( HornetList* list, HornetListNode* node );
  
  // [cc:api] void hornet_list_each( IntPtr list, EachCallbackType callback, IntPtr arg )
  C_API void hornet_list_each( HornetList* list, HornetListEachCallbackType callback, void* arg );
  
  // [cc:api] void hornet_list_reach( IntPtr list, EachCallbackType callback, IntPtr arg )
  C_API void hornet_list_reach( HornetList* list, HornetListEachCallbackType callback, void* arg );
C_API_END

#endif
