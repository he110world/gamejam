CC = gcc -Wall -ansi -g

all:
	$(CC) gamejam.cpp filewatcher/FileWatcher.cpp filewatcher/FileWatcherLinux.cpp stb_image.cpp -I. -o gamejam -Ifilewatcher -ltcc -ldl -lGL -lGLU `sdl-config --cflags --libs` -lcurl

clean:
	@echo Cleaning up...
	@rm lesson03
	@echo Done.
