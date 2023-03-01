#include "json.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "deserialize.h"
#include "error.h"
#include "result.h"
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

////////////////////////////////////////////////////////////////////////////////

ResChar peek_char(Deserializer *self) {
    if (self->input[0] == '\0') {
        return Err(ResChar, Error("EOF"));
    }
    return Ok(ResChar, self->input[0]);
}

ResChar next_char(Deserializer *self) {
    char ch;
    TRY(ResChar, peek_char(self), ch);
    self->input++;
    return Ok(ResChar, ch);
}

ResBool parse_bool(Deserializer *self) {
    if (strstr(self->input, "true") == self->input) {
        self->input = self->input + strlen("true");
        return Ok(ResBool, true);
    } else if (strstr(self->input, "false") == self->input) {
        self->input = self->input + strlen("false");
        return Ok(ResBool, false);
    }
    return Err(ResBool, Error("Expected boolean"));
}

ResUint parse_unsigned(Deserializer *self) {
    char ch;
    do
        TRY(ResUint, next_char(self), ch)
    while (isspace(ch));
    if (ch <= '0' || ch >= '9') {
        return Err(ResUint, Error("Expected unsigned integer"));
    }
    uint64_t res = ch - '0';
    while (true) {
        ch = peek_char(self).body.Ok;
        if (ch >= '0' && ch <= '9') {
            TRY(ResUint, next_char(self));
            res *= 10;
            res += ch - '0';
        } else {
            return Ok(ResUint, res);
        }
    }
}

ResInt parse_signed(Deserializer *self) {
    char ch;
    do
        TRY(ResInt, next_char(self), ch)
    while (isspace(ch));
    if ((ch <= '0' || ch >= '9') && ch != '-') {
        return Err(ResInt, Error("Expected integer"));
    }
    int64_t res;
    TRY(ResInt, parse_unsigned(self), res);
    if (ch == '-') res = -res;
    return Ok(ResInt, res);
}

// TODO: escape sequences
ResStr parse_str(Deserializer *self) {
    char ch;
    do
        TRY(ResStr, next_char(self), ch)
    while (isspace(ch));
    if (ch != '"') {
        return Err(ResStr, Error("Expected string"));
    }
    char *end = strchr(self->input, '"');
    if (end == NULL) {
        return Err(ResStr, Error("Unterminated string"));
    }
    str res = {self->input, end - self->input};
    self->input = end + 1;
    return Ok(ResStr, res);
}
