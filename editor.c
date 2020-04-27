#include <ctype.h>
#include <appendBuffer.h>
#include <terminal.h>



/***define***/ 

#define EDITOR_VERSION "0.1"

#define CTRL_KEY(k) ((k)&0x1f)


/***output***/

void editorDrawRows (struct abuf *ab)
{
    int y ;
    for (y=0 ; y<E.screenrows ;y++)
    {
        if (y==E.screenrows/3)
        {
            char welcome [80];
            int welcomelen = snprintf (welcome,sizeof(welcome), "Text editor --version %s", EDITOR_VERSION );
            if (welcomelen > E.screencols) welcomelen = E.screencols;
            int padding = (E.screencols - welcomelen)/2;
            if (padding)
            {
                abAppend(ab,"~",1);
                padding--; 
            }
            while (padding--)
            {
                abAppend(ab," ",1);
            }            
            abAppend(ab,welcome,welcomelen);
        }
        else 
        {
            abAppend(ab,"~",1);
        }

        abAppend(ab,"\x1b[K",3);
        if (y<E.screenrows-1)
        {
            abAppend(ab,"\r\n",2);
        }
    }
}

void editorRefeshScreen ()
{
    struct abuf ab = ABUF_INIT;

    abAppend(&ab,"\x1b[?25l",6);
    abAppend(&ab,"\x1b[H",3);

    editorDrawRows(&ab);

    char buf [32];
    snprintf(buf,sizeof(buf),"\x1b[%d;%dH",E.cy+1,E.cx+1);
    abAppend(&ab,buf,strlen(buf));

    abAppend(&ab,"\x1b[?25h",6);

    write(STDOUT_FILENO,ab.b,ab.len);
    abFree(&ab);
}



/***input***/

void editorMoveCursor(int key) 
{
    switch (key)
    {
    case ARROW_LEFT:
        if(E.cx !=0)
        {
            E.cx--;
        }
        break;
    case ARROW_RIGHT:
        if(E.cx != E.screencols-1)
        { 
            E.cx++;
        }
        break;
    case ARROW_UP:
        if(E.cy != 0)
        {
            E.cy--;
        }
        break; 
    case ARROW_DOWN:
        if(E.cy != E.screenrows -1)
        {
            E.cy++;
        }
        break ;        
    }
}

void editorProcessKeypress()
{
    int c = editorReadKey();
    switch (c)
    {
    case CTRL_KEY('q'):            
        write(STDOUT_FILENO,"\x1b[2J",4);
        write(STDOUT_FILENO,"\x1b[H",3);
        exit(0);
        break;

    case HOME_KEY:
        E.cx =0 ; 
        break ; 
    case END_KEY:
        E.cx =E.screencols -1;
        break; 

    case PAGE_UP:
    case PAGE_DOWN:
        {
            int times = E.screenrows;
            while (times--)
            {
                editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
            }
            
        }       
        break ; 
    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
        editorMoveCursor(c);
        break; 
    }
}


/*** init ***/

void initEditor ()
{
    E.cx=0;
    E.cy=0;
    if (getWindowsSize(&E.screenrows ,&E.screencols)==-1)
        die ("getWindowSize");
}

int main () 
{
    enableRawMode();
    initEditor () ; 

    while (1)
    {
        editorRefeshScreen();
        editorProcessKeypress();        
    }
    return 0 ; 
}