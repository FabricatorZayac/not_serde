#include <ctype.h>
#include <stddef.h>
#include <stdint.h>

#include "deserialize.h"
#include "error.h"
#include "json.h"
#include "serialize.h"
#include "sumtype.h"

serde_struct(RGB,  //
             (r, uint8_t),
             (g, uint8_t),
             (b, uint8_t));
serde_struct(User,  //
             (is_human, bool),
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
        .is_human = true,
        .name = "Jeff",
        .age = 25,
        .color = &color,
        .ser = serialize_User,
    };

    Serializer ser = {.output = String_new()};
    serialize(&ser, &foo);

    printf("%s\n", ser.output.body);

    Deserializer de;
    {
        str x;
        de.input = "  \"Hello World\", asd";
        match(deserialize(&de, &x),
              of(Ok) printf("%.*s\n", x.size, x.body),
              of(Err, Error e) e.print(e));
    }
    {
        bool x;
        de.input = "   true, asd";
        match(deserialize(&de, &x),
              of(Ok) printf("%s\n", x ? "true" : "false"),
              of(Err, Error e) e.print(e));
    }
    {
        RGB x = {.de = deserialize_RGB};
        de.input = "{\"r\": 5, \"g\": 4, \"b\": 2}";
        match(deserialize(&de, &x),
              of(Ok) printf("{.r = %d, .g = %d, .b = %d}\n", x.r, x.g, x.b),
              of(Err, Error e) e.print(e));
    }
    {
        RGB y = {.de = deserialize_RGB};
        User x = {.color = &y, .de = deserialize_User};
        de.input =
            "{\"is_human\":true,\"name\":\"Jeff\",\"age\":25,\"color\":{\"r\":20,\"g\":150,\"b\":30}}";
        match(
            deserialize(&de, &x),
            of(Ok) printf(
                "{.is_human = %s, .name = \"%s\", .age = %d, .color = {.r = %d, .g = %d, .b = %d}}\n",
                x.is_human ? "true" : "false",
                x.name,
                x.age,
                x.color->r,
                x.color->g,
                x.color->b),
            of(Err, Error e) e.print(e));
    }

    ser.output.impl.destroy(&ser.output);
    return 0;
}
