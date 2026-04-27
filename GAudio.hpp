#pragma once

class GAudio {
public:
        GAudio();

        void PlayFile(
                const char* audio_file_path,
                float pos_x = 0.0, float pos_y = 0.0, float pos_z = 0.0
        );

        void SetListenerPosition(float x, float y, float z);
        void SetListenerRotation(float yaw);

        enum class Format {
                U8 = 1,
                S16 = 2,
                S24 = 3,
                S32 = 4,
                F32 = 5
        };

        typedef void(*MicrophoneDataCallback)(
                void* pcm_frames, unsigned int pcm_frames_count,
                Format format,
                unsigned int channels,
                unsigned int sample_rate
        );
        void RecordMicrophone(MicrophoneDataCallback data_callback);

        void PlayRaw(
                void* pcm_frames, unsigned int pcm_frames_count,
                Format format,
                unsigned int channels,
                unsigned int sample_rate,
                float pos_x = 0.0, float pos_y = 0.0, float pos_z = 0.0
        );

        ~GAudio();
};