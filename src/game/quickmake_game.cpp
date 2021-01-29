#include "quickmake_game.h"

static void initSounds (game_sounds *gameSounds) {
    instrument_type instrumentType;
    sound_instrument *instrument;
    sound_envelope *envelope;
    sound_waveform *waveform;
    sound_filter *filter;

    instrumentType = INSTRUMENT_TYPE_PIANO;
    instrument = &gameSounds->instruments[instrumentType];
    instrument->type = instrumentType;
    envelope = &instrument->envelope;
    envelope->attackTime = 0.01f;
    envelope->attackVolume = 1.0f;
    envelope->decayTime = 0.07f;
    envelope->sustain = true;
    envelope->sustainVolume = 0.8f;
    envelope->releaseTime = 0.6f;
    waveform = &instrument->waveforms[0];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.6f;
    waveform->muliplier = 1.0f;
    waveform = &instrument->waveforms[1];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.3f;
    waveform->muliplier = 2.0f;
    waveform = &instrument->waveforms[2];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.1f;
    waveform->muliplier = 3.0f;
    instrument->numWaveForms = 3;

    gameSounds->numPlayingSounds = 0;

    instrumentType = INSTRUMENT_TYPE_TRUMPET;
    instrument = &gameSounds->instruments[instrumentType];
    instrument->type = instrumentType;
    envelope = &instrument->envelope;
    envelope->attackTime = 0.1f;
    envelope->attackVolume = 0.7f;
    envelope->decayTime = 0.07f;
    envelope->sustain = true;
    envelope->sustainVolume = 0.5f;
    envelope->releaseTime = 0.1f;
    waveform = &instrument->waveforms[0];
    waveform->waveType = OSCILLATOR_TYPE_SAWTOOTH;
    waveform->volume = 1.0f;
    waveform->muliplier = 1.0f;
    filter = &instrument->filter;
    filter->cutoff = 0.5f;
    filter->resonance = 0.2f;
    calcFeedbackForFilter(filter);
    instrument->numWaveForms = 1;

    instrumentType = INSTRUMENT_TYPE_GUITAR;
    instrument = &gameSounds->instruments[instrumentType];
    instrument->type = instrumentType;
    envelope = &instrument->envelope;
    envelope->attackTime = 0.01f;
    envelope->attackVolume = 1.0f;
    envelope->decayTime = 0.03f;
    envelope->sustain = true;
    envelope->sustainVolume = 0.6f;
    envelope->releaseTime = 0.4f;
    waveform = &instrument->waveforms[0];
    waveform->waveType = OSCILLATOR_TYPE_TRIANGLE;
    waveform->volume = 1.0f;
    waveform->muliplier = 1.0f;
    waveform = &instrument->waveforms[1];
    waveform->waveType = OSCILLATOR_TYPE_TRIANGLE;
    waveform->volume = 0.8f;
    waveform->muliplier = 2.0f;
    waveform = &instrument->waveforms[2];
    waveform->waveType = OSCILLATOR_TYPE_TRIANGLE;
    waveform->volume = 0.6f;
    waveform->muliplier = 3.0f;
    waveform = &instrument->waveforms[3];
    waveform->waveType = OSCILLATOR_TYPE_TRIANGLE;
    waveform->volume = 0.4f;
    waveform->muliplier = 4.0f;
    instrument->numWaveForms = 4;
    filter = &instrument->filter;
    filter->cutoff = 0.5f;
    filter->resonance = 0.2f;
    calcFeedbackForFilter(filter);

    instrumentType = INSTRUMENT_TYPE_VIOLIN;
    instrument = &gameSounds->instruments[instrumentType];
    instrument->type = instrumentType;
    envelope = &instrument->envelope;
    envelope->attackTime = 0.03f;
    envelope->attackVolume = 1.0f;
    envelope->decayTime = 0.15f;
    envelope->sustain = true;
    envelope->sustainVolume = 0.9f;
    envelope->releaseTime = 0.2f;
    waveform = &instrument->waveforms[0];
    waveform->waveType = OSCILLATOR_TYPE_SAWTOOTH;
    waveform->volume = 1.0f;
    waveform->muliplier = 1.0f;
    waveform = &instrument->waveforms[1];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.7f;
    waveform->muliplier = 2.0f;
    waveform = &instrument->waveforms[2];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.3f;
    waveform->muliplier = 3.0f;
    waveform = &instrument->waveforms[3];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.2f;
    waveform->muliplier = 4.0f;
    instrument->numWaveForms = 4;
    filter = &instrument->filter;
    filter->cutoff = 0.4f;
    filter->resonance = 0.3f;
    calcFeedbackForFilter(filter);

    instrumentType = INSTRUMENT_TYPE_FLUTE;
    instrument = &gameSounds->instruments[instrumentType];
    instrument->type = instrumentType;
    envelope = &instrument->envelope;
    envelope->attackTime = 0.15f;
    envelope->attackVolume = 0.5f;
    envelope->decayTime = 0.07f;
    envelope->sustain = true;
    envelope->sustainVolume = 0.45f;
    envelope->releaseTime = 0.1f;
    waveform = &instrument->waveforms[0];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 1.0f;
    waveform->muliplier = 1.0f;
    waveform = &instrument->waveforms[1];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.7f;
    waveform->muliplier = 2.0f;
    waveform = &instrument->waveforms[2];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.5f;
    waveform->muliplier = 3.0f;
    waveform = &instrument->waveforms[3];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.3f;
    waveform->muliplier = 4.0f;
    waveform = &instrument->waveforms[4];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.1f;
    waveform->muliplier = 5.0f;
    waveform = &instrument->waveforms[5];
    waveform->waveType = OSCILLATOR_TYPE_NOISE;
    waveform->volume = 0.03f;
    waveform->muliplier = 1.0f;
    instrument->numWaveForms = 6;
    filter = &instrument->filter;
    filter->cutoff = 0.4f;
    filter->resonance = 0.3f;
    calcFeedbackForFilter(filter);

    instrumentType = INSTRUMENT_TYPE_SNARE_DRUM;
    instrument = &gameSounds->instruments[instrumentType];
    instrument->type = instrumentType;
    envelope = &instrument->envelope;
    envelope->attackTime = 0.01f;
    envelope->attackVolume = 1.0f;
    envelope->decayTime = 0.01f;
    envelope->sustain = false;
    envelope->sustainVolume = 0.35f;
    envelope->releaseTime = 0.14f;
    waveform = &instrument->waveforms[0];
    waveform->waveType = OSCILLATOR_TYPE_NOISE;
    waveform->volume = 1.0f;
    waveform->muliplier = 1.0f;
    instrument->numWaveForms = 1;
    filter = &instrument->filter;
    filter->cutoff = 0.84f;
    filter->resonance = 0.2f;
    calcFeedbackForFilter(filter);
}

