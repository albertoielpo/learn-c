#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Example
 * subnet: FFFFFF00 (255.255.255.0)
 * @param label
 * @param ip
 */
static void print_ip(const char *label, uint32_t ip) {
    printf("%s: %0X (%d.%d.%d.%d)\n", label, ip,
           (ip >> 24) & 0xFF,
           (ip >> 16) & 0xFF,
           (ip >> 8) & 0xFF,
           ip & 0xFF);
}

static uint32_t ip_to_32(char *ip_s) {
    int a = atoi(strtok(ip_s, "."));
    int b = atoi(strtok(NULL, "."));
    int c = atoi(strtok(NULL, "."));
    int d = atoi(strtok(NULL, "."));

    return ((uint32_t)a << 24) | (b << 16) | (c << 8) | d;
}

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <cidr>\n", argv[0]);
        return 1;
    }

    int a, b, c, d, mask;
    if (sscanf(argv[1], "%d.%d.%d.%d/%d", &a, &b, &c, &d, &mask) != 5) {
        fprintf(stderr, "Invalid CIDR format. Expected: A.B.C.D/N\n");
        return 1;
    }
    if (a < 0 || a > 255 || b < 0 || b > 255 ||
        c < 0 || c > 255 || d < 0 || d > 255) {
        fprintf(stderr, "Octets must be 0-255\n");
        return 1;
    }
    if (mask < 0 || mask > 32) {
        fprintf(stderr, "Mask must be 0-32\n");
        return 1;
    }

    printf("cidr: %s\n", argv[1]);

    uint32_t subnet = 0xFFFFFFFF << (32 - mask);
    print_ip("subnet", subnet);

    uint32_t network_addr = ((uint32_t)a << 24) | (b << 16) | (c << 8) | d;
    uint32_t hosts_from = network_addr + 1;
    uint32_t broadcast = network_addr | ~subnet;
    uint32_t hosts_to = broadcast - 1;

    print_ip("network addr", network_addr);
    print_ip("hosts from", hosts_from);
    print_ip("hosts to", hosts_to);
    print_ip("broadcast", broadcast);

    return 0;
}
