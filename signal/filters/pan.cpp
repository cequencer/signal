#include "pan.h" 

namespace libsignal
{

void Pan::process(sample **out, int num_frames)
{
	for (int frame = 0; frame < num_frames; frame++)
	{
		sample pan = this->pan->out[0][frame];
		sample in = this->input->out[0][frame];

		if (this->num_output_channels == 2)
		{
			out[0][frame] = in * (1.0 - pan);
			out[1][frame] = in * pan;
		}
		else if (this->num_output_channels == 4)
		{
			// TODO calculate channels frames based on pan as angle in radians
		}
	}
}

}
