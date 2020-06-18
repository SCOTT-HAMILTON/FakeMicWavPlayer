#ifndef RUN_COMMAND_H
#define RUN_COMMAND_H


#include "FakeLib.h"

#include <variant>
#include <pulse/pulseaudio.h>

#include <exception>
#include <vector>

namespace FakeLibImplementation
{

struct ConnectionToServerFailure : public std::exception
{
	const char * what () const throw ()
	{ 
		return "Failed to connect to pulse audio server.";
	}
};

struct UnknownObjectError : public std::exception
{
	const char * what () const throw ()
	{ 
		return "Object command is unknown.";
	}
};

constexpr pa_context_flags_t F_0 = static_cast<pa_context_flags_t>(0);
constexpr pa_context_flags_t F_1 = static_cast<pa_context_flags_t>(1);

using ObjectVariant = std::variant<
	move_source_output_port_t,
	info_list<module_infos_t>,
	info_list<sink_infos_t>,
	info_list<source_infos_t>,
	info_list<source_output_infos_t>,
	load_module_infos_t,
	module_infos_t,
	sink_infos_t,
	source_infos_t,
	source_output_infos_t,
	unload_module_infos_t>;


std::vector<ObjectVariant> run_pa_commands(std::vector<ObjectVariant>& objects);
}

#endif //RUN_COMMAND_H
