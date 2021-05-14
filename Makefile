PKGS=glfw3 glew
CFLAGS=-Wall -Wextra -std=c11 -pedantic `pkg-config --cflags $(PKGS)`
LIBS=`pkg-config --libs $(PKGS)`

mpm: main.c la.c la.h
	$(CC) $(CFLAGS) -o mpm main.c la.c $(LIBS)
