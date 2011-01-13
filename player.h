/* mplayer_slave.c - mplayer interface for viplay
 * author: Eugene Ma (edma2) */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <poll.h>

int player_init(int *fd_read, int *fd_write);
void player_deinit(int *fd_read, int *fd_write);
int player_cmd(int fd_send, char *buf_cmd);
int player_cmd_load(int fd_send, char *path);
int player_cmd_toggle(int fd_send);
int player_cmd_quit(int fd_send);
int player_query(int fd_send, int fd_recv, char *buf_query, char *buf_reply, int recvlen);
int player_query_percent(int fd_send, int fd_recv, char *buf_reply, int len);
int player_query_title(int fd_send, int fd_recv, char *buf_reply, int len);
