#pragma once

#include <string>
#include <vector>

class GAudio { public: GAudio(); ~GAudio();
        class ISound { public: ISound(){} virtual ~ISound(){}
                virtual void SetVolume(float volume) = 0;
                virtual void SetPosition3D(float x, float y, float z) = 0; // Spatialization is disabled by default unless this is called
        }; // TODO: Refactor into not an interface

        void LoadSoundFileData(std::string sound_file_or_dir_path);
        class SoundFile : public ISound { public: ~SoundFile(); void SetVolume(float volume); void SetPosition3D(float x, float y, float z);
                SoundFile(std::string loaded_sound_file_path);
                void Play(bool loop);
        };

        enum class Format : uint8_t { U8, S16, S24, S32, F32 };

        class SoundStream : public ISound { public: ~SoundStream(); void SetVolume(float volume); void SetPosition3D(float x, float y, float z);
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