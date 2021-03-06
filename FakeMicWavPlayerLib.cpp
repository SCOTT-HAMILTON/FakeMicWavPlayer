#include "FakeMicWavPlayerLib.h"
#include "FakeLib/FakeLib.h"
#include "FakeLib/CallBacks.h"
#include "FakeLib/FakeLibImplementation.h"
#include "FakeLib/FakeLibUtils.hpp"
#include "OggPlayer.h"

#include <utility>

FakeLib FakeMicWavPlayer::fakeLib;
sink_infos_t FakeMicWavPlayer::fakeCombinedSink;
sink_infos_t FakeMicWavPlayer::sourceLoopbackSink;
sink_infos_t FakeMicWavPlayer::userVolumeControlSink;

void FakeMicWavPlayer::clean() {
	using namespace FakeLibUtils;
	// Looping until all combined sink modules are deleted (up to 10 times max)
	for (int i = 0; i < 10; ++i) {
		auto result = fakeLib
				.clear_commands()
				.get_module_list()
				.run_commands();
		auto module_list = extract<info_list<module_infos_t>>(result);
		module_infos_t module;
		try {
			module = find_by_name(module_list, "module-combine-sink");
		} catch (ObjectNotFoundError&) {
			// Good, there is no more "module-null-sink" modules loaded
			break;
		}
		std::cerr << "Deleting combined sink module\n";
		// deleting the module
		fakeLib
			.clear_commands()
			.unload_module(module.index)
			.run_commands();
	}
	// Looping until all null sink modules are deleted (up to 10 times max)
	for (int i = 0; i < 10; ++i) {
		auto result = fakeLib
				.clear_commands()
				.get_module_list()
				.run_commands();
		auto module_list = extract<info_list<module_infos_t>>(result);
		module_infos_t module;
		try {
			module = find_by_name(module_list, "module-null-sink");
		} catch (ObjectNotFoundError&) {
			// Good, there is no more "module-null-sink" modules loaded
			break;
		}
		std::cerr << "Deleting null sink module\n";
		// deleting the module
		fakeLib
			.clear_commands()
			.unload_module(module.index)
			.run_commands();
	}
	// Looping until all sourceLoopbackSinkName is deleted (up to 10 times max)
	for (int i = 0; i < 10; ++i) {
		auto result = fakeLib
				.clear_commands()
				.get_module_list()
				.run_commands();
		auto module_list = extract<info_list<module_infos_t>>(result);
		module_infos_t module;
		try {
			module = find_by_name(module_list, "module-loopback");
		} catch (ObjectNotFoundError&) {
			// Good, there is no more "module-null-sink" modules loaded
			break;
		}
		std::cerr << "Deleting loopback module\n";
		// deleting the module
		fakeLib
			.clear_commands()
			.unload_module(module.index)
			.run_commands();
	}
}

static bool load_module(
		const std::string& module,
		std::string args,
		const std::string& description,
		std::string name = ""
	)
{
	if (module == "module-null-sink"
		|| module == "module-combine-sink"
		) {
		auto dev_desc = description;
		std::replace(std::begin(dev_desc), std::end(dev_desc), ' ', '_');
		std::replace(std::begin(dev_desc), std::end(dev_desc), '.', '-');
		args += " sink_properties=device.description="+dev_desc;
	}
	std::cerr << "[log] Load " << module << " args for " << description << " : `" << args << "`\n";
	using namespace FakeLibUtils;
	auto result = FakeMicWavPlayer::fakeLib
		.clear_commands()
		.load_module(module,
					 args,
					 description)
		.run_commands();
	auto load_module_infos = extract<load_module_t>(result);
	if (!load_module_infos.success) {
		std::cerr << "Couldn't load " << description << ", exitting\n";
		return true;
	}
	return false;
}

