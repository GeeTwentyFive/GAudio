#include "../GAudio.hpp"

#include <stdexcept>
#include <iostream>

int main() { try {
        GAudio ga;
        ga.LoadSoundFileData("EXAMPLE.wav");

        GAudio::SoundFile example("EXAMPLE.wav");
        example.SetPosition3D(-1, 0, 0);
        example.Play(true);

        GAudio::SoundStream mic_stream{};
        mic_stream.SetPosition3D(1, 0, 0);
        for (;;) {
                std::vector<float> mic_data = ga.PopMicrophoneData();
                mic_stream.SubmitPCM(mic_data.data(), mic_data.size());
        }

        return 0;
} catch (const std::exception& e) { std::cerr << e.what() << std::endl; return 1; }}