/******************************************
*       A better version of hist,         *
**      which reads an input file        **
***     and creates a histogram         ***
**	of word lengths in that file.    ** 
*					  *
*******************************************/



#include <stdio.h>
#include <stdlib.h>

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
	die("Can't open file\n", conn);
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


void print_at_coor(int y, int scl, struct Connection *conn)
{
    int x = 0;

    if (y <= 0) {
	return;
    }

    else {
        printf("%4d|", y);

        for (x = 1; x <= conn->hist->cols; ++x) {
	    if (conn->hist->wlen[x] >= y) {
	        printf(" | ");
	    } else {
	        printf("   ");
	    }
        }
        putchar('\n');

        print_at_coor(y - scl, scl, conn);
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
    
	
void Histogram_print(struct Connection *conn, int maxheight)
{
    int y = conn->hist->rows;
    int scl = scale(conn, MAXHEIGHT);   

    // Print the y-axis and all points on the histogram
    printf("   Y\n");
    print_at_coor(y, scl, conn);

    // Print the x-axis
    printf("     ");
    for (y = 0; y < conn->hist->cols; ++y) {
	printf("---");
    }

    printf(" X");
    printf("\n    ");
    for (y = 1; y <= conn->hist->cols; ++y) {
	printf("%3d", y);
    }
    putchar('\n');
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

    Histogram_print(conn, MAXHEIGHT);

    if(conn) {
        if(conn->file) fclose(conn->file);
        if(conn->hist) free(conn->hist);
        free(conn); 
    }

    return 0;
}
