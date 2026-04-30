#include "GAudio.hpp"

#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_ENCODING
#define MA_NO_GENERATION
#include "libs/miniaudio.h"

#include <stdexcept>
#include <string>
#include <cstdint>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <filesystem>


#define DEVICE_IO_SAMPLE_RATE 48000
#define RING_BUFFER_SIZE 256


#define ERROR(msg) throw std::runtime_error(std::string("[ERROR] ") + __FILE__ + "@" + std::to_string(__LINE__) + " (" + __func__ + "): " + (msg))

#define _MA_SOUND_FLAGS (MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_PITCH | MA_SOUND_FLAG_NO_SPATIALIZATION)


static ma_engine engine;
static ma_device mic;
static ma_pcm_rb mic_data_ring_buffer;
static bool capturing_mic = false;


GAudio::GAudio() { ma_result result; ma_engine_config engine_config = ma_engine_config_init();
        engine_config.sampleRate = DEVICE_IO_SAMPLE_RATE;
        result = ma_engine_init(&engine_config, &engine); if (result != MA_SUCCESS) ERROR(std::string("Failed to initialize miniaudio engine - ") + std::to_string(result));

        ma_device_config mic_config = ma_device_config_init(ma_device_type_capture);
        mic_config.capture.format = ma_format_f32;
        mic_config.capture.channels = 1;
        mic_config.sampleRate = DEVICE_IO_SAMPLE_RATE;
        mic_config.dataCallback = [](ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
                void* mic_buffer_out; ma_uint32 nFramesToWrite;
                if (ma_pcm_rb_acquire_write(&mic_data_ring_buffer, &nFramesToWrite, &mic_buffer_out) != MA_SUCCESS) return;
                if (frameCount < nFramesToWrite) nFramesToWrite = frameCount;
                memcpy(mic_buffer_out, pInput, nFramesToWrite * ma_get_bytes_per_frame(pDevice->capture.format, pDevice->capture.channels));
                ma_pcm_rb_commit_write(&mic_data_ring_buffer, nFramesToWrite);
        };
        if (ma_device_init(NULL, &mic_config, &mic) == MA_SUCCESS) {
                result = ma_pcm_rb_init(ma_format_f32, 1, RING_BUFFER_SIZE, NULL, NULL, &mic_data_ring_buffer); if (result != MA_SUCCESS) ERROR("Failed to initialize ring buffer for mic data");
                if (ma_device_start(&mic) != MA_SUCCESS) { ma_device_uninit(&mic); ma_pcm_rb_uninit(&mic_data_ring_buffer); }
                else capturing_mic = true;
        }
}

void GAudio::SetListenerPosition3D(float x, float y, float z) { ma_engine_listener_set_position(&engine, 0, x, y, z); }
void GAudio::SetListenerRotation3D(float yaw) { ma_engine_listener_set_direction(&engine, 0, std::sin(yaw), 0, -std::cos(yaw)); }

GAudio::_Sound::_Sound() { this->sound = new ma_sound{}; }
void GAudio::_Sound::SetVolume(float volume) { ma_sound_set_volume((ma_sound*)this->sound, volume); }
void GAudio::_Sound::SetPosition3D(float x, float y, float z) { ma_sound_set_spatialization_enabled((ma_sound*)sound, true); ma_sound_set_position((ma_sound*)this->sound, x, y, z); }
GAudio::_Sound::~_Sound() { ma_sound_uninit((ma_sound*)sound); delete (ma_sound*)this->sound; }

std::unordered_map<std::string, ma_sound> loaded_sound_files_data;
void _LoadSoundFileData(const std::string sound_file_path) { ma_result result;
        if (loaded_sound_files_data.find(sound_file_path) != loaded_sound_files_data.end()) return;
        ma_sound _sound; result = ma_sound_init_from_file(&engine, sound_file_path.c_str(), _MA_SOUND_FLAGS, NULL, NULL, &_sound); if (result != MA_SUCCESS) ERROR(std::string("miniaudio failed to load sound data from file ") + sound_file_path + " - " + std::to_string(result));
        loaded_sound_files_data[sound_file_path] = _sound;
}
void GAudio::LoadSoundFileData(const std::string sound_file_or_dir_path) {
        if (!std::filesystem::is_directory(sound_file_or_dir_path)) _LoadSoundFileData(sound_file_or_dir_path);
        for (auto const& fs_entry : std::filesystem::recursive_directory_iterator(sound_file_or_dir_path)) {
                if (std::filesystem::is_regular_file(fs_entry)) _LoadSoundFileData(fs_entry.path().generic_string());
        }
}
GAudio::SoundFile::SoundFile(std::string loaded_sound_file_path) { ma_result result;
        if (loaded_sound_files_data.find(loaded_sound_file_path) == loaded_sound_files_data.end()) ERROR(std::string("Tried to create instance of unloaded sound file ") + loaded_sound_file_path);
        result = ma_sound_init_copy(&engine, &loaded_sound_files_data[loaded_sound_file_path], _MA_SOUND_FLAGS, NULL, (ma_sound*)sound); if (result != MA_SUCCESS) ERROR(std::string("Failed to create instance of loaded sound file ") + loaded_sound_file_path);
}
void GAudio::SoundFile::Play(bool loop) { ma_sound_seek_to_pcm_frame((ma_sound*)sound, 0); ma_sound_set_looping((ma_sound*)sound, loop); ma_sound_start((ma_sound*)sound); }
void GAudio::SoundFile::Stop() { ma_sound_stop((ma_sound*)sound); }

