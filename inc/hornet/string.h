// [cc:namespace] Hornet
// [cc:class_name] String
#ifndef __HORNET_STRING_H__
#define __HORNET_STRING_H__

typedef struct {
  int32 alignment_size;
  int32 max_size;
  int32 length;
  char* string;
} HornetString;

typedef struct {
  string left_string;
  int32  left_length;
  string right_string;
  int32  right_length;
} HornetStringParseResult;

C_API_BEGIN
  // [cc:api] IntPtr hornet_string_new( int alignment_size )
  C_API HornetString* hornet_string_new( int32 alignment_size );
  
  // [cc:api] IntPtr hornet_string_clone( IntPtr _string )
  C_API HornetString* hornet_string_clone( HornetString* string );
  
  // [cc:api] void hornet_string_delete( IntPtr _string )
  C_API void hornet_string_delete( HornetString* string );
  
  // [cc:api] int hornet_string_get_length( IntPtr _string )
  C_API int32 hornet_string_get_length( HornetString* string );
  
  // [cc:api] string hornet_string_get_string( IntPtr _string )
  C_API string hornet_string_get_string( HornetString* string );
  
  // [cc:api] void hornet_string_clear( IntPtr _string )
  C_API void hornet_string_clear( HornetString* string );
  
  // [cc:api] bool hornet_string_add_int32( IntPtr _string, string format, int value )
  C_API bool hornet_string_add_int32( HornetString* string, String format, int32 value );
  
  // [cc:api] bool hornet_string_add_int64( IntPtr _string, string format, long value )
  C_API bool hornet_string_add_int64( HornetString* string, String format, int64 value );
  
  // [cc:api] bool hornet_string_add_double( IntPtr _string, string format, double value )
  C_API bool hornet_string_add_double( HornetString* string, String format, double value );
  
  // [cc:api] bool hornet_string_add_string( IntPtr _string, string format, string value )
  C_API bool hornet_string_add_string( HornetString* string, String format, String value );
  
  // [cc:api] bool hornet_string_add_p( IntPtr _string, string format, IntPtr value )
  C_API bool hornet_string_add_p( HornetString* string, String format, void* value );
  
  // [cc:api] bool hornet_string_add_time( IntPtr _string, double time )
  C_API bool hornet_string_add_time( HornetString* string, double time );
  
  // [cc:api] bool hornet_string_add_errmsg( IntPtr _string )
  C_API bool hornet_string_add_errmsg( HornetString* string );
  
  // [cc:api] void hornet_string_remove( IntPtr _string, int length )
  C_API void hornet_string_remove( HornetString* string, int32 length );
  
  // [cc:api] void hornet_string_set_lower( IntPtr _string, int offset, int length )
  C_API void hornet_string_set_lower( HornetString* string, int32 offset, int32 length );
  
  // [cc:api] void hornet_string_set_upper( IntPtr _string, int offset, int length )
  C_API void hornet_string_set_upper( HornetString* string, int32 offset, int32 length );
  
  // [cc:api] int hornet_string_to_int32( string _string )
  C_API int32 hornet_string_to_int32( String string );
  
  // [cc:api] long hornet_string_to_int64( string _string )
  C_API int64 hornet_string_to_int64( String string );
  
  // [cc:api] double hornet_string_to_double( string _string )
  C_API double hornet_string_to_double( String string );
  
  // [cc:api] string hornet_string_get_empty()
  C_API string hornet_string_get_empty();
  
  C_API HornetString hornet_string_fixed( char* fixed_string, int32 max_size );
  
  C_API HornetStringParseResult hornet_string_split_once( String string, String delimiter );
C_API_END

#endif
