#include "OggPlayer.h"

#include<signal.h>
#include<unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>

void sig_handler(int signo)
{
	exit(1);
}


int main(int argc, char *argv[]) {

	signal(SIGINT, sig_handler);
	signal(SIGUSR1, sig_handler);
	signal(SIGKILL, sig_handler);
	signal(SIGSTOP, sig_handler);
	
	if  (OggPlayer::init(argv[1], argv[2]) != 0)
		return 1;
	if  (OggPlayer::play() != 0)
		return 1;
	return 0;

}
