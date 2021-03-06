#pragma once

#include "../node.h"
#include "../buffer.h"

#include <vector>

namespace libsignal
{
	class Grain
	{
		public:
			Grain(BufferRef buffer, int start, int length, float rate, float pan) :
				buffer(buffer), sample_start(start), sample_length(length), rate(rate), pan(pan)
			{
				this->samples_done = 0;
			}

			bool finished()
			{
				return this->samples_done >= this->sample_length;
			}

			BufferRef buffer;
			double sample_start;
			int sample_length;
			double samples_done;
			float rate;
			float pan;
	};

	class Granulator : public Node
	{
		public:
			Granulator(BufferRef buffer = nullptr, NodeRef clock = 0, NodeRef pos = 0, NodeRef grain_length = 0.1, NodeRef rate = 1.0, NodeRef max_grains = 2048);

			BufferRef buffer;
			BufferRef envelope;

			NodeRef pos;
			NodeRef clock;
			NodeRef grain_length;
			NodeRef rate;
			NodeRef pan;
			NodeRef max_grains;

			virtual void process(sample **out, int num_frames);
			virtual void set_spatialisation(int num_channels, NodeRef pan);

		private:
			sample clock_last;

			std::vector <Grain *> grains;
	};

	REGISTER(Granulator, "granulator");
}
