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


GAudio::GAudio() { ma_result result = ma_engine_init(NULL, &engine); if (result != MA_SUCCESS) ERROR(std::string("Failed to initialize miniaudio engine - ") + std::to_string(result)); }

GAudio::SoundStream::SoundStream(
        Format format = GAudio::Format::F32,
        uint32_t channels = 1,
        uint32_t sample_rate = 48000
) {
        // TODO
}
void GAudio::SoundStream::SetVolume(float volume) {} // TODO
void GAudio::SoundStream::SetPosition3D(float x, float y, float z) {} // TODO
void GAudio::SoundStream::SubmitPCM(const void* pcm_frames, uint32_t pcm_frames_count) {} // TODO
GAudio::SoundStream::~SoundStream() {} // TODO

void LoadSoundFileData(std::string sound_file_or_dir_path) {
        // TODO
}
GAudio::SoundFile::SoundFile(std::string loaded_sound_file_path) {
        // TODO
}
void GAudio::SoundFile::Play(bool loop) {} // TODO
GAudio::SoundFile::~SoundFile() {} // TODO

void GAudio::SetListenerPosition3D(float x, float y, float z) { ma_engine_listener_set_position(&engine, 0, x, y, z); }
void GAudio::SetListenerRotation3D(float yaw) { ma_engine_listener_set_direction(&engine, 0, sin(yaw), 0, cos(yaw)); }

GAudio::~GAudio() { ma_engine_uninit(&engine); }