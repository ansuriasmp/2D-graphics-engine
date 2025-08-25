#include "DemoEngine_PCH.h"   
#include "AudioEngine.h"
#include <iostream>

namespace DemoEngine {

    AudioEngine::AudioEngine() {}

    AudioEngine::~AudioEngine() {
        Shutdown();
    }

    void AudioEngine::Init() {
        m_Soloud.init();
        std::cout << "[Audio] SoLoud initialized.\n";
    }

    void AudioEngine::Shutdown() {
        m_Soloud.deinit();
        std::cout << "[Audio] SoLoud shut down.\n";
    }

    void AudioEngine::PlaySound(const std::string& filepath) {
        if (m_Wav.load(filepath.c_str()) != SoLoud::SO_NO_ERROR) {
            std::cerr << "[Audio] Failed to load sound: " << filepath << "\n";
            return;
        }

        m_Soloud.play(m_Wav);
        std::cout << "[Audio] Playing sound: " << filepath << "\n";
    }

}
