#include "CallBacks.h"
#include "FakeLib.h"

#include <iostream>
#include <iterator>

void state_cb(pa_context *c, void *userdata) {
	pa_context_state_t state;
	int *ready = static_cast<int*>(userdata);
	state = pa_context_get_state(c);
	switch (state) {
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

template<typename pa_type, typename user_type>
void get_infos_list_cb(__attribute__((unused)) pa_context *c,
		const pa_type* l,
		int eol,
		void* userdata,
		std::function<void(user_type&, const pa_type*)> implementation
		)
{
	info_list<user_type>* infos = static_cast<info_list<user_type>*>(userdata);
	if (eol > 0) {
		return;
	}
	for (int ctr = 0; ctr < 16; ++ctr) {
		if (!infos->at(ctr).initialized) {
			infos->at(ctr).initialized = true;
			implementation(infos->at(ctr), l);
			break;
		}
	}
}

template<typename pa_type, typename user_type>
void get_infos_cb(__attribute__((unused)) pa_context *c,
		const pa_type* l,
		int eol,
		void* userdata,
		std::function<void(user_type&, const pa_type*)> implementation
		)
{
	user_type* infos = static_cast<user_type*>(userdata);
	if (eol > 0) {
		return;
	}
	implementation(*infos, l);
}

template<typename user_type>
void success_callback(pa_context *c, int success, void *userdata) {
	user_type* user = static_cast<user_type*>(userdata);
	user->success = success;
}

void move_source_output_port_cb(pa_context *c, int success,
				     void *userdata) {
	success_callback<success_info_t>(c, success, userdata);
}

void unload_module_cb(pa_context *c, int success,
				     void *userdata) {
	success_callback<unload_module_infos_t>(c, success, userdata);
}

void load_module_infos_cb(__attribute__((unused)) pa_context *c, uint32_t index,
		    void *userdata) {
	load_module_infos_t *load_module_infos = static_cast<load_module_infos_t*>(userdata);
	load_module_infos->success = 1;
}

void module_infos_list_cb(pa_context *c, const pa_module_info *l, int eol, void *userdata) {
	get_infos_list_cb<pa_module_info, module_infos_t>(
			c,
			l,
			eol,
			userdata,
			[](module_infos_t& module_info, const pa_module_info* l){
				module_info.name = l->name;
				module_info.index = l->index;
				module_info.initialized = true;
			}
	);
}

void sink_infos_list_cb(pa_context *c, const pa_sink_info *l, int eol,
		   void *userdata) {
	get_infos_list_cb<pa_sink_info, sink_infos_t>(
			c,
			l,
			eol,
			userdata,
			[](sink_infos_t& sink_info, const pa_sink_info* l){
				sink_info.name = l->name;
				sink_info.description = l->description;
				sink_info.index = l->index;
				sink_info.initialized = true;
			}
	);
}

void source_infos_list_cb(pa_context *c, const pa_source_info *l, int eol,
		     void *userdata) {
	get_infos_list_cb<pa_source_info, source_infos_t>(
			c,
			l,
			eol,
			userdata,
			[](source_infos_t& source_info, const pa_source_info* l){
				source_info.name = l->name;
				source_info.description = l->description;
				source_info.index = l->index;
				source_info.initialized = true;
			}
	);
}

void source_output_infos_list_cb(pa_context *c, const pa_source_output_info *l,
			    int eol, void *userdata) {
	get_infos_list_cb<pa_source_output_info, source_output_infos_t>(
			c,
			l,
			eol,
			userdata,
			[](source_output_infos_t& source_output_info, const pa_source_output_info* l){
				source_output_info.initialized = true;
				source_output_info.name = l->name;
				source_output_info.source = l->source;
				source_output_info.index = l->index;
				source_output_info.source_process_binary = 
					pa_proplist_gets(
					    l->proplist,
					    PA_PROP_APPLICATION_PROCESS_BINARY);
			}
	);
}

void unload_module_infos_cb(pa_context *c, int success, void *userdata) {
	success_callback<unload_module_infos>(c, success, userdata);
}


void module_infos_cb(pa_context *c, const pa_module_info *l, int eol, void *userdata) {
	get_infos_cb<pa_module_info, module_infos_t>(
			c,
			l,
			eol,
			userdata,
			[](module_infos_t& module_info, const pa_module_info* l){
				module_info.name = l->name;
				module_info.index = l->index;
				module_info.initialized = true;
			}
	);
}

void sink_infos_cb(pa_context *c, const pa_sink_info *l, int eol,
		   void *userdata) {
	get_infos_cb<pa_sink_info, sink_infos_t>(
			c,
			l,
			eol,
			userdata,
			[](sink_infos_t& sink_info, const pa_sink_info* l){
				sink_info.name = l->name;
				sink_info.description = l->description;
				sink_info.index = l->index;
				sink_info.initialized = true;
			}
	);
}

void source_infos_cb(pa_context *c, const pa_source_info *l, int eol,
		     void *userdata) {
	get_infos_cb<pa_source_info, source_infos_t>(
			c,
			l,
			eol,
			userdata,
			[](source_infos_t& source_info, const pa_source_info* l){
				source_info.name = l->name;
				source_info.description = l->description;
				source_info.index = l->index;
				source_info.initialized = true;
			}
	);
}

void source_output_infos_cb(pa_context *c, const pa_source_output_info *l,
			    int eol, void *userdata) {
	get_infos_cb<pa_source_output_info, source_output_infos_t>(
			c,
			l,
			eol,
			userdata,
			[](source_output_infos_t& source_output_info, const pa_source_output_info* l){
				source_output_info.initialized = true;
				source_output_info.name = l->name;
				source_output_info.source = l->source;
				source_output_info.index = l->index;
				source_output_info.source_process_binary = 
					pa_proplist_gets(
					    l->proplist,
					    PA_PROP_APPLICATION_PROCESS_BINARY);
			}
	);
}
