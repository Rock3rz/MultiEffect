/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include<juce_dsp/juce_dsp.h>
#include"SpectrumAnalyzer.h"

//==============================================================================
/**
*/
// Macro per i parametri dei preset
#define SMALL_SPACE_ROOM_SIZE 0.2f
#define SMALL_SPACE_DAMPING 0.5f
#define SMALL_SPACE_WET_LEVEL 0.3f
#define SMALL_SPACE_WIDTH 0.5f

#define CAVERN_ROOM_SIZE 1.0f
#define CAVERN_DAMPING 0.8f
#define CAVERN_WET_LEVEL 0.6f
#define CAVERN_WIDTH 1.0f

#define EMPTY_ROOM_SIZE 0.6f
#define EMPTY_ROOM_DAMPING 0.2f
#define EMPTY_ROOM_WET_LEVEL 0.5f
#define EMPTY_ROOM_WIDTH 0.8f

#define FURNISHED_ROOM_SIZE 0.6f
#define FURNISHED_ROOM_DAMPING 0.7f
#define FURNISHED_ROOM_WET_LEVEL 0.5f
#define FURNISHED_ROOM_WIDTH 0.7f

#define RUBBER_ROOM_SIZE 0.4f
#define RUBBER_ROOM_DAMPING 0.9f
#define RUBBER_ROOM_WET_LEVEL 0.4f
#define RUBBER_ROOM_WIDTH 0.6f

#define GLASS_ROOM_SIZE 0.5f
#define GLASS_ROOM_DAMPING 0.1f
#define GLASS_ROOM_WET_LEVEL 0.4f
#define GLASS_ROOM_WIDTH 0.9f

struct ReverbPreset{
    const char* name;
    float roomSize;
    float damping;
    float wetLevel;
    float width;

};

class MultiEffectAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    MultiEffectAudioProcessor();
    ~MultiEffectAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void fillBuffer(juce::AudioBuffer<float>& buffer, int channel, float g);
    void readFromBuffer(int channel, juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer, float g, float t, float dryWet);
    void updateBufferPosition(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer);
    void updateFilters();
    

    juce::AudioProcessorValueTreeState apvt;
    bool isFeedbackActive = false;
    bool isDelayActive = false;
    bool isDistortionActive = false;
    bool isDelayLowPassActive = false;
    bool isReverbActive = false;
    int distortionType = 0;

    int presetIndex = 6;
    int tempIndex = 0;
    void loadPreset(int index);
    
   

    juce::AudioVisualiserComponent waveViewer; //oggetto viewer
    SpectrumAnalyzer spectrum; // analizzatore di spettro

private:
    //delay

    juce::AudioBuffer<float> delayBuffer;
    int mWritePosition{ 0 };
    std::vector<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>> DelayLowPassFilters;
    
    juce::dsp::ProcessSpec spec; //spec del delay
    juce::dsp::ProcessSpec distorsionSpec;
    juce::dsp::ProcessSpec eQSpec;

    //riverbero
    juce::Reverb reverb;
    static const ReverbPreset presets[];

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowShelf;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> peak;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> highShelf;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> distortionLowPass;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> distortionHighPass;

    //albero in che gestisce tutti gli stati del plugin
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiEffectAudioProcessor)
};
