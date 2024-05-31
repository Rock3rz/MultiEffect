/*
  ==============================================================================

    MySlider.cpp
    Created: 29 May 2024 10:10:40pm
    Author:  pavon

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MySlider.h"

#define MARGIN 9.0f
#define VerticalSliderWidht 50
#define VerticalSliderHeight 200

//==============================================================================
MySlider::MySlider()
{

}

void MySlider::SetValueArcColour(juce::Colour MyColour) {
    ChosenColour = MyColour;
}

void MySlider::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle,
    float rotaryEndAngle, juce::Slider& slider) {

    auto radius = (float)juce::jmin(width / 2, height / 2) - MARGIN;
    auto centreX = (float)x + (float)width * 0.5f;
    auto centreY = (float)y + (float)height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    g.setColour(juce::Colour::fromRGB(52, 52, 52).brighter(0.1f)); //grigio scuro
    g.fillEllipse(rx, ry, rw, rw);

    g.setColour(juce::Colours::darkgrey.darker(0.3f));
    g.drawEllipse(rx, ry, rw, rw, 1.0f);

    //----------------------------------------------------------POINTER---------------------------------------------------------
    juce::Path p;
    auto pointerLength = radius * 0.45f;
    auto pointerThickness = 2.7f;
    p.addRectangle(-pointerThickness * 0.6, -radius -2.5, pointerThickness, pointerLength);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    g.setColour(ChosenColour);
    g.fillPath(p);


    //----------------------------------------------------------DASHED ARC------------------------------------------------------
    juce::Path dashedArc;
    dashedArc.addCentredArc(centreX, centreY, radius + 6, radius + 6, 0.0f, rotaryStartAngle, rotaryEndAngle, true);

    juce::PathStrokeType dashedStroke(2.0f);
    float dashLenghts[2] = {0.1f, 6.0f};
    dashedStroke.createDashedStroke(dashedArc, dashedArc, dashLenghts, 2);
    g.setColour(juce::Colours::darkgrey);
    g.strokePath(dashedArc, dashedStroke);  

    //------------------------------------------------------------ARC-------------------------------------------------------------
    juce::Path valueArc;
    juce::PathStrokeType stroke(4.0f);
    stroke.setEndStyle(juce::PathStrokeType::EndCapStyle::butt);
    valueArc.addCentredArc(centreX, centreY, radius + 6, radius + 6, 0.0f, rotaryStartAngle, angle, true);
    g.setColour(ChosenColour);
    g.strokePath(valueArc, stroke);
    
}

/*void drawLinearSlider(juce::Graphics& g, int x, int	y, int width, int height, float	sliderPos, float minSliderPos,
    float maxSliderPos, juce::Slider::SliderStyle, juce::Slider& slider) {

}*/