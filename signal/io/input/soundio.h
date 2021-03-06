#pragma once

#ifdef HAVE_SOUNDIO

#define AudioIn AudioIn_SoundIO

#include <soundio/soundio.h>
#include <vector>

#include "abstract.h"

#include "../../buffer.h"
#include "../../graph.h"

namespace libsignal
{
	class AudioIn_SoundIO : public AudioIn_Abstract
	{
	public:
        AudioIn_SoundIO();
		virtual int init() override;
		virtual int start() override;
		virtual int close() override;
		virtual void process(sample **out, int num_samples) override;

		struct SoundIo *soundio;
		struct SoundIoDevice *device;
		struct SoundIoInStream *instream;

		Buffer *buffer;
		int read_pos;
		int write_pos;
	};
}


#endif