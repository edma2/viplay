#include <ncurses.h>
#include <panel.h>
#include <menu.h>

#define BIND_DOWN        'j'
#define BIND_UP          'k'
#define BIND_QUIT        'q'

int main(void) {
        ITEM *i[] = { new_item("eugene", "ma"), new_item("foo", "bar"), NULL };
        MENU *m = new_menu(i);
        char input;

        /* Initialize screen */
        initscr();
        scrollok(stdscr, TRUE);
        nonl();
        noecho();
        curs_set(0);
        post_menu(m);
        refresh();

        while (1) {
                input = getch();
                if (input == BIND_DOWN)
                        menu_driver(m, REQ_DOWN_ITEM);
                else if (input == BIND_UP)
                        menu_driver(m, REQ_UP_ITEM);
                else if (input == BIND_QUIT)
                        break;
        }
        endwin();

        return 0;
}
