#include <stdio.h>
#include <time.h>

/*
 * snprintf writes at most sizeof(buffer) bytes (including the trailing '\0')
 */
void *date_human(char *buffer, size_t buffer_size, struct tm *tm_localtime)
{
    snprintf(buffer, buffer_size, "%d/%d/%d", tm_localtime->tm_mday, tm_localtime->tm_mon + 1, tm_localtime->tm_year + 1900);
}

int main(void)
{
    /* Seconds since the Epoch.  */
    time_t now = time(NULL);
    if (now == (time_t)(-1))
    {
        perror("time() failed");
        return 1;
    }
    printf("Unix timestamp in seconds: %ld\n", now);

    /* Convert to broken‐down local time */
    struct tm *tm_localtime = localtime(&now);
    if (tm_localtime == NULL)
    {
        perror("localtime");
        return 1;
    }

    // from 1749026918 into 4/6/2025
    // printf("%d/%d/%d\n", tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900);

    char buffer[11]; // xx/yy/zzzz + 1
    date_human(buffer, 11, tm_localtime);
    printf("Date human: %s\n", buffer);

    return 0;
}