#include "../third-party/sds/sds.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// write and read data using files and sds library

typedef struct
{
    uint32_t id; // from 0 to 4294967295
    sds value;   // this is a pointer to a char[] managed by sds. Lenght is variable
} Product;

/**
 * Write products in bin format
 */
uint8_t write_products(char *filename, Product *products, uint32_t total);

/**
 * Write products in text format
 * The goal is to produce the same result of write_products exept than the file is human readable
 * No spaces, No logic
 */
uint8_t write_products_text(char *filename, Product *products, uint32_t total);

/**
 * Read products from file
 */
Product *read_products(char *filename, uint32_t *total);

int main(void) {

    char *file_dat = "fsds.dat";
    char *file_txt = "fsds.txt";

    uint8_t products_length = 2;

    // stack allocation
    // Product products[] = {
    //     {1, sdsnew("\ntuKRwhsPRHwyHsKzBfMgu5xsHM3ibN2q\nwsEsQedejoLd0Ijpw4VF34YYSPWRJSwH\nJLAn4KYdp2c1FBpQjx2aq8GTzl6PzXsa\nIkke4NQZDht7hcZORBgDYt5pk8eHFayK\nxLOwP90h7M5thBNIOTOk8uMbKHnqIKrG\n5P57C1ODEgPb7o8hWhFf9LgjhDZEaeQn\nywYDxLp3vwR2i9w0FsqIyAeHBZigpvgJ\nPPQ5y8VUf5pE6GAOWutdLuXAUypSgKcy\n9FoMhQsBnikq1qWx6EmPi3W7LV2VBQlQ\n2EnHiIHjXB2fvSz1TljHuWtoHhFwvwZZ\n")},
    //     {2, sdsnew("\n43011177008036462081163683831814\n43049371996655126951442175704007\n43212211589381182905954125163267\n84471639398539196193520520887294\n65121337443298857533283632753854\n08355087685632916468232062748783\n23425470602532722280233940753825\n45630213653587964361592150209465\n05189695881152884827165807635341\n298118848462063490156990651150ZZ\n")}};

    // heap allocation
    Product *products = malloc(sizeof(Product) * products_length);
    products[0].id = 4294967294U; // in dat are used 4 bytes (hexdump FE FF FF FF that is 0xFFFFFFFE or -2 complement) in txt are used 2 bytes (written as -2)
    products[0].value = sdsnew("\ntuKRwhsPRHwyHsKzBfMgu5xsHM3ibN2q\nwsEsQedejoLd0Ijpw4VF34YYSPWRJSwH\nJLAn4KYdp2c1FBpQjx2aq8GTzl6PzXsa\nIkke4NQZDht7hcZORBgDYt5pk8eHFayK\nxLOwP90h7M5thBNIOTOk8uMbKHnqIKrG\n5P57C1ODEgPb7o8hWhFf9LgjhDZEaeQn\nywYDxLp3vwR2i9w0FsqIyAeHBZigpvgJ\nPPQ5y8VUf5pE6GAOWutdLuXAUypSgKcy\n9FoMhQsBnikq1qWx6EmPi3W7LV2VBQlQ\n2EnHiIHjXB2fvSz1TljHuWtoHhFwvwZZ\n");
    products[1].id = 2147483647U; // in dat are used 4 bytes (hexdump FF FF FF 7F that is 0x7FFFFFFF or 2147483647) in txt are used 10 bytes
    products[1].value = sdsnew("\n43011177008036462081163683831814\n43049371996655126951442175704007\n43212211589381182905954125163267\n84471639398539196193520520887294\n65121337443298857533283632753854\n08355087685632916468232062748783\n23425470602532722280233940753825\n45630213653587964361592150209465\n05189695881152884827165807635341\n298118848462063490156990651150ZZ\n");

    printf("size of products %ld bytes that is not the actual size\n", sizeof(Product));

    uint8_t res = write_products(file_dat, products, products_length);
    uint8_t res2 = write_products_text(file_txt, products, products_length);

    // deallocate products
    for (uint8_t ii = 0; ii < products_length; ii++) {
        // deallocate sds string
        sdsfree(products[ii].value);
    }

    // deallocate products
    free(products);

    // -rw-rw-r--  1 alberto alberto   682 Jun 12 12:23 fsds.dat
    // -rw-rw-r--  1 alberto alberto   675 Jun 12 12:23 fsds.txt

    if (res != 0 || res2 != 0) {
        printf("write_products in error");
        return 1;
    }

    // read from file
    uint32_t total = 0;
    Product *products_res = read_products(file_dat, &total);
    printf("Read from file %d products\n", total);
    for (uint32_t ii = 0; ii < total; ii++) {
        printf("id %u, name %s\n", products_res[ii].id, products_res[ii].value);
    }

    free(products_res);

    // read from file text
    uint32_t total_text = 0;
    Product *products_res_text = read_products(file_dat, &total_text);
    printf("Read from file text %d products\n", total_text);
    for (uint32_t ii = 0; ii < total_text; ii++) {
        printf("id %u, name %s\n", products_res_text[ii].id, products_res_text[ii].value);
    }

    free(products_res_text);

    return 0;
}

