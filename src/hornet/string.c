#include <hornet.h>
#include <sys/time.h>
#include <ctype.h>

static string s_string_empty = "";

static int32 string_need_size( String format, va_list list ){
  va_list copied_list;
  va_copy( copied_list, list );
  int32 need_size = vsnprintf( null, 0, format, copied_list );
  va_end( copied_list );
  return need_size + 1;
}

static bool string_add_check( HornetString* string, int32 size ){
  int32 max_size = string->length + size;
  if ( string->max_size < 0 ){
    return ( ( string->max_size + max_size ) <= 0 );
  }
  
  if ( string->max_size < max_size ){
    max_size = hornet_memory_aligned_size( max_size, string->alignment_size );
    char* new_string = (char*)C_MEMORY_REALLOCATE( string->string, max_size );
    if ( null == new_string ) return false;
    
    string->max_size = max_size;
    string->string = new_string;
  }
  return true;
}

static bool string_add( HornetString* string, String format, ... ){
  va_list list;
  va_start( list, format );
  int32 need_size = string_need_size( format, list );
  bool is_add_check = string_add_check( string, need_size );
  if ( is_add_check ){
    vsnprintf( &(string->string[ string->length ]), need_size, format, list );
    string->length += ( need_size - 1 );
  }
  va_end( list );
  return is_add_check;
}

HornetString* hornet_string_new( int32 alignment_size ){
  HornetString* string = (HornetString*)C_MEMORY_ALLOCATE( sizeof( HornetString ) );
  do{
    if ( null == string ) break;
    
    string->alignment_size = alignment_size;
    string->max_size = 0;
    string->length = 0;
    string->string = null;
    return string;
  }while ( 0 );
  hornet_string_delete( string );
  return null;
}

HornetString* hornet_string_clone( HornetString* string ){
  if ( null == string ) return null;
  HornetString* cloned_string = (HornetString*)C_MEMORY_ALLOCATE( sizeof( HornetString ) );
  do{
    if ( null == cloned_string ) break;
    
    cloned_string->alignment_size = string->alignment_size;
    cloned_string->max_size = string->max_size;
    cloned_string->length = string->length;
    if ( 0 < cloned_string->max_size ){
      cloned_string->string = (char*)C_MEMORY_ALLOCATE( cloned_string->max_size );
      if ( null == cloned_string->string ) break;
      memcpy( cloned_string->string, string->string, cloned_string->length + 1 );
    }else if ( cloned_string->max_size < 0 ){
      cloned_string->string = string->string;
    }
    
    return cloned_string;
  }while ( 0 );
  hornet_string_delete( cloned_string );
  return null;
}

void hornet_string_delete( HornetString* string ){
  if ( null != string ){
    if ( 0 < string->max_size ){
      if ( null != string->string ){
        C_MEMORY_FREE( string->string );
      }
    }
    C_MEMORY_FREE( string );
  }
}

int32 hornet_string_get_length( HornetString* string ){
  return string->length;
}

string hornet_string_get_string( HornetString* string ){
  return ( null != string->string ) ? string->string : s_string_empty;
}

void hornet_string_clear( HornetString* string ){
  string->length = 0;
  if ( null != string->string ) string->string[ 0 ] = '\0';
}

bool hornet_string_add_int32( HornetString* string, String format, int32 value ){
  return string_add( string, format, value );
}

bool hornet_string_add_int64( HornetString* string, String format, int64 value ){
  return string_add( string, format, value );
}

bool hornet_string_add_double( HornetString* string, String format, double value ){
  return string_add( string, format, value );
}

bool hornet_string_add_string( HornetString* string, String format, String value ){
  return string_add( string, format, value );
}

bool hornet_string_add_p( HornetString* string, String format, void* value ){
  return string_add( string, format, value );
}

bool hornet_string_add_time( HornetString* string, double time ){
  struct tm local;
  time_t seconds = (time_t)time;
  int32 msec = (time - (double)seconds) * 1000;
  localtime_r( &seconds, &local );
  if ( ! hornet_string_add_int32( string, "%02d/", (local.tm_year + 1900) % 100 ) ) return false;
  if ( ! hornet_string_add_int32( string, "%02d/", local.tm_mon + 1 ) ) return false;
  if ( ! hornet_string_add_int32( string, "%02d ", local.tm_mday ) ) return false;
  if ( ! hornet_string_add_int32( string, "%02d:", local.tm_hour ) ) return false;
  if ( ! hornet_string_add_int32( string, "%02d:", local.tm_min ) ) return false;
  if ( ! hornet_string_add_int32( string, "%02d.", local.tm_sec ) ) return false;
  if ( ! hornet_string_add_int32( string, "%03d", msec ) ) return false;
  return true;
}

void hornet_string_remove( HornetString* string, int32 length ){
  if ( length <= 0 ) return;
  
  if ( string->length <= length ){
    hornet_string_clear( string );
  }else{
    string->length -= length;
    string->string[ string->length ] = '\0';
  }
}

void hornet_string_to_lower( HornetString* string, int32 offset, int32 length ){
  if ( string->length <= offset ) return;
  if ( string->length < offset + length ) return;
  
  for ( int32 i = 0; i < length; ++i ){
    string->string[ offset + i ] = tolower( string->string[ offset + i ] );
  }
}

void hornet_string_to_upper( HornetString* string, int32 offset, int32 length ){
  if ( string->length <= offset ) return;
  if ( string->length < offset + length ) return;
  
  for ( int32 i = 0; i < length; ++i ){
    string->string[ offset + i ] = toupper( string->string[ offset + i ] );
  }
}

string hornet_string_get_empty(){
  return s_string_empty;
}

HornetString hornet_string_fixed( char* fixed_string, int max_size ){
  HornetString string;
  string.alignment_size = 0;
  string.max_size = - max_size;
  string.string = fixed_string;
  hornet_string_clear( &string );
  return string;
}

HornetStringParseResult hornet_string_parse( String string, String delimiter ){
  HornetStringParseResult result;
  int32 length = strlen( string );
  result.left_string = string;
  result.left_length = length;
  result.right_string = &(string[ length ]);
  result.right_length = 0;
  char* right_string = strstr( string, delimiter );
  if ( null != right_string ){
    result.left_length = (int32)(right_string - string);
    right_string += strlen( delimiter );
    result.right_string = right_string;
    result.right_length = length - result.left_length - strlen( delimiter );
  }
  return result;
}
