#pragma once

struct ma_sound; typedef struct ma_sound ma_sound;

#include <string>
#include <vector>

class GAudio { public: GAudio(); ~GAudio();
        class _Sound { protected: ma_sound* sound = 0; _Sound(){}; public: ~_Sound();
                void SetVolume(float volume);
                void SetPosition3D(float x, float y, float z); // Spatialization is disabled by default unless this is called
        };

        void LoadSoundFileData(std::string sound_file_or_dir_path);
        class SoundFile : public _Sound { public: ~SoundFile(){};
                SoundFile(std::string loaded_sound_file_path);
                void Play(bool loop);
                void Stop();
        };

        enum class Format : uint8_t { U8, S16, S24, S32, F32 };

        class SoundStream : public _Sound { public: ~SoundStream();
                SoundStream(
                        Format format = GAudio::Format::F32,
                        uint32_t channels = 1,
                        uint32_t sample_rate = 48000
                );
                void SubmitPCM(const void* pcm_frames, uint32_t pcm_frames_count);
        };

        std::vector<float> PopMicrophoneData(); // f32, 1 channel, 48kHz (converted if different)

        void SetListenerPosition3D(float x, float y, float z);
        void SetListenerRotation3D(float yaw);
};