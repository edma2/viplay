#include "datastruct/list.h"
#include "player.h"
#include <string.h>

#define PATH_MAXLEN        200
#define TITLE_MAXLEN       200

typedef struct {
        char path[PATH_MAXLEN];
        char title[TITLE_MAXLEN];
} Song;
typedef List Playlist;

Song *song_new(char *path, char *title);
void song_free(Song *s);
int song_play(Player *mp, Song *s);
void song_print_path(Song *s);
void playlist_free(Playlist *pl);
Song *playlist_add(Playlist *pl, Song *s);
void playlist_remove(Playlist *pl, Song *s);
Playlist *playlist_new(char *pl_path);
