/*
  ==============================================================================

    SpectrumAnalyzer.cpp
    Created: 29 May 2024 8:47:36pm
    Author:  lerco

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SpectrumAnalyzer.h"

//==============================================================================
SpectrumAnalyzer::SpectrumAnalyzer() :forwardFFT(fftOrder), window(fftSize, juce::dsp::WindowingFunction<float>::hann)
{
    startTimerHz(30); // Refresh rate di 30Hz
    scopeData.resize(scopeSize);
}

SpectrumAnalyzer::~SpectrumAnalyzer()
{
}

void SpectrumAnalyzer::paint (juce::Graphics& g)
{
   

    g.fillAll(juce::Colours::black);
    drawFrame(g);
}

void SpectrumAnalyzer::resized()
{
    
}

void SpectrumAnalyzer::timerCallback() {
    if (nextFFTBlockReady)
    {
        drawNextFrameOfSpectrum();
        nextFFTBlockReady = false;
        repaint();
    }
}

void SpectrumAnalyzer::pushNextSampleIntoFifo(float sample) noexcept
{
    if (fifoIndex == fftSize)
    {
        if (!nextFFTBlockReady)
        {
            juce::zeromem(fftData, sizeof(fftData));
            memcpy(fftData, fifo, sizeof(fifo));
            nextFFTBlockReady = true;
        }

        fifoIndex = 0;
    }

    fifo[fifoIndex++] = sample;
}

void SpectrumAnalyzer::drawNextFrameOfSpectrum() {
    window.multiplyWithWindowingTable(fftData, fftSize);
    forwardFFT.performFrequencyOnlyForwardTransform(fftData);

    auto mindB = -100.0f;
    auto maxdB = 0.0f;

    for (int i = 0; i < scopeSize; ++i)
    {
        auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scopeSize) * 0.2f);
        auto fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionX * (float)fftSize * 0.5f));
        auto level = juce::jmap(juce::jlimit(mindB, maxdB,
            juce::Decibels::gainToDecibels(fftData[fftDataIndex]) - juce::Decibels::gainToDecibels((float)fftSize)),
            mindB, maxdB, 0.0f, 1.0f);

        scopeData[i] = level;
    }
}

void SpectrumAnalyzer::drawFrame(juce::Graphics& g) {
    auto width = getWidth();
    auto height = getHeight();

    for (int i = 1; i < scopeSize; ++i)
    {
        g.setColour(juce::Colours::blue);
        g.drawLine({ (float)juce::jmap(i - 1, 0, scopeSize - 1, 0, width),
            juce::jmap(scopeData[i - 1], 0.0f, 1.0f, (float)height, 0.0f),
            (float)juce::jmap(i, 0, scopeSize - 1, 0, width),
            juce::jmap(scopeData[i], 0.0f, 1.0f, (float)height, 0.0f) });
    }
}
