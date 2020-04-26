#include "player.h"

#include <pthread.h>
#include <pulse/pulseaudio.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// User defined control fields
static const char* sourceProcessBinary 		= "Discord";
static const char* fakeMonitorName	 	= "TestSink.monitor";
static const char* fakeCombinedMonitorName 	= "TestCombinedSink.monitor";
static const char* fakeSinkName			= "TestSink";
static const char* fakeCombinedSinkName		= "TestCombinedSink";
static const char* defaultCombinedSlavesList	= "alsa_output.usb-Sennheiser_Sennheiser_SC60_for_Lync_55ef416267-00.analog-stereo";

// Field list is here: http://0pointer.de/lennart/projects/pulseaudio/doxygen/structpa__sink__info.html
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
void source_infos_cb(pa_context *c, const pa_source_info *l, int eol, void *userdata);
void source_output_infos_cb(pa_context *c, const pa_source_output_info *l, int eol, void *userdata);
void module_infos_cb(pa_context *c, const pa_module_info* l, int eol, void *userdata);
void load_module_cb(pa_context *c, uint32_t index, void *userdata);
void move_source_output_port_success(pa_context *c, int success, void *userdata);

int get_sources_source_output_and_modules(source_infos_t *sources, source_output_infos_t *source_outputs, module_infos_t* module);
int move_source_output_port(uint32_t sourceIndex, uint32_t portIndex);
int load_module(load_module_infos_t* load_module_infos);

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf (stderr, "Error, 1 argument needed : the wav file to play");
		return 1;
	}
	char combinedSlavesList[1024];
	if (argc > 2) {
		strcpy(combinedSlavesList, argv[2]);
	} else {
		strcpy(combinedSlavesList, defaultCombinedSlavesList);
	}

	const char* fileName = argv[1];


	// This is where we'll store the source's list
	source_infos_t source_infos[16];

	// This is where we'll store the source output's list
	source_output_infos_t source_output_info;

	// This is where we'll store the modules list 
	module_infos_t modules[16];
	int ctr;

	if (get_sources_source_output_and_modules(source_infos, &source_output_info, modules) < 0) {
		fprintf(stderr, "Failed to fetch source, source output and module list\n");
		return 1;
	}
	if (source_output_info.initialized) {
		printf("=======[ Source Output info #%d ]=======\n", ctr+1);
		printf("Name: %s\n", source_output_info.name);
		printf("Source: %d\n", source_output_info.source);
		printf("Index: %d\n", source_output_info.index);
		printf("\n");
	}

	// This is where we store the fake sink name data if exists
	fake_sink_t fake_monitor = {.exists=0};
	fake_sink_t fake_combined_sink = {.exists=0};

	for (ctr = 0; ctr < 16; ctr++) {
		if (! source_infos[ctr].initialized) {
			break;
		}
		printf("=======[ Source #%d ]=======\n", ctr+1);
		printf("Description: %s\n", source_infos[ctr].description);
		printf("Name: %s\n", source_infos[ctr].name);
		printf("Index: %d\n", source_infos[ctr].index);
		printf("\n");
		if (strcmp(source_infos[ctr].name, fakeMonitorName) == 0) {
			fake_monitor.exists = 1;
			fake_monitor.index = source_infos[ctr].index;
		}
		if (strcmp(source_infos[ctr].name, fakeCombinedMonitorName) == 0) {
			fake_combined_sink.exists = 1;
			fake_combined_sink.index = source_infos[ctr].index;
		}
	}

	/* for (ctr = 0; ctr < 16; ctr++) { */
	/* 	if (! modules[ctr].initialized) { */
	/* 		break; */
	/* 	} */
	/* 	printf("=======[ Modules #%d ]=======\n", ctr+1); */
	/* 	printf("Name: %s\n", modules[ctr].name); */
	/* 	printf("Index: %d\n", modules[ctr].index); */
	/* 	printf("\n"); */
	/* } */
	

	if (fake_monitor.exists) {
		printf("Fake Monitor Index : %d\n", fake_monitor.index);
	} else {
		printf("no fake monitor yet\n");
		load_module_infos_t load_fake_module_infos = 
				{.success = 0, .module_name = "module-null-sink", 
				.description = "the fake sink"};
		sprintf(load_fake_module_infos.module_args, "sink_name=%s", fakeSinkName);
		load_module(&load_fake_module_infos);
		time_t startTime = time(NULL);
		// Wait up to two seconds for the module to be loaded (should be instant)
		while (time(NULL)-startTime < 2 && !load_fake_module_infos.success)
			usleep(250000);
		if (!load_fake_module_infos.success) {
			fprintf(stderr, "Error, fake monitor not created, exiting.\n");
			return 1;
		}
		printf ("Fake monitor created! retrying source lookup\n");
	} if (fake_combined_sink.exists) {
		printf("Fake Combined Sink Index : %d\n", fake_combined_sink.index);
	} else {
		printf("no fake combined sink yet\n");
		load_module_infos_t load_fake_combined_sink_infos  = 
				{.success = 0, .module_name = "module-combine-sink", 
				.description = "the fake combined sink"};
		fprintf(stderr, "Combined Slaves List : %s\n", combinedSlavesList);
		sprintf(load_fake_combined_sink_infos.module_args, "sink_name=%s slaves=%s,%s", fakeCombinedSinkName, combinedSlavesList, fakeSinkName);
		load_module(&load_fake_combined_sink_infos);
		time_t startTime = time(NULL);
		// Wait up to two seconds for the module to be loaded (should be instant)
		while (time(NULL)-startTime < 2 && !load_fake_combined_sink_infos.success)
			usleep(250000);
		if (!load_fake_combined_sink_infos.success) {
			fprintf(stderr, "Error, fake monitor not created, exiting.\n");
			return 1;
		}
		printf ("Fake combined monitor created! retrying source lookup\n");
	} 

	if (!fake_monitor.exists || !fake_combined_sink.exists) {
		if (get_sources_source_output_and_modules(source_infos, NULL, NULL) < 0) {
			fprintf(stderr, "Failed to fetch source, source output and module list\n");
			return 1;
		}
		for (ctr = 0; ctr < 16; ctr++) {
			if (! source_infos[ctr].initialized) {
				break;
			}
			if (strcmp(source_infos[ctr].name, fakeMonitorName) == 0) {
				fake_monitor.exists = 1;
				fake_monitor.index = source_infos[ctr].index;
			}
			if (strcmp(source_infos[ctr].name, fakeCombinedMonitorName) == 0) {
				fake_combined_sink.exists = 1;
				fake_combined_sink.index = source_infos[ctr].index;
			}
		}
		if (! fake_monitor.exists) {
			fprintf(stderr, "Fake monitor still not found, exiting.\n");
			return 1;
		}
		if (! fake_combined_sink.exists) {
			fprintf(stderr, "Fake combined sink still not found, exiting.\n");
			return 1;
		}
		printf("Fake Monitor Index : %d\n", fake_monitor.index);
		printf("Fake Combined Sink Index : %d\n", fake_combined_sink.index);
	}

	if (source_output_info.initialized) {
		move_source_output_port(source_output_info.index, fake_monitor.index);
	}	
	play_arguments_t sennheiserPlayArgs = {.fileName = fileName, .device = fakeCombinedSinkName};
	play(&sennheiserPlayArgs);

	if (source_output_info.initialized) {
		// removing back the source output to its previous source
		move_source_output_port(source_output_info.index, source_output_info.source);
	}
	// TODOR moving back source to previous

	return 0;
}

