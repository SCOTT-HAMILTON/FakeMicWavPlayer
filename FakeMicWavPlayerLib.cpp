#include "FakeMicWavPlayerLib.h"
#include "FakeLib.h"
#include "CallBacks.h"
#include "FakeLibImplementation.h"
#include "FakeLibUtils.hpp"
#include "OggPlayer.h"

// Finds a source output in a source output list by its source binary name
auto find_source_output(const info_list<source_output_infos_t>& list,
			const std::string& sourceBinaryName) {
	for (auto info : list) {
		if (info.source_process_binary == sourceBinaryName)
			return info;
	}
	throw ObjectNotFoundError();
}

template <typename user_type>
auto find_by_name(const info_list<user_type>& list,
				  const std::string& name)
{
	for (auto info : list) {
		if (info.name == name)
			return info;
	}
	throw ObjectNotFoundError();
}
template <typename user_type>
auto find_by_index(const info_list<user_type>& list,
				   uint32_t index)
{
	for (auto info : list) {
		if (info.index == index)
			return info;
	}
	throw ObjectNotFoundError();
}

FakeLib FakeMicWavPlayer::fakeLib;
sink_infos_t FakeMicWavPlayer::fakeCombinedSink;

void FakeMicWavPlayer::clean() {
	using namespace FakeLibUtils;
	// Looping until all fakeCombinedMonitor are deleted (up to 10 times max)
	for (int i = 0; i < 10; ++i) {
		auto result = fakeLib
				.clear_commands()
				.get_module_list()
				.run_commands();
		auto module_list = extract<info_list<module_infos_t>>(result);
		module_infos_t fakeCombinedMonitorModule;
		try {
			fakeCombinedMonitorModule = find_by_name(module_list, "module-combine-sink");
		} catch (ObjectNotFoundError) {
			// Good, there is no more "module-null-sink" modules loaded
			break;
		}
		std::cerr << "Deleting\n";
		// deleting the module
		fakeLib
			.clear_commands()
			.unload_module(fakeCombinedMonitorModule.index)
			.run_commands();
	}
	/* print_module_list(module_list); */

}

int FakeMicWavPlayer::init(const std::string& fileName,
		   std::string combinedSlavesList,
		   std::string sourceProcessBinary)
{
	using namespace FakeLibUtils;
	// Placing default values
	if (combinedSlavesList == "default")
		combinedSlavesList = defaultCombinedSlavesList;

	if (sourceProcessBinary == "default")
		sourceProcessBinary = defaultSourceProcessBinary;

	sourceProcessBinary = ".Discord-wrapped";

	auto result = fakeLib
			     .get_module_list()
			     .get_sink_list()
			     .get_source_list()
			     .get_source_output_list()
			     .run_commands();
		
	auto module_list = extract<info_list<module_infos_t>>(result);
	auto sink_list = extract<info_list<sink_infos_t>>(result);
	auto source_list = extract<info_list<source_infos_t>>(result);
	auto source_output_list = extract<info_list<source_output_infos_t>>(result);
	fakeLib.clear_commands();

	std::cerr << "source process binary : " << sourceProcessBinary << '\n';

	// Finding the source source output corresponding to the source process binary passed as argument
	source_output_infos_t process_source_output;
	try {
	 	process_source_output = find_source_output(source_output_list, sourceProcessBinary);
	} catch (ObjectNotFoundError&){
		std::cerr << "Error, source output for `" << sourceProcessBinary << "` not found, exiting\n";
		clean();
		return 1;
	}

	source_infos_t fakeCombinedMonitor;
	try {
		fakeCombinedMonitor = find_by_name(source_list, fakeCombinedMonitorName);
	} catch (ObjectNotFoundError&){
		// Fake Combined Monitor doesn't yet exist, we need to create it
		std::cerr << "Creating fakeCombinedMonitor...\n";
		std::cerr << "Args for fake Combined sink : " << combinedSlavesList << '\n';
		fakeLib.clear_commands();
		auto result = fakeLib
			.load_module("module-combine-sink", std::string("sink_name=fakecombinedsink slaves=")+combinedSlavesList, "The fake combined sink module")
			.run_commands();

		// Checking if the module successfully loaded and thus the fakeCombinedMonitor is available
		result = fakeLib
			.clear_commands()
			.get_source_list()
			.run_commands();
		auto source_list = extract<info_list<source_infos_t>>(result);
		print_source_list(source_list);
		try {
			fakeCombinedMonitor = find_by_name(source_list, fakeCombinedMonitorName);
		} catch (ObjectNotFoundError&) {
			std::cerr << "Failed to create a fake combined sink, exiting\n";
			clean();
			return 1;
		}
	}

	std::cerr << "Fake Combined Monitor Index : " << fakeCombinedMonitor.index << '\n';

	// At this point the fake monitors are setup
	
	// Setting the fake Combined Sink volume to 100%
	result = fakeLib
				.clear_commands()
				.get_sink_list()
				.run_commands();
	sink_list = extract<info_list<sink_infos_t>>(result);
	fakeCombinedSink = find_by_name(sink_list, "fakecombinedsink");

	fakeLib
		.clear_commands()
		.set_sink_volume(fakeCombinedSink.index, 90.0)
		.run_commands();
	
	// Move the found source output port to the fake combined monitor
	result = fakeLib
		.clear_commands()
		.move_source_output_port(fakeCombinedMonitor.index, process_source_output.index)
		.run_commands();
	move_source_output_port_t move_source_output_port_result;
	try {
		move_source_output_port_result = extract<move_source_output_port_t>(result);
	} catch (ObjectNotFoundError&) {
		std::cerr << "Error, couldn't move process source output port to fake combined monitor, exiting\n";
		clean();
		return 1;
	}
	if (move_source_output_port_result.success != 0) {
		std::cerr << "Error, couldn't move process source output port to fake combined monitor, exiting\n";
		clean();
		return 1;
	}
	std::cerr << "Successfully moved the source output of " << sourceProcessBinary << " to fake combined sink\n";

	// Now we need to play
	std::cerr << "Initializing...\n";
	if  (OggPlayer::init(fileName, fakeCombinedSinkName) != 0)
		return 1;
	std::cerr << "Initialized. Playing\n";
	
	// Setting volume of Ogg Player
	result = fakeLib
				.clear_commands()
				.get_sink_input_list()
				.run_commands();
	auto sink_input_list = extract<info_list<sink_input_infos_t>>(result);
	print_sink_input_list(sink_input_list);
	try {
		auto fakePlayerSinkInput = find_by_name(sink_input_list, "Playing Music with Fake");
		fakeLib
			.clear_commands()
			.set_sink_input_volume(fakePlayerSinkInput.index, 100.0) // Setting volume to 100%
			.run_commands();
	} catch (ObjectNotFoundError&) {
		std::cerr << "[error] Couldn't found the Ogg Player, things might not work\n";
	}

	return 0;
}

int FakeMicWavPlayer::play() {
	if  (OggPlayer::play() != 0)
		return 1;

	// Clean Up Time
	clean();

	return 0;
}
int FakeMicWavPlayer::set_volume(double volume) {
	if (volume < 0.0)
		throw NegativeVolumeError();
	if (volume > 100.0)
		throw TooHighVolumeError();

	fakeLib
		.clear_commands()
		.set_sink_volume(fakeCombinedSink.index, volume)
		.run_commands();
	// Clean Up Time
	clean();

	return 0;
}


