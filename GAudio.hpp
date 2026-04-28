#pragma once

#include <string>
#include <vector>

class GAudio {
public:
        GAudio();

        enum class Format : uint8_t { U8, S16, S24, S32, F32 };

        class SoundStream {
        public:
                SoundStream(
                        Format format = GAudio::Format::F32,
                        uint32_t channels = 1,
                        uint32_t sample_rate = 48000
                );
                void SetVolume(float volume);
                void SetLooping(bool on);
                void SetSpatialization(bool on);
                void SetPosition3D(float x, float y, float z);
                void SetVelocity3D(float x, float y, float z);
                void SubmitPCM(const void* pcm_frames, uint32_t pcm_frames_count);
                ~SoundStream();
        };

        void LoadSoundFileData(std::string sound_file_or_dir_path);

        class SoundFile : public SoundStream {
        public:
                SoundFile(std::string loaded_sound_file_path);
                void Play();
                ~SoundFile();
        };

        void Set3DAttenuation(float min_distance, float max_distance);

        // f32, 48khz, 1 channel    (converted if different)
        std::vector<float> PopMicrophoneData();

        void SetListenerPosition3D(float x, float y, float z);
        void SetListenerRotation3D(float yaw);
        void SetListenerVelocity3D(float x, float y, float z);

        ~GAudio();
};