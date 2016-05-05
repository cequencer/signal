#include <signal/signal.h>

/*------------------------------------------------------------------------
 * All objects are in the signal:: namespace.
 * Import this namespace for code brevity.
 *-----------------------------------------------------------------------*/
using namespace libsignal;

int main()
{
	/*------------------------------------------------------------------------
	 * Instantiate a single AudioGraph object for all global audio processing.
	 *-----------------------------------------------------------------------*/
	AudioGraph *graph = new AudioGraph();

	/*------------------------------------------------------------------------
	 * 440hz Hello World
	 *-----------------------------------------------------------------------*/
	NodeRef sine = new Sine(440);

	/*------------------------------------------------------------------------
	 * The AudioGraph can have multiple inputs, summed to output.
	 *-----------------------------------------------------------------------*/
	graph->add_output(sine);

	/*------------------------------------------------------------------------
	 * Begin audio processing, and run indefinitely.
	 *-----------------------------------------------------------------------*/
	graph->start();
	graph->wait();
}

