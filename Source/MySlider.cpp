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

void MySlider::drawLinearSlider(juce::Graphics& g, int x, int	y, int width, int height, float	sliderPos, float minSliderPos,
    float maxSliderPos, juce::Slider::SliderStyle style, juce::Slider& slider) {

    juce::Colour sliderBackgroundColour = juce::Colours::darkgrey;
    // Colore della traccia del slider
    juce::Colour sliderTrackColour = juce::Colours::blue;
    //juce::Colour triangleColour = juce::Colours::red; // Colore per i triangoli


    if (style == juce::Slider::SliderStyle::LinearHorizontal) {
        g.setColour(sliderBackgroundColour);
        g.fillRect(x, y, width, height);

        g.setColour(juce::Colours::lightcyan);
        g.drawLine(x, y + height / 2.0f, x + width, y + height / 2.0f);
        //g.drawLine(x, y + height, x + width, y + height);


        // Calcola la posizione della traccia
        float trackWidth = width * 0.3f;
        float trackHeight = height * 0.5f;
        float trackY = y + (height - trackHeight) / 2.0f;
        float trackX = x + (width / 2.0f);


        sliderPos = normalizeValue(sliderPos, 12.f, 188.f); //non so per quale motivo il valore dello slider non è normalizzato
        // Disegna la traccia del slider
        g.setColour(sliderTrackColour);
        if (sliderPos <= 0.5) {       
            g.fillRect(trackX- ((.5f - sliderPos) * width), trackY, ((.5f - sliderPos)*width), trackHeight);
        }
        else if (sliderPos > 0.5) {    
            g.fillRect(trackX, trackY, ((sliderPos-.5f)*width), trackHeight);
        }
      
    }

    else if (style == juce::Slider::SliderStyle::LinearVertical)
    {
        //la parte verticale non l'ho toccata

        g.setColour(sliderBackgroundColour);
        g.fillRect(x, y, width, height);

        g.setColour(juce::Colours::lightcyan);
        //g.drawLine(x + width, y, x + width, y + height);


        // Calcola la posizione della traccia
        float trackWidth = width * 0.3f;
        float trackX = x + (width - trackWidth) / 2.0f;
        float trackStart = juce::jmin(sliderPos, maxSliderPos);
        float trackEnd = juce::jmax(sliderPos, minSliderPos);

        // Disegna la traccia del slider
        g.setColour(sliderTrackColour);
        g.fillRect(trackX, trackStart, trackWidth, trackEnd - trackStart);

       

        /*      !!!WORK IN PROGRESS!!! NON TOCCARE!! HO SONNO!! N */
        /*
         g.setColour(triangleColour);

        // Primo triangolo (sinistra)
        juce::Path leftTriangle;
        leftTriangle.addTriangle(trackX - 10, sliderPos, // Punto in basso a sinistra
            trackX - 20, sliderPos - 5, // Punto in alto
            trackX - 10, sliderPos - 10); // Punto in basso a destra
        

        // Secondo triangolo (destra)
        juce::Path rightTriangle;
        rightTriangle.addTriangle(trackX + trackWidth + 10, sliderPos, // Punto in basso a destra
            trackX + trackWidth + 20, sliderPos - 5, // Punto in alto
            trackX + trackWidth + 10, sliderPos - 10); // Punto in basso a sinistra

        g.fillPath(leftTriangle);
        g.fillPath(rightTriangle);
        */
    }

}//funzione di normalizzazione
    float MySlider::normalizeValue(float value, float minValue, float maxValue) {
        return (value - minValue) / (maxValue - minValue);
}


