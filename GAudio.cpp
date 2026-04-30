#include "GAudio.hpp"

#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_ENCODING
#define MA_NO_GENERATION
#include "libs/miniaudio.h"

#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>


#define DEVICE_IO_SAMPLE_RATE 48000


#define ERROR(msg) throw std::runtime_error(std::string("[ERROR] ") + __FILE__ + "@" + std::to_string(__LINE__) + " (" + __func__ + "): " + (msg))


ma_engine engine;


GAudio::GAudio() { ma_result result; ma_engine_config engine_config = ma_engine_config_init();
        engine_config.sampleRate = DEVICE_IO_SAMPLE_RATE;
        result = ma_engine_init(&engine_config, &engine); if (result != MA_SUCCESS) ERROR(std::string("Failed to initialize miniaudio engine - ") + std::to_string(result));
}

typedef struct {
        ma_data_source_base base;
        ma_format format;
        ma_uint32 channels;
        ma_uint32 sample_rate;
        void* pcm_frames;
        uint32_t pcm_frames_count;
} StreamSource;
// TODO: ^ vtable
inline StreamSource StreamSource_init() { ma_result result;
        StreamSource out;
        ma_data_source_config base_config = ma_data_source_config_init();
        // TODO: base_config.vtable =
        result = ma_data_source_init(&base_config, &out.base); if (result != MA_SUCCESS) ERROR(std::string("Failed to initialize miniaudio custom data source - ") + std::to_string(result));
        return out;
}
inline void SteamSource_uninit(StreamSource* stream_source) { ma_data_source_uninit(&stream_source->base); }
GAudio::SoundStream::SoundStream(
        GAudio::Format format = GAudio::Format::F32,
        uint32_t channels = 1,
        uint32_t sample_rate = 48000
) {
        // TODO: data source -> ma_sound
}
void GAudio::SoundStream::SetVolume(float volume) {} // TODO
void GAudio::SoundStream::SetPosition3D(float x, float y, float z) {} // TODO
void GAudio::SoundStream::SubmitPCM(const void* pcm_frames, uint32_t pcm_frames_count) {} // TODO
GAudio::SoundStream::~SoundStream() {} // TODO

std::unordered_map<std::string, ma_sound> loaded_sound_files_data;
void LoadSoundFileData(std::string sound_file_or_dir_path) {
        // TODO: ma_sound_init_from_file() + (MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_PITCH)
}
GAudio::SoundFile::SoundFile(std::string loaded_sound_file_path) {
        // TODO
}
void GAudio::SoundFile::Play(bool loop) {} // TODO: ma_sound_seek_to_pcm_frame(&sound, 0) + ma_sound_set_looping()
GAudio::SoundFile::~SoundFile() {} // TODO

std::vector<float> GAudio::PopMicrophoneData() {
        // TODO: Pop from mic ring buffer
}

void GAudio::SetListenerPosition3D(float x, float y, float z) { ma_engine_listener_set_position(&engine, 0, x, y, z); }
void GAudio::SetListenerRotation3D(float yaw) { ma_engine_listener_set_direction(&engine, 0, sin(yaw), 0, cos(yaw)); }

GAudio::~GAudio() { ma_engine_uninit(&engine); }