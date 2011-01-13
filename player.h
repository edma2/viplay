/* mplayer_slave.c - mplayer interface for viplay
 * author: Eugene Ma (edma2) */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <poll.h>

typedef struct {
        int fd_read;
        int fd_write;
} Player;

Player *player_new(void);
int player_init(Player *mp);
void player_die(Player *mp);
void player_free(Player *mp);
int player_cmd(Player *mp, char *buf_cmd);
int player_cmd_load(Player *mp, char *path);
int player_cmd_pause(Player *mp);
int player_cmd_quit(Player *mp);
int player_query(Player *mp, char *buf_query, char *buf_reply, int len);
int player_query_percent(Player *mp, char *buf_reply, int len);
int player_query_title(Player *mp, char *buf_reply, int len);
