#pragma once

#include <cstddef>

class AudioBackend {
  public:
	virtual ~AudioBackend() = default;
	virtual void initialize() = 0;

	virtual void pushSample(float sample) = 0;
	virtual void restart() = 0;

	virtual bool isPaused() = 0;
	virtual void pause() = 0;
	virtual void unPause() = 0;

	virtual bool initialized() const = 0;

	float audioSampleRate() const { return AUDIO_SAMPLE_RATE; }
	size_t audioSampleAmount() const { return AUDIO_SAMPLE_AMOUNT; }

  private:
	static constexpr float AUDIO_SAMPLE_RATE = 44100.0;
	static constexpr std::size_t AUDIO_SAMPLE_AMOUNT = 1024;
};
