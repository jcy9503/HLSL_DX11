#pragma once

struct IDirectSound;
struct IDirectSound8;
struct IDirectSoundBuffer;
struct IDirectSoundBuffer8;

class SoundClass
{
private:
    struct WaveHeaderType
    {
        char chunkId[4];
        unsigned long chunkSize;
        char format[4];
        char subChunkId[4];
        unsigned long subChunkSize;
        unsigned short audioFormat;
        unsigned short numChannels;
        unsigned long sampleRate;
        unsigned long bytesPerSecond;
        unsigned short blockAlign;
        unsigned short bitsPerSample;
        char dataChunkId[4];
        unsigned long dataSize;
    };

public:
    SoundClass();
    SoundClass(const SoundClass&);
    ~SoundClass();

    bool Initialize(HWND);
    void Shutdown();

private:
    bool InitializeDirectSound(HWND);
    void ShutdownDirectSound();

    bool LoadWaveFile(const char*, IDirectSoundBuffer8**) const;
    static void ShutdownWaveFile(IDirectSoundBuffer8**);

    bool PlayWaveFile() const;

private:
    IDirectSound8* m_directSound = nullptr;
    IDirectSoundBuffer* m_primaryBuffer = nullptr;
    IDirectSoundBuffer8* m_secondaryBuffer = nullptr;
};
