#ifndef DESERIALIZE_H_
#define DESERIALIZE_H_

#include <stdint.h>

#include "error.h"
#include "result.h"
#include "stdbool.h"

#define DESERIALIZE_METHOD_HEADERS              \
    ResChar peek_char(Deserializer *self);      \
    ResChar next_char(Deserializer *self);      \
    ResBool parse_bool(Deserializer *self);     \
    ResUint parse_unsigned(Deserializer *self); \
    ResInt parse_signed(Deserializer *self);    \
    ResStr parse_str(Deserializer *self);

#endif  // DESERIALIZE_H_
