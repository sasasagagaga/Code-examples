
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "plugin.h"

#define ERROR(...) do { \
        fprintf(stderr, __VA_ARGS__); \
        return ERR_CODE; \
        } while (0); \

typedef struct RandomFactory *(*random_factory_init)(void);

enum
{
    ERR_CODE = 1,
};

enum
{
    NAME_LEN = 64,
    COUNT_LOWER = 0,
    COUNT_UPPER = 1000,
    ARGS = 6
};

// return 1 if error and 0 otherwise
int
get_int(const char *s, int *x)
{
    if (!s) {
        return 1;
    }

    errno = 0;

    char *check;
    long num = strtol(s, &check, 10);
    *x = num;

    return errno || !s[0] || check[0] || num != *x;
}

// return 1 if error and 0 otherwise
int
check_range(int x, int lower_bound, int upper_bound)
{
    return !(lower_bound <= x && x <= upper_bound);
}

int
main(int argc, char *argv[])
{
    const char plugin_name_pref[] = "random_";
    const char plugin_name_suff[] = "_factory";
    
    if (argc != ARGS + 1) {
        ERROR("Amount of arguments is not %d\n", ARGS);
    }

    // Arguments parsing
    //   String args
    const char *file_name = argv[1];
    const char *NAME = argv[2];
    const char *new_instance_args = argv[3];

    if (!strlen(NAME)) {
        ERROR("Some string args are NULL or empty\n");
    }

    if (strlen(NAME) + strlen(plugin_name_pref) + strlen(plugin_name_suff) >= NAME_LEN) {
        ERROR("Length of NAME is out of range\n");
    }

    //   Integer args
    int count, a, b;

    if (get_int(argv[4], &count) || get_int(argv[5], &a) || get_int(argv[6], &b)) {
        ERROR("Some integer args (count, a, b) are not represented in int\n");
    }

    if (check_range(count, COUNT_LOWER, COUNT_UPPER)) {
        ERROR("Argument count is out of range\n");
    }

    if (a > b) {
        ERROR("a is greater then b\n");
    }

    // Main work
    void *handle = dlopen(file_name, RTLD_LAZY);
    if (!handle) {
        ERROR("%s\n", dlerror());
    }

    char plugin_name[NAME_LEN];
    snprintf(plugin_name, sizeof(plugin_name), "random_%s_factory", NAME);
    random_factory_init random_factory = dlsym(handle, plugin_name);
    if (!random_factory) {
        ERROR("%s\n", dlerror());
    }

    struct RandomFactory *factory = random_factory();
    if (!factory) {
        ERROR("Factory can't be created\n");
    }

    struct RandomGenerator *generator = factory->ops->new_instance(factory, new_instance_args);
    if (!generator) {
        ERROR("Generator can't be created\n");
    }

    for (int i = 0; i < count; ++i) {
        if (!generator) {
            ERROR("Strange error with generator occured\n");
        }
        printf("%d\n", generator->ops->next_int(generator, a, b));
    }

    if (!generator) {
        ERROR("Strange error with generator occured\n");
    }
    generator->ops->destroy(generator);

    if (!factory) {
        ERROR("Strange error with factory occured\n");
    }
    factory->ops->destroy(factory);

    if (dlclose(handle) < 0) {
        ERROR("%s\n", dlerror());
    }
}
