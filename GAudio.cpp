#include "GAudio.hpp"

#define MINIAUDIO_IMPLEMENTATION
#include "libs/miniaudio.h"

#include <stdexcept>
#include <string>


#define ERROR(msg) throw std::runtime_error(std::string("[ERROR] ") + __FILE__ + "@" + std::to_string(__LINE__) + " (" + __func__ + "): " + (msg))


ma_engine* engine;


GAudio::GAudio() { ma_result result;
        result = ma_engine_init(NULL, engine); if (result != MA_SUCCESS) ERROR(std::string("Failed to initialize miniaudio engine - ") + std::to_string(result));
}

void GAudio::PlayFile(
        const char* audio_file_path,
        float pos_x = 0.0, float pos_y = 0.0, float pos_z = 0.0
) { ma_result result;
        ma_sound sound; result = ma_sound_init_from_file(engine, audio_file_path, MA_SOUND_FLAG_DECODE, NULL, NULL, &sound); if (result != MA_SUCCESS) ERROR(std::string("Failed to initialize miniaudio sound - ") + std::to_string(result));
        ma_sound_set_position(&sound, pos_x, pos_y, pos_z);
        result = ma_sound_start(&sound); if (result != MA_SUCCESS) ERROR(std::string("Failed to play miniaudio sound - ") + std::to_string(result));
}

void GAudio::SetListenerPosition(float x, float y, float z) { ma_engine_listener_set_position(engine, 0, x, y, z); }
void GAudio::SetListenerRotation(float yaw) { ma_engine_listener_set_direction(engine, 0, sin(yaw), 0, cos(yaw)); }

void GAudio::RecordMicrophone(MicrophoneDataCallback data_callback) {
        // TODO
}

void GAudio::PlayRaw(
        void* pcm_frames, unsigned int pcm_frames_count,
        GAudio::Format format,
        unsigned int channels,
        unsigned int sample_rate,
        float pos_x = 0.0, float pos_y = 0.0, float pos_z = 0.0
) {
        // TODO
}

GAudio::~GAudio() { ma_engine_uninit(engine); }