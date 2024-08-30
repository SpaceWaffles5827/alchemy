#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <unordered_map>

class AudioManager {
public:
    static AudioManager& getInstance();

    void initialize();                          
    void cleanup();                           
    ALuint loadWAV(const std::string& filename);
    void playSound(ALuint buffer, bool loop = false, float gain = 1.0f);
    void stopSound();                         

private:
    AudioManager();
    ~AudioManager();

    ALuint source;
    ALCdevice* device;
    ALCcontext* context;
    std::unordered_map<std::string, ALuint> soundBuffers;
};

#endif // AUDIOMANAGER_H
