#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// write and read data using files

typedef struct
{
    uint32_t id;
    char value[50]; // 49+1 always allocated
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

int main(void)
{

    char *file_dat = "fss.dat";
    char *file_txt = "fss.txt";

    uint8_t products_length = 2;

    // stack allocation
    // Product products[] = {
    //     {1, "this is my string"},
    //     {2, "this is another string"}};

    // heap allocation
    Product *products = malloc(sizeof(Product) * products_length);
    products[0].id = 1;
    strcpy(products[0].value, "this is my string");

    products[1].id = 2;
    strcpy(products[1].value, "this is another string");

    uint8_t res = write_products(file_dat, products, products_length);
    uint8_t res2 = write_products_text(file_txt, products, products_length);

    // deallocate products
    free(products);

    // in dat format is writen 116 bytes because is fixed length
    // -rw-rw-r--  1 alberto alberto   116 Jun 12 11:04 fss.dat
    // -rw-rw-r--  1 alberto alberto    42 Jun 12 11:04 fss.txt

    if (res != 0 || res2 != 0)
    {
        printf("write_products in error");
        return 1;
    }

    // read from file
    uint32_t total = 0;
    Product *products_res = read_products(file_dat, &total);
    printf("Read from file %d products\n", total);
    for (uint32_t ii = 0; ii < total; ii++)
    {
        printf("id %d, name %s\n", products_res[ii].id, products_res[ii].value);
    }

    free(products_res);

    // read from file text
    uint32_t total_text = 0;
    Product *products_res_text = read_products(file_dat, &total_text);
    printf("Read from file text %d products\n", total_text);
    for (uint32_t ii = 0; ii < total_text; ii++)
    {
        printf("id %d, name %s\n", products_res_text[ii].id, products_res_text[ii].value);
    }

    free(products_res_text);

    return 0;
}

uint8_t close_file(FILE *file)
{
    if (fclose(file) == EOF)
    {
        printf("Cannot close file");
        return 1;
    }
    return 0;
}

/**
 * Return 0 if all good
 * Return 1 if errors
 */
uint8_t write_products(char *filename, Product *products, uint32_t total)
{
    FILE *file;
    file = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("Cannot open file");
        return 1;
    }

    if (fwrite(&total, sizeof(int), 1, file) != 1)
    {
        printf("cannot write total to file");
        fclose(file);
        return 1;
    }

    if (fwrite(products, sizeof(Product), total, file) != total)
    {
        printf("cannot write products to file");
        close_file(file);
        return 1;
    }

    return close_file(file);
}

uint8_t write_products_text(char *filename, Product *products, uint32_t total)
{
    FILE *file;
    file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Cannot open file");
        return 1;
    }

    if (fprintf(file, "%d", total) < 0)
    {
        printf("cannot write total to file");
        fclose(file);
        return 1;
    }
    for (uint32_t ii = 0; ii < total; ii++)
    {
        if (fprintf(file, "%d%s", products[ii].id, products[ii].value) < 0)
        {
            printf("cannot write products to file");
            fclose(file);
            return 1;
        }
    }

    return close_file(file);
}

Product *read_products(char *filename, uint32_t *total)
{
    FILE *file;
    file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Cannot open file");
        return NULL;
    }

    if (fread(total, sizeof(int), 1, file) != 1)
    {
        printf("Cannot read file");
        close_file(file);
        return NULL;
    }

    Product *products = malloc(sizeof(Product) * *total);
    if (fread(products, sizeof(Product), *total, file) != *total)
    {
        printf("Cannot read products");
        close_file(file);
        return NULL;
    }

    if (close_file(file) != 0)
    {
        printf("Cannot close file");
    }
    return products;
}