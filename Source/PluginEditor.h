/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SpectrumAnalyzer.h"
#include "MySlider.h"


//==============================================================================
/**
*/
class MultiEffectAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MultiEffectAudioProcessorEditor (MultiEffectAudioProcessor&);
    ~MultiEffectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    //-------------------------------------LOOK&FEEL-----------------------------------------------------------------------
    MySlider myLookAndFeelDistortion;
    MySlider myLookAndFeelDelayLine;
    MySlider myLookAndFeelReverb;
    MySlider myLookAndFeelEQ;
    
    //--------------------------------------DISTORTION------------------------------------------------------------------------

    //Distortion Commands
    juce::Slider distortionGainSlider;
    juce::Slider distortionOffsetSlider;
    juce::Slider distortionThresholdSlider;

    //Distortion Labels
    juce::Label distortionGainLabel;
    juce::Label distortionOffsetLabel;
    juce::Label distortionThresholdLabel;

    juce::GroupComponent borderDistortion;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionGainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionOffsetSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionTresholdAttachment;

    //toggle
    juce::ToggleButton toggleActiveDistotion;

    //--------------------------------------DELAY---------------------------------------------------------------------
        //delay Commands
    juce::Slider delayGainSlider;
    juce::Slider delayTimeSlider;
    juce::Slider delayDryWetSlider;
    juce::Slider delayLowPassFilter;

    //delay Labels
    juce::Label delayGainLabel;
    juce::Label delayTimeLabel;
    juce::Label delayLowPassLabel;
    juce::Label delayDryWetLabel;

    juce::GroupComponent borderDelay;

    //delayAttachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayGainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayDryWetSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayLowPassFilterAttachment;

    //toggle feedBack/Forward
    juce::ToggleButton isFeedback;
    juce::ToggleButton isFeedForward;
    juce::ToggleButton toggleActiveDelay;
    juce::ToggleButton toggleActiveDelayLowPass;


    //--------------------------------------REVERB---------------------------------------------------------------------
        //Reverb Commands
    juce::Slider reverbRoomSizeSlider;
    juce::Slider reverbDampingSlider;
    juce::Slider reverbDryWetLevelSlider;
    juce::Slider reverbWidthSlider;

    //Reverb Labels
    juce::Label reverbRoomSizeLabel;
    juce::Label reverbDampingLabel;
    juce::Label reverbDryWetLevelLabel;
    juce::Label reverbWidthLabel;

    juce::GroupComponent borderReverb;

    //ReverbAttachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbRoomSizeSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbDampingSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbDryWetLevelSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbWidthSliderAttachment;

    //toggle
    juce::ToggleButton toggleActiveReverb;

   //--------------------------------------------EQ---------------------------------
    
    //sliders
    juce::Slider eqLowSlider;
    juce::Slider eqMidSlider;
    juce::Slider eqHighSlider;
    juce::Slider eqMasterOutSlider;

    //Labels
    juce::Label eqLowLabel;
    juce::Label eqMidLabel;
    juce::Label eqHighLabel;
    juce::Label eqMasterOutLabel;

    juce::GroupComponent borderEQ;

    //Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqLowSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqMidSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqHighSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqMasterOutSliderAttachment;

    //AnalyserComponent spectrum;

    MultiEffectAudioProcessor& audioProcessor;

    juce::Image Background;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiEffectAudioProcessorEditor)
};
