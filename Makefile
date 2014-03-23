all:
	gcc hist.c -o hist -lncurses


clean:
	if [ -f hist ]; then rm hist; fi
