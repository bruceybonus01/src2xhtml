IDIR=include/

build:
	$(CC) -std=c89 -Wall -I$(IDIR) -o src2xhtml src/*.c

clean:
	rm src2xhtml
