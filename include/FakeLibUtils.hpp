#ifndef FAKE_LIB_UTILS_H
#define FAKE_LIB_UTILS_H

#include "FakeLibImplementation.h"

#include <functional>
#include <iostream>
#include <vector>
#include <variant>
#include <exception>

namespace FakeLibUtils
{

using namespace FakeLibImplementation;

template<typename user_type>
static void print_list(info_list<user_type> list,
			std::function<void(const user_type&, int)> print){
	for (int ctr = 0; ctr < 16; ++ctr) {
		if (list[ctr].initialized)
			print(list[ctr], ctr);
	}
}

static void print_module_list(info_list<module_infos_t> list){
	print_list<module_infos_t>(
		list,
		[](const auto& info, int ctr) {
			std::cerr << "=======[ Module info #" << ctr + 1 << " ]=======\n";
			std::cerr << "Name: " << 
					info.name << '\n';
			std::cerr << "Index: " << 
					info.index << '\n';
			std::cerr << '\n';
		}
	);
}

static void print_sink_list(info_list<sink_infos_t> list){
	print_list<sink_infos_t>(
		list,
		[](const auto& info, int ctr) {
			std::cerr << "=======[ Sink info #" << ctr + 1 << " ]=======\n";
			std::cerr << "Name: " << 
					info.name << '\n';
			std::cerr << "Index: " << 
					info.index << '\n';
			std::cerr << "Description: " << 
					info.description << '\n';
			std::cerr << '\n';
		}
	);
}

static void print_source_list(info_list<source_infos_t> list){
	print_list<source_infos_t>(
		list,
		[](const auto& info, int ctr) {
			std::cerr << "=======[ Source info #" << ctr + 1 << " ]=======\n";
			std::cerr << "Name: " << 
					info.name << '\n';
			std::cerr << "Index: " << 
					info.index << '\n';
			std::cerr << "Description: " << 
					info.description << '\n';
			std::cerr << '\n';
		}
	);
}

static void print_source_output_list(info_list<source_output_infos_t> list){
	print_list<source_output_infos_t>(
		list,
		[](const auto& info, int ctr) {
			std::cerr << "=======[ Source Output info #" << ctr + 1 << " ]=======\n";
			std::cerr << "Name: " << 
					info.name << '\n';
			std::cerr << "Source: " << 
					info.source << '\n';
			std::cerr << "Index: " << 
					info.index << '\n';
			std::cerr << "Source Process Binary Name: " << 
					info.source_process_binary << '\n';
			std::cerr << '\n';
		}
	);
}

template<typename info_type>
static void reset_info_list(info_list<info_type>& list){
	for (auto& object : list) {
		object.initialized = false;
	}
}
template<typename user_type>
static auto extract(std::vector<ObjectVariant>& results) {
	for (auto& result : results) {
		try {
			return std::get<user_type>(result);
		} catch (const std::bad_variant_access&) {
		}
	}
	throw ObjectNotFoundInResultsError();
}
}


#endif //FAKE_LIB_UTILS_H
