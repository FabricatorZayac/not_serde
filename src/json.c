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
#define ser_primitive_impl(T, method_name)                   \
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

ser_primitive_impl(int8_t, serialize_int8);
ser_primitive_impl(int16_t, serialize_int16);
ser_primitive_impl(int32_t, serialize_int32);
ser_primitive_impl(int64_t, serialize_int64);

ser_primitive_impl(uint8_t, serialize_uint8);
ser_primitive_impl(uint16_t, serialize_uint16);
ser_primitive_impl(uint32_t, serialize_uint32);
ser_primitive_impl(uint64_t, serialize_uint64);

ser_primitive_impl(float, serialize_float);
ser_primitive_impl(double, serialize_double);

ser_primitive_impl(char, serialize_char);

SerResult serialize_bool(Serializer *self, bool v) {
    char buffer[BUFSIZ];
    sprintf(buffer, "%s", v ? "true" : "false");
    String_append_slice(&self->output, buffer);
    return Ok(SerResult, {});
}

SerResult serialize_cstr(Serializer *self, char *v) {
    char buffer[BUFSIZ];
    sprintf(buffer, "\"%s\"", v);
    String_append_slice(&self->output, buffer);
    return Ok(SerResult, {});
}

SerResult serialize_str(Serializer *self, str v) {
    char buffer[BUFSIZ];
    sprintf(buffer, "\"%.*s\"", v.size, v.body);
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
    char ch;
    do
        TRY(ResBool, next_char(self), ch)
    while (isspace(ch));

    if (strncmp(self->input - 1, "true", 4) == 0) {
        self->input = self->input + strlen("true");
        return Ok(ResBool, true);
    } else if (strncmp(self->input - 1, "false", 5) == 0) {
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
    if (ch != '-') self->input--;
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

////////////////////////////////////////////////////////////////////////////////

#define de_primitive_impl(T, parse_method)                     \
    DeResult deserialize_##T(Deserializer *self, T *visitor) { \
        TRY(DeResult, parse_method(self), *visitor);           \
        return Ok(DeResult, {});                               \
    }

de_primitive_impl(bool, parse_bool);
de_primitive_impl(uint8_t, parse_unsigned);
de_primitive_impl(uint16_t, parse_unsigned);
de_primitive_impl(uint32_t, parse_unsigned);
de_primitive_impl(uint64_t, parse_unsigned);
de_primitive_impl(int8_t, parse_signed);
de_primitive_impl(int16_t, parse_signed);
de_primitive_impl(int32_t, parse_signed);
de_primitive_impl(int64_t, parse_signed);
de_primitive_impl(str, parse_str);
de_primitive_impl(char, next_char);

DeResult deserialize_default(Deserializer *self, void *visitor) {
    struct void_struct {
        SerResult (*ser)(Serializer *, struct void_struct *);
        DeResult (*de)(Deserializer *, struct void_struct *);
    } *value = visitor;
    return value->de(self, visitor);
}