int get_sources_source_output_and_modules(
		source_infos_t* sources, 
		source_output_infos_t* source_outputs,
		module_infos_t* modules
		) {
	// Define our pulse audio loop and connection variables
	pa_mainloop *ml;
	pa_mainloop_api *mlapi;
	pa_operation *op;
	pa_context *ctx;

	// We'll need these state variables to keep track of our requests
	int state = 0;
	int ready = 0;

	// Initialize our device lists
	if (sources != NULL)
		memset(sources, 0, sizeof(source_infos_t) * 16);
	if (modules != NULL)
		memset(modules, 0, sizeof(module_infos_t) * 16);

	// Create a mainloop API and connection to the default server
	ml = pa_mainloop_new();
	mlapi = pa_mainloop_get_api(ml);
	ctx = pa_context_new(mlapi, "test");

	// This function connects to the pulse server
	pa_context_connect(ctx, NULL, 0, NULL);

	// This function defines a callback so the server will tell us it's state.
	// Our callback will wait for the state to be ready.  The callback will
	// modify the variable to 1 so we know when we have a connection and it's
	// ready.
	// If there's an error, the callback will set ready to 2
	pa_context_set_state_callback(ctx, state_cb, &ready);

	// Now we'll enter into an infinite loop until we get the data we receive
	// or if there's an error
	for (;;) {
		// We can't do anything until PA is ready, so just iterate the mainloop
		// and continue
		if (ready == 0) {
			pa_mainloop_iterate(ml, 1, NULL);
			continue;
		}
		// We couldn't get a connection to the server, so exit out
		if (ready == 2) {
			pa_context_disconnect(ctx);
			pa_context_unref(ctx);
			pa_mainloop_free(ml);
			return -1;
		}
		// At this point, we're connected to the server and ready to make
		// requests
		switch (state) {
			// State 0: we haven't done anything yet
			case 0:
				// This sends an operation to the server.  pa_sinklist_info is
				// our callback function and a pointer to our devicelist will
				// be passed to the callback The operation ID is stored in the
				// op variable
				if (sources != NULL) {
					op = pa_context_get_source_info_list(ctx,
							source_infos_cb,
							sources
							);

					// Update state for next iteration through the loop
					++state;
					break;
				} else
					++state;
			case 1:
				// Now we wait for our operation to complete.  When it's
				// complete our pa_source_outputlist is filled out, and we move
				// along to the next state
				if (source_outputs != NULL) {
					if (pa_operation_get_state(op) == PA_OPERATION_DONE) {
						pa_operation_unref(op);

						// Now we perform another operation to get the source
						// (input device) list just like before.  This time we pass
						// a pointer to our input structure
						op = pa_context_get_source_output_info_list(
								ctx,
								source_output_infos_cb,
								source_outputs
								);
						// Update the state so we know what to do next
						++state;
					}
					break;
				} else
					++state;
			case 2:
				if (modules != NULL) {
					if (pa_operation_get_state(op) == PA_OPERATION_DONE) {
						pa_operation_unref(op);
						op = pa_context_get_module_info_list(
								ctx,
								module_infos_cb,
								modules
								);
						// Update the state so we know what to do next
						++state;
					}
					break;
				} else
					++state;
			case 3:
				if (pa_operation_get_state(op) == PA_OPERATION_DONE) {
					// Now we're done, clean up and disconnect and return
					pa_operation_unref(op);
					pa_context_disconnect(ctx);
					pa_context_unref(ctx);
					pa_mainloop_free(ml);
					return 0;
				}
				break;
			default:
				// We should never see this state
				fprintf(stderr, "in state %d\n", state);
				return -1;
		}
		// Iterate the main loop and go again.  The second argument is whether
		// or not the iteration should block until something is ready to be
		// done.  Set it to zero for non-blocking.
		pa_mainloop_iterate(ml, 1, NULL);
	}
}

