all: lib main.c
	clang -Werror -W -D_DEFAULT_SOURCE -std=c17 `pkg-config --cflags --libs libpulse-simple` -fuse-ld=lld -L. -lFakeLib main.c -o FakeMicWavPlayer

lib: FakeLib.c FakeLib.h player.c player.h
	clang -Werror -W -D_DEFAULT_SOURCE -std=c17 `pkg-config --cflags libpulse-simple` -c player.c FakeLib.c 
	llvm-ar rcs libFakeLib.a player.o FakeLib.o

clean: FakeMicWavPlayer
	rm FakeMicWavPlayer
	rm *.o
	rm *.a