template<typename user_type>
static auto get_info_from_load_module_and_check(
		const std::string& module,
		const std::string& args,
		const std::string& description,
		const std::string& nameToCheck,
		std::function<
			decltype(std::declval<FakeLib>().run_commands())()
		> fetch_info_list_func) {
	using namespace FakeLibUtils;
	auto result = fetch_info_list_func();
	auto list = extract<info_list<user_type>>(result);
	user_type info;
	try {
		info  = find_by_name(list, nameToCheck);
	} catch (ObjectNotFoundError&) {
		// Doesn't exist, we create it with the give module and module args
		if (load_module(
					module,
					args,
					description,
					nameToCheck
					)) {
			throw CantLoadModuleError();
		}
		result = fetch_info_list_func();
		list = extract<info_list<user_type>>(result);
		try {
			info  = find_by_name(list, nameToCheck);
		} catch (ObjectNotFoundError&) {
			// Still doesn't exist, exitting
			std::cerr << "[error] Couldn't load the module " << module << " for " << description << '\n';
			throw CantLoadModuleError();
		}
	}
	return info;
}

static auto get_sink_from_load_module_and_check(
		const std::string& module,
		const std::string& args,
		const std::string& description,
		const std::string& nameToCheck) {
	return get_info_from_load_module_and_check<sink_infos_t>(
		module,
		args,
		description,
		nameToCheck,
		[]() {
			return FakeMicWavPlayer::fakeLib
					.clear_commands()
					.get_sink_list()
					.run_commands();
		}
	);
}

static auto get_source_from_load_module_and_check(
		const std::string& module,
		const std::string& args,
		const std::string& description,
		const std::string& nameToCheck) {
	return get_info_from_load_module_and_check<source_infos_t>(
		module,
		args,
		description,
		nameToCheck,
		[]() {
			return FakeMicWavPlayer::fakeLib
					.clear_commands()
					.get_source_list()
					.run_commands();
		}
	);
}

