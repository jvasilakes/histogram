all:
	gcc hist.c -o hist -lm -lncurses


clean:
	if [ -f hist ]; then rm hist; fi
