#ifndef SERIALIZE_H_
#define SERIALIZE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cursed_macros.h"
#include "error.h"
#include "option.h"

#define SERIALIZE_FIELD(field, ...) \
    SerializeStruct_key(s, #field); \
    SerializeStruct_value(s, v->field);

/**
 * \def SERIALIZE_METHOD_HEADERS
 * In serializer header
 * `#define SerializeStruct_key(self, key)`
 * `#define SerializeStruct_value(self, value)`
 * Before applying this macro
 * `typedef *your serializer type* Serializer`
 * `typedef Result({}, Error) SerResult;`
 * //`typedef Result(SeqSerType, Error) SerSeqResult;`
 * `typedef Result(SeqStructType, Error) SerStructResult;`
 */
#define SERIALIZE_METHOD_HEADERS                              \
    SerResult serialize_bool(Serializer *self, bool v);       \
                                                              \
    SerResult serialize_int8(Serializer *self, int8_t v);     \
    SerResult serialize_int16(Serializer *self, int16_t v);   \
    SerResult serialize_int32(Serializer *self, int32_t v);   \
    SerResult serialize_int64(Serializer *self, int64_t v);   \
                                                              \
    SerResult serialize_uint8(Serializer *self, uint8_t v);   \
    SerResult serialize_uint16(Serializer *self, uint16_t v); \
    SerResult serialize_uint32(Serializer *self, uint32_t v); \
    SerResult serialize_uint64(Serializer *self, uint64_t v); \
                                                              \
    SerResult serialize_float(Serializer *self, float v);     \
    SerResult serialize_double(Serializer *self, double v);   \
                                                              \
    SerResult serialize_char(Serializer *self, char v);       \
    SerResult serialize_cstr(Serializer *self, char *v);      \
    SerResult serialize_str(Serializer *self, str v);         \
                                                              \
    SerResult serialize_unit(Serializer *self);               \
                                                              \
    SerStructResult serialize_struct(Serializer *self,        \
                                     char *name,              \
                                     size_t len);             \
    SerResult SerializeStruct_end(SerializeStruct *self);     \
                                                              \
    SerResult serialize_default(Serializer *self, void *v);

// clang-format off
#define serialize(serializer, value)       \
    (_Generic((value),                     \
              int8_t   : serialize_int8,   \
              int16_t  : serialize_int16,  \
              int32_t  : serialize_int32,  \
              int64_t  : serialize_int64,  \
              uint8_t  : serialize_uint8,  \
              uint16_t : serialize_uint16, \
              uint32_t : serialize_uint32, \
              uint64_t : serialize_uint64, \
              float    : serialize_float,  \
              double   : serialize_double, \
              char     : serialize_char,   \
              bool     : serialize_bool,   \
              char *   : serialize_cstr,   \
              str      : serialize_str,    \
              default  : serialize_default) (serializer, value))
// clang-format on

#endif  // SERIALIZE_H_
