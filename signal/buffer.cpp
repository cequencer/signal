#include "buffer.h"

#ifdef HAVE_SNDFILE
	#include <sndfile.h>
#endif

#include <stdlib.h>
#include <string.h>

namespace libsignal
{

Buffer::Buffer()
{
}

Buffer::Buffer(int num_channels, int num_frames)
{
	this->num_channels = num_channels;
	this->num_frames = num_frames;
	this->sample_rate = 44100.0;
	this->interpolate = SIGNAL_INTERPOLATE_NONE;

	this->data = (sample **) malloc(sizeof(void *) * this->num_channels);
	for (int channel = 0; channel < this->num_channels; channel++)
	{
		this->data[channel] = (sample *) malloc(sizeof(sample) * this->num_frames);
		memset(this->data[channel], 0, sizeof(sample) * this->num_frames);
	}
}

Buffer::Buffer(const char *filename)
{
	this->open(filename);
}

void Buffer::open(const char *filename)
{
	#ifdef HAVE_SNDFILE

    SF_INFO info;
    SNDFILE *sndfile = sf_open(filename, SFM_READ, &info);

    if (!sndfile)
    {
        printf("Failed to read soundfile\n");
        exit(1);
    }

	printf("Read %d channels, %ld frames\n", info.channels, (long int) info.frames);
    this->data = (sample **) malloc(sizeof(void *) * info.channels);
	for (int channel = 0; channel < info.channels; channel++)
	{
		long long length = sizeof(sample) * info.frames;
		this->data[channel] = (sample *) malloc(length + 1024);
		
		memset(this->data[channel], 0, length);
	}

	int frames_per_read = 1024;
	int samples_per_read = frames_per_read * info.channels;
	sample buffer[samples_per_read];
    int ptr = 0;

    while (true)
    {
        int count = sf_readf_float(sndfile, buffer, frames_per_read);
		for (int frame = 0; frame < count; frame++)
		{
			// TODO: Vector-accelerated de-interleave
			for (int channel = 0; channel < info.channels; channel++)
			{
				this->data[channel][ptr] = buffer[frame * info.channels + channel];
			}
			ptr++;
		}
        if (count < frames_per_read)
            break;
    }

	this->num_channels = info.channels;
	this->num_frames = info.frames;
	this->sample_rate = info.samplerate;
	this->duration = this->num_frames / this->sample_rate;

	#endif
}

}
