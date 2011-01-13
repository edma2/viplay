/* client.c - example client for player.c */
#include "player.h"

#define BUFSIZE         200

int main(int argc, char *argv[]) {
        Player *mp;
        char buf_out[BUFSIZE];
        char buf_in[BUFSIZE];
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

        /* Input loop */
        while (1) {
                fgets(buf_in, sizeof(buf_in), stdin);
                c = buf_in[0];
                if (c == 'q') {
                        player_cmd_quit(mp);
                        break;
                } else if (c == 'p') {
                        player_query_percent(mp, buf_out, sizeof buf_out);
                        printf("--Percent: %s\n", buf_out);
                } else if (c == 'a') {
                        player_query_title(mp, buf_out, sizeof buf_out);
                        printf("--Title: %s\n", buf_out);
                } else if (c == 't') {
                        player_cmd_pause(mp);
                        printf("\n");
                } else if (c == 'l') {
                        printf("\nEnter path of file: ");
                        if (fgets(buf_in, sizeof(buf_in), stdin) == NULL)
                                continue;
                        player_cmd_load(mp, buf_in);
                        printf("Loaded file %s", buf_in);
                }
        }

        /* Clean up*/
        player_die(mp);
        player_free(mp);

        return 0;
}
