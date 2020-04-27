all: main.c player.c player.h
	clang -Werror -W -D_BSD_SOURCE -std=c17 `pkg-config --cflags --libs libpulse-simple` -fuse-ld=lld player.c main.c -o FakeMicWavPlayer
clean: FakeMicWavPlayer
	rm FakeMicWavPlayer
