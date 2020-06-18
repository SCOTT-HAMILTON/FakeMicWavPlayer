#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <pulse/pulseaudio.h>

#include <functional>

void state_cb(pa_context *c, void *userdata);

template<typename pa_type, typename user_type>
void get_infos_list_cb(__attribute__((unused)) pa_context *c,
		const pa_type* l,
		int eol,
		void* userdata,
		std::function<void(user_type&, const pa_type*)> implementation
		);

template<typename pa_type, typename user_type>
void get_infos_cb(__attribute__((unused)) pa_context *c,
		const pa_type* l,
		int eol,
		void* userdata,
		std::function<void(user_type&, const pa_type*)> implementation
		);

template<typename user_type>
void success_callback(pa_context *c, int success, void *userdata);

void move_source_output_port_cb(pa_context *c, int success,
				     void *userdata);
void module_infos_list_cb(pa_context *c, const pa_module_info *l, int eol,
		     void *userdata);
void sink_infos_list_cb(pa_context *c, const pa_sink_info *l, int eol,
		   void *userdata);
void source_infos_list_cb(pa_context *c, const pa_source_info *l, int eol,
		     void *userdata);
void source_output_infos_list_cb(pa_context *c, const pa_source_output_info *l,
			    int eol, void *userdata);

void load_module_infos_cb(pa_context *c, uint32_t index, void *userdata);

void module_infos_cb(pa_context *c, const pa_module_info *l, int eol,
		     void *userdata);
void sink_infos_cb(pa_context *c, const pa_sink_info *l, int eol,
		   void *userdata);
void source_infos_cb(pa_context *c, const pa_source_info *l, int eol,
		     void *userdata);
void source_output_infos_cb(pa_context *c, const pa_source_output_info *l,
			    int eol, void *userdata);
void unload_module_cb(pa_context *c, int success,
				     void *userdata);

/* void module_infos_cb(pa_context *c, const pa_module_info *l, int eol, */
/* 		     void *userdata); */
/* void sink_infos_cb(pa_context *c, const pa_sink_info *l, int eol, */
/* 		   void *userdata); */
/* void source_infos_cb(pa_context *c, const pa_source_info *l, int eol, */
/* 		     void *userdata); */
/* void source_output_infos_cb(pa_context *c, const pa_source_output_info *l, */
/* 			    int eol, void *userdata); */

#endif //CALLBACKS_H
