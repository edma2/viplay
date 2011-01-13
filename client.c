/* client.c - example client for player.c */
#include "player.h"
#include <ncurses.h>

#define BUFSIZE         200
#define PATHMAX         100
#define VLINE_OFFSET    20

int main(int argc, char *argv[]) {
        int player_write_fd, player_read_fd;
        char path[PATHMAX];
        char buf_out[BUFSIZE];
        char c;

        /* New process forked after this step */
        if (player_init(&player_read_fd, &player_write_fd) < 0) {
                fprintf(stderr, "error: unable to run mplayer\n");
                return -1;
        }

        /* Start ncurses */
        initscr();
        mvvline(0, VLINE_OFFSET, ACS_VLINE, 20);

        /* Input loop */
        while (1) {
                mvprintw(0, VLINE_OFFSET, "> ");
                c = getch();
                if (c == 'q') {
                        player_cmd_quit(player_write_fd);
                        break;
                } else if (c == 'p') {
                        player_query_percent(player_write_fd, player_read_fd, buf_out, sizeof buf_out);
                        mvprintw(0, VLINE_OFFSET, "--Percent: %s\n", buf_out);
                } else if (c == 'a') {
                        player_query_title(player_write_fd, player_read_fd, buf_out, sizeof buf_out);
                        mvprintw(0, VLINE_OFFSET, "--Title: %s\n", buf_out);
                } else if (c == 't') {
                        player_cmd_toggle(player_write_fd);
                        printw("\n");
                } else if (c == 'l') {
                        mvprintw(0, VLINE_OFFSET, "\nEnter path of file: ");
                        if (getnstr(path, sizeof path) == ERR) {
                                fprintf(stderr, "error: getnstr() failed\n");
                                break;
                        }
                        if (strlen(path) < sizeof path) {
                                path[strlen(path)] = '\n';
                        } else {
                                fprintf(stderr, "error: path too long\n");
                                break;
                        }
                        player_cmd_load(player_write_fd, path);
                        mvprintw(0, VLINE_OFFSET, "Loaded file %s", path);
                }
                /* Refresh ncurses window */
                refresh();
        }

        /* Clean up*/
        player_deinit(&player_read_fd, &player_write_fd);
        endwin();

        return 0;
}
