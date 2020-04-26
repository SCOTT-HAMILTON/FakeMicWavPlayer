all: main.c player.c player.h
	clang -D_BSD_SOURCE -std=c17 `pkg-config --cflags --libs libpulse-simple` -fuse-ld=lld -lpthread player.c main.c -o FakeMicWavPlayer
