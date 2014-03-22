all:
	gcc hist.c -o hist

curses:
	gcc hist_curses.c -o hc -lncurses


clean:
	rm hist
	rm hc
