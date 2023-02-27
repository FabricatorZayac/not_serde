#ifndef JSON_H_
#define JSON_H_

#include <stdbool.h>
#include <stdint.h>

#include "error.h"
#include "result.h"
#include "serialize.h"
#include "sumtype.h"
#include "vector.h"

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
typedef Result(Serializer *, Error) SerSeqResult;

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

#define JsonValue(Type, ...)                                               \
    (JsonValue) {                                                          \
        .switcher = Json##Type, __VA_OPT__(.body.Json##Type = __VA_ARGS__) \
    }

#endif  // JSON_H_
