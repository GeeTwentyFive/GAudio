#pragma once

class GAudio {
public:
        GAudio();

        void Play(
                const char* audio_file_path,
                float pos_x = 0.0, float pos_y = 0.0, float pos_z = 0.0
        );

        void SetListenerPosition(float x, float y, float z);
        void SetListenerRotation(float yaw);

        ~GAudio();
};