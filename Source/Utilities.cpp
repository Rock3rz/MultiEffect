/*
  ==============================================================================

    Utilities.cpp
    Created: 4 Jun 2024 12:13:25am
    Author:  lerco

  ==============================================================================
*/

#include <JuceHeader.h>
#include <juce_graphics/juce_graphics.h>
#include "Utilities.h"


void Utilities::drawAxis(juce::Graphics& g, int startX, int startY, int width, int height) {
    //disegno Gli assi
    juce::Path xAxis;
    juce::Path yAxis;
    juce::Line<float> xLength = juce::Line<float>(startX, startY + height / 2, startX + width, startY + height / 2);
    juce::Line<float> yLength = juce::Line<float>(startX + width / 2, startY + height, startX + width / 2, startY);
    xAxis.addArrow(xLength, .5f, 8.0f, 4.0f);
    yAxis.addArrow(yLength, .5f, 8.0f, 4.0f);

    g.setColour(juce::Colours::grey);
    g.fillPath(xAxis);
    g.fillPath(yAxis);
}

void Utilities::drawTanhFunction(juce::Graphics& g, int startX, int startY, int width, int height)
{
    // Draw background rectangle
    g.setColour(juce::Colours::darkgrey.withAlpha(0.9f).darker(0.7f));
    g.fillRoundedRectangle(startX, startY, width, height, 4);

    Utilities::drawAxis(g, startX, startY, width, height);

    // Draw the tanh function
    juce::Path tanhPath;
    g.setColour(juce::Colours::whitesmoke);

    float scaleX = (width - 20.0f) / 4.0f; // Scale factor for the x axis
    float scaleY = (height - 20.0f) / 2.0f; // Scale factor for the y axis
    float offsetX = startX + width / 2.0f; // Center of the rectangle on the x axis
    float offsetY = startY + height / 2.0f; // Center of the rectangle on the y axis

    for (float x = -2.0f; x <= 2.0f; x += 0.01f)
    {
        float y = std::tanh(x);
        float screenX = offsetX + x * scaleX;
        float screenY = offsetY - y * scaleY;

        if (x == -2.0f)
        {
            tanhPath.startNewSubPath(screenX, screenY);
        }
        else
        {
            tanhPath.lineTo(screenX, screenY);
        }
    }

    g.strokePath(tanhPath, juce::PathStrokeType(1.0f));
}

//funzione che disegna l'hard clip
void Utilities::drawHardClippingFunction(juce::Graphics& g, int startX, int startY, int width, int height)
{
    Utilities::drawAxis(g, startX, startY, width, height);
    g.setColour(juce::Colours::white);
    g.drawLine(startX + 12.0f, startY + height - 25.0f, startX + 35.0f, startY + height - 25.0f, 1.0f);
    g.drawLine(startX + 60.0f, startY + 25.0f, startX + width - 12.5f, startY + 25.0f, 1.0f);
    g.drawLine(startX + 35.0f, startY + height - 25.0f, startX + 60.0f, startY + 25.0f);

}

void Utilities::drawMidClippingFunction(juce::Graphics& g, int startX, int startY, int width, int height)
{
    // Disegna il rettangolo di sfondo


    Utilities::drawAxis(g, startX, startY, width, height);

    // Disegna la funzione di clipping intermedia
    juce::Path midClippingPath;
    g.setColour(juce::Colours::whitesmoke);

    float scaleX = (width - 20.0f) / 4.0f; // Fattore di scala per l'asse x
    float scaleY = (height - 20.0f) / 2.0f; // Fattore di scala per l'asse y
    float offsetX = startX + width / 2.0f; // Centro del rettangolo sull'asse x
    float offsetY = startY + height / 2.0f; // Centro del rettangolo sull'asse y

    for (float x = -2.0f; x <= 2.0f; x += 0.01f)
    {
        float y = std::copysign(1.0f - std::exp(-std::abs(x)), x);
        float screenX = offsetX + x * scaleX;
        float screenY = offsetY - y * scaleY;

        if (x == -2.0f)
        {
            midClippingPath.startNewSubPath(screenX, screenY);
        }
        else
        {
            midClippingPath.lineTo(screenX, screenY);
        }
    }

    g.strokePath(midClippingPath, juce::PathStrokeType(1.0f));
}

float Utilities::linearToDb(float value) {
    return 20 * std::log10(value);
}

float Utilities::linearToMs(float value) {
    return value * 1000;
}

float Utilities::linearToMhz(float value) {
    return value * 1000;
}

//funzione di normalizzazione
float Utilities::normalizeValue(float value, float minValue, float maxValue) {
    return (value - minValue) / (maxValue - minValue);
}

juce::AudioBuffer<float> Utilities::zeroBuffer(juce::AudioBuffer<float> buf,int numChannel) {
    
    for (int channel = 0; channel < numChannel; ++channel) {
        auto* channelData = buf.getWritePointer(channel);
        for (int i = 0; i < buf.getNumSamples(); ++i) {
            channelData[i] *= 0.f;
        }
    }
    return buf;
}
