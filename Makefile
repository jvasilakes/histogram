all:
	gcc hist.c -o hist

curses:
	gcc hist_curses.c -o hc -lncurses


clean:
	if [ -f hist ]; then rm hist; fi
