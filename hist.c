#include <stdio.h>
#include <stdlib.h>

#define MAX 100
#define MAXHEIGHT 30


void print_at_coor(int y, int wlen[], int largest);
int scale(int most, int height);


main(int argc, char *argv[])
{

    int wlen[MAX];		/* Array of word lengths. E.g. wlen[3] = 5 means 
				   that there are 5 words with 3 characters. */

    int c;
    int y;
    int nc;
    int largest; 		/* Holds character count of largest word found. */
    int most;			/* Holds the number of instances of words of the most common size */
    nc = largest = most = 0;


    if (argc != 2) {
	printf("Usage: histogram.app [filename]\n");
	exit(1);
    }


    for (y = 0; y < MAX ; ++y) {	/* initialize all members to 0. */
	wlen[y] = 0;
    }

    FILE *f1, *fopen();

    if ((f1 = fopen(argv[1], "r")) == NULL) {
	printf("Can't open file %s\n", argv[1]);
	exit(1);
    }

    while ((c = getc(f1)) != EOF) {
	if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
	    if (c != '.' && c != ',' && c != '?' && c != '!' && c != ';' && c != '\'') {
	        ++nc;
	    }
        }

	else {			/* If we do encounter one of the above esc sequences / characters */
	    ++wlen[nc];		/* ++ the member of wlen that corresponds to the word's length. */
	    if (nc > largest) {
		largest = nc;
	    }
	    if (wlen[nc] > most) {
		most = wlen[nc];
	    }
	    nc = 0;		/* reset nc */
	}
    }

    if (most > MAXHEIGHT) {	

	int scl = scale(most, MAXHEIGHT);

        printf("   Y\n");
        for (y = most; y > 0; y -= scl) {
	    print_at_coor(y, wlen, largest);
            putchar('\n');
	}

	printf("   1|");
    	for (y = 0; y < largest; ++y) {
       	     printf(" | ");
    	}
	putchar('\n');

    }

    else {
        printf("   Y\n");
        for (y = most; y > 0; --y) {
	    print_at_coor(y, wlen, largest);
            putchar('\n');
        }
    }

    printf("     ");
    for (y = 0; y < largest; ++y) {
        printf("---");
    }

    printf(" X");
    printf("\n    ");
    for (y = 1; y <= largest; ++y) {
	printf("%3d", y);
    }
    putchar('\n');

    return 0;
}


void print_at_coor(int y, int wlen[], int largest) /* Prints a * at given coordinates  x,y */
{
    int x;

    printf("%4d|", y);
    for (x=1; x<=largest; ++x) {
	if (wlen[x] >= y) {
	    printf(" | ");
	}
	else {
	    printf("   ");
	}
    }
}

int scale(int most, int maxheight)
{
    int i;

    for (i = 2; i < (most / 2); ++i) {
	if ((most % i == 0) && (most / i <= maxheight)) {
	    return i;
	}
	else {}
    }

    return scale((most + 1), maxheight);	// If most is prime, make it not prime and scale again.
}
