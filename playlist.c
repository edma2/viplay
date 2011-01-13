#include "playlist.h"

static void song_free_traverse_helper(void *data);

int main(void) {
        Playlist *pl;
        Node *p;

        pl = playlist_new("db");
        if (pl == NULL) {
                fprintf(stderr, "error: failed to create playlist\n");
                return -1;
        }

        for (p = list_first(pl); p != NULL; p = p->next)
                song_print_path((Song *)p->data);

        playlist_free(pl);

        return 0;
}

Song *song_new(char *path, char *title) {
        Song *s;

        if (path == NULL)
                return NULL;
        s = malloc(sizeof(Song));
        if (s == NULL)
                return NULL;
        strncpy(s->path, path, PATH_MAXLEN);
        //strncpy(s->title, title, TITLE_MAXLEN);
        
        return s;
}

void song_free(Song *s) {
        free(s);
}

int song_play(Player *mp, Song *s) {
        return player_cmd_load(mp, s->path)
}

void song_print_path(Song *s) {
        if (s == NULL)
                return;
        printf("%s\n", s->path);
}

static void song_free_traverse_helper(void *data) {
        song_free((Song *)data);
}

void playlist_free(Playlist *pl) {
        if (pl == NULL)
                return;
        list_traverse(pl, song_free_traverse_helper);
        list_free(pl);
}

Song *playlist_add(Playlist *pl, Song *s) {
        return (Song *)list_append(pl, s)->data;
}

void playlist_remove(Playlist *pl, Song *s) {
        list_remove(pl, s);
}

Playlist *playlist_new(char *pl_path) {
        Playlist *pl;
        Song *s;
        FILE *fp;
        char song_path[PATH_MAXLEN];

        if (pl_path == NULL)
                return NULL;
        pl = list_new();
        if (pl == NULL)
                return NULL;
        fp = fopen(pl_path, "r");
        if (fp == NULL) {
                playlist_free(pl);
                return NULL;
        }

        while (fgets(song_path, PATH_MAXLEN, fp) != NULL) {
                /* get rid of new line */
                song_path[strlen(song_path)-1] = '\0';
                s = song_new(song_path, NULL);
                if (s == NULL) {
                        playlist_free(pl);
                        return NULL;
                }
                if (playlist_add(pl, s) == NULL) {
                        playlist_free(pl);
                        return NULL;
                }

        }

        return pl;
}
