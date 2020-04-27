#ifndef HELP_H
#define HELP_H

#include <stdio.h>

void help() {
	printf("             MIT License - open source project do what ever "
	       "you want with this           \n");
	printf("§=============================================================="
	       "==========================§\n");
	printf("§       ./FakeMicWavPlayer <wav file> [sink list|default] "
	       "[source process binary|default]§\n");
	printf("§____________________________. .______________. "
	       ".________________________________. ._____§\n");
	printf("	                     | |              | |              "
	       "                  | |\n");
	printf("      The wav file to open  = _|              | |              "
	       "                  | |\n");
	printf("                                              | |              "
	       "                  | |\n");
	printf("  A list of sinks to play the wav file in.    =_|              "
	       "                  | |\n");
	printf("  You don't have to mention the virtual sink.                  "
	       "                  | |\n");
	printf("  This could be any sinks outputed from                        "
	       "                  | |\n");
	printf("  `pactl list short sinks` in the second                       "
	       "                  | |\n");
	printf("  column. If you don't know what the virtual                   "
	       "                  | |\n");
	printf("  sink is or what even sinks are, go check                     "
	       "                  | |\n");
	printf("  out the details in the project github :                      "
	       "                  | |\n");
	printf("  "
	       "https://github.com/SCOTT-HAMILTON/"
	       "FakeMicWavPlayer#how-does-it-work            | |\n");
	printf("  Or even the the excellent pulseaudio under                   "
	       "                  | |\n");
	printf("  the hood reference guide :                                   "
	       "                  | |\n");
	printf("  https://gavv.github.io/articles/pulseaudio-under-the-hood/   "
	       "                  | |\n");
	printf("  The default value is stored in :                         "
	       "			 | |\n");
	printf("  `defaultSourceProcessBinary` variable                        "
	       "                  | |\n");
	printf("                                                               "
	       "                  | |\n");
	printf("                                                               "
	       "                  | |\n");
	printf("         The binary name of the application you want to play "
	       "the wav file to.    =_|\n");
	printf("         The default value is 'Discord'.                       "
	       "                     \n");
}

#endif // HELP_H
