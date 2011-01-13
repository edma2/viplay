/* mplayer_slave.c - mplayer interface for viplay
 * author: Eugene Ma (edma2) */
#include "player.h"

#define PIPE_READ       0
#define PIPE_WRITE      1
#define CMD_MAX         150

typedef struct {
        int fd_read;
        int fd_write;
} Player;

int player_init(int *fd_read, int *fd_write) {
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

void player_deinit(int *fd_read, int *fd_write) {
        wait(NULL); 
        close(*fd_read);
        close(*fd_write);
}

int player_cmd(int fd_send, char *buf_cmd) {
        return (write(fd_send, buf_cmd, strlen(buf_cmd)) != strlen(buf_cmd)) ? -1 : 0;
}

int player_cmd_toggle(int fd_send) {
        char *cmd = "pause\n";
        return player_cmd(fd_send, cmd);
}

int player_cmd_quit(int fd_send) {
        char *cmd = "quit\n";
        return player_cmd(fd_send, cmd);
}

int player_cmd_load(int fd_send, char *path) {
        char cmd[CMD_MAX] = "load ";
        int i;

        for (i = strlen(cmd); i < CMD_MAX; i++) {
                cmd[i] = *path;
                if (*path++ == '\0')
                        break;
        }
        return player_cmd(fd_send, cmd);
}

/* Store len received bytes into buf_reply */
int player_query_percent(int fd_send, int fd_recv, char *buf_reply, int len) {
        char *cmd = "pausing get_percent_pos\n";
        return player_query(fd_send, fd_recv, cmd, buf_reply, len);
}

int player_query_title(int fd_send, int fd_recv, char *buf_reply, int len) {
        char *cmd = "pausing get_meta_title\n";
        return player_query(fd_send, fd_recv, cmd, buf_reply, len);
}

int player_query(int fd_send, int fd_recv, char *buf_query, char *buf_reply, int len) {
        struct pollfd pfd;
        int i, j;
        char c;

        /* Flush input */
        while (read(fd_recv, &c, 1) > 0)
                ;
        /* Write, wait, and read response */
        if (write(fd_send, buf_query, strlen(buf_query)) != strlen(buf_query))
                return -1;
        pfd.fd = fd_recv;
        pfd.events = POLLIN;
        if (poll(&pfd, 1, 500) < 0)
                return -1;
        read(fd_recv, buf_reply, len);

        /* Parse the information we receive */
        for (i = 0; i < len; i++) {
                if (buf_reply[i] == '=')
                        break;
        }
        /* Skip first single quote, if present */
        if (i+1 < len && buf_reply[i+1] == '\'')
                i++;
        /* Read until the end */
        for (j = 0; j < len - i; j++) {
                buf_reply[j] = buf_reply[++i];
                /* Stop at either of these conditions */
                if (buf_reply[i] == '\n' || buf_reply[i] == '\0' || buf_reply[i] == '\'')
                        break;
        }
        buf_reply[j] = '\0';

        return 0;
}
