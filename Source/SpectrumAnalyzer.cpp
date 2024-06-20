/*
  ==============================================================================

    SpectrumAnalyzer.cpp
    Created: 29 May 2024 8:47:36pm
    Author:  lerco

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SpectrumAnalyzer.h"
#include "Defines.h"

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

// funzione che viene chiamata ogni tot (30 hz)

void SpectrumAnalyzer::timerCallback() {
    if (nextFFTBlockReady)
    {
        //se il prossimo blocco è pronto disegnalo e resetta il flag
        drawNextFrameOfSpectrum(); 
        nextFFTBlockReady = false;
        repaint();
    }
}

void SpectrumAnalyzer::pushNextSampleIntoFifo(float sample) noexcept
{
    if (fifoIndex == fftSize) //se il fifo è pieno
    {
        if (!nextFFTBlockReady) //se il prossimo blocco non è pronto
        {
            juce::zeromem(fftData, sizeof(fftData)); //azzera ffData
            memcpy(fftData, fifo, sizeof(fifo)); //copia i dati da FIFO a ffData
            nextFFTBlockReady = true; // il prossimo blocco è pieno
        }

        fifoIndex = 0; //resetta l'indice
    }

    fifo[fifoIndex++] = sample; //inserisci il campione nel fifo e incrementa l'indice
}

//calcolo il prossimo frame
void SpectrumAnalyzer::drawNextFrameOfSpectrum() {
    window.multiplyWithWindowingTable(fftData, fftSize); //windowing
    forwardFFT.performFrequencyOnlyForwardTransform(fftData); //esegue la FFT

    auto mindB = -100.0f; //valori minimi e massimi 
    auto maxdB = 0.0f;

    for (int i = 0; i < scopeSize; ++i)
    {   // Calcola la proporzione X distorta per la visualizzazione
        auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scopeSize) * 0.2f);

        // Trova l'indice corrispondente nei dati FFT
        auto fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionX * (float)fftSize * 0.5f));

        // Calcola il livello in base ai dB
        auto level = juce::jmap(juce::jlimit(mindB, maxdB,
            juce::Decibels::gainToDecibels(fftData[fftDataIndex]) - juce::Decibels::gainToDecibels((float)fftSize)),
            mindB, maxdB, 0.0f, 1.0f);

        scopeData[i] = level; //salva il livello nello scopedata
    }
}

void SpectrumAnalyzer::drawFrame(juce::Graphics& g) {
    //ottiene larghezza e altezza del componente
    auto width = getWidth();
    auto height = getHeight();

    // disegna il tutto
    for (int i = 1; i < scopeSize; ++i)
    {
        g.setColour(juce::Colours::blue);
        g.drawLine({ (float)juce::jmap(i - 1, 0, scopeSize - 1, 0, width),
            juce::jmap(scopeData[i - 1], 0.0f, 1.0f, (float)height, 0.0f),
            (float)juce::jmap(i, 0, scopeSize - 1, 0, width),
            juce::jmap(scopeData[i], 0.0f, 1.0f, (float)height, 0.0f) });
    }
}
