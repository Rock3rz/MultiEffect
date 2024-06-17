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

    //filtri non funzionanti --- 
    //juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> distortionLowPass;
    //juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> distortionHighPass;

    //albero in che gestisce tutti gli stati del plugin
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiEffectAudioProcessor)
};
