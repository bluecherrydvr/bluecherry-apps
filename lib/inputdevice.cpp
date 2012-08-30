#include "libbluecherry.h"

input_device::input_device()
	: _audio_enabled(false)
{
}

input_device::~input_device()
{
}

void input_device::set_audio_enabled(bool v)
{
	_audio_enabled = v;
}

