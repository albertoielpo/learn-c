#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct
{
    char *name;
    int sleep_seconds;
} FooParam;

void *foo(void *param)
{
    FooParam *num = (FooParam *)(param);
    sleep(num->sleep_seconds); // sleep in seconds
    printf("Thread %s that sleep %d seconds\n", num->name, num->sleep_seconds);
    // always return a void*. Return NULL in case of no value
    return NULL;
}

int main(void)
{

    // Result is ...
    // This is the main thread
    // Thread thread 2 that sleep 1 seconds
    // Thread thread 1 that sleep 2 seconds

    // init threads
    pthread_t thread1;
    pthread_t thread2;

    // assign in the stack FooParam with name define in the static memory
    FooParam foo_param_1 = {"thread 1", 2};
    FooParam foo_param_2 = {"thread 2", 1};

    // Creating a thread and execute it
    pthread_create(&thread1, NULL, foo, (void *)&foo_param_1);
    pthread_create(&thread2, NULL, foo, (void *)&foo_param_2);

    printf("This is the main thread\n");

    // pause the execution of the main thread waiting for child threads to join
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}
