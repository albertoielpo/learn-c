#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main(void)
{
  uint8_t num = 10;
  printf("stack allocation: %u ---> %p\n", num, &num); // stack address

  uint8_t num2 = 150;
  printf("stack allocation: %u ---> %p\n", num2, &num2); // stack address + 1 byte

  // here can be a padding and free space
  char str[] = {"hello world"};
  printf("stack allocation: %s ---> %p\n", str, str); // stack address
  // here it frees the variable str

  char *str2 = "hello world";
  printf("static allocation: %s ---> %p\n", str2, str2); // static memory address

  // malloc allocate also metadata so not just 12*1 byte
  char *str3 = malloc(sizeof(char) * 12);
  memcpy(str3, "hello world", 12);
  printf("heap allocation: %s ---> %p\n", str3, str3); // heap memory address

  return 0;
}
