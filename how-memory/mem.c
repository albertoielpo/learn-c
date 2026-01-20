/*
 * Memory Layout Visualization - mem.c
 * ====================================
 *
 *     HIGH MEMORY (0x7fff...)
 *     ┌─────────────────────────────────────────────────────┐
 *     │               STACK SEGMENT                         │
 *     │           (grows downward ↓)                        │
 *     ├─────────────────────────────────────────────────────┤
 *     │  main() stack frame:                                │
 *     │                                                     │
 *     │  0x7fffffffd8cc                                     │
 *     │  ┌───────────────────────────────────────────────┐  │
 *     │  │ str[] = "hello world"                         │  │
 *     │  └───────────────────────────────────────────────┘  │
 *     │                                                     │
 *     │  0x7fffffffd8b7                                     │
 *     │  ┌───────────────────────────────────────────────┐  │
 *     │  │ num2 = 150                                    │  │
 *     │  └───────────────────────────────────────────────┘  │
 *     │                                                     │
 *     │  0x7fffffffd8b6                                     │
 *     │  ┌───────────────────────────────────────────────┐  │
 *     │  │ num = 10                                      │  │
 *     │  └───────────────────────────────────────────────┘  │
 *     ├─────────────────────────────────────────────────────┤
 *     │  fone(1) stack frame:   n = 1                       │
 *     │  0x7fffffffd89c  &n ────┐                           │
 *     ├─────────────────────────┼───────────────────────────┤
 *     │  fone(2) stack frame:   │  n = 2                    │
 *     │  0x7fffffffd87c  &n ────┤  ↓ Stack growing down     │
 *     ├─────────────────────────┼───────────────────────────┤
 *     │  fone(3) stack frame:   │  n = 3                    │
 *     │  0x7fffffffd85c  &n ────┤  ↓ (0x20 bytes/frame)     │
 *     ├─────────────────────────┼───────────────────────────┤
 *     │  fone(4) stack frame:   │  n = 4 (deepest)          │
 *     │  0x7fffffffd83c  &n ────┤  ↓                        │
 *     ├─────────────────────────┼───────────────────────────┤
 *     │  (available stack space)                            │
 *     ↓                                                     ↓
 *
 *            ⋮  (unmapped region)  ⋮
 *
 *     ┌─────────────────────────────────────────────────────┐
 *     │               HEAP SEGMENT                          │
 *     │           (grows upward ↑)                          │
 *     ├─────────────────────────────────────────────────────┤
 *     │  0x5555555596b0                                     │
 *     │  ┌───────────────────────────────────────────────┐  │
 *     │  │ str3 = "hello world" (malloc'd + metadata)    │  │
 *     │  └───────────────────────────────────────────────┘  │
 *     │                                                     │
 *     └─────────────────────────────────────────────────────┘
 *
 *            ⋮  (unmapped region)  ⋮
 *
 *     ┌─────────────────────────────────────────────────────┐
 *     │          DATA/RODATA SEGMENT                        │
 *     │      (static/global variables & constants)          │
 *     ├─────────────────────────────────────────────────────┤
 *     │  0x55555555604c                                     │
 *     │  ┌───────────────────────────────────────────────┐  │
 *     │  │ str2 -> "hello world" (string literal)        │  │
 *     │  └───────────────────────────────────────────────┘  │
 *     │                                                     │
 *     └─────────────────────────────────────────────────────┘
 *
 *     ┌─────────────────────────────────────────────────────┐
 *     │             TEXT SEGMENT                            │
 *     │         (executable code: main, fone)               │
 *     └─────────────────────────────────────────────────────┘
 *     LOW MEMORY (0x5555...)
 *
 * Observations:
 * -------------
 * 1. Stack grows DOWN: Each fone() call allocates frame at LOWER address
 *    - Frame spacing: 0x20 (32) bytes between each call
 *    - Deepest call (n=11) at lowest address: 0x7fffffffd75c
 *
 * 2. Within main() frame: Variables appear at increasing addresses
 *    - This is layout within ONE frame, not stack growth direction
 *
 * 3. Heap vs Stack: ~0x2aaa... address space separation
 *    - Heap grows up from low addresses
 *    - Stack grows down from high addresses
 *    - Maximizes available memory before collision
 *
 * 4. Static data in read-only region (0x5555...)
 *    - String literals are immutable
 *    - Shared across entire program lifetime
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * recursive example
 * 4 0x7fffffffd83c
 * 3 0x7fffffffd85c
 * 2 0x7fffffffd87c
 * 1 0x7fffffffd89c
 */
void fone(int n) {
    if (n > 3)
        return;
    fone(++n);
    printf("%d %p\n", n, &n);
}

int main(void) {
    uint8_t num = 10;
    printf("stack allocation: %u ---> %p\n", num, &num); // stack address

    uint8_t num2 = 150;
    printf("stack allocation: %u ---> %p\n", num2, &num2); // stack address + 1 byte

    // here can be a padding and free space
    char str[] = {"hello world"};
    printf("stack allocation: %s ---> %p\n", str, str); // stack address

    char *str2 = "hello world";
    printf("static allocation: %s ---> %p\n", str2, str2); // static memory address

    // malloc allocate also metadata so not just 12*1 byte
    char *str3 = malloc(sizeof(char) * 12);
    memcpy(str3, "hello world", 12);
    printf("heap allocation: %s ---> %p\n", str3, str3); // heap memory address

    // recursive function stack allocation example
    fone(0);

    return 0;
}
