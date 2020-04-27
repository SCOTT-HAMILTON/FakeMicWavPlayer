#include "FakeLib.h"
#include "help.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		help();
		return 1;
	}
	char combinedSlavesList[1024];
	char sourceProcessBinary[1024];
	if (argc > 2 && strcmp(argv[2], "default") != 0) {
		strcpy(combinedSlavesList, argv[2]);
	} else {
		strcpy(combinedSlavesList, defaultCombinedSlavesList);
	}

	if (argc > 3 && strcmp(argv[3], "default") != 0) {
		strcpy(sourceProcessBinary, argv[3]);
	} else {
		strcpy(sourceProcessBinary, defaultSourceProcessBinary);
	}

	const char *fileName = argv[1];

	// This is where we'll store the source's list
	source_infos_t source_infos[16];

	// This is where we'll store the source output's list
	source_output_infos_t source_output_info = {.source_process_binary =
							sourceProcessBinary};

	// This is where we'll store the modules list
	module_infos_t modules[16];
	int ctr;

	if (get_sources_source_output_and_modules(
		source_infos, &source_output_info, modules) < 0) {
		fprintf(
		    stderr,
		    "Failed to fetch source, source output and module list\n");
		return 1;
	}
	if (source_output_info.initialized) {
		printf("=======[ Source Output info #%d ]=======\n", ctr + 1);
		printf("Name: %s\n", source_output_info.name);
		printf("Source: %d\n", source_output_info.source);
		printf("Index: %d\n", source_output_info.index);
		printf("\n");
	}

	// This is where we store the fake sink name data if exists
	fake_sink_t fake_monitor = {.exists = 0};
	fake_sink_t fake_combined_sink = {.exists = 0};

	for (ctr = 0; ctr < 16; ctr++) {
		if (!source_infos[ctr].initialized) {
			break;
		}
		printf("=======[ Source #%d ]=======\n", ctr + 1);
		printf("Description: %s\n", source_infos[ctr].description);
		printf("Name: %s\n", source_infos[ctr].name);
		printf("Index: %d\n", source_infos[ctr].index);
		printf("\n");
		if (strcmp(source_infos[ctr].name, fakeMonitorName) == 0) {
			fake_monitor.exists = 1;
			fake_monitor.index = source_infos[ctr].index;
		}
		if (strcmp(source_infos[ctr].name, fakeCombinedMonitorName) ==
		    0) {
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
		load_module_infos_t load_fake_module_infos = {
		    .success = 0,
		    .module_name = "module-null-sink",
		    .description = "the fake sink"};
		sprintf(load_fake_module_infos.module_args, "sink_name=%s",
			fakeSinkName);
		load_module(&load_fake_module_infos);
		time_t startTime = time(NULL);
		// Wait up to two seconds for the module to be loaded (should be
		// instant)
		while (time(NULL) - startTime < 2 &&
		       !load_fake_module_infos.success)
			usleep(250000);
		if (!load_fake_module_infos.success) {
			fprintf(stderr,
				"Error, fake monitor not created, exiting.\n");
			return 1;
		}
		printf("Fake monitor created! retrying source lookup\n");
	}
	if (fake_combined_sink.exists) {
		printf("Fake Combined Sink Index : %d\n",
		       fake_combined_sink.index);
	} else {
		printf("no fake combined sink yet\n");
		load_module_infos_t load_fake_combined_sink_infos = {
		    .success = 0,
		    .module_name = "module-combine-sink",
		    .description = "the fake combined sink"};
		fprintf(stderr, "Combined Slaves List : %s\n",
			combinedSlavesList);
		sprintf(load_fake_combined_sink_infos.module_args,
			"sink_name=%s slaves=%s,%s", fakeCombinedSinkName,
			combinedSlavesList, fakeSinkName);
		load_module(&load_fake_combined_sink_infos);
		time_t startTime = time(NULL);
		// Wait up to two seconds for the module to be loaded (should be
		// instant)
		while (time(NULL) - startTime < 2 &&
		       !load_fake_combined_sink_infos.success)
			usleep(250000);
		if (!load_fake_combined_sink_infos.success) {
			fprintf(stderr,
				"Error, fake monitor not created, exiting.\n");
			return 1;
		}
		printf(
		    "Fake combined monitor created! retrying source lookup\n");
	}

	if (!fake_monitor.exists || !fake_combined_sink.exists) {
		if (get_sources_source_output_and_modules(source_infos, NULL,
							  NULL) < 0) {
			fprintf(stderr, "Failed to fetch source, source output "
					"and module list\n");
			return 1;
		}
		for (ctr = 0; ctr < 16; ctr++) {
			if (!source_infos[ctr].initialized) {
				break;
			}
			if (strcmp(source_infos[ctr].name, fakeMonitorName) ==
			    0) {
				fake_monitor.exists = 1;
				fake_monitor.index = source_infos[ctr].index;
			}
			if (strcmp(source_infos[ctr].name,
				   fakeCombinedMonitorName) == 0) {
				fake_combined_sink.exists = 1;
				fake_combined_sink.index =
				    source_infos[ctr].index;
			}
		}
		if (!fake_monitor.exists) {
			fprintf(stderr,
				"Fake monitor still not found, exiting.\n");
			return 1;
		}
		if (!fake_combined_sink.exists) {
			fprintf(
			    stderr,
			    "Fake combined sink still not found, exiting.\n");
			return 1;
		}
		printf("Fake Monitor Index : %d\n", fake_monitor.index);
		printf("Fake Combined Sink Index : %d\n",
		       fake_combined_sink.index);
	}

	if (source_output_info.initialized) {
		move_source_output_port(source_output_info.index,
					fake_monitor.index);
	}
	play_arguments_t sennheiserPlayArgs = {.fileName = fileName,
					       .device = fakeCombinedSinkName};
	play(&sennheiserPlayArgs);

	if (source_output_info.initialized) {
		// removing back the source output to its previous source
		move_source_output_port(source_output_info.index,
					source_output_info.source);
	}
	// TODOR moving back source to previous

	return 0;
}

