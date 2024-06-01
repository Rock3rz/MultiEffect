/*
  ==============================================================================

    MySlider.h
    Created: 29 May 2024 10:10:40pm
    Author:  pavon

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

class MySlider : public juce::LookAndFeel_V4
{
public:
    MySlider();

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;

    void drawLinearSlider(juce::Graphics& g, int x, int	y, int width, int height, float	sliderPos, float minSliderPos,
        float maxSliderPos, juce::Slider::SliderStyle, juce::Slider& slider) override;

    void SetValueArcColour(juce::Colour MyColour);

    float normalizeValue(float value, float minValue, float maxValue);

private:

    juce::Colour ChosenColour = juce::Colours::mediumaquamarine; //colore di default

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MySlider)
};

