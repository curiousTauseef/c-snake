#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <termios.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include "keys.h"

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

// https://stackoverflow.com/questions/2649733/hide-cursor-on-remote-terminal/2676973
void console_show_cursor(bool show)
{
#define CSI "\e["
    if (show) {
        fputs(CSI "?25h", stdout);
    } else {
        fputs(CSI "?25l", stdout);
    }
#undef CSI
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



/* this variable is our reference to the second thread */
pthread_t console_get_key_non_blocking_thread;

/* this function is run by the second thread */
void *console_get_key_non_blocking(void *input_void_ptr)
{
    /* increment x to 100 */
    int *input = (int *)input_void_ptr;
    //while(++(*input) < 100);

    *input = console_get_key();

    /* the function must return something - NULL will do */
    return NULL;
}

void console_get_key_listen_start(key *input)
{
    /* input is already a reference */
    /* create a second thread which executes console_get_key_non_blocking(&x) */
    if(pthread_create(&console_get_key_non_blocking_thread, NULL, console_get_key_non_blocking, input)) {
        fprintf(stderr, "Error creating thread\n");
        exit(1);
    }
}

void console_get_key_listen_stop()
{
    /* wait for the second thread to finish */
    if(pthread_join(console_get_key_non_blocking_thread, NULL)) {
        fprintf(stderr, "Error joining thread\n");
        exit(1);
    }
}

void console_get_key_listen_cancel()
{
    pthread_cancel(console_get_key_non_blocking_thread);
}
