#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

#define PID_STR_SIZE 10

static int success_locks = 0;
static char *lock_filename = NULL;

void kill_handler(int signum)
{
        FILE *fp = NULL;

        if (lock_filename == NULL) {
                exit(EXIT_SUCCESS);
        }

        if (access(lock_filename, F_OK) == 0) {
                unlink(lock_filename);
        }
        free(lock_filename);

        while (fp == NULL) {
                fp = fopen("results.txt", "a+");
        }

        fprintf(fp, "%d %d\n", getpid(), success_locks);
        fclose(fp);

        exit(EXIT_SUCCESS);
}


int lock_file()
{
        int lock_fd = -1;
        pid_t pid = 0;
        char pid_str[PID_STR_SIZE] = {0};

        if (lock_filename == NULL) {
                return -1;
        }

        while (lock_fd == -1) {
                lock_fd = open(lock_filename,
                               O_CREAT | O_EXCL | O_WRONLY,
                               S_IRUSR | S_IWUSR);
        }

        pid = getpid();
        sprintf(pid_str, "%d", pid);

        if (write(lock_fd, pid_str, strlen(pid_str) + 1) == -1) {
                close(lock_fd);
                return -1;
        }

        close(lock_fd);
        return 0;
}

int unlock_file()
{
        pid_t pid = 0;
        pid_t program_pid = getpid();
        char pid_str[PID_STR_SIZE] = {0};
        int lock_fd = -1;
        int err = 0;

        if (lock_filename == NULL) {
                err = -1;
                goto out;
        }

        while (lock_fd == -1) {
                lock_fd = open(lock_filename, O_RDONLY);
        }

        if (read(lock_fd, pid_str, sizeof(pid_str)) == -1) {
                err = -1;
                goto out;
        }

        pid = atoi(pid_str);
        if (pid == 0 || pid != program_pid) {
                err = -1;
                goto out;
        }

        unlink(lock_filename);
out:
        if (lock_fd != -1)
                close(lock_fd);
        return err;
}

int main(int argc, char **argv)
{
        char *filename = NULL;
        int lock_fd = -1;
        FILE *fp = NULL;

        signal(SIGINT, kill_handler);

        if (argc != 2) {
                printf("Pass filename you want to lock\n");
                return EXIT_FAILURE;
        }

        filename = argv[1];
        lock_filename = malloc((strlen(filename) + 5) * sizeof(char));

        if (!lock_filename) {
                printf("Malloc error\n");
                return EXIT_FAILURE;
        }

        sprintf(lock_filename, "%s.lck", filename);

        while (1) {
                if (lock_file()) {
                        printf("Failed to lock file\n");
                        continue;
                }

                fp = fopen(filename, "a+");
                fprintf(fp, "hello from %d\n", getpid());
                sleep(1);
                fclose(fp);

                if (unlock_file()) {
                        printf("Failed to unlock file\n");
                        continue;
                }

                success_locks++;
        }
}