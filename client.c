/* client.c - example client for player.c */
#include "player.h"
#include <ncurses.h>

#define BUFSIZE         200
#define PATHMAX         100
#define VLINE_OFFSET    20

int main(int argc, char *argv[]) {
        Player *mp;
        char path[PATHMAX];
        char buf_out[BUFSIZE];
        char c;

        mp = player_new();
        if (mp == NULL) {
                fprintf(stderr, "error: malloc() failed\n");
                return -1;
        }

        /* New process forked after this step */
        if (player_init(mp) < 0) {
                fprintf(stderr, "error: unable to run mplayer\n");
                player_free(mp);
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
                        player_cmd_quit(mp);
                        break;
                } else if (c == 'p') {
                        player_query_percent(mp, buf_out, sizeof buf_out);
                        mvprintw(0, VLINE_OFFSET, "--Percent: %s\n", buf_out);
                } else if (c == 'a') {
                        player_query_title(mp, buf_out, sizeof buf_out);
                        mvprintw(0, VLINE_OFFSET, "--Title: %s\n", buf_out);
                } else if (c == 't') {
                        player_cmd_pause(mp);
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
                        player_cmd_load(mp, path);
                        mvprintw(0, VLINE_OFFSET, "Loaded file %s", path);
                }
                /* Refresh ncurses window */
                refresh();
        }

        /* Clean up*/
        player_die(mp);
        player_free(mp);
        endwin();

        return 0;
}
