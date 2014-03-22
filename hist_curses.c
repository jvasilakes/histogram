/******************************************
*       A better version of hist,         *
**      which reads an input file        **
***     and creates a histogram         ***
**	of word lengths in that file.    ** 
*					  *
*******************************************/



#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#define MAX 100
#define MAXHEIGHT 30



struct Histogram {
    int rows;
    int cols;
    int wlen[MAX];
};


struct Connection {
    FILE *file;
    struct Histogram *hist;
};


void die(char *message, struct Connection *conn)
{
    printf("ERROR: %s", message);

    if(conn) {
        if(conn->file) fclose(conn->file);
        if(conn->hist) free(conn->hist);
        free(conn); 
    }

    exit(1);
}


void File_open(char *filename, struct Connection *conn)
{
    if (!(conn->file = fopen(filename, "r"))) {
	die("Can't open file.\n", conn);
    }
}


void File_read(struct Connection *conn)
{
    char c;
    int wc = 0;

    while ((c = getc(conn->file)) != EOF) {
	if (c != ' ' && c != '\t' && c != '\n' && c != '\r'
	    && c != '.' && c != ',' && c != '?' && c != '!'
	    && c != ';' && c != '\'' && c != ':') {

	    ++wc;

	} else {
	    ++conn->hist->wlen[wc];

	    if (wc > conn->hist->cols) {
		conn->hist->cols = wc;
	    }
	    if (conn->hist->wlen[wc] > conn->hist->rows) {
		conn->hist->rows = conn->hist->wlen[wc];
	    }

	    wc = 0;
	}
    }
}


int scale(struct Connection *conn, int maxheight)
{
    int lower = conn->hist->rows / maxheight;
    int upper = (conn->hist->rows * 2) / maxheight;

    int scale = upper - lower;

    if (scale == 0) {
	return 1;
    } else {
        return scale;
    }
}


void mvset(int cursor[], int y, int x)
{
    cursor[0] = cursor[0] + y; 
    cursor[1] = cursor[1] + x;
    
    move(cursor[0], cursor[1]);
}


void print_at_coor(int y, int scl, struct Connection *conn, int cursor[])
{
    int x = 0;

    if (y <= 0) {
	return;
    }

    else {
	mvset(cursor, 1, 0);
        printw("%4d", y);
	addch(ACS_VLINE);

	mvset(cursor, 0, 6);
        for (x = 1; x <= conn->hist->cols; ++x) {
	    if (conn->hist->wlen[x] >= y) {
	        addch(ACS_LANTERN);
	    } else {}
	    mvset(cursor, 0, 3);
        }
	cursor[1] = 0;

        print_at_coor(y - scl, scl, conn, cursor);
    }
}

	
void Histogram_print(struct Connection *conn, int maxheight, int cursor[])
{
    int y = conn->hist->rows;
    int scl = scale(conn, MAXHEIGHT);   


    // Print the y-axis and all points on the histogram
    move(cursor[0], cursor[1]);
    printw("   Y");
    print_at_coor(y, scl, conn, cursor);

    // Print the x-axis
    mvset(cursor, 1, 0);

    printw("      ");

    mvset(cursor, 0, 2);

    for (y = 0; y < conn->hist->cols; ++y) {
        mvset(cursor, 0, 3);

	addch(ACS_HLINE);
	addch(ACS_HLINE);
	addch(ACS_HLINE);
    }

    printw(" X");

    mvset(cursor, 1, 0);
    cursor[1] = 1;

    for (y = 1; y <= conn->hist->cols; ++y) {
        mvset(cursor, 0, 3);
	printw("%3d", y);
    }

    mvset(cursor, -2, -(cursor[1] - 6));
}


void curs_seek(int cursor[], int dir)
{
    switch(dir)
    {	
	case KEY_UP:	mvset(cursor, -1, 0);
			break;

	case KEY_DOWN:	mvset(cursor, 1, 0);
			break;

	case KEY_LEFT: 	mvset(cursor, 0, -1);
			break;

	case KEY_RIGHT:	mvset(cursor, 0, 1);
			break;
    }
}


main(int argc, char *argv[])
{
    if (argc != 2) {
	printf("USAGE: hist [file]\n");
	exit(1);
    }

    struct Connection *conn = malloc(sizeof(struct Connection));
    conn->hist = malloc(sizeof(struct Histogram));
    conn->hist->rows = 0;
    conn->hist->cols = 0;

    int y;
    for (y = 0; y < MAX; ++y) {
	conn->hist->wlen[y] = 0;
    }

    File_open(argv[1], conn);
     
    File_read(conn);

    initscr();
    keypad(stdscr, TRUE);
    raw();
    noecho();

    int cursor[2] = {0, 0};

    Histogram_print(conn, MAXHEIGHT, cursor);
    int dir;
    while ((dir = getch()) != KEY_F(1)) {
        curs_seek(cursor, dir);
    }

    getch();
    endwin();

    if(conn) {
        if(conn->file) fclose(conn->file);
        if(conn->hist) free(conn->hist);
        free(conn); 
    }

    return 0;
}
