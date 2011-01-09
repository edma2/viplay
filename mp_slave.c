/* mp_slave.c - mplayer interface for viplayer
 * author: Eugene Ma (edma2) */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <poll.h>

#define BUFSIZE         300
#define PIPE_READ       0
#define PIPE_WRITE      1

int mplayer_open(int *fd_read, int *fd_write);
int mplayer_query(int fd_send, int fd_recv, char *buf_query, char *buf_reply, int recvlen);

int mplayer_open(int *fd_read, int *fd_write) {
        char *arglist[] = { "mplayer", "-slave", "-quiet", "-idle", NULL };
        int pipe_input[2];
        int pipe_output[2];
        int cpid;

        /* Create pipes */
        if (pipe(pipe_output) < 0)
                return -1;
        if (pipe(pipe_input) < 0) {
                close(pipe_output[PIPE_READ]);
                close(pipe_output[PIPE_WRITE]);
                return -1;
        }

        /* Fork a new process */
        cpid = fork();
        if (cpid == 0) {
                /* Close the pipe that the parent reads from */
                close(pipe_output[PIPE_READ]);
                /* Close the pipe that the parent writes to */
                close(pipe_input[PIPE_WRITE]);
                /* Redirect stdin to input pipe */
                if (dup2(pipe_input[PIPE_READ], STDIN_FILENO) < 0)
                        return -1;
                /* Redirect stdout to output pipe */
                if (dup2(pipe_output[PIPE_WRITE], STDOUT_FILENO) < 0)
                        return -1;
                /* Start mplayer */
                execvp(arglist[0], arglist);
                fprintf(stderr, "error: mplayer crashed\n");
        } else if (cpid > 0) {
                /* Close the pipe that the child reads from */
                close(pipe_input[PIPE_READ]);
                /* Close the pipe that the child writes to */
                close(pipe_output[PIPE_WRITE]);
                /* Set the output stream from mplayer to non-blocking */
                fcntl(pipe_output[PIPE_READ], F_SETFL, O_NONBLOCK);
                /* Assign the read/write file descriptors to arguments */
                *fd_read = pipe_output[PIPE_READ];
                *fd_write = pipe_input[PIPE_WRITE];
                return 0;
        }

        return -1;
}

int mplayer_query(int fd_send, int fd_recv, char *buf_query, char *buf_reply, int recvlen) {
        struct pollfd pfd;
        int i, j;
        char c;

        /* Write, wait, and read response */
        if (write(fd_send, buf_query, strlen(buf_query)) != strlen(buf_query))
                return -1;

        pfd.fd = fd_recv;
        pfd.events = POLLIN;
        if (poll(&pfd, 1, 500) < 0)
                return -1;

        /* Read only the first len bytes of stream
         * and throw away the rest */
        read(fd_recv, buf_reply, recvlen);
        while (read(fd_recv, &c, 1) > 0)
                ;

        /* Skip parsing if we don't request information */
        if (strncmp("get_", buf_query, 4) != 0) {
                buf_reply[0] = '\0';
                return 0;
        }

        /* Find the equals sign */
        for (i = 0; i < recvlen; i++) {
                if (buf_reply[i] == '=')
                        break;
        }
        /* Skip first single quote, if present */
        if (i+1 < recvlen && buf_reply[i+1] == '\'')
                i++;

        /* Read until the end */
        for (j = 0; j < recvlen - i; j++) {
                buf_reply[j] = buf_reply[++i];
                /* Stop at either of these conditions */
                if (buf_reply[i] == '\n' || buf_reply[i] == '\0' || buf_reply[i] == '\'')
                        break;
        }
        buf_reply[j] = '\0';

        return 0;
}

int main(int argc, char *argv[]) {
        char send[BUFSIZE];
        char recv[BUFSIZE];
        int fd_write, fd_read;

        /* New process forked after this step */
        if (mplayer_open(&fd_read, &fd_write) < 0) {
                fprintf(stderr, "error: unable to run mplayer\n");
                return -1;
        }

        /* Parent process only */
        while (1) {
                /* Read input from user */
                printf("\n> ");
                if (fgets(send, sizeof(send), stdin) == NULL)
                        break;

                /* Query mplayer */
                if (mplayer_query(fd_write, fd_read, send, recv, sizeof(recv)) < 0) {
                        fprintf(stderr, "error: communication error with mplayer\n");
                        break;
                }
                /* Print reply */
                printf("%s", recv);
                if (strcmp(send, "quit\n") == 0)
                        break;
        }

        /* Wait for child process to die */
        wait(NULL); 
        close(fd_read);
        close(fd_write);

        return 0;
}
