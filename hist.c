4******************************************
*                  HIST                   *
**          Reads an input file          **
***       and creates a histogram       ***
**      of word lengths in that file.    ** 
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
    int lspace;
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
	if (isalpha(c)) ++wc;

	else if (wc != 0) {
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
    maxheight = (maxheight * 2) / 3;
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


int digits(int num)
{
    int i = 0;
    
    while (num != 0) {
	num = num / 10;
	i++;
    }

    return i;
}  


void print_at_coor(int y, int start, struct Connection *conn, struct Cursor *curs)
{
    int x = 0;
    int scl = conn->hist->scale;

    if (y <= 0) {
	return;
    }

    else {
	int nstart = digits(y);

	mvset(curs, 1, (start - nstart));
        printw("%d", y);
	addch(ACS_VLINE);

	mvset(curs, 0, nstart + 2);
        for (x = 1; x <= conn->hist->cols; ++x) {
	    if (conn->hist->wlen[x] >= y) {
	        addch(ACS_DIAMOND);
	    } else {}
	    mvset(curs, 0, 3);
        }
	curs->x = 0;

        print_at_coor(y - scl, start, conn, curs);
    }
}

	
void Histogram_print(struct Connection *conn, int maxheight, struct Cursor *curs)
{
    int y = conn->hist->rows;
    conn->hist->scale = scale(conn, MAXHEIGHT);   
    int space = conn->hist->lspace = digits(y);


    // Print the y-axis and all points on the histogram
    move(2, space);
    printw("Y");
    mvset(curs, 2, 0);
    print_at_coor(y, space, conn, curs);

    // Print the x-axis
    mvset(curs, 1, space);
    addch('+');

    mvset(curs, 0, 1);
    for (y = 0; y < conn->hist->cols; ++y) {

	addch(ACS_HLINE);
	addch(ACS_HLINE);
	addch(ACS_HLINE);

        mvset(curs, 0, 3);
    }

    printw(" X");

    //mvset(curs, 1, 0);
    curs->x = 0;

    mvset(curs, 1, space + 2);
    for (y = 1; y <= conn->hist->cols; ++y) {
	printw("%d", y);
        mvset(curs, 0, 3);
    }

    mvset(curs, -2, -(curs->x - 6));
}


void curs_seek(struct Cursor *curs, struct Connection *conn, int dir)
{
    switch(dir)
    {	
        case KEY_UP:    if (curs->y == 3) break;
			else {
			    mvset(curs, -1, 0);
	    		    break;
			}

        case KEY_DOWN:  if (curs->y == ((conn->hist->rows / conn->hist->scale)  + 4)) {
			    break;	
			}
			else {
			    mvset(curs, 1, 0);
	 		    break;
			}

        case KEY_LEFT:  if (curs->x == (conn->hist->lspace - 1)) break;	
			else {
			    mvset(curs, 0, -1);
			    break;
			}

        case KEY_RIGHT: if (curs->x == conn->hist->lspace +(conn->hist->cols * 3)) {
		 	    break;	
			}
			else {
			    mvset(curs, 0, 1);
			    break;
			}
    }

    mvprintw(30, 5, "[%d, %d]", curs->y, curs->x);
    move(curs->y, curs->x);
    refresh();
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

    mvprintw(0, 0, "%s", argv[1]);

    Histogram_print(conn, MAXHEIGHT, curs);

    int dir;
    while ((dir = getch()) != KEY_F(1)) {
        curs_seek(curs, conn, dir);
    }
    endwin();

    if(conn) {
        if(conn->file) fclose(conn->file);
        if(conn->hist) free(conn->hist);
        free(conn); 
    }

    return 0;
}