int FakeMicWavPlayer::init(
		   std::string source,
		   std::string combinedSlavesList,
		   std::string sourceProcessBinary)
{
	using namespace FakeLibUtils;
	// Placing default values
	if (combinedSlavesList == "default")
		combinedSlavesList = defaultCombinedSlavesList;

	if (sourceProcessBinary == "default")
		sourceProcessBinary = defaultSourceProcessBinary;

	// Finding the source source output corresponding to the source process binary passed as argument
	source_output_infos_t process_source_output;
	info_list<source_output_infos_t> source_output_list;
	try {
		fakeLib.clear_commands();
		fakeLib.get_source_output_list();
		auto result = fakeLib.run_commands();
		source_output_list = extract<info_list<source_output_infos_t>>(result);
	 	process_source_output = find_by_process_binary(source_output_list, sourceProcessBinary);
	} catch (ObjectNotFoundError&){
		print_source_output_list(source_output_list);
		std::cerr << "[error] Source output for `" << sourceProcessBinary << "` not found, exiting\n";
		clean();
		return 1;
	}

	// Creating ource LoopBack Mixer sink if doesn't exist
	source_infos_t sourceLoopBackMixerMonitor;
	try {
		sourceLoopBackMixerMonitor = get_source_from_load_module_and_check(
				"module-null-sink",
				std::string("sink_name=")+sourceLoopBackMixerSinkName,
				"The Null sink for source loopback mixer",
				sourceLoopBackMixerMonitorName);
	} catch (CantLoadModuleError&) {
		auto result = fakeLib
						.clear_commands()
						.get_sink_list()
						.run_commands();
		auto sink_list = extract<info_list<sink_infos_t>>(result);
		print_sink_list(sink_list);
		clean();
		return 1;
	}
	// Creating loopback sink if doesn't exist
	try {
		sourceLoopbackSink = get_sink_from_load_module_and_check(
				"module-combine-sink",
				std::string("sink_name=")+sourceLoopbackSinkName+std::string(" slaves=")+sourceLoopBackMixerSinkName,
				"The Combined Sink for source loopback",
				sourceLoopbackSinkName);
	} catch (CantLoadModuleError&) {
		clean();
		return 1;
	}
	{
		auto result = fakeLib
						.clear_commands()
						.get_source_list()
						.run_commands();
		auto list = extract<info_list<source_infos_t>>(result);
		print_source_list(list);
	}
	if (load_module("module-loopback",
					 std::string("source=")+source+std::string(" sink=")+sourceLoopBackMixerSinkName,
					 "The source loopback module")) {
		// Couldn't load the module
		clean();
		return 1;
	}

	// Creating user volume control sink if doesn't exist
	try {
		auto args = std::string("sink_name=")+userVolumeControlSinkName+std::string(" slaves=")+combinedSlavesList;
		userVolumeControlSink = get_sink_from_load_module_and_check(
			"module-combine-sink",
			args,
			"The Fake Sink to control the user volume",
			userVolumeControlSinkName
		);
	} catch (CantLoadModuleError&) {
		clean();
		return 1;
	}
	// Creating fake combined sink if doesn't exist
	try {
		auto args = std::string("sink_name=")+fakeCombinedSinkName+std::string(" slaves=")+sourceLoopbackSinkName+std::string(",")+userVolumeControlSinkName;
		fakeCombinedSink = get_sink_from_load_module_and_check(
			"module-combine-sink",
			args,
			"The fake combined sink module",
			fakeCombinedSinkName
		);
	} catch (CantLoadModuleError&) {
		clean();
		return 1;
	}
	std::cerr << "[log] Fake Combined Sink Index  : " << fakeCombinedSink.index << '\n';
	std::cerr << "[log] Source Loopback Index : " << sourceLoopbackSink.index << '\n';
	std::cerr << "[log] User Volume Control Sink Index : " << userVolumeControlSink.index << '\n';
	std::cerr << "[log] Source Loop Back Mixer Monitor Source  Index : " << sourceLoopBackMixerMonitor.index << '\n';

	// At this point the application is ready to play

	// Setting the Fake Combined Sink volume to 100%
	{
		auto result = fakeLib
					.clear_commands()
					.get_sink_list()
					.run_commands();
		auto sink_list = extract<info_list<sink_infos_t>>(result);
		auto sink = find_by_name(sink_list, fakeCombinedSinkName);
		fakeLib
			.clear_commands()
			.set_sink_volume(sink.index, 100.0)
			.run_commands();
	}
	// Setting the Source Loopback Sink volume to 100%
	{
		auto result = fakeLib
					.clear_commands()
					.get_sink_list()
					.run_commands();
		auto sink_list = extract<info_list<sink_infos_t>>(result);
		auto sink = find_by_name(sink_list, sourceLoopbackSinkName);
		fakeLib
			.clear_commands()
			.set_sink_volume(sink.index, 100.0)
			.run_commands();
	}
	// Setting the Source Loopback Mixer Sink volume to 100%
	{
		auto result = fakeLib
					.clear_commands()
					.get_sink_list()
					.run_commands();
		auto sink_list = extract<info_list<sink_infos_t>>(result);
		auto sink = find_by_name(sink_list, sourceLoopBackMixerSinkName);
		fakeLib
			.clear_commands()
			.set_sink_volume(sink.index, 100.0)
			.run_commands();
	}
	// Setting the User Volume Control Sink volume to 9%
	{
		auto result = fakeLib
					.clear_commands()
					.get_sink_list()
					.run_commands();
		auto sink_list = extract<info_list<sink_infos_t>>(result);
		auto sink = find_by_name(sink_list, userVolumeControlSinkName);
		fakeLib
			.clear_commands()
			.set_sink_volume(sink.index, 0.1)
			.run_commands();
	}

	// Move the found source output port to the fake combined monitor
	{
		auto result = fakeLib
		.clear_commands()
		.move_source_output_port(process_source_output.index, sourceLoopBackMixerMonitor.index)
		.run_commands();
		move_source_output_port_t move_source_output_port_result;
		try {
			move_source_output_port_result = extract<move_source_output_port_t>(result);
			if (move_source_output_port_result.success == 0) {
				throw ObjectNotFoundError();
			}
		} catch (ObjectNotFoundError&) {
			std::cerr << "[error] couldn't move process source output port to monitor source at " << sourceLoopBackMixerMonitor.index << ", exiting\n";
			clean();
			return 1;
		}
		if (move_source_output_port_result.success == 0) {
			std::cerr << "[error] couldn't move process source output port to monitor source at " << sourceLoopBackMixerMonitor.index << ", exiting\n";
			clean();
			return 1;
		}
		std::cerr << "[log] Successfully moved the source output of " << sourceProcessBinary << " to " << sourceLoopBackMixerMonitor.index << '\n';
	}
	return 0;
}

