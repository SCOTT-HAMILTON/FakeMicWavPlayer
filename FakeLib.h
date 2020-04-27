#ifndef FAKE_LIB_H
#define FAKE_LIB_H

#include "player.h"

#include <pulse/pulseaudio.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

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
typedef struct source_infos {
	uint8_t initialized;
	char name[512];
	uint32_t index;
	char description[256];
} source_infos_t;

typedef struct source_output_infos {
	uint8_t initialized;
	char name[512];
	uint32_t index;
	uint32_t source;
	const char *source_process_binary;
} source_output_infos_t;

typedef struct module_infos {
	uint8_t initialized;
	char name[512];
	uint32_t index;
} module_infos_t;

typedef struct load_module_infos {
	int success;
	char module_name[512];
	char module_args[1024];
	/* arbitrary string to know what this module was loaded for */
	char description[512];
} load_module_infos_t;

typedef struct fake_sink {
	int exists;
	uint32_t index;
} fake_sink_t;

void state_cb(pa_context *c, void *userdata);
void source_infos_cb(pa_context *c, const pa_source_info *l, int eol,
		     void *userdata);
void source_output_infos_cb(pa_context *c, const pa_source_output_info *l,
			    int eol, void *userdata);
void module_infos_cb(pa_context *c, const pa_module_info *l, int eol,
		     void *userdata);
void load_module_cb(pa_context *c, uint32_t index, void *userdata);
void move_source_output_port_success(pa_context *c, int success,
				     void *userdata);

int get_sources_source_output_and_modules(source_infos_t *sources,
					  source_output_infos_t *source_outputs,
					  module_infos_t *module);
int move_source_output_port(uint32_t sourceIndex, uint32_t portIndex);
int load_module(load_module_infos_t *load_module_infos);

#endif //FAKE_LIB_H
