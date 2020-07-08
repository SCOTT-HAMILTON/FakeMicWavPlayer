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

	program.add_argument("<Source>")
		.help("The source name to keep recording with");

	program.add_argument("<Process Binary>")
		.help("The binary name of the app to send the sound to.");

	program.add_argument("-f", "--ogg-file")
		.default_value("")
		.help("The ogg audio file to play.");
	program.add_argument("-a", "--application")
		.default_value("")
		.help("The application playing the sound to send");
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
	
	std::string ogg_file, application_name;
	try {
		ogg_file = program.get<std::string>("-f");
	} catch (std::bad_any_cast&){}
	try {
		application_name = program.get<std::string>("-a");
	} catch (std::bad_any_cast&){}
	auto source = program.get<std::string>("<Source>");
	std::string sourceProcessBinary = program.get<std::string>("<Process Binary>");
	std::string combinedSlavesList = program.get<const char*>("-s");
	FakeMicWavPlayer::clean();
	if (ogg_file == "" && application_name == "") {
		std::cerr << "[error] You need to provide either an ogg file to play or a application already playing sound.";
		return 1;
	}
	if (ogg_file != "" && application_name != "") {
		std::cerr << "[error] You can't stream both from an ogg file and an already playing app.";
		return 1;
	}
	if (application_name != "") {
		if (FakeMicWavPlayer::initWithSinkInput(application_name, source, combinedSlavesList,
				sourceProcessBinary) != 0)
			return 1;
		return 0;	
	}
	if (ogg_file != "") {
		if (FakeMicWavPlayer::initWithAudioFile(ogg_file.c_str(), source, combinedSlavesList,
				sourceProcessBinary) != 0)
			return 1;

		if (FakeMicWavPlayer::set_source_volume(90.0) != 0)
			return 1;

		if (FakeMicWavPlayer::set_user_volume(40.0) != 0)
			return 1;
		
		while (FakeMicWavPlayer::playNonBlocking() == 0);
		FakeMicWavPlayer::cleanPlayer();
	}
	FakeMicWavPlayer::clean();
	return 0;
}
