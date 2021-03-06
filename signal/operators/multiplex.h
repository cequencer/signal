#pragma once

#include "../constants.h"
#include "../node.h"
#include "../core.h"
#include "../registry.h"

#include <list>

namespace libsignal
{

	class Multiplex : public Node
	{

	public:

		Multiplex(std::initializer_list<NodeRef> inputs = {}) : Node()
		{
			this->name = "multiplex";
			this->no_input_automix = true;

			for (NodeRef input : inputs)
			{
				this->add_input(input);
			}
		}

		virtual void process(sample **out, int num_frames)
		{
			int global_channel = 0; 
			for (NodeRef input : this->inputs)
			{
				for (int this_channel = 0; this_channel < input->num_output_channels; this_channel++)
				{
					memcpy(out[global_channel + this_channel], input->out[this_channel], num_frames * sizeof(sample));
				}
				global_channel += input->num_output_channels;
			}
		}

		virtual void update_channels()
		{
			this->num_input_channels = 0;
			for (NodeRef input : this->inputs)
			{
				this->num_input_channels += input->num_output_channels;
			}

			this->num_output_channels = this->num_input_channels;

			this->min_input_channels = this->max_input_channels = this->num_input_channels;
			this->min_output_channels = this->max_output_channels = this->num_output_channels;

			signal_debug("Node %s set num_out_channels to %d", this->name.c_str(), this->num_output_channels);
		}

		virtual void add_input(NodeRef input)
		{
			this->inputs.push_back(input);
            std::string input_name = "input" + std::to_string(this->inputs.size());;
            this->Node::add_input(input_name, inputs.back());
		}

		virtual void set_input(std::string name, const NodeRef &node)
		{
			if (this->params.find(name) == this->params.end())
			{
				this->inputs.push_back(node);
				this->Node::add_input(name, inputs.back());
			}

			this->Node::set_input(name, node);
		}


		std::list <NodeRef> inputs;
	};

	REGISTER(Multiplex, "multiplex");

}

