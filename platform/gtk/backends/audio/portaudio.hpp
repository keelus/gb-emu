#pragma once

#include <iostream>
#include <portaudio.h>

#include "audio/ringbuffer.hpp"
#include "backends/audio/audio_backend.hpp"
#include "utils.hpp"

class AudioBackendPortAudio : public AudioBackend {
  public:
	void initialize() override {
		PaError err = Pa_Initialize();
		if(err != paNoError) {
			m_initializationError = Pa_GetErrorText(err);
			std::string errorMsg = "Pa_Initialize() error: " + m_initializationError;

			std::cerr << "Platform[GTK]: " << errorMsg << std::endl;
			showErrorDialog("PortAudio Error", "Audio could not be initialized: \"" + errorMsg + "\"");

			return;
		}

		err = Pa_OpenDefaultStream(&m_stream, 0, 1, paFloat32, audioSampleRate(), audioSampleAmount(), audioCallback,
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

	~AudioBackendPortAudio() override {
		if(!m_initialized) { return; }

		Pa_StopStream(m_stream);
		Pa_CloseStream(m_stream);

		Pa_Terminate();
	}

	static int audioCallback(const void *input, void *output, unsigned long framesPerBuffer,
							 const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags,
							 void *userData) {
		AudioBackendPortAudio *audioSubsystem = static_cast<AudioBackendPortAudio *>(userData);
		float *buffer = (float *)output;

		for(size_t i = 0; i < framesPerBuffer; i++) {
			if(audioSubsystem->m_audioPaused || !audioSubsystem->m_audioSampleBuffer.popSample(buffer[i])) {
				buffer[i] = 0.0f;
			}
		}

		return paContinue;
	}

	void pushSample(float sample) override { m_audioSampleBuffer.pushSample(sample); }

	void restart() override {
		m_audioPaused = true;
		m_audioSampleBuffer.reset();
	}

	bool isPaused() override { return m_audioPaused; }
	void pause() override { m_audioPaused = true; }
	void unPause() override { m_audioPaused = false; }

	bool initialized() const override { return m_initialized; }

  private:
	bool m_initialized = false;
	std::string m_initializationError;

	PaStream *m_stream = nullptr;
	bool m_audioPaused = true;
	AudioRingBuffer<4096> m_audioSampleBuffer;
};
