#include "FakeMicWavPlayerLib.h"

#include <argparse/argparse.hpp>

#include <algorithm>
#include <iostream>
#include <string>
#include <cctype>

#include<signal.h>
#include<unistd.h>

void sig_handler(int signo)
{
	if (signo == SIGINT)
		std::cerr << "received SIGINT\n";
	if (signo == SIGUSR1)
		std::cerr << "received SIGUSR1\n";
	else if (signo == SIGKILL)
		std::cerr << "received SIGKILL\n";
	else if (signo == SIGSTOP)
		std::cerr << "received SIGSTOP\n";

	FakeMicWavPlayer::clean();
	exit(1);
}

int main(int argc, char *argv[]) {

	signal(SIGINT, sig_handler);
	signal(SIGUSR1, sig_handler);
	signal(SIGKILL, sig_handler);
	signal(SIGSTOP, sig_handler);


	argparse::ArgumentParser program("FakeMicWavPlayer");

	program.add_argument("<Ogg File>")
		.help("The ogg audio file to play.");

	program.add_argument("<Process Binary>")
		.help("The binary name of the app to send the sound to.");

	program.add_argument("-s", "--sink-list")
		.default_value("default")
		.action([](const std::string& str){
				std::string string(str);
				string.erase(std::remove(string.begin(), string.end(), ' '), string.end());
				return string;
				})
	.help("The comma-sepatated list of sinks to play the ogg file to.");

	try {
		program.parse_args(argc, argv);
	}
	catch (const std::runtime_error& err) {
		std::cout << err.what() << std::endl;
		std::cout << program;
		return 1;
	}

	auto ogg_file = program.get<std::string>("<Ogg File>");
	std::string sourceProcessBinary = program.get<std::string>("<Process Binary>");
	std::string combinedSlavesList = program.get<const char*>("-s");

	if (FakeMicWavPlayer::init(ogg_file.c_str(), combinedSlavesList,
			sourceProcessBinary) != 0)
		return 1;

	if (FakeMicWavPlayer::set_volume(90.0) != 0)
		return 1;
	
	while (FakeMicWavPlayer::playNonBlocking() == 0);
	FakeMicWavPlayer::cleanPlayer();
	FakeMicWavPlayer::clean();
}
