
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

int
main(int argc, char *argv[])
{
    int n = 10;

    if (argc > 1) {
        n = strtol(argv[1], NULL, 10);
    }

    for (int i = 0; i < n; ++i) {
        wchar_t wc = rand();

        putwchar(wc);
    }
}
