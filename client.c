/* client.c - test client for mplayer_slave */
#include "player.h"
#include <ncurses.h>

#define BUFSIZE         200
#define PATHMAX         100

int main(int argc, char *argv[]) {
        int player_write_fd, player_read_fd;
        char path[PATHMAX];
        char buf_in[BUFSIZE];
        char buf_out[BUFSIZE];
        char c;

        /* New process forked after this step */
        if (player_init(&player_read_fd, &player_write_fd) < 0) {
                fprintf(stderr, "error: unable to run mplayer\n");
                return -1;
        }

        /* Input loop */
        while (1) {
                printf("> ");
                fgets(buf_in, BUFSIZE, stdin);
                c = buf_in[0];
                if (c == 'q') {
                        printf("Goodbye!\n");
                        player_cmd_quit(player_write_fd);
                        break;
                } else if (c == 'p') {
                        player_query_percent(player_write_fd, player_read_fd, buf_out, sizeof buf_out);
                        printf("--Percent: %s\n", buf_out);
                } else if (c == 'a') {
                        player_query_title(player_write_fd, player_read_fd, buf_out, sizeof buf_out);
                        printf("--Title: %s\n", buf_out);
                } else if (c == 't') {
                        player_cmd_toggle(player_write_fd);
                } else if (c == 'l') {
                        printf("Enter path of file: ");
                        if (fgets(path, sizeof(path), stdin) == NULL)
                                continue;
                        player_cmd_load(player_write_fd, path);
                }
        }
        player_deinit(&player_read_fd, &player_write_fd);

        return 0;
}
