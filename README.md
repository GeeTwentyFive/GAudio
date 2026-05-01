# Usage example
```cpp
GAudio ga;
ga.LoadSoundFileData("sounds/");

ga.SetListenerPosition3D(7, 2, 7);
ga.SetListenerRotation(PI/2);

GAudio::SoundFile sound_from_file("sounds/sound_file.ogg")
sound_from_file.SetVolume(0.5);
sound_from_file.SetPosition3D(0, 6, 7);
sound_from_file.Play();

GAudio::SoundStream mic_stream{};
mic_stream.SetPosition3D(10, 0, 10);
for (;;) {
        std::vector<float> mic_data = ga.PopMicrophoneData();
        mic_stream.SubmitPCM(mic_data.data(), mic_data.size());
}
```


## TODO
- Fix mic -> SoundStream output stutter
- Pre-allocate buffer for PopMicrophoneData() output
