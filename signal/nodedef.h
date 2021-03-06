#pragma once

#include <string>
#include <unordered_map>

namespace libsignal
{

class NodeDefinition
{
	public:
		NodeDefinition();
		~NodeDefinition();
		NodeDefinition(std::string name);
		NodeDefinition(std::string name, float value);

		void set_id(int value);
		void add_input(std::string name, NodeDefinition *def);
		void add_input(std::string name, float value);
		void set_name (std::string name);
		void set_value (float value);

		std::string name;
		float value;
		bool is_constant = false;
		std::unordered_map <std::string, NodeDefinition *> params;

		int id;
		std::string input_name;
};

}
