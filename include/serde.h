#ifndef SERDE_H_
#define SERDE_H_

#include <ctype.h>

#include "cursed_macros.h"
#include "deserialize.h"
#include "serialize.h"

#define DE_FIELD(field, ...)                \
    do                                      \
        _TRY(DeResult, next_char(self), ch) \
    while (ch != ':');                      \
    deserialize(self, &visitor->field);

#define serde_struct(Name, ...)                                              \
    typedef struct Name {                                                    \
        SerResult (*ser)(Serializer *, struct Name *);                       \
        DeResult (*de)(Deserializer *, struct Name *);                       \
        FOREACH(GET_FIELD, __VA_ARGS__);                                     \
    } Name;                                                                  \
    enum { Name##_FIELDCOUNT = NUM_ARGS(__VA_ARGS__) };                      \
    SerResult serialize_##Name(Serializer *self, Name *v) {                  \
        SerializeStruct *s;                                                  \
        TRY(SerResult, serialize_struct(self, #Name, Name##_FIELDCOUNT), s); \
        FOREACH(SERIALIZE_FIELD, FOREACH(GET_ENUM, __VA_ARGS__));            \
        SerializeStruct_end(s);                                              \
        return Ok(SerResult, {});                                            \
    }                                                                        \
    DeResult deserialize_##Name(Deserializer *self, Name *visitor) {         \
        char ch;                                                             \
        do                                                                   \
            TRY(DeResult, next_char(self), ch)                               \
        while (ch != '{');                                                   \
        FOREACH(DE_FIELD, FOREACH(GET_ENUM, __VA_ARGS__));                   \
        return Ok(DeResult, {});                                             \
    }

typedef struct {
    char *body;
    int size;
} str;

#endif  // SERDE_H_
