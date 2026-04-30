#pragma once

#include <string>
#include <vector>

class GAudio { public: GAudio(); ~GAudio();
        void SetListenerPosition3D(float x, float y, float z);
        void SetListenerRotation3D(float yaw); // in radians, forward = -Z

        class _Sound { protected: void* sound = 0; _Sound(); public: ~_Sound();
                void SetVolume(float volume);
                void SetPosition3D(float x, float y, float z); // Spatialization is disabled by default unless this is called
        };

        void LoadSoundFileData(std::string sound_file_or_dir_path);
        class SoundFile : public _Sound { public: ~SoundFile(){};
                SoundFile(std::string loaded_sound_file_path);
                void Play(bool loop = false);
                void Stop();
        };

        enum class Format : uint8_t { U8, S16, S24, S32, F32 };

        class SoundStream : public _Sound { private: void* stream = 0; public: ~SoundStream();
                SoundStream(
                        Format format = GAudio::Format::F32,
                        uint32_t channels = 1,
                        uint32_t sample_rate = 48000
                );
                void SubmitPCM(const void* pcm_frames, uint32_t pcm_frames_count);
        };

        std::vector<float> PopMicrophoneData(); // f32, 1 channel, 48kHz (converted if different)
};