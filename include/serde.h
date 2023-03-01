#ifndef SERDE_H_
#define SERDE_H_

#include "deserialize.h"
#include "serialize.h"

#define serde_struct(Name, ...)                                              \
    typedef struct Name {                                                    \
        SerResult (*ser)(Serializer *, struct Name *);                       \
        FOREACH(GET_FIELD, __VA_ARGS__);                                     \
    } Name;                                                                  \
    enum { Name##_FIELDCOUNT = NUM_ARGS(__VA_ARGS__) };                      \
    SerResult serialize_##Name(Serializer *self, Name *v) {                  \
        SerializeStruct *s;                                                  \
        TRY(SerResult, serialize_struct(self, #Name, Name##_FIELDCOUNT), s); \
        FOREACH(SERIALIZE_FIELD, FOREACH(GET_ENUM, __VA_ARGS__));            \
        SerializeStruct_end(s);                                              \
        return Ok(SerResult, {});                                            \
    }

#endif  // SERDE_H_
