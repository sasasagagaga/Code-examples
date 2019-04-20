
#include <stdio.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

struct MyBuf
{
    long mtype;
    long long x1;
    long long x2;
};

int
main(int argc, char *argv[])
{
    key_t key = strtoul(argv[1], NULL, 10);
    int n = strtol(argv[2], NULL, 10);
    long long val1 = strtoll(argv[3], NULL, 10);
    long long val2 = strtoll(argv[4], NULL, 10);
    long long maxval = strtoll(argv[5], NULL, 10);

    int *pids = calloc(n, sizeof(pids[0]));

    int msgid = msgget(key, 0600 | IPC_CREAT | IPC_EXCL);

    for (int i = 1; i <= n; ++i) {
        int pid = fork();
        if (pid < 0) {
            for (int j = 0; j < i - 1; ++j) {
                kill(pids[j], SIGKILL);
            }

            for (int j = 0; j < i - 1; ++j) {
                wait(NULL);
            }

            free(pids);
            msgctl(msgid, IPC_RMID, NULL);
            return 1;
        }
        if (pid == 0) {
            while (1) {
                struct MyBuf rv;
                msgrcv(msgid, &rv, sizeof(struct MyBuf) - sizeof(long), i, 0);

                long long x3 = rv.x1 + rv.x2;
                printf("%d %lld\n", i - 1, x3);
                fflush(stdout);

                if ((x3 > 0 ? x3 : -x3) > maxval) {
                    break;
                }

                rv.x1 = rv.x2;
                rv.x2 = x3;
                rv.mtype = x3 % n + 1;
                msgsnd(msgid, &rv, sizeof(struct MyBuf) - sizeof(long), 0);
            }
            _exit(0);
        }

        pids[i - 1] = pid;
    }

    msgsnd(msgid, &(struct MyBuf){ 1, val1, val2 }, sizeof(struct MyBuf) - sizeof(long), 0);

    wait(NULL);
    for (int i = 0; i < n; ++i) {
        kill(pids[i], SIGKILL);
    }

    for (int i = 0; i < n - 1; ++i) {
        wait(NULL);
    }

    free(pids);
    msgctl(msgid, IPC_RMID, NULL);
}

