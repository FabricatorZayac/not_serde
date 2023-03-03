#ifndef DESERIALIZE_H_
#define DESERIALIZE_H_

#include <stdint.h>

#include "error.h"
#include "result.h"
#include "stdbool.h"

// Instead of visitors I just use pointers
#define DESERIALIZE_METHOD_HEADERS                                        \
    DeResult deserialize_bool(Deserializer *self, bool *visitor);         \
                                                                          \
    DeResult deserialize_uint8_t(Deserializer *self, uint8_t *visitor);   \
    DeResult deserialize_uint16_t(Deserializer *self, uint16_t *visitor); \
    DeResult deserialize_uint32_t(Deserializer *self, uint32_t *visitor); \
    DeResult deserialize_uint64_t(Deserializer *self, uint64_t *visitor); \
                                                                          \
    DeResult deserialize_int8_t(Deserializer *self, int8_t *visitor);     \
    DeResult deserialize_int16_t(Deserializer *self, int16_t *visitor);   \
    DeResult deserialize_int32_t(Deserializer *self, int32_t *visitor);   \
    DeResult deserialize_int64_t(Deserializer *self, int64_t *visitor);   \
                                                                          \
    DeResult deserialize_char(Deserializer *self, char *visitor);         \
    DeResult deserialize_str(Deserializer *self, str *visitor);           \
                                                                          \
    DeResult deserialize_default(Deserializer *self, void *visitor);

// clang-format off
#define deserialize(deserializer, visitor)       \
    (_Generic((visitor),                         \
              int8_t *   : deserialize_int8_t,   \
              int16_t *  : deserialize_int16_t,  \
              int32_t *  : deserialize_int32_t,  \
              int64_t *  : deserialize_int64_t,  \
              uint8_t *  : deserialize_uint8_t,  \
              uint16_t * : deserialize_uint16_t, \
              uint32_t * : deserialize_uint32_t, \
              uint64_t * : deserialize_uint64_t, \
              bool *     : deserialize_bool,     \
              str *      : deserialize_str,      \
              default    : deserialize_default) (deserializer, visitor))
// clang-format on

#endif  // DESERIALIZE_H_
