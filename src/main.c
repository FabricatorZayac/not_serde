#include <stddef.h>
#include <stdint.h>

#include "error.h"
#include "json.h"
#include "serialize.h"

serde_struct(RGB, (r, uint8_t), (g, uint8_t), (b, uint8_t));
serde_struct(User, (name, char *), (age, int), (color, RGB *));

int main() {
    RGB *color = &(RGB) {
        .r = 20,
        .g = 150,
        .b = 30,
        .ser = serialize_RGB,
    };
    User *foo = &(User) {
        .name = "Jeff",
        .age = 25,
        .color = color,
        .ser = serialize_User,
    };

    Serializer ser = {.output = String_new()};

    serialize(&ser, foo, void);
    /* serialize(&ser, 5.6); */
    /* serialize(&ser, 200); */
    /* serialize(&ser, (bool)true); */
    /* serialize(&ser, "Hello, World!"); */
    /* serialize(&ser, (char)'u'); */

    printf("%s", ser.output.body);

    return 0;
}
