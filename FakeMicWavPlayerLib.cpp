#include "FakeMicWavPlayerLib.h"
#include "FakeLib.h"
#include "CallBacks.h"
#include "FakeLibImplementation.h"
#include "FakeLibUtils.hpp"
#include "player.h"

#include <exception>

auto find_source(const info_list<source_infos_t>& list,
			const std::string& name) {
	for (auto info : list) {
		if (info.name == name)
			return info;
	}
	throw ObjectNotFoundError();
}
// Finds a source output in a source output list by its source binary name
auto find_source_output(const info_list<source_output_infos_t>& list,
			const std::string& sourceBinaryName) {
	for (auto info : list) {
		if (info.source_process_binary == sourceBinaryName)
			return info;
	}
	throw ObjectNotFoundError();
}
auto find_module(const info_list<module_infos_t>& list,
			const std::string& name) {
	for (auto info : list) {
		if (info.name == name)
			return info;
	}
	throw ObjectNotFoundError();
}

struct FakeLibSingleton {
	static FakeLib fakeLib;
};
FakeLib FakeLibSingleton::fakeLib;

void clean() {
	using namespace FakeLibUtils;
	FakeLib& fakeLib = FakeLibSingleton::fakeLib;
	// Looping until all fakeMonitor are deleted (up to 10 times max)
	for (int i = 0; i < 10; ++i) {
		auto result = fakeLib
				.clear_commands()
				.get_module_list()
				.run_commands();
		auto module_list = extract<info_list<module_infos_t>>(result);
		module_infos_t fakeMonitorModule;
		try {
			fakeMonitorModule = find_module(module_list, "module-null-sink");
		} catch (ObjectNotFoundError) {
			// Good, there is no more "module-null-sink" modules loaded
			break;
		}
		std::cerr << "Deleting\n";
		// deleting the module
		fakeLib
			.clear_commands()
			.unload_module(fakeMonitorModule.index)
			.run_commands();
	}
	// Looping until all fakeCombinedMonitor are deleted (up to 10 times max)
	for (int i = 0; i < 10; ++i) {
		auto result = fakeLib
				.clear_commands()
				.get_module_list()
				.run_commands();
		auto module_list = extract<info_list<module_infos_t>>(result);
		module_infos_t fakeCombinedMonitorModule;
		try {
			fakeCombinedMonitorModule = find_module(module_list, "module-combine-sink");
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

int FakeAndPlayWav(const std::string& fileName,
		   std::string combinedSlavesList,
		   std::string sourceProcessBinary)
{
	using namespace FakeLibUtils;
	// Placing default values
	if (combinedSlavesList == "")
		combinedSlavesList = defaultCombinedSlavesList;

	if (sourceProcessBinary == "")
		sourceProcessBinary = defaultSourceProcessBinary;

	sourceProcessBinary = ".Discord-wrapped";

	FakeLib& fakeLib = FakeLibSingleton::fakeLib;
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
	/* print_module_list(module_list); */
	/* print_sink_list(sink_list); */
	/* print_source_list(source_list); */
	/* print_source_output_list(source_output_list); */

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

	// Checking if the fakeMonitor already exists
	source_infos_t fakeMonitor;
	try {
		fakeMonitor = find_source(source_list, fakeMonitorName);
	} catch (ObjectNotFoundError&){
		// Fake Monitor doesn't yet exist, we need to create it
		std::cerr << "Creating fakeMonitor...\n";
		fakeLib.clear_commands();
		auto result = fakeLib
			.load_module("module-null-sink", "sink_name=fakesink", "The fake sink module")
			.run_commands();

		// Checking if the module successfully loaded and thus the fakeMonitor is available
		result = fakeLib
			.clear_commands()
			.get_source_list()
			.run_commands();
		auto source_list = extract<info_list<source_infos_t>>(result);
		try {
			fakeMonitor = find_source(source_list, fakeMonitorName);
		} catch (ObjectNotFoundError&) {
			std::cerr << "Failed to create a fake sink, exiting\n";
			clean();
			return 1;
		}
	}
	source_infos_t fakeCombinedMonitor;
	try {
		fakeCombinedMonitor = find_source(source_list, fakeCombinedMonitorName);
	} catch (ObjectNotFoundError&){
		// Fake Combined Monitor doesn't yet exist, we need to create it
		std::cerr << "Creating fakeCombinedMonitor...\n";
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
		try {
			fakeCombinedMonitor = find_source(source_list, fakeCombinedMonitorName);
		} catch (ObjectNotFoundError&) {
			std::cerr << "Failed to create a fake combined sink, exiting\n";
			clean();
			return 1;
		}
	}

	std::cerr << "Fake Monitor Index : " << fakeMonitor.index << '\n';
	std::cerr << "Fake Combined Monitor Index : " << fakeCombinedMonitor.index << '\n';

	// At this point the fake monitors are setup
	
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
	play_arguments_t playerArgs = {.fileName = fileName.c_str(),
					       .device = fakeCombinedSinkName};
	play(&playerArgs);

	// Clean Up Time
	clean();

	return 0;
}
