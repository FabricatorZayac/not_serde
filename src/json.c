#include "json.h"

#include <string.h>

#include "sumtype.h"

// Option for nice clang formatting for _Generic is only available in clang16
// Meanwhile I have clang15 and I don't feel like building from source
// NOTE: maybe enable clang format when I get my hands on clang16
// clang-format off
#define primitive_impl(T, label, method_name)                   \
    SerResult method_name(Serializer *self, T v) {              \
        char buffer[BUFSIZ];                                    \
        sprintf(buffer,                                         \
                _Generic((v),                                   \
                         int8_t   : "%d",                       \
                         int16_t  : "%d",                       \
                         int32_t  : "%d",                       \
                         int64_t  : "%ld",                      \
                         uint8_t  : "%u",                       \
                         uint16_t : "%u",                       \
                         uint32_t : "%u",                       \
                         uint64_t : "%lu",                      \
                         float    : "%f",                       \
                         double   : "%f",                       \
                         char     : "%c"),                      \
                v);                                             \
        String_append_slice(&self->output, buffer);             \
        return Ok(SerResult, {});                               \
    }
// clang-format on

primitive_impl(int8_t, JsonNumber, serialize_int8_t);
primitive_impl(int16_t, JsonNumber, serialize_int16_t);
primitive_impl(int32_t, JsonNumber, serialize_int32_t);
primitive_impl(int64_t, JsonNumber, serialize_int64_t);

primitive_impl(uint8_t, JsonNumber, serialize_uint8_t);
primitive_impl(uint16_t, JsonNumber, serialize_uint16_t);
primitive_impl(uint32_t, JsonNumber, serialize_uint32_t);
primitive_impl(uint64_t, JsonNumber, serialize_uint64_t);

primitive_impl(float, JsonNumber, serialize_float);
primitive_impl(double, JsonNumber, serialize_double);

primitive_impl(char, JsonString, serialize_char);

SerResult serialize_bool(Serializer *self, bool v) {
    char buffer[BUFSIZ];
    sprintf(buffer, "%s", v ? "true" : "false");
    String_append_slice(&self->output, buffer);
    return Ok(SerResult, {});
}

SerResult serialize_str(Serializer *self, char *v) {
    char buffer[BUFSIZ];
    sprintf(buffer, "\"%s\"", v);
    String_append_slice(&self->output, buffer);
    return Ok(SerResult, {});
}

SerResult serialize_unit(Serializer *self) {
    char buffer[BUFSIZ];
    sprintf(buffer, "null");
    String_append_slice(&self->output, buffer);
    return Ok(SerResult, {});
}

// NOTE: unimplemented
// SerSeqResult serialize_seq(Serializer *self, OptSize len) {
//     char buffer[BUFSIZ];
//     sprintf(buffer, "[");
//     String value = String_from_arr(buffer, strlen(buffer));
//     self->output.append(&self->output, &value);
//     value.impl.destroy(&value);
//     return Ok(SerSeqResult, self);
// }

SerStructResult serialize_struct(Serializer *self, char *name, size_t len) {
    char buffer[BUFSIZ];
    sprintf(buffer, "{");
    String_append_slice(&self->output, buffer);
    return Ok(SerStructResult, self);
}

SerResult SerializeStruct_end(SerializeStruct *self) {
    char buffer[BUFSIZ];
    sprintf(buffer, "}");
    String_append_slice(&self->output, buffer);
    return Ok(SerResult, {});
}

SerResult serialize_default(Serializer *self, void *v) {
    struct void_struct {
        SerResult (*ser)(Serializer *, struct void_struct *);
    } *value = v;
    return value->ser(self, v);
}
