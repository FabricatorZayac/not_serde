#include <stddef.h>
#include <stdint.h>

#include "error.h"
#include "json.h"
#include "serialize.h"
#include "sumtype.h"

serde_struct(RGB,  //
             (r, uint8_t),
             (g, uint8_t),
             (b, uint8_t));
serde_struct(User,  //
             (name, char *),
             (age, int),
             (color, RGB *));

int main() {
    RGB color = {
        .r = 20,
        .g = 150,
        .b = 30,
        .ser = serialize_RGB,
    };
    User foo = {
        .name = "Jeff",
        .age = 25,
        .color = &color,
        .ser = serialize_User,
    };

    Serializer ser = {.output = String_new()};
    serialize(&ser, &foo);

    printf("%s\n", ser.output.body);

    Deserializer de = {.input = "  654, asd"};
    match(parse_unsigned(&de),
          of(Ok, int x) printf("%d\n", x),
          of(Err, Error e) e.print(e););

    de.input = " -542, asd";
    match(parse_signed(&de),
          of(Ok, int x) printf("%d\n", x),
          of(Err, Error e) e.print(e););

    ser.output.impl.destroy(&ser.output);

    de.input = "  \"Hello World\", asd";
    match(parse_str(&de),
          of(Ok, str x) printf("%.*s\n", x.size, x.body),
          of(Err, Error e) e.print(e););

    return 0;
}
