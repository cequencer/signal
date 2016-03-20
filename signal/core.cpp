#include "core.h"

#include <iostream>
#include <stdlib.h>
#include <stdarg.h>

#include "util.h"

using namespace libsignal;

void signal_init()
{
	rng_init();
}

void signal_debug(char const * msg, ... )
{
	#ifdef DEBUG

		va_list v;
		va_start(v, msg);
		vfprintf(stdout, msg, v);
		fprintf(stdout, "\n");
		va_end(v);

	#endif
}

void signal_assert(bool equality, char const *msg, ...)
{
	if (!equality)
	{
		va_list v;
		va_start(v, msg);
		fprintf(stdout, "SIGNUM FATAL ERROR: ");
		vfprintf(stdout, msg, v);
		fprintf(stdout, "\n");
		va_end(v);
		exit(1);
	}
}

void signal_warn(char const * msg, ... )
{
	va_list v;
	va_start(v, msg);
	vfprintf(stderr, msg, v);
	fprintf(stderr, "\n");
	va_end(v);
}