// This callback gets called when our context changes state.  We really only
// care about when it's ready or if it has failed
void state_cb(pa_context *c, void *userdata) {
	pa_context_state_t state;
	int *ready = userdata;

	state = pa_context_get_state(c);
	switch  (state) {
		// There are just here for reference
		case PA_CONTEXT_UNCONNECTED:
		case PA_CONTEXT_CONNECTING:
		case PA_CONTEXT_AUTHORIZING:
		case PA_CONTEXT_SETTING_NAME:
		default:
			break;
		case PA_CONTEXT_FAILED:
		case PA_CONTEXT_TERMINATED:
			*ready = 2;
			break;
		case PA_CONTEXT_READY:
			*ready = 1;
			break;
	}
}

// pa_mainloop will call this function when it's ready to tell us about a sink.
// Since we're not threading, there's no need for mutexes on the devicelist
// structure
void source_infos_cb(pa_context *c, const pa_source_info *l, int eol, void *userdata) {
	source_infos_t *source_infos = userdata;
	int ctr = 0;

	// If eol is set to a positive number, you're at the end of the list
	if (eol > 0) {
		return;
	}

	// We know we've allocated 16 slots to hold devices.  Loop through our
	// structure and find the first one that's "uninitialized."  Copy the
	// contents into it and we're done.  If we receive more than 16 devices,
	// they're going to get dropped.  You could make this dynamically allocate
	// space for the device list, but this is a simple example.
	for (ctr = 0; ctr < 16; ctr++) {
		if (! source_infos[ctr].initialized) {
			strncpy(source_infos[ctr].name, l->name, 511);
			strncpy(source_infos[ctr].description, l->description, 255);
			source_infos[ctr].index = l->index;
			source_infos[ctr].initialized = 1;
			break;
		}
	}
}

// See above.  This callback is pretty much identical to the previous
void source_output_infos_cb(pa_context *c, const pa_source_output_info *l, int eol, void *userdata) {
	source_output_infos_t *source_output_info = userdata;
	int ctr = 0;

	if (eol > 0) {
		return;
	}

	const char* processBinary = pa_proplist_gets(l->proplist, PA_PROP_APPLICATION_PROCESS_BINARY);
	if (strcmp(processBinary, sourceProcessBinary) == 0 && ! source_output_info->initialized) {
		strncpy(source_output_info->name, l->name, 511);
		source_output_info->source = l->source;
		source_output_info->index = l->index;
		source_output_info->initialized = 1;
	}
}

void module_infos_cb(pa_context *c, const pa_module_info* l, int eol, void *userdata) {
	module_infos_t* module_infos = userdata;

	if (eol > 0) {
		return;
	}

	int ctr = 0;

	for (ctr = 0; ctr < 16; ctr++) {
		if (! module_infos[ctr].initialized) {
			strncpy(module_infos[ctr].name, l->name, 511);
			module_infos[ctr].index = l->index;
			module_infos[ctr].initialized = 1;
			break;
		}
	}

}

