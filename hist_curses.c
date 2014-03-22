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
    int scale;
};


struct Connection {
    FILE *file;
    struct Histogram *hist;
};


struct Cursor {
    int y;
    int x;
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


void mvset(struct Cursor *curs, int y, int x)
{
    curs->y = curs->y + y; 
    curs->x = curs->x + x;
    
    move(curs->y, curs->x);
}


void print_at_coor(int y, struct Connection *conn, struct Cursor *curs)
{
    int x = 0;
    int scl = conn->hist->scale;

    if (y <= 0) {
	return;
    }

    else {
	mvset(curs, 1, 0);
        printw("%4d", y);
	addch(ACS_VLINE);

	mvset(curs, 0, 6);
        for (x = 1; x <= conn->hist->cols; ++x) {
	    if (conn->hist->wlen[x] >= y) {
	        addch(ACS_LANTERN);
	    } else {}
	    mvset(curs, 0, 3);
        }
	curs->x = 0;

        print_at_coor(y - scl, conn, curs);
    }
}

	
void Histogram_print(struct Connection *conn, int maxheight, struct Cursor *curs)
{
    int y = conn->hist->rows;
    conn->hist->scale = scale(conn, MAXHEIGHT);   


    // Print the y-axis and all points on the histogram
    move(curs->y, curs->x);
    printw("   Y");
    print_at_coor(y, conn, curs);

    // Print the x-axis
    mvset(curs, 1, 0);

    printw("      ");

    mvset(curs, 0, 2);

    for (y = 0; y < conn->hist->cols; ++y) {
        mvset(curs, 0, 3);

	addch(ACS_HLINE);
	addch(ACS_HLINE);
	addch(ACS_HLINE);
    }

    printw(" X");

    mvset(curs, 1, 0);
    curs->x = 1;

    for (y = 1; y <= conn->hist->cols; ++y) {
        mvset(curs, 0, 3);
	printw("%3d", y);
    }

    mvset(curs, -2, -(curs->x - 6));
}


void curs_seek(struct Cursor *curs, struct Connection *conn, int dir)
{

    switch(dir)
    {	
        case KEY_UP:	mvset(curs, -1, 0);
		    	break;

        case KEY_DOWN:	mvset(curs, 1, 0);
		 	break;

        case KEY_LEFT: 	mvset(curs, 0, -1);
			break;

        case KEY_RIGHT:	mvset(curs, 0, 1);
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

    struct Cursor *curs = malloc(sizeof(struct Cursor));
    curs->y = 0;
    curs->x = 0;

    Histogram_print(conn, MAXHEIGHT, curs);
    int dir;
    while ((dir = getch()) != KEY_F(1)) {
        curs_seek(curs, conn, dir);
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