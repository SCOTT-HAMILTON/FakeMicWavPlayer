# LIBS=-lm -lpthread -L/usr/local/lib64 -ldbus-1 -lpulse -lpulsecommon-13.99 -lpulsecore-13.99 -lpulse-mainloop-glib  -fuse-ld=lld -L. 
INCLUDES=-W -D_DEFAULT_SOURCE -std=c17 `pkg-config --cflags libpulse-simple`
LIBS=-L/usr/lib64 -lpulse-simple -lpulse -fuse-ld=lld -L.
CC=clang


all: lib main.c
	${CC} ${INCLUDE} ${LIBS} libFakeLib.a  main.c -o FakeMicWavPlayer
	${CC} -g ${INCLUDE} ${LIBS} libFakeLib-d.a main.c -o FakeMicWavPlayer_g

lib: FakeLib.c FakeLib.h player.c player.h
	${CC} ${INCLUDES} -c player.c FakeLib.c 
	llvm-ar rcs libFakeLib.a player.o FakeLib.o
	${CC} -fPIC ${INCLUDES} -c player.c FakeLib.c 
	${CC} -shared player.o FakeLib.o -o libFakeLib.so
	${CC} -g ${INCLUDES} -c player.c FakeLib.c 
	llvm-ar rcs libFakeLib-d.a player.o FakeLib.o
	${CC} -g -fPIC ${INCLUDES} -c player.c FakeLib.c 
	${CC} -shared player.o FakeLib.o -o libFakeLib-d.so

install:
	install libFakeLib.a ${INSTALL_PREFIX}/lib
	install libFakeLib-d.a ${INSTALL_PREFIX}/lib
	install libFakeLib.so ${INSTALL_PREFIX}/lib
	install libFakeLib-d.so ${INSTALL_PREFIX}/lib
	install FakeLib.h ${INSTALL_PREFIX}/include
	install player.h ${INSTALL_PREFIX}/include

clean: FakeMicWavPlayer
	rm FakeMicWavPlayer
	rm FakeMicWavPlayer_g
	rm *.o
	rm *.a
	rm *.so



