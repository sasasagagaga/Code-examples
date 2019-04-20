
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

enum
{
    CNT_RAND_SYMB = 10
};

char
get_rand_char(int fd, char a, char b)
{
    unsigned rnd;
    read(fd, &rnd, sizeof(rnd));

    double r = (rnd / (UINT_MAX + 1.));
    return (char) (r * (b - a)) + a;
}

int
main(int argc, char *argv[])
{
    char *path;
    if (getenv("XDG_RUNTIME_DIR")) {
        path = strdup(getenv("XDG_RUNTIME_DIR"));
    } else if (getenv("TMPDIR")) {
        path = strdup(getenv("TMPDIR"));
    } else {
        path = strdup("/tmp");
    }

    char filename[PATH_MAX];
    int fd;

    int rand_fd = open("/dev/urandom", O_RDONLY, 0);

    do {
        char rand_part[CNT_RAND_SYMB + 1];
        for (int i = 0; i < CNT_RAND_SYMB; ++i) {
            rand_part[i] = get_rand_char(rand_fd, 'a', 'z');
        }
        rand_part[CNT_RAND_SYMB] = 0;

        int fnlen = snprintf(filename, sizeof(filename), "%s/%d%s.py", path, getpid(), rand_part);
        filename[fnlen + CNT_RAND_SYMB] = 0;

        fd = open(filename, O_WRONLY | O_CREAT | O_EXCL, 0700);
    } while (fd < 0);

    FILE *f = fdopen(fd, "w");

    close(rand_fd);
    free(path);

    fprintf(f, "#! /usr/bin/python -E\n");
    fprintf(f, "from os import remove\n");
    fprintf(f, "from sys import argv\n");

    fprintf(f, "print(%s", argv[1]);
    for (int i = 2; argv[i]; ++i) {
        fprintf(f, "*%s", argv[i]);
    }
    fprintf(f, ")\n");

    fprintf(f, "remove(argv[0])\n");

    fclose(f);

    execlp(filename, filename, NULL);
}

