#include <alchemy/audioManager.h>
#include <iostream>
#include <cstdio>

AudioManager::AudioManager() : source(0), device(nullptr), context(nullptr) {}

AudioManager::~AudioManager() {
    cleanup();
}

AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}

void AudioManager::initialize() {
    device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open audio device" << std::endl;
        return;
    }

    context = alcCreateContext(device, nullptr);
    if (!context || !alcMakeContextCurrent(context)) {
        std::cerr << "Failed to set audio context" << std::endl;
        if (context) alcDestroyContext(context);
        alcCloseDevice(device);
        return;
    }

    alGenSources(1, &source);
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "Error generating OpenAL source: " << error << std::endl;
    }
}

void AudioManager::cleanup() {
    // add clean up
}

ALuint AudioManager::loadWAV(const std::string& filename) {
    if (soundBuffers.find(filename) != soundBuffers.end()) {
        return soundBuffers[filename];
    }

    ALuint buffer = 0;
    FILE* file = fopen(filename.c_str(), "rb");
    if (!file) {
        std::cerr << "Failed to open WAV file: " << filename << std::endl;
        return 0;
    }

    char chunkId[4];
    fread(chunkId, sizeof(char), 4, file);
    if (strncmp(chunkId, "RIFF", 4) != 0) {
        std::cerr << "Invalid WAV file: RIFF header not found" << std::endl;
        fclose(file);
        return 0;
    }

    fseek(file, 20, SEEK_SET);
    short audioFormat;
    fread(&audioFormat, sizeof(short), 1, file);
    if (audioFormat != 1) {
        std::cerr << "Unsupported WAV file format, only PCM is supported" << std::endl;
        fclose(file);
        return 0;
    }

    short channels;
    fread(&channels, sizeof(short), 1, file);
    int sampleRate;
    fread(&sampleRate, sizeof(int), 1, file);

    fseek(file, 34, SEEK_SET);
    short bitsPerSample;
    fread(&bitsPerSample, sizeof(short), 1, file);

    fseek(file, 40, SEEK_SET);
    int dataSize;
    fread(&dataSize, sizeof(int), 1, file);

    void* data = malloc(dataSize);
    fread(data, dataSize, 1, file);
    fclose(file);

    ALenum format = 0;
    if (channels == 1) {
        format = (bitsPerSample == 16) ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8;
    }
    else if (channels == 2) {
        format = (bitsPerSample == 16) ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8;
    }
    else {
        std::cerr << "Unsupported number of channels: " << channels << std::endl;
        free(data);
        return 0;
    }

    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, data, dataSize, sampleRate);
    free(data);

    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "Error loading WAV data into buffer: " << error << std::endl;
        return 0;
    }

    soundBuffers[filename] = buffer;
    return buffer;
}

void AudioManager::playSound(ALuint buffer, bool loop, float gain) {
    alSourceStop(source);
    alSourcei(source, AL_BUFFER, buffer);
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    alSourcef(source, AL_GAIN, gain);
    alSourcePlay(source);

    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "Error playing sound: " << error << std::endl;
    }
}

void AudioManager::stopSound() {
    alSourceStop(source);
}
