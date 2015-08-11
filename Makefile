all:
	gcc main.c -o autocable `pkg-config --cflags --libs jack`