uint8_t close_file(FILE *file) {
    if (fclose(file) == EOF) {
        printf("Cannot close file");
        return 1;
    }
    return 0;
}

uint8_t file_err(char *msg, FILE *file) {
    printf("%s", msg);
    close_file(file);
    return 1;
}

/**
 * Return 0 if all good
 * Return 1 if errors
 */
uint8_t write_products(char *filename, Product *products, uint32_t total) {
    FILE *file;
    file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Cannot open file");
        return 1;
    }

    if (fwrite(&total, sizeof(int), 1, file) != 1) {
        return file_err("cannot write total to file", file);
    }

    // sizeof(Product) here cannot be used because value is just the pointer
    // the following code is WRONG !!!
    // if (fwrite(products, sizeof(Product), total, file) != total)
    // {
    //     printf("cannot write products to file");
    //     close_file(file);
    //     return 1;
    // }

    // here the correct implementation
    for (uint32_t ii = 0; ii < total; ii++) {
        Product *p = &products[ii];
        uint32_t id = p->id;
        uint32_t len = (uint32_t)sdslen(p->value);

        // write the ID
        if (fwrite(&id, sizeof(uint32_t), 1, file) != 1) {
            return file_err("cannot write id to file", file);
        }
        // write the string length
        if (fwrite(&len, sizeof(uint32_t), 1, file) != 1) {
            return file_err("cannot write str len to file", file);
        }
        // write the string bytes (no trailing ‘\0’)
        if (fwrite(p->value, sizeof(char), len, file) != len) {
            return file_err("cannot write str to file", file);
        }
    }

    return close_file(file);
}

uint8_t write_products_text(char *filename, Product *products, uint32_t total) {
    FILE *file;
    file = fopen(filename, "w");
    if (file == NULL) {
        printf("Cannot open file");
        return 1;
    }

    if (fprintf(file, "%d", total) < 0) {
        return file_err("cannot write total to file", file);
    }
    for (uint32_t ii = 0; ii < total; ii++) {
        // here .value resolve automatically the pointer and write the actual value
        if (fprintf(file, "%d%s", products[ii].id, products[ii].value) < 0) {
            return file_err("cannot write products to file", file);
        }
    }

    return close_file(file);
}

Product *read_products(char *filename, uint32_t *total) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Cannot open file");
        return NULL;
    }

    // Read count
    uint32_t count;
    if (fread(&count, sizeof(uint32_t), 1, file) != 1) {
        file_err("cannot read products", file);
        return NULL;
    }

    Product *products = malloc(count * sizeof(Product));

    for (uint32_t i = 0; i < count; i++) {
        uint32_t id, len;
        if (fread(&id, sizeof(uint32_t), 1, file) != 1) {
            file_err("cannot read id", file);
            return NULL;
        }
        if (fread(&len, sizeof(uint32_t), 1, file) != 1) {
            file_err("cannot read len", file);
            return NULL;
        }

        // allocate an sds string of exactly 'len' bytes
        sds str = sdsnewlen(NULL, len);
        if (fread(str, sizeof(char), len, file) != len) {
            file_err("cannot read len", file);
            return NULL;
        }

        products[i].id = id;
        products[i].value = str;
    }

    close_file(file);
    *total = count;
    return products;
}
