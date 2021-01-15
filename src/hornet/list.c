#include <hornet.h>

static bool on_list_clear( HornetList* list, void* arg, HornetListNode* node, void* value ){
  hornet_list_remove( list, node );
  return true;
}

HornetList* hornet_list_new( HornetListDeleteValueCallbackType delete_value_callback ){
  HornetList* list = (HornetList*)C_MEMORY_ALLOCATE( sizeof( HornetList ) );
  do{
    if ( null == list ) break;
    
    list->count = 0;
    list->root.prev = list->root.next = list->root.value = &(list->root);
    list->delete_value_callback = delete_value_callback;
    
    return list;
  }while ( 0 );
  hornet_list_delete( list );
  return null;
}

void hornet_list_delete( HornetList* list ){
  if ( null != list ){
    hornet_list_clear( list );
    C_MEMORY_FREE( list );
  }
}

int32 hornet_list_get_count( HornetList* list ){
  return list->count;
}

HornetListNode* hornet_list_get_head_node( HornetList* list ){
  return ( list->root.value == list->root.next ) ? null : list->root.next;
}

HornetListNode* hornet_list_get_tail_node( HornetList* list ){
  return ( list->root.value == list->root.prev ) ? null : list->root.prev;
}

HornetListNode* hornet_list_get_next_node( HornetList* list, HornetListNode* node ){
  return ( list->root.value == node->next ) ? null : node->next;
}

HornetListNode* hornet_list_get_prev_node( HornetList* list, HornetListNode* node ){
  return ( list->root.value == node->prev ) ? null : node->prev;
}

void hornet_list_clear( HornetList* list ){
  hornet_list_each( list, on_list_clear, null );
}

HornetListNode* hornet_list_add( HornetList* list, HornetListNode* node, void* value ){
  HornetListNode* new_node = (HornetListNode*)C_MEMORY_ALLOCATE( sizeof( HornetListNode ) );
  do{
    if ( null == new_node ) break;
    
    new_node->prev = list->root.prev;
    new_node->next = list->root.value;
    new_node->value = value;
    list->root.prev->next = new_node;
    list->root.prev = new_node;
    ++(list->count);
    return new_node;
  }while ( 0 );
  if ( null != new_node ) C_MEMORY_FREE( new_node );
  return null;
}

void hornet_list_remove( HornetList* list, HornetListNode* node ){
  if ( 0 == list->count ) return;
  
  void* value = hornet_list_get_value( list, node );
  node->prev->next = node->next;
  node->next->prev = node->prev;
  C_MEMORY_FREE( node );
  --(list->count);
  if ( null != list->delete_value_callback ) list->delete_value_callback( value );
}

HornetListNode* hornet_list_head_push( HornetList* list, void* value ){
  return hornet_list_add( list, hornet_list_get_head_node( list ), value );
}

HornetListNode* hornet_list_tail_push( HornetList* list, void* value ){
  return hornet_list_add( list, hornet_list_get_tail_node( list ), value );
}

void hornet_list_head_pop( HornetList* list ){
  hornet_list_remove( list, hornet_list_get_head_node( list ) );
}

void hornet_list_tail_pop( HornetList* list ){
  hornet_list_remove( list, hornet_list_get_tail_node( list ) );
}

void* hornet_list_get_value( HornetList* list, HornetListNode* node ){
  return ( list->root.value == node ) ? null : node->value;
}

void hornet_list_each( HornetList* list, HornetListEachCallbackType callback, void* arg ){
  int32 count = list->count;
  HornetListNode* node = hornet_list_get_head_node( list );
  HornetListNode* next_node;
  for ( ; 0 < count; --count, node = next_node ){
    next_node = hornet_list_get_next_node( list, node );
    if ( ! callback( list, arg, node, node->value ) ) break;
  }
}

void hornet_list_reach( HornetList* list, HornetListEachCallbackType callback, void* arg ){
  int32 count = list->count;
  HornetListNode* node = hornet_list_get_tail_node( list );
  HornetListNode* prev_node;
  for ( ; 0 < count; --count, node = prev_node ){
    prev_node = hornet_list_get_prev_node( list, node );
    if ( ! callback( list, arg, node, node->value ) ) break;
  }
}
