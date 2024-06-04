/*
  ==============================================================================

    MySlider.cpp
    Created: 29 May 2024 10:10:40pm
    Author:  pavon

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MySlider.h"
#include "Utilities.h"

#define MARGIN 9.0f
#define VerticalSliderWidht 50
#define VerticalSliderHeight 200
#define triangleOffsetX1 20
#define triangleOffsetY1 6
#define triangleOffsetX2 8
#define triangleOffsetY2 0 
#define triangleOffsetX3 20
#define triangleOffsetY3 6

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
    juce::Colour sliderTrackColour = juce::Colours::azure;
    juce::Colour triangleColours = juce::Colours::white;
    //juce::Colour triangleColour = juce::Colours::red; // Colore per i triangoli


    if (style == juce::Slider::SliderStyle::LinearHorizontal) {
        g.setColour(sliderBackgroundColour);
        g.drawRoundedRectangle(x, y, width, height, 4, 2);

        g.setColour(juce::Colours::lightcyan);
        g.drawLine(x, y + height / 2.0f, x + width, y + height / 2.0f);
        //g.drawLine(x, y + height, x + width, y + height);


        // Calcola la posizione della traccia
        float trackWidth = width * 0.3f;
        float trackHeight = height * 0.3f;
        float trackY = y + (height - trackHeight) / 2.0f;
        float trackX = x + (width / 2.0f);


        sliderPos = Utilities::normalizeValue(sliderPos, 12.f, 188.f); //non so per quale motivo il valore dello slider non è normalizzato
        // Disegna la traccia del slider
        g.setColour(sliderTrackColour);
        if (sliderPos <= 0.5) {       
            g.fillRoundedRectangle(trackX- ((.5f - sliderPos) * width), trackY, ((.5f - sliderPos)*width), trackHeight,4);
        }
        else if (sliderPos > 0.5) {    
            g.fillRoundedRectangle(trackX, trackY, ((sliderPos-.5f)*width), trackHeight,4);
        }


        
        // Calculate the triangle position
        float trianglePosX = x + sliderPos * width;
        float trianglePosY = y + height / 2.0f;
        float h = 25.0f;
        float lato = 30.0f;

        // Draw the triangle
        juce::Path orizontalTriangle;
        orizontalTriangle.addTriangle(trianglePosX, trianglePosY,
            trianglePosX + lato / 2.0f, trianglePosY + h,
            trianglePosX - lato / 2.0f, trianglePosY + h);
        g.setColour(juce::Colours::white);
        g.fillPath(orizontalTriangle);
    }


    else if (style == juce::Slider::SliderStyle::LinearVertical)
    {

        g.setColour(juce::Colours::lightblue.withAlpha(0.5f).brighter(0.3f));  //colore per la barra
        //g.fillRect(x + (width/4) + 3, y, width * 0.4, height);

        // Calcola la posizione della traccia
        float trackWidth = width * .2f;
        float trackX = x + (width - trackWidth) / 2.0f;
        float trackStart = juce::jmin(sliderPos, maxSliderPos);
        float trackEnd = juce::jmax(sliderPos, minSliderPos);

        // Disegna la traccia del slider
        
        g.fillRoundedRectangle(trackX, trackStart, trackWidth, trackEnd - trackStart,3);       

        
        // Primo triangolo (sinistra)
        juce::Path leftTriangle;
        leftTriangle.addTriangle(trackX - triangleOffsetX1, sliderPos + triangleOffsetY1, 
            trackX - triangleOffsetX2, sliderPos + triangleOffsetY2,
            trackX - triangleOffsetX3, sliderPos - triangleOffsetY3); 
        

        // Secondo triangolo (destra)
        juce::Path rightTriangle;
        rightTriangle.addTriangle(trackX + trackWidth + triangleOffsetX1, sliderPos + triangleOffsetY1, 
            trackX + trackWidth + triangleOffsetX2, sliderPos + triangleOffsetY2, 
            trackX + trackWidth + triangleOffsetX3, sliderPos - triangleOffsetY3); 
         
        g.setColour(juce::Colours::lightcyan);    //colore dei tratti 
        g.drawLine(trackX + trackWidth + 3, y+1, trackX + trackWidth + 3, y + height);
        g.drawLine(trackX - 3, y+1, trackX - 3, y + height);

        

        g.setColour(juce::Colours::grey); // colore dei segni
      //disegno i trattini
        for (int i = 0; i <= 8; ++i) {
            auto yOffset = i * (height / 8);
            if (i % 2 == 0) {
                auto yOffset = i * (height / 8);
                g.drawLine(trackX - 7, y + yOffset, trackX - 16, y + yOffset,2);
                g.drawLine(trackX + trackWidth + 7, y + yOffset, trackX + trackWidth + 16, y + yOffset, 2);
            }
            else {
                g.drawLine(trackX - 7, y + yOffset, trackX - 12, y + yOffset, 2);
                g.drawLine(trackX + trackWidth + 7, y + yOffset, trackX + trackWidth + 12, y + yOffset, 2);
            }
        }
        

        g.setColour(triangleColours); // colore dei triangoli
        g.fillPath(leftTriangle);
        g.fillPath(rightTriangle);

        
    }

}


