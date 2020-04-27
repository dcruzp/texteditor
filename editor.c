
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE


#include <ctype.h>
#include <sys/types.h>
#include <appendBuffer.h>
#include <terminal.h>



/***define***/ 

#define EDITOR_VERSION "0.1"
#define EDITOR_TAB_STOP 8 
#define CTRL_KEY(k) ((k)&0x1f)


/*** row operation ***/

int editorRowCxToRx(erow * row , int cx)
{
    int rx = 0; 
    int j;
    for (j = 0 ; j<cx ; j++)
    {
        if (row ->chars[j] == '\t')
        {
            rx += (EDITOR_TAB_STOP -1) - (rx %EDITOR_TAB_STOP);
        }
        rx ++ ; 
    }
    return rx ; 
}


void editorUpdateRow (erow *row)
{
    int tabs = 0;
    int j;

    for (j=0 ;j<row->size; j++)
    {
        if (row->chars[j]== '\t')tabs++;
    }

    free (row->render);
    row->render = malloc (row->size + tabs*(EDITOR_TAB_STOP-1) + 1);

   
    int idx =0 ;
    for (j=0;j<row->size;j++)
    {
        if (row->chars[j] == '\t')
        {
            row->render[idx++] = ' ';
            while (idx % EDITOR_TAB_STOP != 0)
            {
                row->render[idx++]=' ';
            }
        }
        else 
        {
            row->render[idx++]=  row->chars[j];
        }
    }
    row->render [idx]= '\0';
    row->rsize = idx;
}


void editorAppendRow (char *s , size_t len)
{
    E.row = realloc(E.row ,sizeof(erow) * (E.numrows +1));

    int at = E.numrows; 
    E.row[at].size = len ; 
    E.row[at].chars = malloc (len +1);
    memcpy (E.row[at].chars , s, len );
    E.row[at].chars[len] = '\0';

    E.row[at].rsize = 0;
    E.row[at].render = NULL;
    editorUpdateRow(&E.row[at]);

    E.numrows++; 
}


/***file i/o ***/

void editorOpen (char *filename)
{
    FILE *fp = fopen (filename, "r");
    if (!fp) die ("fopen");
    
    char *line = NULL;
    size_t linecap =0;
    ssize_t linelen; 

    while((linelen = getline(&line , &linecap, fp))!=-1)
    {
        while (linelen >0 && (line[linelen -1]=='\n' || line[linelen-1]=='\r'))
        {
            linelen--;
        }
        editorAppendRow(line,linelen);
    }
    free(line);
    fclose(fp);   
}


/***output***/

void editorScroll ()
{
    E.rx = 0;
    if (E.cy <E.numrows)
    {
        E.rx = editorRowCxToRx(&E.row[E.cy],E.cx);
    }

    if (E.cy < E.rowoff)
    {
        E.rowoff = E.cy;
    }
    if (E.cy >= E.rowoff + E.screenrows) 
    {
        E.rowoff = E.cy - E.screenrows + 1;
    }
    if (E.rx < E.coloff) 
    {
        E.coloff = E.rx;
    }
    if (E.rx >= E.coloff + E.screencols)
    {
        E.coloff = E.rx - E.screencols + 1;
    }
}

void editorDrawRows (struct abuf *ab)
{
    int y ;
    for (y=0 ; y<E.screenrows ;y++)
    {
        int filerow = y + E.rowoff;
        if (filerow>=E.numrows)
        {
            if (E.numrows >0 && y ==E.screenrows/3)
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
        }
        else 
        {
            int len = E.row[filerow].rsize - E.coloff;
            if (len <0) len = 0 ;
            if (len > E.screencols) len =E.screencols;
            abAppend(ab,&E.row[filerow].render[E.coloff],len);
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
    editorScroll();

    struct abuf ab = ABUF_INIT;

    abAppend(&ab,"\x1b[?25l",6);
    abAppend(&ab,"\x1b[H",3);

    editorDrawRows(&ab);

    char buf [32];
    snprintf(buf,sizeof(buf),"\x1b[%d;%dH",(E.cy-E.rowoff)+1,(E.rx -E.coloff)+1);
    abAppend(&ab,buf,strlen(buf));

    abAppend(&ab,"\x1b[?25h",6);

    write(STDOUT_FILENO,ab.b,ab.len);
    abFree(&ab);
}



/***input***/

void editorMoveCursor(int key) 
{
    erow * row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy]; 

    switch (key)
    {
    case ARROW_LEFT:
        if(E.cx !=0)
        {
            E.cx--;
        }
        else if (E.cy >0)
        {
            E.cy--;
            E.cx = E.row[E.cy].size;
        }
        break;
    case ARROW_RIGHT:
        if (row&&E.cx <row->size)
        {
            E.cx++;
        }
        else if (row && E.cx == row->size)
        {
            E.cy++;
            E.cx=0;
        }
        break;
    case ARROW_UP:
        if(E.cy != 0)
        {
            E.cy--;
        }
        break; 
    case ARROW_DOWN:
        if(E.cy < E.numrows )
        {
            E.cy++;
        }
        break ;        
    }

    row = (E.cy >= E.numrows ) ? NULL : &E.row[E.cy];;
    int rowlen = row ? row->size : 0; 
    if (E.cx > rowlen)
    {
        E.cx =rowlen;
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
    E.cx = 0;
    E.cy = 0;
    E.rx = 0 ;
    E.rowoff = 0;
    E.coloff = 0;
    E.numrows = 0;
    E.row = NULL; 

    if (getWindowsSize(&E.screenrows ,&E.screencols)==-1)
    {
        die ("getWindowSize");
    }
}

int main (int argc , char *argv[]) 
{
    enableRawMode();
    initEditor () ; 
    if (argc>=2)
    {
        editorOpen(argv[1]);
    }

    while (1)
    {
        editorRefeshScreen();
        editorProcessKeypress();        
    }
    return 0 ; 
}