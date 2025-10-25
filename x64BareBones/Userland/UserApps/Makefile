include ../Makefile.inc

.PHONY: all clean PongisGolf Terminal

all: PongisGolf Terminal

PongisGolf:
	$(MAKE) -C PongisGolf

Terminal: PongisGolf
	$(MAKE) -C Terminal

clean:
	$(MAKE) -C PongisGolf clean
	$(MAKE) -C Terminal clean
