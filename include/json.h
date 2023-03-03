#ifndef JSON_H_
#define JSON_H_

#include <stdbool.h>
#include <stdint.h>

#include "deserialize.h"
#include "error.h"
#include "result.h"
#include "serde.h"
#include "vector.h"

#if 0
typedef struct JsonValue JsonValue;
typedef struct {
    char *key;
    JsonValue *value;
} JsonField;

data(JsonValue,
     (JsonNull),
     (JsonBool, bool),
     (JsonNumber, double),
     (JsonString, char *),
     (JsonArray, JsonValue *),
     (JsonObject, JsonField *));

    #define JsonValue(Type, ...)                                               \
        (JsonValue) {                                                          \
            .switcher = Json##Type, __VA_OPT__(.body.Json##Type = __VA_ARGS__) \
        }
#endif

DefineVec(String, char);
static inline void String_append_slice(String *self, char *slice) {
    String value = String_from_arr(slice, strlen(slice));
    self->append(self, &value);
    value.impl.destroy(&value);
}

typedef Result(struct {}, Error) SerResult;
typedef struct {
    String output;
} Serializer;

#define SEQ_STUFF 0
#if SEQ_STUFF
typedef Serializer SerializeSeq;
typedef Result(SerializeSeq *, Error) SerSeqResult;
#endif

typedef Serializer SerializeStruct;
typedef Result(SerializeStruct *, Error) SerStructResult;

SERIALIZE_METHOD_HEADERS

#define SerializeStruct_key(self, key)                     \
    if (self->output.body[self->output.size - 1] != '{') { \
        self->output.push_back(&self->output, ',');        \
    }                                                      \
    serialize(self, key);

#define SerializeStruct_value(self, key)        \
    self->output.push_back(&self->output, ':'); \
    serialize(self, key);

#if SEQ_STUFF
    #define SerializeSeq_element(self, key)                    \
        if (self->output.body[self->output.size - 1] != '[') { \
            self->output.push_back(&self->output, ',');        \
        }                                                      \
        serialize(self, key);
#endif

////////////////////////////////////////////////////////////////////////////////

typedef struct {
    char *input;
} Deserializer;

typedef Result(char, Error) ResChar;
typedef Result(bool, Error) ResBool;
typedef Result(uint64_t, Error) ResUint;
typedef Result(int64_t, Error) ResInt;
typedef Result(str, Error) ResStr;

// Parser. Can literally be whatever
ResChar peek_char(Deserializer *self);
ResChar next_char(Deserializer *self);
ResBool parse_bool(Deserializer *self);
ResUint parse_unsigned(Deserializer *self);
ResInt parse_signed(Deserializer *self);
ResStr parse_str(Deserializer *self);
//

typedef Result(struct {}, Error) DeResult;
DESERIALIZE_METHOD_HEADERS

#endif  // JSON_H_
