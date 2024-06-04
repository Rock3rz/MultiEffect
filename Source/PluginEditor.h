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

    bool isSoftDistortionSelected = true;
    bool isMidDistortionSelected = false;
    bool isHardDistortionSelected = false;
    

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
    juce::Label softDistortionLabel;
    juce::Label midDistortionLabel;
    juce::Label hardDistortionLabel;
    juce::Label distortionGainValue;

    //Distortion Utilities
    juce::GroupComponent borderDistortionGain;
    juce::GroupComponent borderDistortionOffset;
    juce::GroupComponent borderDistortionTreshold;

    //Distortion Attachment
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionGainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionOffsetSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionTresholdAttachment;

    //toggle activation
    juce::ToggleButton toggleActiveDistotion;

    //toggle distortionType
    juce::ToggleButton softDistortion;
    juce::ToggleButton midDistortion;
    juce::ToggleButton hardDistortion;

    //--------------------------------------DELAY---------------------------------------------------------------------
        //delay Commands
    juce::Slider delayGainSlider;
    juce::Slider delayTimeSlider;
    juce::Slider delayDryWetSlider;
    juce::Slider delayLowPassFilter;

    //delay Labels
    juce::Label delayDryWetLabel;
    juce::Label delayGainValue;
    juce::Label delayTimeMsValue;
    juce::Label delayDryWetValue;
    juce::Label delayFilterFrequencyValue;
    
    //Delay Utilities
    juce::GroupComponent borderDelayGain;
    juce::GroupComponent borderDelayTime;
    juce::GroupComponent borderDelayLowPass;
    juce::GroupComponent borderDelayDryWet;


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
    juce::Label reverbDryWetLevelLabel;
    
    //Reverb Utilities
    juce::GroupComponent borderReverbDamping;
    juce::GroupComponent borderReverbWidth;

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

    //Eq DB Value
    juce::Label eqLowValue;
    juce::Label eqMidValue;
    juce::Label eqHighValue;
    juce::Label eqMasterOutValue;
    

    //Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqLowSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqMidSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqHighSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqMasterOutSliderAttachment;

   
    //------------------------------VIEWER---------------------------------

    MultiEffectAudioProcessor& audioProcessor;
    juce::GroupComponent borderWV;
    juce::GroupComponent borderSV;

    //------------------------------IMAGES----------------------------
    juce::Image Background;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiEffectAudioProcessorEditor)
};
