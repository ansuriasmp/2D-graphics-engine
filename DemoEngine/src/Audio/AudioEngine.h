#pragma once

#include <soloud.h>
#include <soloud_wav.h>
#include <string>

namespace DemoEngine {

    class AudioEngine
    {
    public:
        AudioEngine();
        ~AudioEngine();

        void Init();
        void Shutdown();

        void PlaySound(const std::string& filepath);

    private:
        SoLoud::Soloud m_Soloud;
        SoLoud::Wav m_Wav;
    };

}

