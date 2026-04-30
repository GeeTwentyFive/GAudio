#include "GAudio.hpp"

#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_ENCODING
#define MA_NO_GENERATION
#include "libs/miniaudio.h"

#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>


#define DEVICE_IO_SAMPLE_RATE 48000
#define MIC_BUFFER_SIZE 256


#define ERROR(msg) throw std::runtime_error(std::string("[ERROR] ") + __FILE__ + "@" + std::to_string(__LINE__) + " (" + __func__ + "): " + (msg))

#define _MA_SOUND_FLAGS (MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_PITCH | MA_SOUND_FLAG_NO_SPATIALIZATION)


ma_engine engine;
ma_device mic;
bool capturing_mic = false;


GAudio::GAudio() { ma_result result; ma_engine_config engine_config = ma_engine_config_init();
        engine_config.sampleRate = DEVICE_IO_SAMPLE_RATE;
        result = ma_engine_init(&engine_config, &engine); if (result != MA_SUCCESS) ERROR(std::string("Failed to initialize miniaudio engine - ") + std::to_string(result));

        ma_device_config mic_config = ma_device_config_init(ma_device_type_capture);
        mic_config.capture.format = ma_format_f32;
        mic_config.capture.channels = 1;
        mic_config.sampleRate = DEVICE_IO_SAMPLE_RATE;
        mic_config.dataCallback = [](ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
                // TODO: Write to ring buffer
        };
        if (ma_device_init(NULL, &mic_config, &mic) == MA_SUCCESS) {
                if (ma_device_start(&mic) != MA_SUCCESS) ma_device_uninit(&mic);
                else capturing_mic = true;
        }
}

void GAudio::_Sound::SetVolume(float volume) { ma_sound_set_volume(this->sound, volume); }
void GAudio::_Sound::SetPosition3D(float x, float y, float z) { ma_sound_set_spatialization_enabled(sound, true); ma_sound_set_position(this->sound, x, y, z); }
GAudio::_Sound::~_Sound() { ma_sound_uninit(sound); }

std::unordered_map<std::string, ma_sound> loaded_sound_files_data;
void _LoadSoundFileData(const std::string sound_file_path) { ma_result result;
        ma_sound _sound; result = ma_sound_init_from_file(&engine, sound_file_path.c_str(), _MA_SOUND_FLAGS, NULL, NULL, &_sound); if (result != MA_SUCCESS) ERROR(std::string("miniaudio failed to load sound data from file ") + sound_file_path + " - " + std::to_string(result));
        loaded_sound_files_data[sound_file_path] = _sound;
}
void LoadSoundFileData(const std::string sound_file_or_dir_path) {
        if (!std::filesystem::is_directory(sound_file_or_dir_path)) _LoadSoundFileData(sound_file_or_dir_path);
        for (auto const& fs_entry : std::filesystem::recursive_directory_iterator(sound_file_or_dir_path)) {
                if (std::filesystem::is_regular_file(fs_entry)) _LoadSoundFileData(fs_entry.path().generic_string());
        }
}
GAudio::SoundFile::SoundFile(std::string loaded_sound_file_path) { ma_result result;
        if (loaded_sound_files_data.find(loaded_sound_file_path) == loaded_sound_files_data.end()) ERROR(std::string("Tried to create instance of unloaded sound file ") + loaded_sound_file_path);
        result = ma_sound_init_copy(&engine, &loaded_sound_files_data[loaded_sound_file_path], _MA_SOUND_FLAGS, NULL, sound); if (result != MA_SUCCESS) ERROR(std::string("Failed to create instance of loaded sound file ") + loaded_sound_file_path);
}
void GAudio::SoundFile::Play(bool loop) { ma_sound_seek_to_pcm_frame(sound, 0); ma_sound_set_looping(sound, loop); ma_sound_start(sound); }
void GAudio::SoundFile::Stop() { ma_sound_stop(sound); }

typedef struct {
        ma_data_source_base base;
        ma_format format;
        ma_uint32 channels;
        ma_uint32 sample_rate;
        void* pcm_frames;
        uint32_t pcm_frames_count;
} StreamDataSource;
// TODO: ^ vtable
inline StreamDataSource StreamDataSource_init() { ma_result result;
        StreamDataSource out;
        ma_data_source_config base_config = ma_data_source_config_init();
        // TODO: base_config.vtable =
        result = ma_data_source_init(&base_config, &out.base); if (result != MA_SUCCESS) ERROR(std::string("Failed to initialize miniaudio custom data source - ") + std::to_string(result));
        return out;
}
inline void StreamDataSource_uninit(StreamDataSource* stream_source) { ma_data_source_uninit(&stream_source->base); }
GAudio::SoundStream::SoundStream(
        GAudio::Format format = GAudio::Format::F32,
        uint32_t channels = 1,
        uint32_t sample_rate = 48000
) {
        // TODO: data source -> ma_sound
}
void GAudio::SoundStream::SubmitPCM(const void* pcm_frames, uint32_t pcm_frames_count) {} // TODO
GAudio::SoundStream::~SoundStream() {} // TODO

std::vector<float> GAudio::PopMicrophoneData() {
        if (!capturing_mic) return std::vector<float>(0);
        std::vector<float> mic_data; mic_data.reserve(MIC_BUFFER_SIZE);
        // TODO: Pop from mic ring buffer
}

void GAudio::SetListenerPosition3D(float x, float y, float z) { ma_engine_listener_set_position(&engine, 0, x, y, z); }
void GAudio::SetListenerRotation3D(float yaw) { ma_engine_listener_set_direction(&engine, 0, sin(yaw), 0, cos(yaw)); }

GAudio::~GAudio() { ma_engine_uninit(&engine); }