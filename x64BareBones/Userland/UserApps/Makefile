include ../Makefile.inc

.PHONY: all clean PongisGolf ReadersWriters Terminal

all: PongisGolf ReadersWriters Terminal

PongisGolf:
	$(MAKE) -C PongisGolf

ReadersWriters:
	$(MAKE) -C ReadersWriters

Terminal: PongisGolf ReadersWriters
	$(MAKE) -C Terminal

clean:
	$(MAKE) -C PongisGolf clean
	$(MAKE) -C ReadersWriters clean
	$(MAKE) -C Terminal clean