int FakeMicWavPlayer::initWithAudioFile(const std::string& fileName,
										std::string source,
										std::string combinedSlavesList,
										std::string sourceProcessBinary)
{
	using namespace FakeLibUtils;
	// Init
	if (init(source, combinedSlavesList, sourceProcessBinary) != 0) {
		return 1;
	}

	// Now we need to play
	std::cerr << "Initializing...\n";
	if  (OggPlayer::init(fileName, fakeCombinedSinkName) != 0)
		return 1;
	std::cerr << "Initialized. Playing\n";
	
	// Setting volume of Ogg Player
	{
		auto result = fakeLib
					.clear_commands()
					.get_sink_input_list()
					.run_commands();
		auto sink_input_list = extract<info_list<sink_input_infos_t>>(result);
		try {
			auto fakePlayerSinkInput = find_by_name(sink_input_list, "Playing Music with Fake");
			fakeLib
				.clear_commands()
				.set_sink_input_volume(fakePlayerSinkInput.index, 100.0) // Setting volume to 100%
				.run_commands();
		} catch (ObjectNotFoundError&) {
			std::cerr << "[error] Couldn't found the Ogg Player, things might not work\n";
		}
	}

	return 0;
}
int FakeMicWavPlayer::initWithSinkInput(const std::string& sinkInputName,
										std::string source,
										std::string combinedSlavesList,
										std::string sourceProcessBinary)
{
	using namespace FakeLibUtils;

	sink_input_infos_t sink_input;
	{
		auto result = fakeLib
						.clear_commands()
						.get_sink_input_list()
						.run_commands();
		auto sink_input_list = extract<info_list<sink_input_infos_t>>(result);
		// Finding sink input
		try {
			sink_input = find_by_process_binary(sink_input_list, sinkInputName);
		} catch (ObjectNotFoundError&) {
			std::cerr << "[error] Sink Input `" << sinkInputName << "` not found, exitting\n";
			return 1;
		}
	}
	std::cerr << "[log] Sink Input `" << sinkInputName << "` Index : " << sink_input.index << '\n';

	// Init
	if (init(source, combinedSlavesList, sourceProcessBinary) != 0) {
		return 1;
	}

	// Move application audio stream to the fake combined sink
	{
		auto result = fakeLib
						.clear_commands()
						.move_sink_input_port(sink_input.index, fakeCombinedSink.index)
						.run_commands();
		try {
			auto move_result = extract<move_sink_input_port_t>(result);
			if (move_result.success == 0) {
				throw ObjectNotFoundError();
			}
		} catch (ObjectNotFoundError&) {
			std::cerr << "[error] couldn't move `" << sinkInputName << "` to fakeCombinedSink, exitting\n";
			clean();
			return 1;
		}
	}
	
	{
		auto result = fakeLib
						.clear_commands()
						.get_sink_input_list()
						.run_commands();
		auto sink_input_list = extract<info_list<sink_input_infos_t>>(result);
		sink_input_infos_t sink_input;
		try {
			sink_input = find_by_process_binary(sink_input_list, sinkInputName);
		} catch (ObjectNotFoundError&) {
			// Should not happen because it has already been found above...
			std::cerr << "[error] Sink Input `" << sinkInputName << "` not found, exitting\n";
			return 1;
		}
		if (sink_input.sink != fakeCombinedSink.index) {
			std::cerr << "[error] Failed to move Sink Input `" << sinkInputName << "` of Index: " << sink_input.index << " to Fake Combined Sink of Index: " << fakeCombinedSink.index << '\n';
			print_sink_input_list(sink_input_list);
			return 1;
		}
	}
	std::cerr << "[log] Successfully moved the application `" << sinkInputName << " to the fake combined sink" << '\n';

	return 0;
}

int FakeMicWavPlayer::playNonBlocking() {
	return OggPlayer::playNonBlocking();
}
int FakeMicWavPlayer::cleanPlayer() {
	return OggPlayer::clean();
}

int FakeMicWavPlayer::set_source_volume(double volume) {
	if (volume < 0.0)
		throw NegativeVolumeError();
	if (volume > 200.0)
		throw TooHighVolumeError();

	fakeLib
		.clear_commands()
		.set_sink_volume(sourceLoopbackSink.index, volume)
		.run_commands();
	return 0;
}
int FakeMicWavPlayer::set_user_volume(double volume) {
	if (volume < 0.0)
		throw NegativeVolumeError();
	if (volume > 200.0)
		throw TooHighVolumeError();

	fakeLib
		.clear_commands()
		.set_sink_volume(userVolumeControlSink.index, volume)
		.run_commands();
	return 0;
}
