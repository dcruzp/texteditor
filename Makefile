CC = gcc 
CFLAGS = -Wall -Wextra -pedantic -std=c99 -I.

editor.o: editor.c 
	$(CC) editor.c appendBuffer.c terminal.c -o editor.o  $(CFLAGS)

clean:
	rm -f editor.o