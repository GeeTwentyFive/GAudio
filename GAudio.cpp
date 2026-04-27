#include "GAudio.hpp"

#define MINIAUDIO_IMPLEMENTATION
#include "libs/miniaudio.h"

#include <stdexcept>
#include <string>


#define ERROR(msg) throw std::runtime_error(std::string("[ERROR] ") + __FILE__ + "@" + std::to_string(__LINE__) + " (" + __func__ + "): " + (msg))


GAudio::GAudio() {
        // TODO
}

void GAudio::Play(
                void* pcm_frames, unsigned int pcm_frames_count,
                Format format,
                unsigned int channels,
                unsigned int sample_rate,
                float pos_x = 0.0, float pos_y = 0.0, float pos_z = 0.0
) {
        // TODO
}

void GAudio::SetListenerPosition(float x, float y, float z) {
        // TODO
}
void GAudio::SetListenerRotation(float yaw) {
        // TODO
}

void GAudio::RecordMicrophone(MicrophoneDataCallback data_callback) {
        // TODO
}

GAudio::~GAudio() {
        // TODO
}