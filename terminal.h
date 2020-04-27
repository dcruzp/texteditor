#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

void die (const char *s);
void disableRawMode ();
void enableRawMode ();
int editorReadKey ();
int getCursorPosition (int *rows  , int *cols);
int getWindowsSize (int *rows , int *cols);

struct editorConfig
{
    int cx , cy; 
    int screenrows ; 
    int screencols ; 
    struct termios orig_termios; 
};

enum editorKey 
{
    ARROW_LEFT = 1000 , 
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};

struct editorConfig E;
