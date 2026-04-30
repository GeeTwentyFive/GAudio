#include "GAudio.hpp"

#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_ENCODING
#define MA_NO_GENERATION
#include "libs/miniaudio.h"

#include <stdexcept>
#include <string>
#include <vector>


#define ERROR(msg) throw std::runtime_error(std::string("[ERROR] ") + __FILE__ + "@" + std::to_string(__LINE__) + " (" + __func__ + "): " + (msg))


ma_engine engine;


GAudio::GAudio() { ma_result result;
        result = ma_engine_init(NULL, &engine); if (result != MA_SUCCESS) ERROR(std::string("Failed to initialize miniaudio engine - ") + std::to_string(result));
}

std::vector<ma_sound> active_sounds;
void GAudio::Play(
        const char* audio_file_path,
        float pos_x = 0.0, float pos_y = 0.0, float pos_z = 0.0
) { ma_result result;
        ma_sound sound; result = ma_sound_init_from_file(&engine, audio_file_path, MA_SOUND_FLAG_DECODE, NULL, NULL, &sound); if (result != MA_SUCCESS) ERROR(std::string("Failed to initialize miniaudio sound - ") + std::to_string(result));
        ma_sound_set_position(&sound, pos_x, pos_y, pos_z);
        result = ma_sound_start(&sound); if (result != MA_SUCCESS) ERROR(std::string("Failed to play miniaudio sound - ") + std::to_string(result));
        active_sounds.push_back(sound);

        // Clean up ended sounds
        for (auto sound_it = active_sounds.begin(); sound_it != active_sounds.end(); ) {
                if (!ma_sound_at_end(&(*sound_it))) { sound_it++; continue; }
                ma_sound_uninit(&(*sound_it));
                delete &(*sound_it);
                sound_it = active_sounds.erase(sound_it);
        }
}

void GAudio::SetListenerPosition(float x, float y, float z) { ma_engine_listener_set_position(&engine, 0, x, y, z); }
void GAudio::SetListenerRotation(float yaw) { ma_engine_listener_set_direction(&engine, 0, sin(yaw), 0, cos(yaw)); }

GAudio::~GAudio() {
        for (ma_sound& sound : active_sounds) { ma_sound_uninit(&sound); delete &sound; }
        ma_engine_uninit(&engine);
}