extern "C" void loadNextAssetFile (asset_pack_data *assetPackData, game_memory *gameMemory, memory_arena *workingMemory, 
                                   platform_options *options) 
{
    game_state *gameState = (game_state *)gameMemory->memory;
    if (!gameState->assetsInitialized) {
        gameState->assetsInitialized = true;

        gameState->memory = {};
        gameState->memory.size = 0;
        gameState->memory.capacity = gameMemory->memoryCapacity - sizeof(game_state);
        gameState->memory.base = (char *)gameMemory->memory + sizeof(game_state);

        gameState->assets = {};
        game_assets *assets = &gameState->assets;
        initGameAssets(assets, &gameState->memory);
    } 

    // if asset pack tracking not initialized
    if (assetPackData->cursor == 0) {
        assetPackData->cursor = (char *)assetPackData->assetData;
        assetPackData->numFiles = *((unsigned int *)assetPackData->assetData);
        assetPackData->cursor = (char *)assetPackData->cursor + sizeof(unsigned int);
    }

    if (assetPackData->numFiles == assetPackData->currentIndex) {
        assetPackData->complete = true;
        return;
    }

    assetPackData->complete = false;
    assetPackData->needPlatformLoad = false;
    qmpack_file_header *fileHeader = (qmpack_file_header *)assetPackData->cursor;

    assetPackData->lastAssetType = fileHeader->type;
    assetPackData->cursor = (char *)assetPackData->cursor + sizeof(qmpack_file_header);

    switch (fileHeader->type) {
    default:
        assert(false); // must provide a valid type
        break;
    case ASSET_TYPE_ANIMATION_DATA:
        //parseAnimationData(assetData, &gameState->assets, key, workingMemory);
        break;
    case ASSET_TYPE_OBJ:
        parseOBJ(assetPackData->cursor, &gameState->assets, fileHeader->name, workingMemory);
        assetPackData->needPlatformLoad = true;
        break;
    // TODO(ebuchholz): make this a real file format, and parse it
    case ASSET_TYPE_QMM:
        //parseQMM(assetData, &gameState->assets, key, workingMemory);
        break;
    case ASSET_TYPE_BMP:
        parseBitmap(assetPackData->cursor, &gameState->assets, fileHeader->name, workingMemory);
        assetPackData->needPlatformLoad = true;
        break;
    case ASSET_TYPE_WAV:
        parseWav(assetPackData->cursor, &gameState->assets, fileHeader->name, workingMemory, options);
        break;
    case ASSET_TYPE_MIDI:
        parseMidi(assetPackData->cursor, &gameState->assets, fileHeader->name, workingMemory, options);
        break;
    case ASSET_TYPE_ATLAS_DATA:
        parseAtlas(assetPackData->cursor, &gameState->assets, fileHeader->name, workingMemory);
        break;
    case ASSET_TYPE_ATLAS_TEXTURE:
        parseBitmap(assetPackData->cursor, &gameState->assets, fileHeader->name, workingMemory);
        assetPackData->needPlatformLoad = true;
        break;
    case ASSET_TYPE_DATA:
        loadDataFile(assetPackData->cursor, &gameState->assets, fileHeader->name, workingMemory, fileHeader->size);
        break;
    }
    assetPackData->cursor = (char *)assetPackData->cursor + fileHeader->size;
    ++assetPackData->currentIndex;
}


