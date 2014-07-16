/*******************************************
*                  HIST                   *
**          Reads an input file          **
***       and creates a histogram       ***
**      of word lengths in that file.    ** 
*					  *
*******************************************/

#include "main.h"



struct Histogram {
    int rows;
    int cols;
    int wlen[MAX];
    int scale;

    //Number of digits to the left of the Y-axis
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

	// Only count letters, no numbers, punctuation, etc.
	if (isalpha(c)) {
		++wc;
	}

	else { 
	    if (wc > 0) {
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
}


// Return number of digits in a number (e.g. digits(1223) = 4)
int digits(int num)
{
    int i = 0;
    
    while (num != 0) {
	num = num / 10;
	i++;
    }

    return i;
}  


/* Find by what number the y axis 
 (number of occurrances of words 
 of a certain length) ought to be scaled */
int scale(struct Connection *conn, int maxheight)
{
    int len = 0;
    int scale = 1;
    int orig_scale = 1;
    int rows = conn->hist->rows;

    len = digits(conn->hist->rows);

    if (len == 0 || len == 1) return 1;

    scale = orig_scale = pow(10, (len-2));

    while ((rows / scale) > maxheight) {
	scale = scale + orig_scale;
    }

    return scale;
}


// Set the cursor struct and move cursor
void mvset(struct Cursor *curs, int y, int x)
{
    curs->y = curs->y + y; 
    curs->x = curs->x + x;
    
    move(curs->y, curs->x);
}




// Print a diamond at a given y, x coordinate
void print_at_coor(int y, int start, struct Connection *conn, struct Cursor *curs)
{
    int x = 0;
    int scl = conn->hist->scale;

    if (y <= 0) {
	return;
    }

    else {
	/* Figure out where to start printing the y-axis indices.
	   to ensure a straight y-axis. */
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
    conn->hist->scale = scale(conn, MAXHEIGHT);   
    //int y = (conn->hist->rows / conn->hist->scale) * conn->hist->scale;
    int y = roundto(conn->hist->rows, conn->hist->scale);
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

    curs->x = 0;

    mvset(curs, 1, space + 2);
    for (y = 1; y <= conn->hist->cols; ++y) {
	printw("%d", y);
        mvset(curs, 0, 3);
    }

    mvset(curs, -2, -(curs->x - 6));
}


// Used to move the cursor around the graph using the arrow keys.
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

    mvprintw(35,
	     conn->hist->lspace,
	     "%d, %d ",
	     roundto(conn->hist->rows - (conn->hist->scale * (curs->y - 3)), conn->hist->scale),
	     (curs->x - (conn->hist->lspace-2)) / 3
	    );
    move(curs->y, curs->x);
    refresh();
}


main(int argc, char *argv[])
{
    if (argc != 2) {
	printf("USAGE: hist [file]\n");
	exit(1);
    }

    // Build the connection b/n file and histogram struct
    struct Connection *conn = malloc(sizeof(struct Connection));
    conn->hist = malloc(sizeof(struct Histogram));
    conn->hist->rows = 0;
    conn->hist->cols = 0;

    // Initialize wlen[] to all 0's.
    int y;
    for (y = 0; y < MAX; ++y) {
	conn->hist->wlen[y] = 0;
    }

    /* Open and read the text file given
       as an argument into the histogram struct. */
    File_open(argv[1], conn);
    File_read(conn);

    // Start the curses screen
    initscr();
    keypad(stdscr, TRUE);
    raw();
    noecho();

    struct Cursor *curs = malloc(sizeof(struct Cursor));
    curs->y = 0;
    curs->x = 0;

    // Print the filename above the histogram.
    mvprintw(0, 0, "%s", argv[1]);

    // Print the key
    mvprintw(2, (conn->hist->cols*3)+11, "-- KEY --");
    mvprintw(4, (conn->hist->cols*3)+7, "X: Length in characters");
    mvprintw(5, (conn->hist->cols*3)+7, "Y: Number of occurrences");

    // Print the histogram.
    Histogram_print(conn, MAXHEIGHT, curs);

    // Allow user to move cursor around the graph.
    int dir;
    while ((dir = getch()) != 'q') {
        curs_seek(curs, conn, dir);
    }

    /* Once the user presses 'q', close the curses
       windows and free all structs. */
    endwin();

    if(conn) {
        if(conn->file) fclose(conn->file);
        if(conn->hist) free(conn->hist);
        free(conn); 
    }

    if(curs) free(curs);

    return 0;
}
