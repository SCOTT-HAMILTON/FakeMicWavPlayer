#include "FakeLib.h"
#include "CallBacks.h"
#include "FakeLibImplementation.h"
#include "FakeLibUtils.hpp"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

using namespace FakeLibImplementation;
using namespace FakeLibUtils;

class FakeLib
{
public:
	auto move_source_output_port(uint32_t portIndex, uint32_t sourceIndex) {
		move_source_output_port_t infos {
			.success = 0,
			.portIndex = portIndex,
			.sourceIndex = sourceIndex
		};
		commandObjects.push_back(infos);
	}
	auto get_module_list() {
		info_list<module_infos_t> infos;
		reset_info_list(infos);
		commandObjects.push_back(infos);
		return *this;
	}
	auto get_sink_list() {
		info_list<sink_infos_t> infos;
		reset_info_list(infos);
		commandObjects.push_back(infos);
		return *this;
	}
	auto get_source_list() {
		info_list<source_infos_t> infos;
		reset_info_list(infos);
		commandObjects.push_back(infos);
		return *this;
	}
	auto get_source_output_list() {
		info_list<source_output_infos_t> infos;
		reset_info_list(infos);
		commandObjects.push_back(infos);
		return *this;
	}
	auto load_module(const std::string& name, 
			 const std::string& arguments,
			 const std::string& description = ""){
		load_module_infos_t infos = {
			.success = 0,
			.name = name,
			.arguments = arguments,
			.description = description
		};
		commandObjects.push_back(infos);
		return *this;
	}
	auto get_module(uint32_t index) {
		module_infos_t infos = {
			.initialized = false,
			.index  = index
		};
		commandObjects.push_back(infos);
		return *this;
	}
	auto get_sink(uint32_t index) {
		sink_infos_t infos = {
			.initialized = false,
			.index  = index
		};
		commandObjects.push_back(infos);
		return *this;
	}
	auto get_source(uint32_t index) {
		source_infos_t infos = {
			.initialized = false,
			.index  = index
		};
		commandObjects.push_back(infos);
		return *this;
	}
	auto get_source_output(uint32_t index) {
		source_output_infos_t infos = {
			.initialized = false,
			.index  = index
		};
		commandObjects.push_back(infos);
		return *this;
	}

	auto run_commands() {
		return run_pa_commands(commandObjects);
	}

private:
	std::vector<ObjectVariant> commandObjects;

};

int FakeAndPlayWav(std::string fileName, std::string combinedSlavesList,
		std::string sourceProcessBinary) {

	// Placing default values
	if (combinedSlavesList == "")
		combinedSlavesList = defaultCombinedSlavesList;

	if (sourceProcessBinary == "")
		sourceProcessBinary = defaultSourceProcessBinary;

	FakeLib fakeLib;
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
	
	print_module_list(module_list);
	print_sink_list(sink_list);
	print_source_list(source_list);
	print_source_output_list(source_output_list);

	return 0;
}