extern "C" void updateGame (game_input *input, game_memory *gameMemory, platform_options *options, game_window gameWindow) {
    game_state *gameState = (game_state *)gameMemory->memory;
    if (!gameState->gameInitialized) {
        gameState->gameInitialized = true;
        gameState->sounds = {};
        initSounds(&gameState->sounds);

        gameState->consoleGame = {};
    }

    // general purpose temporary storage
    gameState->tempMemory = {};
    gameState->tempMemory.size = 0;
    gameState->tempMemory.capacity = gameMemory->tempMemoryCapacity;
    gameState->tempMemory.base = (char *)gameMemory->tempMemory;

    // Zero memory here? since uint8array.fill is slow in firefix

    updateConsoleGame(&gameState->memory, &gameState->tempMemory, &gameState->consoleGame, &gameState->assets, input,  &gameState->sounds, gameWindow);
}

extern "C" void getGameSoundSamples (game_memory *gameMemory, game_sound_output *soundOutput) { 
    game_state *gameState = (game_state *)gameMemory->memory;
    game_sounds *gameSounds = &gameState->sounds;
    game_assets *assets = &gameState->assets;

    float volume = 1.0f;
    float dt = 1.0f / (float)soundOutput->samplesPerSecond;
    
    sound_sample *sampleOut = soundOutput->samples;

    // update background music
    updateBGM(gameSounds, sampleOut, soundOutput->sampleCount, soundOutput->samplesPerSecond);

    sampleOut = soundOutput->samples;
    for (int i = 0; i < soundOutput->sampleCount; ++i) 
    {
        float sampleValue = 0.0f;
        
        // play back synthesized instruments
        for (int soundIndex = 0; soundIndex < MAX_INSTRUMENT_SOUNDS; ++soundIndex) {
            synth_sound *sound = gameSounds->instrumentSounds + soundIndex;
            sound_instrument *instrument = gameSounds->instruments + sound->instrumentType;
            if (sound->active) {
                sampleValue += updateInstrument(sound, instrument, dt);
            }
        }

        // play back PCM audio
        for (int soundIndex = gameSounds->numPlayingSounds - 1; soundIndex >= 0; --soundIndex) {
            playing_sound *sound = gameSounds->playingSounds + soundIndex;
            sound_asset *soundAsset = getSound(assets, sound->key);

            short soundValue = soundAsset->samples[sound->currentSample];
            float floatValue = (float)soundValue / 32767;
            sampleValue += floatValue;

            ++sound->currentSample;
            if (sound->currentSample >= soundAsset->numSamples) {
                playing_sound *lastSound = gameSounds->playingSounds + (gameSounds->numPlayingSounds - 1);
                *sound = *lastSound;
                --gameSounds->numPlayingSounds;
            }
        }

        // clamp between -1 and 1
        sampleValue = sampleValue > 1.0f ? 1.0f : sampleValue;
        sampleValue = sampleValue < -1.0f ? -1.0f : sampleValue;
        sampleOut->value = (0.5f * sampleOut->value) + (0.5f * (sampleValue * volume));
        assert(sampleOut->value >= -1.0f && sampleOut->value <= 1.0f);
        ++sampleOut;
    }
}
