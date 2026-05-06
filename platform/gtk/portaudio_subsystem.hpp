#pragma once

#include "audio/ringbuffer.hpp"
#include "portaudio.h"
#include "utils.hpp"
#include <iostream>

class PortAudioSubsystem {
  public:
	PortAudioSubsystem() {
		std::cout << "PortAudioSubsystem constructor running." << std::endl;

		PaError err = Pa_Initialize();
		if(err != paNoError) {
			m_initializationError = Pa_GetErrorText(err);
			std::string errorMsg = "Pa_Initialize() error: " + m_initializationError;

			std::cerr << "Platform[GTK]: " << errorMsg << std::endl;
			showErrorDialog("PortAudio Error", "Audio could not be initialized: \"" + errorMsg + "\"");

			return;
		}

		err = Pa_OpenDefaultStream(&m_stream, 0, 1, paFloat32, AUDIO_SAMPLE_RATE, AUDIO_SAMPLE_AMOUNT, audioCallback,
								   this);
		if(err != paNoError) {
			m_stream = nullptr;
			Pa_Terminate();

			m_initializationError = Pa_GetErrorText(err);
			std::string errorMsg = "Pa_OpenDefaultStream() error: " + m_initializationError;

			std::cerr << "Platform[GTK]: " << errorMsg << std::endl;
			showErrorDialog("PortAudio Error", "Audio could not be initialized: \"" + errorMsg + "\"");

			return;
		}

		err = Pa_StartStream(m_stream);
		if(err != paNoError) {
			Pa_CloseStream(m_stream);
			m_stream = nullptr;
			Pa_Terminate();

			m_initializationError = Pa_GetErrorText(err);
			std::string errorMsg = "Pa_StartStream() error: " + m_initializationError;

			std::cerr << "Platform[GTK]: " << errorMsg << std::endl;
			showErrorDialog("PortAudio Error", "Audio could not be initialized: \"" + errorMsg + "\"");

			return;
		}

		m_initialized = true;
	}

	~PortAudioSubsystem() {
		if(!m_initialized) { return; }

		Pa_StopStream(m_stream);
		Pa_CloseStream(m_stream);

		Pa_Terminate();
	}

	static int audioCallback(const void *input, void *output, unsigned long framesPerBuffer,
							 const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags,
							 void *userData) {
		PortAudioSubsystem *audioSubsystem = static_cast<PortAudioSubsystem *>(userData);
		float *buffer = (float *)output;

		for(size_t i = 0; i < framesPerBuffer; i++) {
			if(audioSubsystem->m_audioPaused || !audioSubsystem->m_audioSampleBuffer.popSample(buffer[i])) {
				buffer[i] = 0.0f;
			}
		}

		return paContinue;
	}

	void pushSample(float sample) { m_audioSampleBuffer.pushSample(sample); }

	void reset() {
		m_audioPaused = true;
		m_audioSampleBuffer.reset();
	}

	void pause() { m_audioPaused = true; }
	void unPause() { m_audioPaused = false; }

	bool initialized() const { return m_initialized; }

	float audioSampleRate() const { return AUDIO_SAMPLE_RATE; }
	size_t audioSampleAmount() const { return AUDIO_SAMPLE_AMOUNT; }

  private:
	bool m_initialized = false;
	std::string m_initializationError;

	PaStream *m_stream = nullptr;
	bool m_audioPaused = true;
	AudioRingBuffer<4096> m_audioSampleBuffer;
	static constexpr float AUDIO_SAMPLE_RATE = 44100.0;
	static constexpr size_t AUDIO_SAMPLE_AMOUNT = 1024;
};
