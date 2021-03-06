#pragma once

#include "../node.h"

#include <vamp-hostsdk/PluginHostAdapter.h>
#include <vamp-hostsdk/PluginInputDomainAdapter.h>
#include <vamp-hostsdk/PluginLoader.h>

#include <algorithm>

using namespace std;

using Vamp::Plugin;
using Vamp::PluginHostAdapter;
using Vamp::RealTime;
using Vamp::HostExt::PluginLoader;
using Vamp::HostExt::PluginWrapper;
using Vamp::HostExt::PluginInputDomainAdapter;

namespace libsignal
{
	class VampAnalysis : public UnaryOpNode
	{
		public:
			VampAnalysis(NodeRef input = 0.0, string plugin_id = "vamp-example-plugins:spectralcentroid:linearcentroid") :
				UnaryOpNode(input)
			{
				this->name = "vamp";

				this->current_frame = 0;

				/*------------------------------------------------------------------------
				 * Strip leading `vamp:` from plugin ID, if given. 
				 *-----------------------------------------------------------------------*/
				if (plugin_id.find("vamp:") == 0)
					plugin_id = plugin_id.substr(5);

				/*------------------------------------------------------------------------
				 * Check that plugin ID is valid.
				 *-----------------------------------------------------------------------*/
				size_t num_colons = std::count(plugin_id.begin(), plugin_id.end(), ':');
				if (num_colons != 2)
					throw std::runtime_error("Invalid Vamp plugin ID: " + plugin_id);

				int first_separator = plugin_id.find(':');
				int second_separator = plugin_id.find(':', first_separator + 1);
				string vamp_plugin_library = plugin_id.substr(0, first_separator);
				string vamp_plugin_feature = plugin_id.substr(first_separator + 1, second_separator - first_separator - 1);
				string vamp_plugin_output = plugin_id.substr(second_separator + 1);
				signal_debug("Loading plugin %s, %s, %s", vamp_plugin_library.c_str(), vamp_plugin_feature.c_str(), vamp_plugin_output.c_str());

				PluginLoader *loader = PluginLoader::getInstance();
				PluginLoader::PluginKey key = loader->composePluginKey(vamp_plugin_library, vamp_plugin_feature);

				this->plugin = loader->loadPlugin(key, this->graph->sample_rate, PluginLoader::ADAPT_ALL);

				if (!this->plugin)
					throw std::runtime_error("Failed to load Vamp plugin: " + plugin_id);

				/*------------------------------------------------------------------------
				 * Get required output index.
				 *-----------------------------------------------------------------------*/
				Plugin::OutputList outputs = this->plugin->getOutputDescriptors();
				this->output_index = -1;

				for (unsigned int oi = 0; oi < outputs.size(); oi++)
				{
					if (outputs[oi].identifier == vamp_plugin_output)
					{
						this->output_index = oi;
						break;
					}
				}

				signal_debug("Loaded plugin (output index %d)", this->output_index);
				this->plugin->initialise(1, SIGNAL_DEFAULT_BLOCK_SIZE, SIGNAL_DEFAULT_BLOCK_SIZE);
			}

			~VampAnalysis()
			{
				// close vamp plugins
			}

			virtual void process(sample **out, int num_frames)
			{
				RealTime rt = RealTime::frame2RealTime(this->current_frame, this->graph->sample_rate);
				Plugin::FeatureSet features = this->plugin->process(this->input->out, rt);
				if (features[this->output_index].size())
				{
					Plugin::Feature feature = features[this->output_index][0];

					if (feature.values.size() > 0)
					{
						float value = feature.values[0];
						printf("Got results (size = %ld): %f\n", features[this->output_index].size(), value);

						for (int channel = 0; channel < this->num_input_channels; channel++)
						{
							for (int frame = 0; frame < num_frames; frame++)
							{
								out[channel][frame] = value;
							}
						}

						this->current_frame += num_frames;
					}
					else
					{
						for (int channel = 0; channel < this->num_input_channels; channel++)
						{
							memset(out[channel], 0, num_frames * sizeof(sample));
						}
					}
				}
			}

			int current_frame;
			int output_index;
			Plugin *plugin;
	};

	class VampEventExtractor : public VampAnalysis
	{
		public:
			VampEventExtractor(NodeRef input = 0.0, string plugin_id = "vamp:vamp-example-plugins:percussiononsets:onsets") :
				VampAnalysis(input, plugin_id)
			{
				this->name = "vamp_events";
				this->set_property("timestamps", { 0 });
				this->set_property("labels", { "" });
			}

			virtual void process(sample **out, int num_frames)
			{
				RealTime rt = RealTime::frame2RealTime(this->current_frame, this->graph->sample_rate);
				Plugin::FeatureSet features = this->plugin->process(this->input->out, rt);
				if (features[this->output_index].size())
				{
					Plugin::Feature feature = features[this->output_index][0];

					if (feature.hasTimestamp)
					{
						long ts = RealTime::realTime2Frame(feature.timestamp, this->graph->sample_rate);
						printf("Got ts: %ld (%f)\n", ts, this->graph->sample_rate);
						std::vector<float> timestamps = this->get_property("timestamps")->float_array_value();
						timestamps.push_back(ts);
						this->set_property("timestamps", timestamps);
					}
				}
			}
	};

	class VampSegmenter : public VampAnalysis
	{
		public:
			VampSegmenter(NodeRef input = 0.0, string plugin_id = "vamp:vamp-example-plugins:percussiononsets:onsets") :
				VampAnalysis(input, plugin_id)
			{
				this->name = "vamp_segmenter";
				this->set_property("timestamps", { 0 });
				this->set_property("values", { 0 });
				this->set_property("durations", { 0 });
			}

			float last_value = -1;
			long last_timestamp = -1;

			virtual void process(sample **out, int num_frames)
			{
				RealTime rt = RealTime::frame2RealTime(this->current_frame, this->graph->sample_rate);
				Plugin::FeatureSet features = this->plugin->process(this->input->out, rt);
				if (features[this->output_index].size())
				{
					Plugin::Feature feature = features[this->output_index][0];

					if (feature.values.size() > 0)
					{
						long timestamp = RealTime::realTime2Frame(feature.timestamp, this->graph->sample_rate);
						float value = feature.values[0];
						value = midi_to_freq(roundf(freq_to_midi(value)));

						if (value != last_value && (!isnan(value) || !isnan(last_value)))
						{
							printf("value %f, last_value %f\n", value, last_value);
							if (!isnan(value))
							{
								std::vector<float> values = this->get_property("values")->float_array_value();
								values.push_back(value);
								this->set_property("values", values);

								std::vector<float> timestamps = this->get_property("timestamps")->float_array_value();
								timestamps.push_back(timestamp);
								this->set_property("timestamps", timestamps);
							}

							if (last_timestamp >= 0 && !isnan(last_value))
							{
								float duration = (float) (timestamp - last_timestamp);
								std::vector<float> durations = this->get_property("durations")->float_array_value();
								durations.push_back(duration);
								this->set_property("durations", durations);
							}

							this->last_value = value;
							this->last_timestamp = timestamp;
						}
					}
				}
			}
	};

	REGISTER(VampAnalysis, "vamp");
	REGISTER(VampEventExtractor, "vamp_events");
	REGISTER(VampSegmenter, "vamp_segmenter");
}
