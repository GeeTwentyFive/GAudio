#pragma once

#include <string>
#include <vector>

class GAudio { public: GAudio(); ~GAudio();
        enum class Format : uint8_t { U8, S16, S24, S32, F32 };

        class SoundStream { public: ~SoundStream();
                SoundStream(
                        Format format = GAudio::Format::F32,
                        uint32_t channels = 1,
                        uint32_t sample_rate = 48000
                );
                void SetVolume(float volume);
                void SetPosition3D(float x, float y, float z); // Spatialization is disabled by default unless this is called
                void SubmitPCM(const void* pcm_frames, uint32_t pcm_frames_count);
        };

        void LoadSoundFileData(std::string sound_file_or_dir_path);
        class SoundFile : public SoundStream { public: ~SoundFile();
                SoundFile(std::string loaded_sound_file_path);
                void Play(bool loop);
        };

        void SetListenerPosition3D(float x, float y, float z);
        void SetListenerRotation3D(float yaw);

        std::vector<float> PopMicrophoneData(); // f32, 1 channel, 48khz (converted if different)
};