
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>

enum
{
    START_MEM = 8
};

enum
{
    BORDER_CODE = 0x20
};

typedef int(*compar)(const void *, const void *, void *);

typedef struct Vector {
    wchar_t *data;
    int used, alloc;
} Vector;


int
comp(const int *i, const int *j, Vector *vec)
{
    return wcsncmp(vec->data + *i, vec->data + *j, vec->alloc / 2);
}

int
main(void)
{
    setlocale(LC_ALL, "");

    Vector vec = { calloc(START_MEM, sizeof(vec.data[0])), 0, START_MEM };

    wint_t wc;
    while ((wc = getwchar()) != WEOF) {
        if (wc <= BORDER_CODE) {
            continue;
        }

        if (vec.used >= vec.alloc) {
            wchar_t *tmp = vec.data;
            vec.data = realloc(vec.data, (vec.alloc *= 2) * sizeof(vec.data[0]));

            if (!vec.data) {
                free(tmp);
                return -1;
            }
        }

        vec.data[vec.used++] = wc;
    }

    int n = vec.used;
    if (!n) {
        putwchar(L'\n');
        free(vec.data);

        return 0;
    }

    vec.used = vec.alloc = 2 * n;
    
    wchar_t *tmp = vec.data;
    vec.data = realloc(vec.data, vec.alloc * sizeof(vec.data[0]));
    
    if (!vec.data) {
        free(tmp);
        return -1;
    }

    wcsncpy(vec.data + n, vec.data, n);

    int *p = calloc(n, sizeof(p[0]));
    for (int i = 0; i < n; ++i) {
        p[i] = i;
    }

    qsort_r(p, n, sizeof(p[0]), (compar) comp, &vec);

    for (int i = 0; i < n; ++i) {
        putwchar(vec.data[(p[i] + n - 1) % n]);
    }
    putwchar(L'\n');

    free(vec.data);
    free(p);
}
