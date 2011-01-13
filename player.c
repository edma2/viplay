/* mplayer_slave.c - mplayer interface for viplay
 * author: Eugene Ma (edma2) */
#include "player.h"

#define PIPE_READ       0
#define PIPE_WRITE      1
#define CMD_MAX         150

int player_init(Player *mp) {
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
                return -1;
        } else if (cpid > 0) {
                /* Close the pipe that the child reads from */
                close(pipe_input[PIPE_READ]);
                /* Close the pipe that the child writes to */
                close(pipe_output[PIPE_WRITE]);
                /* Set the output stream from mplayer to non-blocking */
                fcntl(pipe_output[PIPE_READ], F_SETFL, O_NONBLOCK);
                /* Assign the read/write file descriptors to arguments */
                mp->fd_read = pipe_output[PIPE_READ];
                mp->fd_write = pipe_input[PIPE_WRITE];
        }

        return 0;
}

Player *player_new(void) {
        Player *mp;

        mp = malloc(sizeof(Player));
        if (mp == NULL)
                return NULL;
        mp->fd_read = -1;
        mp->fd_write = -1;

        return mp;
}

void player_free(Player *mp) {
        free(mp);
}

void player_die(Player *mp) {
        wait(NULL); 
        close(mp->fd_read);
        close(mp->fd_write);
}

int player_cmd(Player *mp, char *buf_cmd) {
        if (mp == NULL)
                return -1;
        return (write(mp->fd_write, buf_cmd, strlen(buf_cmd)) != strlen(buf_cmd)) ? -1 : 0;
}

int player_cmd_pause(Player *mp) {
        char *cmd = "pause\n";
        return player_cmd(mp, cmd);
}

int player_cmd_quit(Player *mp) {
        char *cmd = "quit\n";
        return player_cmd(mp, cmd);
}

int player_cmd_load(Player *mp, char *path) {
        char cmd[CMD_MAX] = "load ";
        int i;

        for (i = strlen(cmd); i < CMD_MAX; i++) {
                cmd[i] = *path;
                if (*path++ == '\0')
                        break;
        }
        return player_cmd(mp, cmd);
}

/* Store len received bytes into buf_reply */
int player_query_percent(Player *mp, char *buf_reply, int len) {
        char *cmd = "pausing get_percent_pos\n";
        return player_query(mp, cmd, buf_reply, len);
}

int player_query_title(Player *mp, char *buf_reply, int len) {
        char *cmd = "pausing get_meta_title\n";
        return player_query(mp, cmd, buf_reply, len);
}

int player_query(Player *mp, char *buf_query, char *buf_reply, int len) {
        struct pollfd pfd;
        int i, j;
        char c;

        if (mp == NULL)
                return -1;

        /* Flush input */
        while (read(mp->fd_read, &c, 1) > 0)
                ;
        /* Write, wait, and read response */
        if (write(mp->fd_write, buf_query, strlen(buf_query)) != strlen(buf_query))
                return -1;
        pfd.fd = mp->fd_read;
        pfd.events = POLLIN;
        if (poll(&pfd, 1, 500) < 0)
                return -1;
        read(mp->fd_read, buf_reply, len);

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