void load_module_cb(pa_context *c, uint32_t index, void *userdata) {
	load_module_infos_t* load_module_infos = userdata;
	printf ("Module for %s loaded,  index : %d\n", (const char*) load_module_infos->description, index);
	load_module_infos->success = 1;
	printf("setting success");
}

int move_source_output_port(uint32_t sourceIndex, uint32_t portIndex){
	pa_mainloop *ml;
	pa_mainloop_api *mlapi;
	pa_operation *op;
	pa_context *ctx;

	int state = 0;
	int ready = 0;

	ml = pa_mainloop_new();
	mlapi = pa_mainloop_get_api(ml);
	ctx = pa_context_new(mlapi, "test");
	pa_context_connect(ctx, NULL, 0, NULL);

	pa_context_set_state_callback(ctx, state_cb, &ready);

	pa_cvolume channels_volume;
	pa_volume_t volume = PA_VOLUME_NORM;
	pa_cvolume_set(&channels_volume, 2, volume);

	for (;;) {
		// We can't do anything until PA is ready, so just iterate the mainloop
		// and continue
		if (ready == 0) {
			pa_mainloop_iterate(ml, 1, NULL);
			continue;
		}
		// We couldn't get a connection to the server, so exit out
		if (ready == 2) {
			pa_context_disconnect(ctx);
			pa_context_unref(ctx);
			pa_mainloop_free(ml);
			return -1;
		}
		// At this point, we're connected to the server and ready to make
		// requests
		switch (state) {
			// State 0: we haven't done anything yet
			case 0:
				// This sends an operation to the server.  pa_sinklist_info is
				// our callback function and a pointer to our devicelist will
				// be passed to the callback The operation ID is stored in the
				// op variable
				printf("moving source %d to port %d\n", sourceIndex, portIndex);
				op = pa_context_move_source_output_by_index(
						ctx, 
						sourceIndex,
						// try port lol
						portIndex,
						move_source_output_port_success,
						NULL
						);

				// Update state for next iteration through the loop
				++state;
				break;
			case 1:
				// Now we wait for our operation to complete.  When it's
				if (pa_operation_get_state(op) == PA_OPERATION_DONE) {
					pa_operation_unref(op);
					pa_context_disconnect(ctx);
					pa_context_unref(ctx);
					pa_mainloop_free(ml);
					return 0;
				}
				break;
			default:
				// We should never see this state
				fprintf(stderr, "in state %d\n", state);
				return -1;
		}
		pa_mainloop_iterate(ml, 1, NULL);
	}
}

void move_source_output_port_success(pa_context *c, int success, void *userdata){
	printf("Port move request result : %d\n", success);
}


int load_module(load_module_infos_t* load_module_infos) {
	pa_mainloop *ml;
	pa_mainloop_api *mlapi;
	pa_operation *op;
	pa_context *ctx;

	int state = 0;
	int ready = 0;

	ml = pa_mainloop_new();
	mlapi = pa_mainloop_get_api(ml);
	ctx = pa_context_new(mlapi, "test");
	pa_context_connect(ctx, NULL, 0, NULL);

	pa_context_set_state_callback(ctx, state_cb, &ready);

	pa_cvolume channels_volume;
	pa_volume_t volume = PA_VOLUME_NORM;
	pa_cvolume_set(&channels_volume, 2, volume);

	for (;;) {
		// We can't do anything until PA is ready, so just iterate the mainloop
		// and continue
		if (ready == 0) {
			pa_mainloop_iterate(ml, 1, NULL);
			continue;
		}
		// We couldn't get a connection to the server, so exit out
		if (ready == 2) {
			pa_context_disconnect(ctx);
			pa_context_unref(ctx);
			pa_mainloop_free(ml);
			return -1;
		}
		// At this point, we're connected to the server and ready to make
		// requests
		switch (state) {
			// State 0: we haven't done anything yet
			case 0:
				// This sends an operation to the server.  pa_sinklist_info is
				// our callback function and a pointer to our devicelist will
				// be passed to the callback The operation ID is stored in the
				// op variable
				printf("Loading module %s for %s\n", load_module_infos->module_name, load_module_infos->description);
				op = pa_context_load_module(
						ctx, 
						load_module_infos->module_name,
						load_module_infos->module_args,
						load_module_cb,
						load_module_infos
						);

				// Update state for next iteration through the loop
				++state;
				break;
			case 1:
				// Now we wait for our operation to complete.  When it's
				if (pa_operation_get_state(op) == PA_OPERATION_DONE) {
					pa_operation_unref(op);
					pa_context_disconnect(ctx);
					pa_context_unref(ctx);
					pa_mainloop_free(ml);
					return 0;
				}
				break;
			default:
				// We should never see this state
				fprintf(stderr, "in state %d\n", state);
				return -1;
		}
		pa_mainloop_iterate(ml, 1, NULL);
	}
	return 0;
}
