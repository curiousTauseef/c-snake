#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <termios.h>
#include <getopt.h>
#include <sys/ioctl.h>

#define KEY_QUIT      0
#define KEY_DOWN      66
#define KEY_UP        65
#define KEY_LEFT      68
#define KEY_RIGHT     67
#define KEY_ESCAPE    27
//#define KEY_PAGE_UP   53
//#define KEY_PAGE_DOWN 54

struct termios origterm, tmpterm;

void console_disable_char()
{
    freopen("/dev/tty", "rw", stdin);
    tcgetattr(STDIN_FILENO, &origterm);
    tmpterm = origterm;
    tmpterm.c_lflag &= ~(ICANON | ECHO);
    tcsetattr (STDIN_FILENO, TCSANOW, &tmpterm);
}

void console_enable_char()
{
    freopen("/dev/tty", "rw", stdin);
    tcsetattr (STDIN_FILENO, TCSANOW, &origterm);
}

int console_get_key()
{
    freopen("/dev/tty", "rw", stdin);
    int ch;

    ch = getchar();

    if (ch == KEY_ESCAPE) { // 27 means escape key
        ch = getchar();
        if (ch == KEY_ESCAPE) {
            return ch;
        }
        ch = getchar();
        return ch;
    }

    return 0;
}

/**
 * Move the consoles cursor up and left
 * @param int x how many spaces left
 * @param int y how many spaces up
 */
void console_cursor_move_by(int x, int y)
{
    //printf("\033[%d;%df", x, y);
    printf("\033[%dA", y);
    printf("\033[%dD", x);
}
