
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

enum
{
    MAX_SIZE = 1000000,
    MAX_LEN = 32,
    START_MEM = 8,
};

typedef struct Type
{
    char name[MAX_LEN];
    size_t size, allign;
} Type;


void
find_type(Type *types, int arr_size, char *type_name, size_t *size, size_t *allign)
{
    *size = 0;
    *allign = 0;

    for (int i = 0; i < arr_size; ++i) {
        if (strcmp(type_name, types[i].name) == 0) {
            *size = types[i].size;
            *allign = types[i].allign;

            break;
        }
    }
}

void
end(Type *types)
{
    free(types);
    puts("0 0");
}

int
main(void)
{
    int used = 0;
    int alloc = START_MEM;
    Type *types = calloc(START_MEM, sizeof(types[0]));

    Type buf;
    while (scanf("%s%zu%zu", buf.name, &buf.size, &buf.allign) == 3) {
        if (used >= alloc) {
            types = realloc(types, (alloc *= 2) * sizeof(types[0]));
        }

        types[used++] = buf;
    }

    size_t size = 0;
    size_t allign = 1;

    char name[MAX_LEN];
    size_t count;
    while (scanf("%s%zu", name, &count) == 2) {
        size_t cur_size, cur_allign;
        find_type(types, used, name, &cur_size, &cur_allign);

        size_t extra = size % cur_allign;
        if (extra && __builtin_add_overflow(size, cur_allign - extra, &size)) {
            end(types);
            return 0;
        }

        size_t tmp;
        if (__builtin_mul_overflow(cur_size, count, &tmp) || 
                __builtin_add_overflow(size, tmp, &size)) {
            end(types);
            return 0;
        }

        if (allign < cur_allign) {
            allign = cur_allign;
        }
    }

    size_t extra = size % allign;
    if (extra && __builtin_add_overflow(size, allign - extra, &size)) {
        end(types);
        return 0;
    }

    if (size == 0 || allign == 0) {
        size = 1;
        allign = 1;
    }

    printf("%zu %zu\n", size, allign);

    free(types);
}

