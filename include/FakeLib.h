#ifndef FAKE_LIB_H
#define FAKE_LIB_H

#include "player.h"

#include <pulse/pulseaudio.h>
#include <pulse/operation.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <string>
#include <array>
#include <exception>

// User defined control fields
static const char *defaultSourceProcessBinary = "Discord";
static const char *fakeMonitorName = "TestSink.monitor";
static const char *fakeCombinedMonitorName = "TestCombinedSink.monitor";
static const char *fakeSinkName = "TestSink";
static const char *fakeCombinedSinkName = "TestCombinedSink";
static const char *defaultCombinedSlavesList =
    "alsa_output.usb-Sennheiser_Sennheiser_SC60_for_Lync_55ef416267-00.analog-"
    "stereo";

// Field list is here:
// http://0pointer.de/lennart/projects/pulseaudio/doxygen/structpa__sink__info.html

struct ObjectNotFoundInResultsError : public std::exception
{
	const char * what () const throw ()
	{ 
		return "Failed to found object in results";
	}
};

typedef struct move_source_ouptput_port {
	int success;
	uint32_t portIndex;
	uint32_t sourceIndex;
} move_source_output_port_t;

typedef struct load_module_infos {
	int success;
	std::string name;
	std::string arguments;
	/* arbitrary string to know what this module was loaded for */
	std::string description;
} load_module_infos_t;

typedef struct module_infos {
	bool initialized;
	std::string name;
	uint32_t index;
} module_infos_t;

typedef struct sink_infos {
	bool initialized;
	std::string name;
	uint32_t index;
	std::string description;
} sink_infos_t;

typedef struct source_infos {
	bool initialized;
	std::string name;
	uint32_t index;
	std::string description;
} source_infos_t;

typedef struct source_output_infos {
	bool initialized;
	std::string name;
	uint32_t index;
	uint32_t source;
	std::string source_process_binary;
} source_output_infos_t;

typedef struct unload_module_infos {
	int success;
	uint32_t index;
} unload_module_infos_t;

typedef struct fake_sink {
	int exists;
	uint32_t index;
} fake_sink_t;

typedef struct success_info {
	int success;
} success_info_t;

template<typename info_type>
using info_list = std::array<info_type, 16>;

int move_source_output_port(uint32_t sourceIndex, uint32_t portIndex);
int load_module(load_module_infos_t *load_module_infos);
int unload_module(load_module_infos_t *load_module_infos);
int FakeAndPlayWav(std::string fileName, std::string sinks,
		   std::string processBinaryName);

#endif // FAKE_LIB_H
