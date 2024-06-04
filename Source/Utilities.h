/*
  ==============================================================================

    Utilities.h
    Created: 4 Jun 2024 12:13:25am
    Author:  lerco

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

static class Utilities {

public:

   static void drawAxis(juce::Graphics& g, int startX, int startY, int width, int height);
   static void drawTanhFunction(juce::Graphics& g, int startX, int startY, int width, int height);
   static void drawHardClippingFunction(juce::Graphics& g, int startX, int startY, int width, int height);
   static void drawMidClippingFunction(juce::Graphics& g, int startX, int startY, int width, int height);
   static float linearToDb(float value);
   static float linearToMs(float value);
   static float normalizeValue(float value, float minValue, float maxValue);
   
private:

};