typedef struct {
        ma_data_source_base base;
        ma_format format;
        ma_uint32 channels;
        ma_uint32 sample_rate;
        ma_pcm_rb pcm_buffer;
} StreamDataSource;
ma_data_source_vtable stream_source_vtable = {[](ma_data_source* pDataSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead) -> ma_result { ma_result result;
        void* buffer_in; ma_uint32 nFramesToRead;
        result = ma_pcm_rb_acquire_read(&((StreamDataSource*)pDataSource)->pcm_buffer, &nFramesToRead, &buffer_in); if (result != MA_SUCCESS) return result;
        MA_ZERO_MEMORY(pFramesOut, frameCount * ma_get_bytes_per_frame(((StreamDataSource*)pDataSource)->format, ((StreamDataSource*)pDataSource)->channels));
        if (nFramesToRead > frameCount) nFramesToRead = frameCount;
        memcpy(pFramesOut, buffer_in, nFramesToRead * ma_get_bytes_per_frame(((StreamDataSource*)pDataSource)->format, ((StreamDataSource*)pDataSource)->channels));
        ma_pcm_rb_commit_read(&((StreamDataSource*)pDataSource)->pcm_buffer, nFramesToRead);
        *pFramesRead = nFramesToRead;
        return MA_SUCCESS;
}};
inline StreamDataSource StreamDataSource_init(ma_format format, ma_uint32 channels, ma_uint32 sample_rate) { ma_result result;
        StreamDataSource out;
        ma_data_source_config base_config = ma_data_source_config_init();
        base_config.vtable = &stream_source_vtable;
        result = ma_data_source_init(&base_config, &out.base); if (result != MA_SUCCESS) ERROR(std::string("Failed to initialize miniaudio custom data source - ") + std::to_string(result));
        out.format = format; out.channels = channels; out.sample_rate = sample_rate;
        result = ma_pcm_rb_init(out.format, out.channels, RING_BUFFER_SIZE, NULL, NULL, &out.pcm_buffer); if (result != MA_SUCCESS) ERROR("Failed to initialize ring buffer for SoundStream data");
        return out;
}
inline void StreamDataSource_uninit(StreamDataSource* stream_source) { ma_data_source_uninit(&stream_source->base); ma_pcm_rb_uninit(&stream_source->pcm_buffer); }
GAudio::SoundStream::SoundStream(GAudio::Format format, uint32_t channels, uint32_t sample_rate) { ma_result result;
        ma_format _format; switch (format) {
                case GAudio::Format::U8: _format = ma_format_u8; break;
                case GAudio::Format::S16: _format = ma_format_s16; break;
                case GAudio::Format::S24: _format = ma_format_s24; break;
                case GAudio::Format::S32: _format = ma_format_s32; break;
                case GAudio::Format::F32: _format = ma_format_f32; break;
        }
        this->stream = new StreamDataSource(StreamDataSource_init(_format, channels, sample_rate));
        result = ma_sound_init_from_data_source(&engine, this->stream, (MA_SOUND_FLAG_NO_PITCH | MA_SOUND_FLAG_NO_SPATIALIZATION), NULL, (ma_sound*)sound); if (result != MA_SUCCESS) ERROR("Failed to initialize ma_sound from StreamDataSource");
        ma_sound_start((ma_sound*)sound);
}
void GAudio::SoundStream::SubmitPCM(const void* pcm_frames, uint32_t pcm_frames_count) {
        void* buffer_out; ma_uint32 nFramesToWrite;
        if (ma_pcm_rb_acquire_write(&((StreamDataSource*)this->stream)->pcm_buffer, &nFramesToWrite, &buffer_out) != MA_SUCCESS) return;
        if (pcm_frames_count < nFramesToWrite) nFramesToWrite = pcm_frames_count;
        memcpy(buffer_out, pcm_frames, nFramesToWrite * ma_get_bytes_per_frame(((StreamDataSource*)this->stream)->format, ((StreamDataSource*)this->stream)->channels));
        ma_pcm_rb_commit_write(&((StreamDataSource*)this->stream)->pcm_buffer, nFramesToWrite);
        if (!ma_sound_is_playing((ma_sound*)sound)) ma_sound_start((ma_sound*)sound);
}
GAudio::SoundStream::~SoundStream() { StreamDataSource_uninit((StreamDataSource*)this->stream); delete (StreamDataSource*)this->stream; }

std::vector<float> GAudio::PopMicrophoneData() {
        if (!capturing_mic) return std::vector<float>(0);
        void* mic_buffer_in; ma_uint32 nFramesToRead;
        if (ma_pcm_rb_acquire_read(&mic_data_ring_buffer, &nFramesToRead, &mic_buffer_in) != MA_SUCCESS) return std::vector<float>(0);
        std::vector<float> mic_data(nFramesToRead);
        memcpy(mic_data.data(), mic_buffer_in, nFramesToRead * ma_get_bytes_per_frame(ma_format_f32, 1));
        ma_pcm_rb_commit_read(&mic_data_ring_buffer, nFramesToRead);
        return mic_data;
}

GAudio::~GAudio() { for (auto& [_, sound] : loaded_sound_files_data) ma_sound_uninit(&sound); ma_engine_uninit(&engine); }