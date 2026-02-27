#include "../utils/ielp.h"
#include <stdint.h>
#include <stdio.h>

// scenario 1
typedef struct
{
    uint8_t open : 1;
    uint8_t opening : 1;
    uint8_t closing : 1;
    uint8_t closed : 1;
    uint8_t blocked : 1;
    uint8_t disabled : 1;
    uint8_t e1 : 1;
    uint8_t e2 : 1;
} MultiState;

// scenario 2
typedef uint8_t multi_state;

#define MS_OPEN (1u << 0)
#define MS_OPENING (1u << 1)
#define MS_CLOSING (1u << 2)
#define MS_CLOSED (1u << 3)
#define MS_BLOCKED (1u << 4)
#define MS_DISABLED (1u << 5)
#define MS_E1 (1u << 6)
#define MS_E2 (1u << 7)

int main(void) {
    MultiState x = {0};
    x.blocked = 1;
    x.disabled = 1;
    printf("%d\n", x.blocked);
    printf("%d\n", x.disabled);

    // ...
    multi_state y = MS_BLOCKED | MS_DISABLED;
    printf("%d\n", y);
    print_binary(y, 8);

    return 0;
}