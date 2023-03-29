#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#include <getopt.h>


int main(int argc, char **argv) {
    int exit_code = 0;
    int written_bytes = 0;
    int read_bytes = 0;
    int buffer_size = 4096;

    int fd_to_read;
    int fd_to_write;
    int is_zero_block;
    char * restrict buffer;

    static struct option options[] = {
        {"Block size", required_argument, NULL, 'b'},
        {0, 0, 0, 0},
    };
    char *optstring = "b:";

    for (;;) {
        if (getopt(argc, argv, optstring) == -1)
            break;
        
        buffer_size = atoi(optarg);
        if (buffer_size <= 0) {
            fprintf(stderr, 
                    "ERROR: buffer's size must be a positive integer.\n");
            exit_code = 2;
            goto exit;
        }
    }

    if (optind + 1 == argc) {
        fd_to_write = open(argv[optind], 
                       O_WRONLY | O_CREAT | O_TRUNC, 
                       S_IRUSR | S_IWUSR);
        if (fd_to_write == -1) {
            fprintf(stderr, "ERROR: failed to open file for writting.\n");
            exit_code = 3;
        goto exit;
    }
        fd_to_read = 0;
    }
    else if (optind + 2 == argc) {
        fd_to_write = open(argv[optind + 1], 
                       O_WRONLY | O_CREAT | O_TRUNC, 
                       S_IRUSR | S_IWUSR);
        if (fd_to_write == -1) {
            fprintf(stderr, "ERROR: failed to open file for writting.\n");
            exit_code = 3;
        }
        fd_to_read = open(argv[optind], O_RDONLY);
        if (fd_to_read == -1) {
            fprintf(stderr, "ERROR: failed to open file for reading.\n");
            exit_code = 3;
            goto close_fd_to_write;
        }
    }
    else {
        fprintf(stderr, "ERROR: wrong arguments count.\n");
        exit_code = 4;
        goto exit;
    }

    buffer = calloc(buffer_size, sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "ERROR: failed to allocate memory.\n");
        exit_code = 5;
        goto close_fd_to_read;
    }

    for (;;) {
        is_zero_block = 1;
        read_bytes = read(fd_to_read, buffer, buffer_size);
        if (read_bytes == -1) {
            fprintf(stderr, "ERROR: failed to read bytes.\n");
            exit_code = 1;
            goto free_all;
        }
        if (read_bytes == 0)
            break;

        for (int i = 0; i < read_bytes; i++) {
            if (buffer[i] != 0) {
                is_zero_block = 0;
                break;
            }
        }

        if (is_zero_block == 1) {
            if (lseek(fd_to_write, read_bytes, SEEK_CUR) == -1) {
                fprintf(stderr, "ERROR: failed to seek.\n");
                exit_code = 1;
                goto free_all;
            }
        }
        else {
            written_bytes = write(fd_to_write, buffer, read_bytes);
            if (written_bytes != read_bytes) {
                fprintf(stderr, "ERROR: failed to write bytes.\n");
                exit_code = 1;
                goto free_all;
            }
        }
    }

free_all:
    free(buffer);
close_fd_to_read:
    close(fd_to_read);
close_fd_to_write:
    close(fd_to_write);
exit:
    return exit_code;
}
