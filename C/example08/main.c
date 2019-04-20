#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

enum
{
    MAX_SIZE = 1024,
    MAX_GROUPS = 32
};

enum
{
    BIT_PERM = 3,
    OCT_MASK = (1 << BIT_PERM) - 1
};

enum
{
    USER_PERMS = 2,
    GROUP_PERMS = 1,
    OTHER_PERMS = 0
};


int
find_gid(const unsigned *pgid, size_t size, unsigned gid)
{
    for (int i = 0; i < size; ++i) {
        if (pgid[i] == gid) {
            return 1;
        }
    }

    return 0;
}

int
main(void)
{
    unsigned puid;
    scanf("%u\n", &puid);

    char buf[MAX_SIZE];
    fgets(buf, sizeof(buf), stdin);

    size_t numg = 0;
    unsigned pgid[MAX_GROUPS];

    char *p = buf;
    char *check;

    errno = 0;
    unsigned cur = strtol(p, &check, 10);
    for (numg = 0; !errno && *p; ++numg) {
        pgid[numg] = cur;
        p = check;

        cur = strtol(p, &check, 10);
    }


    unsigned pperms;
    scanf("%o\n", &pperms);

    while (fgets(buf, sizeof(buf), stdin)) {
        unsigned uid, gid, perms;
        char name[MAX_SIZE];

        int read_end;
        sscanf(buf, "%u%u%o%n", &uid, &gid, &perms, &read_end);

        int str_beg;
        for (str_beg = read_end; buf[str_beg]; ++str_beg) {
            if (!isspace(buf[str_beg])) {
                break;
            }
        }

        strcpy(name, buf + str_beg);
        for (int i = (int) strlen(name) - 1; i >= 0 && isspace(name[i]); --i) {
            name[i] = 0;
        }

        unsigned oct_perms = 0;
        if (uid == puid) {
            oct_perms = (perms >> (USER_PERMS * BIT_PERM)) & OCT_MASK;
        } else if (find_gid(pgid, numg, gid)) {
            oct_perms = (perms >> (GROUP_PERMS * BIT_PERM)) & OCT_MASK;
        } else {
            oct_perms = (perms >> (OTHER_PERMS * BIT_PERM)) & OCT_MASK;
        }

        if ((oct_perms | pperms) == oct_perms) {
            printf("%s\n", name);
        }
    }
}