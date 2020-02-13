#include <dsbee/dsbee.h>

#include "utility.h" // useful stuff


using namespace dsbee;

// Magic variables corresponding to mouse position
extern float MOUSE_X, MOUSE_Y;



class Synth_Example : public Synth_OneByOne
{
public:
	// Our phase value stays between 0 and 1
	float phase  = 0.0f;

	// The number of samples per second.
	float sampleRate = 48000.f;

	// State for some very simple filters.
	float filter_1 = 0.f, filter_2 = 0.f;

	// This is called at the start of our program. It's getting the sample rate from the audio card
	void start(AudioInfo info) override
	{
		sampleRate = info.sampleRate;

		// Reset our filters
		filter_1 = 0.f;
		filter_2 = 0.f;
	}

	// This is called once per audio sample.
	float makeSample() override
	{
		// Pick an amplification factor.
		float amp = 1.0f;

		// Pick a frequency, calculating it from a MIDI note.
		float midiNote = 36.f + 60.f * MOUSE_X;
		float frequency = MidiFrequency(midiNote);

		// Control our filters
		float filter_alpha = std::pow(.01f, 1.0f - MOUSE_Y);

		// Move our phase ahead proportional to frequency.
		//    Wrap it around so it stays between 0 and 1.
		//    (This keeps the float from getting large and imprecise)
		phase += frequency / sampleRate;
		phase = Wrap0to1(phase);

		// Make a sine wave based on the phase.
		float sine = std::sin(phase * TWO_PI);

		// Make square wave.
		float square = ((phase <= .5f) ? (1.0f) : (-1.0f));

		// Make saw tooth wave
		float sawtooth = (2.0f * phase - 1.0f);

		// Use a utility function to make some white noise
		float whiteNoise = WhiteNoise();

		// Mix the synth and white noise together
		float mix =
			+ 0.70f * sawtooth
			+ 0.20f * whiteNoise;

		// A very simple filter
		filter_1 += (mix      - filter_1) * filter_alpha;
		filter_2 += (filter_1 - filter_2) * filter_alpha;

		// Final result
		return amp * filter_2;
	}
};


Processor *dsbee::GetProcessor()    {return new Synth_Example();}