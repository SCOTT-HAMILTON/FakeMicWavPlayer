#include "FakeMicWavPlayerLib.h"
#include "help.h"

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

	clean();
	exit(1);
}

int main(int argc, char *argv[]) {

	if (signal(SIGINT, sig_handler) == SIG_ERR)
		std::cerr << "\nCan't catch SIGINT\n";
	if (signal(SIGUSR1, sig_handler) == SIG_ERR)
		std::cerr << "\ncan't catch SIGUSR1\n";
	if (signal(SIGKILL, sig_handler) == SIG_ERR)
		std::cerr << "\ncan't catch SIGKILL\n";
	if (signal(SIGSTOP, sig_handler) == SIG_ERR)
		std::cerr << "\ncan't catch SIGSTOP\n";


	argparse::ArgumentParser program("FakeMicWavPlayer");

	program.add_argument("wav file")
		.help("The wav audio file to play.");

	program.add_argument("process binary")
		.help("The binary name of the app to send the sound to.");

	program.add_argument("-s", "--sink-list")
		.default_value("default")
		.action([](const std::string& str){
				std::string string(str);
				string.erase(std::remove(string.begin(), string.end(), ' '), string.end());
				return string;
				})
	.help("The comma-sepatated list of sinks to play the wav file to.");

	try {
		program.parse_args(argc, argv);
	}
	catch (const std::runtime_error& err) {
		std::cout << err.what() << std::endl;
		std::cout << program;
		return 1;
	}

	auto wav_file = program.get<std::string>("wav file");
	std::string sourceProcessBinary = program.get<std::string>("process binary");
	std::string combinedSlavesList = program.get<const char*>("-s");

	return FakeAndPlayWav(wav_file.c_str(), combinedSlavesList,
			sourceProcessBinary);
}
