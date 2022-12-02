#include "Stdafx.h"
#include <mmsystem.h>
#include <dsound.h>
#include "SoundClass.h"

SoundClass::SoundClass()
= default;

SoundClass::SoundClass(const SoundClass&)
{
}

SoundClass::~SoundClass()
= default;

bool SoundClass::Initialize(const HWND hwnd)
{
    // ���̷�Ʈ ����� �� ���� ���� �ʱ�ȭ
    if (!InitializeDirectSound(hwnd))
        return false;

    // �� ���� ���ۿ� ���̺� ���� �ε�
    constexpr char filename[] = "../HLSL_DX11/Demo14/sample.wav";
    if (!LoadWaveFile(filename, &m_secondaryBuffer))
        return false;

    return PlayWaveFile();
}

void SoundClass::Shutdown()
{
    ShutdownWaveFile(&m_secondaryBuffer);
    ShutdownDirectSound();
}

bool SoundClass::InitializeDirectSound(HWND hwnd)
{
    // �⺻ ���� ����̽��� ���� ���̷�Ʈ ���� �������̽� ������ �ʱ�ȭ
    HRESULT result = DirectSoundCreate8(nullptr, &m_directSound, nullptr);
    if (FAILED(result)) return false;

    // Cooperative level�� Priority�� ���� �� ���� ���� ������ ���� ������ ��.
    result = m_directSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
    if (FAILED(result)) return false;

    // �� ���� ���� Description �ۼ�
    DSBUFFERDESC bufferDesc;
    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
    bufferDesc.dwBufferBytes = 0;
    bufferDesc.dwReserved = 0;
    bufferDesc.lpwfxFormat = nullptr;
    bufferDesc.guid3DAlgorithm = GUID_NULL;

    // �⺻ ���� ����̽��� �� ���� ���� ����.
    result = m_directSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, nullptr);
    if (FAILED(result)) return false;

    // �� ���� ������ ���� �ۼ�(.wav / 44100Hz / 16bit / Stereo = CD Quality)
    WAVEFORMATEX waveFormat;
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nChannels = 2;
    waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;

    // �� ���� ���� ���� ����
    result = m_primaryBuffer->SetFormat(&waveFormat);
    if (FAILED(result)) return false;

    return true;
}

void SoundClass::ShutdownDirectSound()
{
    if (m_primaryBuffer)
    {
        m_primaryBuffer->Release();
        m_primaryBuffer = nullptr;
    }

    if (m_directSound)
    {
        m_directSound->Release();
        m_directSound = nullptr;
    }
}

bool SoundClass::LoadWaveFile(const char* filename, IDirectSoundBuffer8** secondaryBuffer) const
{
    FILE* filePtr = nullptr;
    int error = fopen_s(&filePtr, filename, "rb");
    if (error != 0) return false;

    WaveHeaderType waveFileHeader{};
    size_t count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
    if (count != 1) return false;

    // Chunk ID�� RIFF �������� Ȯ��
    if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
        (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
        return false;

    // Sub Chunk ID�� fmt �������� Ȯ��
    if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
        (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
        return false;

    // ����� ������ WAVE_FORMAT_PCM���� Ȯ��
    if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
        return false;

    if (waveFileHeader.numChannels != 2)
        return false;

    if (waveFileHeader.sampleRate != 44100)
        return false;

    if (waveFileHeader.bitsPerSample != 16)
        return false;

    if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
        (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
        return false;

    // �ش� ���̺� ������ �ε�� �� ���� ������ ���̺� ���� �ۼ�
    WAVEFORMATEX waveFormat;
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nChannels = 2;
    waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;

    // �� ���� ���� Description �ۼ�
    DSBUFFERDESC bufferDesc;
    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
    bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
    bufferDesc.dwReserved = 0;
    bufferDesc.lpwfxFormat = &waveFormat;
    bufferDesc.guid3DAlgorithm = GUID_NULL;

    // ������ ���� �������� �ӽ� ���� ���� ���� 
    IDirectSoundBuffer* tempBuffer = nullptr;
    HRESULT result = m_directSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, nullptr);
    if (FAILED(result)) return false;

    // ���̷�Ʈ ���� 8 �������̽��� ���� ������ �������� �׽�Ʈ �� �� ���� ���� ����
    result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, reinterpret_cast<void**>(&*secondaryBuffer));
    if (FAILED(result)) return false;

    tempBuffer->Release();
    tempBuffer = nullptr;

    // ���̺� ������ ó������ ���� �����͸� �ű��
    error = fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);
    if (error != 0) return false;

    // ���̺� �����͸� ������ �ӽ� ���� ����
    auto waveData = new unsigned char[waveFileHeader.dataSize];
    if (!waveData) return false;

    count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
    if (count != waveFileHeader.dataSize) return false;

    error = fclose(filePtr);
    if (error != 0) return false;

    unsigned char* bufferPtr = nullptr;
    unsigned long bufferSize = 0;
    result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, reinterpret_cast<void**>(&bufferPtr), static_cast<DWORD*>(&bufferSize),
        nullptr, nullptr, 0);
    if (FAILED(result)) return false;

    memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

    result = (*secondaryBuffer)->Unlock(reinterpret_cast<void*>(bufferPtr), bufferSize, nullptr, 0);
    if (FAILED(result)) return false;

    delete[] waveData;
    waveData = nullptr;

    return true;
}

void SoundClass::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer)
{
    if (*secondaryBuffer)
    {
        (*secondaryBuffer)->Release();
        *secondaryBuffer = nullptr;
    }
}

bool SoundClass::PlayWaveFile() const
{
    HRESULT result = m_secondaryBuffer->SetCurrentPosition(0);
    if (FAILED(result)) return false;

    result = m_secondaryBuffer->SetVolume(DSBVOLUME_MAX);
    if (FAILED(result)) return false;

    result = m_secondaryBuffer->Play(0, 0, 0);
    if (FAILED(result)) return false;

    return true;
}
