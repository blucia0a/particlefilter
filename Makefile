all:
	gcc particlefilter.c -o pf.exe

clean:
	-rm pf.exe
