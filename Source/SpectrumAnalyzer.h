/*
  ==============================================================================

    SpectrumAnalyzer.h
    Created: 29 May 2024 8:47:36pm
    Author:  lerco

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class SpectrumAnalyzer  : public juce::Component,juce::Timer
{
public:
    SpectrumAnalyzer();
    ~SpectrumAnalyzer() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    static constexpr int fftOrder = 11;             // FFT order
    static constexpr int fftSize = 1 << fftOrder;   // FFT size
    static constexpr int scopeSize = 512;

    void pushNextSampleIntoFifo(float sample) noexcept;
    void drawFrame(juce::Graphics& g);

private:

    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;
    float fifo[fftSize];
    float fftData[2 * fftSize];
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;
    float scopeData[scopeSize];

    void timerCallback() override;
    void drawNextFrameOfSpectrum();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyzer)
};
