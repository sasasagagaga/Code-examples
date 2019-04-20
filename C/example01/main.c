
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>

enum
{
    KEY_PROC = 0x8BADF00D,
    KEY_ARR = 0xBADCAFE,
    KEY_LEN = 0xDEADBEEF,
    KEY_OFF = 0xD15EA5E,
};

typedef struct Array
{
    long mtype;
    int i;  //   index in offset and length arrays
} Array;

typedef struct FromProc
{
    long mtype;
    int i;
} FromProc;

typedef struct ToProc
{
    long mtype;
    int i1;
    int i2;
    int new_i;
    int woff;
} ToProc;


int
min(int a, int b)
{
    return a < b ? a : b;
}

//   roff - read offset, woff - write offset; return length of resulting array
int
intersect(int *arrs, int roff1, int len1, int roff2, int len2, int woff)
{
    int cnt_inter = 0;
    for (int i = 0, j = 0; i < len1 && j < len2;) {
        int a = arrs[roff1 + i];
        int b = arrs[roff2 + j];

        if (a == b) {
            arrs[woff + cnt_inter] = a;
            ++cnt_inter;
        }

        if (a <= b) {
            ++i;
        }
        if (a >= b) {
            ++j;
        }
    }

    return cnt_inter;
}

int
new_process(int id, int msg_proc_id, int msg_arr_id, int main_proc, 
        int *arrs, int *offset, int *length)
{
    int pid = fork();
    if (pid == 0) {
        //   Sending message about process to main process
        msgsnd(msg_proc_id, &(FromProc){ main_proc, id }, 
                sizeof(FromProc) - sizeof(long), 0);

        while (1) {
            ToProc proc;
            msgrcv(msg_proc_id, &proc, sizeof(proc) - sizeof(long), id, 0);

            int i1 = proc.i1;
            int i2 = proc.i2;
            int new_i = proc.new_i;
            int woff = proc.woff;

            int cnt = intersect(arrs, offset[i1], length[i1], offset[i2], length[i2], woff);
            length[new_i] = cnt;

            msgsnd(msg_proc_id, &(FromProc) { main_proc, id }, 
                    sizeof(FromProc) - sizeof(long), 0);
            msgsnd(msg_arr_id, &(Array) { main_proc, new_i }, 
                    sizeof(Array) - sizeof(long), 0);
        }

        _exit(0);
    }

    return pid;
}

int
main(int argc, char *argv[])
{
    int num_files = argc - 1;

    if (num_files == 0) {
        return 0;
    }

    //   Here I count lengths of initial arrays
    int lenid = shmget(KEY_LEN, 4 * num_files * sizeof(int), 0600 | IPC_CREAT | IPC_EXCL);
    int *length = shmat(lenid, NULL, 0);

    int cnt_nums = 0;
    for (int i = 1; argv[i]; ++i) {
        FILE *f = fopen(argv[i], "r");

        int cur;
        int cur_cnt = 0;
        while (fscanf(f, "%d", &cur) == 1) {
            ++cnt_nums;
            ++cur_cnt;
        }

        length[i - 1] = cur_cnt;

        fclose(f);
    }

    //   Offset in 'arrs' for every array in 'arrs'
    int offid = shmget(KEY_OFF, 4 * num_files * sizeof(int), 0600 | IPC_CREAT | IPC_EXCL);
    int *offset = shmat(offid, NULL, 0);

    //   Shared memory where all arrays are stored
    int arrid = shmget(KEY_ARR, 4 * cnt_nums * sizeof(int), 0600 | IPC_CREAT | IPC_EXCL);
    int *arrs = shmat(arrid, NULL, 0);

    //   Message queues for arrays and for processes
    int msg_proc_id = msgget(KEY_PROC, 0600 | IPC_CREAT | IPC_EXCL);
    int msg_arr_id = msgget(KEY_ARR, 0600 | IPC_CREAT | IPC_EXCL);


    int num_procs = num_files / 2;
    //   All messages for main process go with type of message equal to 'main_proc'
    int main_proc = num_procs + 1;

    //   Here I create processes with IDs from 1 to 'num_procs', main process has
    // (num_procs + 1) ID
    int *pids = calloc(num_procs, sizeof(pids[0]));
    for (int i = 1; i <= num_procs; ++i) {
        int pid = new_process(i, msg_proc_id, msg_arr_id, main_proc, arrs, offset, length);
        pids[i - 1] = pid;
    }

    //   Here I count offsets
    offset[0] = 0;
    for (int i = 1; i < num_files; ++i) {
        offset[i] = offset[i - 1] + length[i - 1];
    }

    //   Here I fill 'arrs' with initial arrays 
    for (int i = 0; i < num_files; ++i) {
        FILE *f = fopen(argv[i + 1], "r");

        for (int j = 0, cur; fscanf(f, "%d", &cur) == 1; ++j) {
            arrs[offset[i] + j] = cur;
        }
    }

    //   Here I send messages about arrays to main process
    for (int i = 0; i < num_files; ++i) {
        msgsnd(msg_arr_id, &(Array) { main_proc, i }, 
                sizeof(Array) - sizeof(long), 0);
    }

    //   Here main process receives and sends messages about processes and arrays
    for (int cnt_arrays_left = num_files, cur_arr = num_files; 
            cnt_arrays_left > 1; --cnt_arrays_left, ++cur_arr) {
        Array arr1, arr2;
        msgrcv(msg_arr_id, &arr1, sizeof(arr1) - sizeof(long), main_proc, 0);
        msgrcv(msg_arr_id, &arr2, sizeof(arr2) - sizeof(long), main_proc, 0);

        FromProc free_proc;
        msgrcv(msg_proc_id, &free_proc, sizeof(free_proc) - sizeof(long), main_proc, 0);

        int i1 = arr1.i;
        int i2 = arr2.i;
        int woff = offset[cur_arr - 1] + length[cur_arr - 1];

        offset[cur_arr] = woff;
        length[cur_arr] = min(length[i1], length[i2]);
        
        msgsnd(msg_proc_id, &(ToProc) { free_proc.i, i1, i2, cur_arr, woff }, 
                sizeof(ToProc) - sizeof(long), 0);
    }

    Array last_arr;
    msgrcv(msg_arr_id, &last_arr, sizeof(last_arr) - sizeof(long), main_proc, 0);

    for (int i = 0; i < length[last_arr.i]; ++i) {
        printf("%d ", arrs[offset[last_arr.i] + i]);
    }
    puts("");

    //   Killing and waiting sons
    for (int i = 0; i < num_procs; ++i) {
        kill(pids[i], SIGKILL);
    }

    for (int i = 0; i < num_procs; ++i) {
        wait(NULL);
    }

    //   Free resources
    free(pids);

    shmdt(arrs);
    shmctl(arrid, IPC_RMID, NULL);

    shmdt(length);
    shmctl(lenid, IPC_RMID, NULL);

    shmdt(offset);
    shmctl(offid, IPC_RMID, NULL);

    msgctl(msg_proc_id, IPC_RMID, NULL);

    msgctl(msg_arr_id, IPC_RMID, NULL);
}

