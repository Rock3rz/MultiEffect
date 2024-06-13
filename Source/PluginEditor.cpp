/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utilities.h"
#include "Defines.h"


#define DelayRotaryLine 100
#define DelayPositionOffSet 360
#define everyDistortionGainY 232
#define UpRotarySlidersPosX 30


//==============================================================================
MultiEffectAudioProcessorEditor::MultiEffectAudioProcessorEditor (MultiEffectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize(globalWindowWidth, globalWindowHeight);
    setResizable(false, false);
    setLookAndFeel(nullptr);

    //-----------------------------------------------MY LOOK&FEEL COLOURS----------------------------------------------------------
    myLookAndFeelDistortion.SetValueArcColour(juce::Colour::fromRGB(163, 196, 238));
    myLookAndFeelDelayLine.SetValueArcColour(juce::Colour::fromRGB(94, 134, 193));
    myLookAndFeelReverb.SetValueArcColour(juce::Colour::fromRGB(86, 163, 255));

    //-----------------------------------------------DISTORTION--------------------------------------------------------------------
    //toggle active
    toggleActiveDistotion.setButtonText("Inactive");

    //Gain
    distortionGainSlider.setLookAndFeel(&myLookAndFeelDistortion);
    distortionGainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    distortionGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0,0);
    distortionGainSlider.setPaintingIsUnclipped(true);
    distortionGainValue.setText(juce::String(Utilities::linearToDb(distortionGainSlider.getValue()), 2) + " dB", juce::dontSendNotification);
    distortionGainSlider.onValueChange = [this]() {
        distortionGainValue.setText(juce::String(Utilities::linearToDb(distortionGainSlider.getValue()), 2) + " dB", juce::dontSendNotification);
        };
    
    

    //Offset
    distortionOffsetSlider.setLookAndFeel(&myLookAndFeelDistortion);
    distortionOffsetSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    distortionOffsetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0, 0);
    distortionOffsetSlider.setValue(0.0f);
    distortionOffsetValue.setText(juce::String(distortionOffsetSlider.getValue(), 1), juce::dontSendNotification);
    distortionOffsetSlider.onValueChange = [this]() {
        distortionOffsetValue.setText(juce::String(distortionOffsetSlider.getValue(), 2), juce::dontSendNotification);
        };
    


    //Treshold
    distortionThresholdSlider.setLookAndFeel(&myLookAndFeelDistortion);
    distortionThresholdSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    distortionThresholdSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0, 0);
    distortionThresholdValue.setText(juce::String(distortionThresholdSlider.getValue(), 1), juce::dontSendNotification);
    distortionThresholdSlider.onValueChange = [this]() {
        distortionThresholdValue.setText(juce::String(distortionThresholdSlider.getValue(), 2), juce::dontSendNotification);
        };

   

    //ToggleDistortinButtons
    softDistortion.setButtonText("Soft");
    midDistortion.setButtonText("Mid");
    hardDistortion.setButtonText("Hard");


    //Toggle active delay
    addAndMakeVisible(toggleActiveDistotion);

    //Toggle distortionType
    addAndMakeVisible(softDistortion);
    addAndMakeVisible(midDistortion);
    addAndMakeVisible(hardDistortion);

    //Labels
    addAndMakeVisible(softDistortionLabel);
    addAndMakeVisible(midDistortionLabel);
    addAndMakeVisible(hardDistortionLabel);
    addAndMakeVisible(distortionGainValue);
    addAndMakeVisible(distortionOffsetValue);
    addAndMakeVisible(distortionThresholdValue);

    //Sliders
    addAndMakeVisible(distortionGainSlider);
    addAndMakeVisible(distortionOffsetSlider);
    addAndMakeVisible(distortionThresholdSlider);

    //Utilities
    addAndMakeVisible(borderDistortionGain);
    borderDistortionGain.setText("Gain");
    borderDistortionGain.setTextLabelPosition(juce::Justification::centredTop);
    addAndMakeVisible(borderDistortionOffset);
    borderDistortionOffset.setText("Offset");
    borderDistortionOffset.setTextLabelPosition(juce::Justification::centredTop);
    addAndMakeVisible(borderDistortionTreshold);
    borderDistortionTreshold.setText("Treshold");
    borderDistortionTreshold.setTextLabelPosition(juce::Justification::centredTop);

    //Distortion attachments
    distortionGainSliderAttachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "DisGAIN", distortionGainSlider);
    distortionOffsetSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "DisOFFSET", distortionOffsetSlider);
    distortionTresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "DisTRESHOLD", distortionThresholdSlider);

    //Attivazione Distorsione
    toggleActiveDistotion.onClick = [this]() {
        toggleActiveDistotion.setButtonText(toggleActiveDistotion.getToggleState() ? "Active" : "Inactive");
        if (toggleActiveDistotion.getToggleState()) {
            audioProcessor.isDistortionActive = true;
            isDistortionSelected = true;
            repaint();
        }
        else if (!toggleActiveDistotion.getToggleState())
        {
            audioProcessor.isDistortionActive = false;
            isDistortionSelected = false;
            repaint();
        }
        };

    //creo lo stato di default
    softDistortion.setToggleState(true, true);
    isMidDistortionSelected = true;

    //condizioni di selezione tipo distorsione
    softDistortion.onClick = [this]() {
        audioProcessor.distortionType = 0;
        midDistortion.setToggleState(false, false);
        hardDistortion.setToggleState(false, false);

        //condizioni per cambiare il contorno immagine
        isSoftDistortionSelected = true;
        isMidDistortionSelected = false;
        isHardDistortionSelected = false;
        repaint();
        };

    midDistortion.onClick = [this]() {
        audioProcessor.distortionType = 1;
        softDistortion.setToggleState(false, false);
        hardDistortion.setToggleState(false, false);

        //condizioni per cambiare il contorno immagine
        isSoftDistortionSelected = false;
        isMidDistortionSelected = true;
        isHardDistortionSelected = false;
        repaint();
        };

    hardDistortion.onClick = [this]() {
        audioProcessor.distortionType = 2;
        softDistortion.setToggleState(false, false);
        midDistortion.setToggleState(false, false);

        //condizioni per cambiare il contorno immagine
        isSoftDistortionSelected = false;
        isMidDistortionSelected = false;
        isHardDistortionSelected = true;
        repaint();
        };



    //-----------------------------------------------DELAY------------------------------------------------------------------------

    //Delay activate
    toggleActiveDelay.setButtonText("Inactive");

    //Toggle LowPass Filter
    toggleActiveDelayLowPass.setButtonText("Filter");

    //Delay ToggleFeedBack
    isFeedback.setButtonText("Delay");
    isFeedForward.setButtonText("Echo");

    //Delay slider Setup
    delayGainSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    delayGainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    delayGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0, 0);
    delayGainSlider.setDoubleClickReturnValue(true, .5f);
    delayGainValue.setText(juce::String(Utilities::linearToDb(delayGainSlider.getValue())), juce::dontSendNotification);
    delayGainSlider.onValueChange = [this]() {
        delayGainValue.setText(juce::String(Utilities::linearToDb(delayGainSlider.getValue()), 1) + " dB", juce::dontSendNotification);
        };
    

    //Time delay slider Setup
    delayTimeSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    delayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0, 0);
    delayTimeMsValue.setText(juce::String(Utilities::linearToMs(delayTimeSlider.getValue()), 0) + " ms", juce::dontSendNotification);
    delayTimeSlider.onValueChange = [this]() {
        delayTimeMsValue.setText(juce::String(Utilities::linearToMs(delayTimeSlider.getValue()), 0) + " ms", juce::dontSendNotification);
        };
    

    //lowPass Filter Slider Setup
    delayLowPassFilter.setLookAndFeel(&myLookAndFeelDelayLine);
    delayLowPassFilter.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    delayLowPassFilter.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0, 0);
    delayFilterFrequencyValue.setText("Prova", juce::dontSendNotification);
    delayLowPassFilter.onValueChange = [this]() {
        delayFilterFrequencyValue.setText(juce::String(Utilities::linearToMhz(delayLowPassFilter.getValue()),0) + "Hz", juce::dontSendNotification);
        };
   

    //dryWet slider Setup
    delayDryWetSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    delayDryWetSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    delayDryWetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    delayDryWetSlider.setRange(0.0f, 1.0f);
    delayDryWetSlider.setValue(0.5f);
    delayDryWetValue.setText(juce::String(delayDryWetSlider.getValue(),1), juce::dontSendNotification);
    delayDryWetSlider.onValueChange = [this]() {
        delayDryWetValue.setText(juce::String(delayDryWetSlider.getValue(), 2), juce::dontSendNotification);
    };
    


    //ToggleActibeDelay
    addAndMakeVisible(toggleActiveDelay);

    //Toggle Low Pass
    addAndMakeVisible(toggleActiveDelayLowPass);

    //DelaySliders
    addAndMakeVisible(delayGainSlider);
    addAndMakeVisible(delayTimeSlider);
    addAndMakeVisible(delayLowPassFilter);
    addAndMakeVisible(delayDryWetSlider);
    addAndMakeVisible(delayGainValue);
    addAndMakeVisible(delayTimeMsValue);
    addAndMakeVisible(delayDryWetValue);
    addAndMakeVisible(delayFilterFrequencyValue);

    //Utilities
    addAndMakeVisible(borderDelayGain);
    borderDelayGain.setText("Gain");
    borderDelayGain.setTextLabelPosition(juce::Justification::centredTop);
    addAndMakeVisible(borderDelayTime);
    borderDelayTime.setText("Time");
    borderDelayTime.setTextLabelPosition(juce::Justification::centredTop);
    addAndMakeVisible(borderDelayLowPass);
    borderDelayLowPass.setText("            ");
    borderDelayLowPass.setTextLabelPosition(juce::Justification::centredTop);
    addAndMakeVisible(borderDelayDryWet);
    borderDelayDryWet.setText("Dry/Wet");
    borderDelayDryWet.setTextLabelPosition(juce::Justification::centredTop);
    borderDelayDryWet.setAlpha(0.7f);
    
   
    //addAndMakeVisible(delayLowPassLabel);
    addAndMakeVisible(delayDryWetLabel);

    //DelayAttachment
    delayGainSliderAttachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "DGAIN", delayGainSlider);
    delayTimeSliderAttachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "DTIME", delayTimeSlider);
    delayDryWetSliderAttachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "DDRYWET", delayDryWetSlider);
    delayLowPassFilterAttachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "DLOWFILTER", delayLowPassFilter);

    //toggle FeedBack/forward
    addAndMakeVisible(isFeedback);
    addAndMakeVisible(isFeedForward);

    isFeedForward.setToggleState(true, true);

    //condizione di attivazione feedback/feedForward
    isFeedback.onClick = [this]() {
        isFeedback.setToggleState(true, true);
        isFeedForward.setToggleState(false, false);
        bool isToggled = isFeedback.getToggleState();
        audioProcessor.isFeedbackActive = isToggled;
        };

    isFeedForward.onClick = [this]() {
        isFeedback.setToggleState(false, false);
        isFeedForward.setToggleState(true, true);
        bool isToggled = isFeedback.getToggleState();
        audioProcessor.isFeedbackActive = isToggled;
        };

    //condizione attivazione delay 
    toggleActiveDelay.onClick = [this]() {
        toggleActiveDelay.setButtonText(toggleActiveDelay.getToggleState() ? "Active" : "Inactive");
        if (toggleActiveDelay.getToggleState()) {
            audioProcessor.isDelayActive = true;
            isDelaySelected = true;
            repaint();
        }
        else if (!toggleActiveDelay.getToggleState())
        {
            audioProcessor.isDelayActive = false;
            isDelaySelected = false;
            repaint();
        }
        };

    //condizione attivazione filtro lowPass
    toggleActiveDelayLowPass.onClick = [this]() {
        if (toggleActiveDelayLowPass.getToggleState()) {
            audioProcessor.isDelayLowPassActive = true;
        }
        else if (!toggleActiveDelayLowPass.getToggleState())
        {
            audioProcessor.isDelayLowPassActive = false;
        }
        };


    //--------------------------------------------------REVERB----------------------------------------------------------------------------------
    
    //toggle active
    toggleActiveReverb.setButtonText("Inactive");

    //Room Size
    reverbRoomSizeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearBar);
    reverbRoomSizeSlider.setLookAndFeel(&myLookAndFeelReverb);
    reverbRoomSizeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    reverbRoomSizeValue.setText(juce::String(reverbRoomSizeSlider.getValue(), 1), juce::dontSendNotification);
    reverbRoomSizeSlider.onValueChange = [this]() {
        reverbRoomSizeValue.setText(juce::String(reverbRoomSizeSlider.getValue(), 2), juce::dontSendNotification);
        };

    //Damping
    reverbDampingSlider.setLookAndFeel(&myLookAndFeelReverb);
    reverbDampingSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    reverbDampingSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0, 0);
    reverbDampingValue.setText(juce::String(reverbDampingSlider.getValue(), 1), juce::dontSendNotification);
    reverbDampingSlider.onValueChange = [this]() {
        reverbDampingValue.setText(juce::String(reverbDampingSlider.getValue(), 2), juce::dontSendNotification);
        };
  

    //dry wet
    reverbDryWetLevelSlider.setLookAndFeel(&myLookAndFeelReverb);
    reverbDryWetLevelSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    reverbDryWetLevelSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    reverbDryWetValue.setText(juce::String(reverbDryWetLevelSlider.getValue(), 1), juce::dontSendNotification);
    reverbDryWetLevelSlider.onValueChange = [this]() {
        reverbDryWetValue.setText(juce::String(reverbDryWetLevelSlider.getValue(), 2), juce::dontSendNotification);
        };
    

    //width
    reverbWidthSlider.setLookAndFeel(&myLookAndFeelReverb);
    reverbWidthSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    reverbWidthSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0, 0);
    reverbWidthValue.setText(juce::String(reverbWidthSlider.getValue(), 1), juce::dontSendNotification);
    reverbWidthSlider.onValueChange = [this]() {
        reverbWidthValue.setText(juce::String(reverbWidthSlider.getValue(), 2), juce::dontSendNotification);
        };

    //Toggle active 
    addAndMakeVisible(toggleActiveReverb);

    //Sliders
    addAndMakeVisible(reverbRoomSizeSlider);
    addAndMakeVisible(reverbDampingSlider);
    addAndMakeVisible(reverbDryWetLevelSlider);
    addAndMakeVisible(reverbWidthSlider);

    //Labels
    addAndMakeVisible(reverbRoomSizeLabel);
    addAndMakeVisible(reverbDryWetLevelLabel);
    addAndMakeVisible(reverbDryWetValue);
    addAndMakeVisible(reverbRoomSizeValue);
    addAndMakeVisible(reverbDampingValue);
    addAndMakeVisible(reverbWidthValue);
    
    //Utilities
    addAndMakeVisible(borderReverbDamping);
    borderReverbDamping.setText("Damping");
    borderReverbDamping.setTextLabelPosition(juce::Justification::centredTop);
    addAndMakeVisible(borderReverbWidth);
    borderReverbWidth.setText("Width");
    borderReverbWidth.setTextLabelPosition(juce::Justification::centredTop);
    addAndMakeVisible(borderReverbDryWet);
    borderReverbDryWet.setText("Dry/Wet");
    borderReverbDamping.setTextLabelPosition(juce::Justification::centredTop);
    addAndMakeVisible(borderReverbRoomSize);
    borderReverbRoomSize.setText("Room Size");
    borderReverbWidth.setTextLabelPosition(juce::Justification::centredTop);
    
   

    //attachments reverb
    reverbRoomSizeSliderAttachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "RevROOMSIZE", reverbRoomSizeSlider);
    reverbDampingSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "RevDAMPING", reverbDampingSlider);
    reverbDryWetLevelSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "RevDRYWET", reverbDryWetLevelSlider);
    reverbWidthSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "RevWIDTH", reverbWidthSlider);

    //Attivazione reverb
    toggleActiveReverb.onClick = [this]() {
        toggleActiveReverb.setButtonText(toggleActiveReverb.getToggleState() ? "Active" : "Inactive");
        if (toggleActiveReverb.getToggleState()) {
            audioProcessor.isReverbActive = true;
            isReverbSelected = true;
            repaint();
        }
        else if (!toggleActiveReverb.getToggleState())
        {
            audioProcessor.isReverbActive = false;
            isReverbSelected = false;
            repaint();
        }
        };

    addAndMakeVisible(comboBox);
    
    comboBox.addItem("Small space", 1);
    comboBox.addItem("Cavern", 2);
    comboBox.addItem("Empty room", 3);
    comboBox.addItem("Furnished room", 4);
    comboBox.addItem("Rubber room", 5);
    comboBox.addItem("Glass room", 6);
    comboBox.addItem("None", 7);

    comboBox.setText("Select a Preset");
    comboBox.onChange = [this]() {
        int index = comboBox.getSelectedId();
        switch (index) {
        case 1:
            //spazio piccolo
            audioProcessor.presetIndex = 0;
            reverbRoomSizeSlider.setValue(SMALL_SPACE_ROOM_SIZE, juce::dontSendNotification);
            reverbRoomSizeSlider.setEnabled(false);
            reverbDampingSlider.setValue(SMALL_SPACE_DAMPING, juce::dontSendNotification);
            reverbDampingSlider.setEnabled(false);
            reverbDryWetLevelSlider.setValue(SMALL_SPACE_WET_LEVEL, juce::dontSendNotification);
            reverbDryWetLevelSlider.setEnabled(false);
            reverbWidthSlider.setValue(SMALL_SPACE_WIDTH, juce::dontSendNotification);
            reverbWidthSlider.setEnabled(false);
            reverbRoomSizeValue.setText(juce::String(reverbRoomSizeSlider.getValue(), 2), juce::dontSendNotification);
            reverbDampingValue.setText(juce::String(reverbDampingSlider.getValue(), 2), juce::dontSendNotification);
            reverbDryWetValue.setText(juce::String(reverbDryWetLevelSlider.getValue(), 2), juce::dontSendNotification);
            reverbWidthValue.setText(juce::String(reverbWidthSlider.getValue(), 2), juce::dontSendNotification);
            repaint();
                break;
        case 2:
            //Caverna
            audioProcessor.presetIndex = 1;
            reverbRoomSizeSlider.setValue(CAVERN_ROOM_SIZE, juce::dontSendNotification);
            reverbRoomSizeSlider.setEnabled(false);
            reverbDampingSlider.setValue(CAVERN_DAMPING, juce::dontSendNotification);
            reverbDampingSlider.setEnabled(false);
            reverbDryWetLevelSlider.setValue(CAVERN_WET_LEVEL, juce::dontSendNotification);
            reverbDryWetLevelSlider.setEnabled(false);
            reverbWidthSlider.setValue(CAVERN_WIDTH, juce::dontSendNotification);
            reverbWidthSlider.setEnabled(false);
            reverbRoomSizeValue.setText(juce::String(reverbRoomSizeSlider.getValue(), 2), juce::dontSendNotification);
            reverbDampingValue.setText(juce::String(reverbDampingSlider.getValue(), 2), juce::dontSendNotification);
            reverbDryWetValue.setText(juce::String(reverbDryWetLevelSlider.getValue(), 2), juce::dontSendNotification);
            reverbWidthValue.setText(juce::String(reverbWidthSlider.getValue(), 2), juce::dontSendNotification);
            repaint();
                break;
        case 3:
            //Stanza Vuota
            audioProcessor.presetIndex = 2;
            reverbRoomSizeSlider.setValue(EMPTY_ROOM_SIZE, juce::dontSendNotification);
            reverbRoomSizeSlider.setEnabled(false);
            reverbDampingSlider.setValue(EMPTY_ROOM_DAMPING, juce::dontSendNotification);
            reverbDampingSlider.setEnabled(false);
            reverbDryWetLevelSlider.setValue(EMPTY_ROOM_WET_LEVEL, juce::dontSendNotification);
            reverbDryWetLevelSlider.setEnabled(false);
            reverbWidthSlider.setValue(EMPTY_ROOM_WIDTH, juce::dontSendNotification);
            reverbWidthSlider.setEnabled(false); reverbRoomSizeValue.setText(juce::String(reverbRoomSizeSlider.getValue(), 2), juce::dontSendNotification);
            reverbDampingValue.setText(juce::String(reverbDampingSlider.getValue(), 2), juce::dontSendNotification);
            reverbDryWetValue.setText(juce::String(reverbDryWetLevelSlider.getValue(), 2), juce::dontSendNotification);
            reverbWidthValue.setText(juce::String(reverbWidthSlider.getValue(), 2), juce::dontSendNotification);
            repaint();
            break;
        case 4:
            //Stanza arredata
            audioProcessor.presetIndex = 3;
            reverbRoomSizeSlider.setValue(FURNISHED_ROOM_SIZE, juce::dontSendNotification);
            reverbRoomSizeSlider.setEnabled(false);
            reverbDampingSlider.setValue(FURNISHED_ROOM_DAMPING, juce::dontSendNotification);
            reverbDampingSlider.setEnabled(false);
            reverbDryWetLevelSlider.setValue(FURNISHED_ROOM_WET_LEVEL, juce::dontSendNotification);
            reverbDryWetLevelSlider.setEnabled(false);
            reverbWidthSlider.setValue(FURNISHED_ROOM_WIDTH, juce::dontSendNotification);
            reverbWidthSlider.setEnabled(false); reverbRoomSizeValue.setText(juce::String(reverbRoomSizeSlider.getValue(), 2), juce::dontSendNotification);
            reverbDampingValue.setText(juce::String(reverbDampingSlider.getValue(), 2), juce::dontSendNotification);
            reverbDryWetValue.setText(juce::String(reverbDryWetLevelSlider.getValue(), 2), juce::dontSendNotification);
            reverbWidthValue.setText(juce::String(reverbWidthSlider.getValue(), 2), juce::dontSendNotification);
            repaint();
            break;
        case 5:
            //stanza di gomma
            audioProcessor.presetIndex = 4;
            reverbRoomSizeSlider.setValue(RUBBER_ROOM_SIZE, juce::dontSendNotification);
            reverbRoomSizeSlider.setEnabled(false);
            reverbDampingSlider.setValue(RUBBER_ROOM_DAMPING, juce::dontSendNotification);
            reverbDampingSlider.setEnabled(false);
            reverbDryWetLevelSlider.setValue(RUBBER_ROOM_WET_LEVEL, juce::dontSendNotification);
            reverbDryWetLevelSlider.setEnabled(false);
            reverbWidthSlider.setValue(RUBBER_ROOM_WIDTH, juce::dontSendNotification);
            reverbWidthSlider.setEnabled(false); reverbRoomSizeValue.setText(juce::String(reverbRoomSizeSlider.getValue(), 2), juce::dontSendNotification);
            reverbDampingValue.setText(juce::String(reverbDampingSlider.getValue(), 2), juce::dontSendNotification);
            reverbDryWetValue.setText(juce::String(reverbDryWetLevelSlider.getValue(), 2), juce::dontSendNotification);
            reverbWidthValue.setText(juce::String(reverbWidthSlider.getValue(), 2), juce::dontSendNotification);
            repaint();
            break;
            
        case 6:
            //stanza di vetro
            audioProcessor.presetIndex = 5;
            reverbRoomSizeSlider.setValue(GLASS_ROOM_SIZE, juce::dontSendNotification);
            reverbRoomSizeSlider.setEnabled(false);
            reverbDampingSlider.setValue(GLASS_ROOM_DAMPING, juce::dontSendNotification);
            reverbDampingSlider.setEnabled(false);
            reverbDryWetLevelSlider.setValue(GLASS_ROOM_WET_LEVEL, juce::dontSendNotification);
            reverbDryWetLevelSlider.setEnabled(false);
            reverbWidthSlider.setValue(GLASS_ROOM_WIDTH, juce::dontSendNotification);
            reverbWidthSlider.setEnabled(false); reverbRoomSizeValue.setText(juce::String(reverbRoomSizeSlider.getValue(), 2), juce::dontSendNotification);
            reverbDampingValue.setText(juce::String(reverbDampingSlider.getValue(), 2), juce::dontSendNotification);
            reverbDryWetValue.setText(juce::String(reverbDryWetLevelSlider.getValue(), 2), juce::dontSendNotification);
            reverbWidthValue.setText(juce::String(reverbWidthSlider.getValue(), 2), juce::dontSendNotification);
            repaint();
            break;
        case 7:
            //None
            audioProcessor.presetIndex = 6;
            reverbRoomSizeSlider.setValue(.5f, juce::dontSendNotification);
            reverbRoomSizeSlider.setEnabled(true);
            reverbDampingSlider.setValue(0.5f, juce::dontSendNotification);
            reverbDampingSlider.setEnabled(true);
            reverbDryWetLevelSlider.setValue(.5f, juce::dontSendNotification);
            reverbDryWetLevelSlider.setEnabled(true);
            reverbWidthSlider.setValue(.5f, juce::dontSendNotification);
            reverbWidthSlider.setEnabled(true); reverbRoomSizeValue.setText(juce::String(reverbRoomSizeSlider.getValue(), 2), juce::dontSendNotification);
            reverbDampingValue.setText(juce::String(reverbDampingSlider.getValue(), 2), juce::dontSendNotification);
            reverbDryWetValue.setText(juce::String(reverbDryWetLevelSlider.getValue(), 2), juce::dontSendNotification);
            reverbWidthValue.setText(juce::String(reverbWidthSlider.getValue(), 2), juce::dontSendNotification);
            repaint();
            break;
        default:
            break;

        }
        };

    //-------------------------------------------------EQ-----------------------------------------------------------
    eqLowSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    eqLowSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    eqLowSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0, 0); // a zero per non sballare le misure
    eqLowLabel.setText("200 Hz", juce::dontSendNotification);
    eqLowValue.setText(juce::String(Utilities::linearToDb(eqLowSlider.getValue()), 1) + " dB", juce::dontSendNotification);

    //Converto i valori in decibel per Low
    eqLowSlider.onValueChange = [this]() {
        eqLowValue.setText(juce::String(Utilities::linearToDb(eqLowSlider.getValue()), 1) + " dB", juce::dontSendNotification);
        };



    eqMidSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    eqMidSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    eqMidSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0,0); // a zero per non sballare le misure
    eqMidLabel.setText("1600 Hz", juce::dontSendNotification);
    eqMidValue.setText("Prova", juce::dontSendNotification);

    //Converto i valori in decibel per mid
    eqMidSlider.onValueChange = [this]() {
        eqMidValue.setText(juce::String(Utilities::linearToDb(eqMidSlider.getValue()), 1) + " dB", juce::dontSendNotification);
        };



    eqHighSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    eqHighSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    eqHighSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0,0); // a zero per non sballare le misure
    eqHighLabel.setText("3200 Hz", juce::dontSendNotification);
    eqHighValue.setText("Prova", juce::dontSendNotification);

    //Converto i valori in decibel per High
    eqHighSlider.onValueChange = [this]() {
        eqHighValue.setText(juce::String(Utilities::linearToDb(eqHighSlider.getValue()), 1) + " dB", juce::dontSendNotification);
        };



    eqMasterOutSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    eqMasterOutSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    eqMasterOutSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0,0); // a zero per non sballare le misure
    eqMasterOutLabel.setText("Master\n  out", juce::dontSendNotification);
    eqMasterOutValue.setText("Prova", juce::dontSendNotification);


    //Converto i valori in decibel per masterOut
    eqMasterOutSlider.onValueChange = [this]() {
        eqMasterOutValue.setText(juce::String(Utilities::linearToDb(eqMasterOutSlider.getValue()), 1) + " dB", juce::dontSendNotification);
        };



    //sliders
    addAndMakeVisible(eqLowSlider);
    addAndMakeVisible(eqMidSlider);
    addAndMakeVisible(eqHighSlider);
    addAndMakeVisible(eqMasterOutSlider);

    //Labels
    addAndMakeVisible(eqLowLabel);
    addAndMakeVisible(eqMidLabel);
    addAndMakeVisible(eqHighLabel);
    addAndMakeVisible(eqMasterOutLabel);

    //ValueLabel
    addAndMakeVisible(eqLowValue);
    addAndMakeVisible(eqMidValue);
    addAndMakeVisible(eqHighValue);
    addAndMakeVisible(eqMasterOutValue);

    //attachment
    eqLowSliderAttachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "EqLOW", eqLowSlider);
    eqMidSliderAttachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "EqMID", eqMidSlider);
    eqHighSliderAttachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "EqHIGH", eqHighSlider);
    eqMasterOutSliderAttachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "EqMASTEROUTGAIN", eqMasterOutSlider);

    //--------------------------------------------------VIEWER----------------------------------------------------------------------------------
    addAndMakeVisible(audioProcessor.waveViewer);
    addAndMakeVisible(borderWV);
    borderWV.setText("Wave Viewer");
    borderWV.setTextLabelPosition(juce::Justification::centredTop);
    addAndMakeVisible(audioProcessor.spectrum);

    addAndMakeVisible(borderSV);
    borderSV.setText("Spectrum Viewer");
    borderSV.setTextLabelPosition(juce::Justification::centredTop);


    //-------------------------------------------------IMAGES------------------------------------------------------------------------------------
    
    Background = juce::ImageCache::getFromMemory(BinaryData::Sfondo_PlugIn_png, BinaryData::Sfondo_PlugIn_pngSize);

    if (Background.isNull())
    {
        DBG("Failed to load background image");
    }
    //-------------------------------------------------SPECTRUM-----------------------------------------------

    moreSamples.setButtonText("+");
    moreSamples.onClick = [this]() {
        if (audioProcessor.spectrum.getScopeSize() < 15000) {
            audioProcessor.spectrum.setScopeSize(audioProcessor.spectrum.getScopeSize() * 2);
            numSaples.setText(juce::String(audioProcessor.spectrum.getScopeSize()) + " Samples", juce::dontSendNotification);
        }
        };

    lessSamples.setButtonText("-");
    lessSamples.onClick = [this]() {
        if (audioProcessor.spectrum.getScopeSize() > 8) {
            audioProcessor.spectrum.setScopeSize(audioProcessor.spectrum.getScopeSize() / 2);
            numSaples.setText(juce::String(audioProcessor.spectrum.getScopeSize()) + " Samples", juce::dontSendNotification);
        }
        };

    numSaples.setText(juce::String(audioProcessor.spectrum.getScopeSize()) + " Samples", juce::dontSendNotification);
    addAndMakeVisible(moreSamples);
    addAndMakeVisible(lessSamples);
    addAndMakeVisible(numSaples);

   
}

MultiEffectAudioProcessorEditor::~MultiEffectAudioProcessorEditor()
{
}

//==============================================================================
void MultiEffectAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
    g.drawImage(Background, 0, 0, globalWindowWidth, globalWindowHeight, 0, 0, globalWindowWidth, globalWindowHeight, false);

    //Calcolo i limiti della finestra
    juce::Rectangle<int> MainWindow(globalWindowWidth, globalWindowHeight);

    //dimensionamento dei rettangoli principali
    juce::Rectangle<int> LeftHalf = MainWindow.removeFromLeft(globalWindowWidth - globalWindowWidth / 2.5);
    juce::Rectangle<int> RightHalf = MainWindow;


    //divisione in 4
    juce::Rectangle<int> Upper = LeftHalf.removeFromTop(LeftHalf.getHeight() / 2);
    juce::Rectangle<int> BottomLeft = LeftHalf;
    juce::Rectangle<int> UpLeft = Upper.removeFromLeft(Upper.getWidth() / 2);
    juce::Rectangle<int> UpRight = Upper;
    juce::Rectangle<int> DownLeft = BottomLeft.removeFromLeft(BottomLeft.getWidth() / 2);
    juce::Rectangle<int> DownRight = BottomLeft;
        
    // Disegna i bordi dei rettangoli
    g.setColour(juce::Colours::lightcyan);
    g.drawRect(LeftHalf, LineThickness);
    g.drawRect(RightHalf, LineThickness);
    g.drawRect(Upper, LineThickness);
    g.drawRect(BottomLeft, LineThickness);
    g.drawRect(UpLeft, LineThickness);
    g.drawRect(UpRight, LineThickness);
    g.drawRect(DownLeft, LineThickness);
    g.drawRect(DownRight, LineThickness);

    juce::Font Titles_Font("North Carossela", 17.0f, juce::Font::bold);
    g.setColour(juce::Colours::deepskyblue);
    g.setFont(Titles_Font);
    g.drawText("Distortion", distortionTitleX, distortionTitleY, everyTitleHeight, everyTitleWidth, juce::Justification::centredTop, true);
    g.drawText("Delay / Echo", delayTitleX, delayTitleY, everyTitleHeight, everyTitleWidth, juce::Justification::centredTop, true);
    g.drawText("Riverbero", revebtTitleX, reverbTitleY, everyTitleHeight, everyTitleWidth, juce::Justification::centredTop, true);
    g.drawText("Master EQ", eqTitleX, eqTitleY, everyTitleHeight, everyTitleWidth, juce::Justification::centredTop, true);

    //------------------------------------------------------DISEGNO IL RIQUADRO SELETTORE DELL'EFFETTO---------------------------
    g.setColour(juce::Colours::deepskyblue.withAlpha(.8f));
    
    if (isDistortionSelected) {
        g.drawRoundedRectangle(distortionSelectionBoxX, distortionSelectionBoxY, selectionEffectWidth, selectinoEffectHeight, selectionBorderThickness, 3); //Distorsione
    }
    else {
        g.drawRoundedRectangle(distortionSelectionBoxX, distortionSelectionBoxY, selectionEffectWidth, selectinoEffectHeight, selectionBorderThickness, 0);
    }
    if (isDelaySelected) {

        g.drawRoundedRectangle(delaySelectionBoxX, delaySelectionBoxY, selectionEffectWidth, selectinoEffectHeight, selectionBorderThickness, 3); //Delay
    } 
    else {
        g.drawRoundedRectangle(delaySelectionBoxX, delaySelectionBoxY, selectionEffectWidth, selectinoEffectHeight, selectionBorderThickness, 0);
    }
    if (isReverbSelected) {
        g.drawRoundedRectangle(reverbSelectionBoxX, reverbSelectionBoxY, selectionEffectWidth, selectinoEffectHeight, selectionBorderThickness, 3);
    }
    else {
        g.drawRoundedRectangle(reverbSelectionBoxX, reverbSelectionBoxY, selectionEffectWidth, selectinoEffectHeight, selectionBorderThickness, 0);
    }
    
    g.drawRoundedRectangle(eqSelectionBoxX, eqSelectionBoxY, selectionEffectWidth, selectinoEffectHeight, selectionBorderThickness,3);

    

    //------------------------------------------------------HARD, MID, SOFT CLIPPING DISEGNO--------------------------------------------

    //------------------------------------------------------SOFT---------------------------------------//
    g.setColour(juce::Colours::darkgrey.withAlpha(0.9f).darker(0.7f));
    g.fillRoundedRectangle(softDistortionDrawX, everyDrawY, graphicSideLenght, graphicSideLenght, 4);
    Utilities::drawTanhFunction(g, softDistortionDrawX, everyDrawY, graphicSideLenght, graphicSideLenght);

    //-----------------------------------------MID-----------------------------------------------------//

    g.setColour(juce::Colours::darkgrey.withAlpha(0.9f).darker(0.7f));
    g.fillRoundedRectangle(midDistortionDrawX, everyDrawY, graphicSideLenght, graphicSideLenght, 4);
    Utilities::drawMidClippingFunction(g, midDistortionDrawX, everyDrawY, graphicSideLenght, graphicSideLenght);
    
   
    //-------------------------------HARD---------------------------------------------------------------//
   
    g.setColour(juce::Colours::darkgrey.withAlpha(0.9f).darker(0.7f));
    g.fillRoundedRectangle(hardDistortionDrawX, 110, graphicSideLenght, graphicSideLenght, 4);
    Utilities::drawHardClippingFunction(g, hardDistortionDrawX, everyDrawY, graphicSideLenght, graphicSideLenght);
   
  
    //--------------------------------DISEGNO I CONTORNI IMMAGINE-----------------------
    g.setColour(juce::Colours::white);
    if (isSoftDistortionSelected) {
        g.drawRoundedRectangle(softDistortionDrawX, everyDrawY, graphicSideLenght, graphicSideLenght, 4, 3);
        g.drawRoundedRectangle(midDistortionDrawX, everyDrawY, graphicSideLenght, graphicSideLenght, 4, 0);
        g.drawRoundedRectangle(hardDistortionDrawX, everyDrawY, graphicSideLenght, graphicSideLenght, 4, 0);
    }
    else if (isMidDistortionSelected) {
        g.drawRoundedRectangle(softDistortionDrawX, everyDrawY, graphicSideLenght, graphicSideLenght, 4, 0);
        g.drawRoundedRectangle(midDistortionDrawX, everyDrawY, graphicSideLenght, graphicSideLenght, 4, 3);
        g.drawRoundedRectangle(hardDistortionDrawX, everyDrawY, graphicSideLenght, graphicSideLenght, 4, 0);
    }
    else if (isHardDistortionSelected) {
        g.drawRoundedRectangle(softDistortionDrawX, everyDrawY, graphicSideLenght, graphicSideLenght, 4, 0);
        g.drawRoundedRectangle(midDistortionDrawX, everyDrawY, graphicSideLenght, graphicSideLenght, 4, 0);
        g.drawRoundedRectangle(hardDistortionDrawX, everyDrawY, graphicSideLenght, graphicSideLenght, 4, 3);
    }

    //-----------------------ROUND RECTANGLES PER I VIEWER------------------------------
    g.setColour(juce::Colours::black.withAlpha(0.9f));
    g.fillRoundedRectangle(everyBlackImageX, viewerBlackImageY, everyWidth, everyHeight, everyBlackImageRoundness);
    g.fillRoundedRectangle(everyBlackImageX, spectrumBlackImageY, everyWidth, everyHeight, everyBlackImageRoundness);

    juce::Path xAxisWV;
    juce::Path yAxisWV;
    juce::Line<float> xLengthWV = juce::Line<float>(viewerXLineGraphicStartX, viewerXLineGraphicStartY, viewerXLineGraphicEndX, viewerXLineGrahpicEndY);
    juce::Line<float> yLengthWV = juce::Line<float>(viewerYLineGraphicStartX, viewerYLineGraphicStartY, viewerYLineGraphicEndX, viewerYLineGraphicEndY);
    xAxisWV.addArrow(xLengthWV, everyArrowGraphicThickness, everyArrowGraphicHeadWidth, everyArrowGraphicHeadLenght);
    yAxisWV.addArrow(yLengthWV, everyArrowGraphicThickness, everyArrowGraphicHeadWidth, everyArrowGraphicHeadLenght);
    

    juce::Font Axis_Font("Abadi", 10.0f, 0);
    g.setFont(Axis_Font);
    g.setColour(juce::Colours::white);
    g.drawText("-1", viewerEveryNumemberX, viewerLowNumberY, viewerEveryNumberLenght, viewerEveryNumberLenght, juce::Justification::centred);
    g.drawText("1", viewerEveryNumemberX, viewerHighNumberY, viewerEveryNumberLenght, viewerEveryNumberLenght, juce::Justification::centred);

    g.setColour(juce::Colours::lightcyan);
    g.fillPath(xAxisWV);
    g.fillPath(yAxisWV);


    juce::Path xAxisSV;
    juce::Path yAxisSV;
    juce::Line<float> xLengthSV = juce::Line<float>(spectrumXLineGraphicStartX, spectrumXLineGraphicStartY, spectrumYLineGraphicEndX, spectrumYLineGraphicEndY);
    g.drawLine(spectrumEveryLineGraphicX, spectrumXLineGraphicY, spectrumXLineGraphicEndX, spectrumXLineGraphicY);
    g.drawLine(spectrumEveryLineGraphicX, spectrumYLineGraphicStartY, spectrumEveryLineGraphicX, spectrumXLineGraphicStartY);
    juce::Line<float> yLengthSV = juce::Line<float>(spectrumEveryLineGraphicX, spectrumYLineGraphicStartArrow, spectrumEveryLineGraphicX, 430);
    xAxisSV.addArrow(xLengthSV, everyArrowGraphicThickness, everyArrowGraphicHeadWidth, everyArrowGraphicHeadLenght);
    yAxisSV.addArrow(yLengthSV, everyArrowGraphicThickness, everyArrowGraphicHeadWidth, everyArrowGraphicHeadLenght);

    g.fillPath(xAxisSV);
    g.fillPath(yAxisSV);

    g.setColour(juce::Colours::whitesmoke.darker(0.6f));
    g.drawText("Frequency", spectrumFreqStartX, spectrumFreqStartY, spectrumGraphicWordsHeight, spectrumGraphicWordsLenght, juce::Justification::centred);

    // Ruoto di -90 gradi per far essere il testo ruotato lungo l'asse  y
    g.addTransform(juce::AffineTransform::rotation(juce::MathConstants<float>::pi * -0.5f, 680, 595));
    g.drawText("dB", spectrumDbStartX, spectrumDBStartY, spectrumGraphicWordsHeight, spectrumGraphicWordsLenght, juce::Justification::centred);

}

void MultiEffectAudioProcessorEditor::resized()
{
    //----------------------------------------------------DISTORTION---------------------------------------------------------------------

    //Sliders
    distortionGainSlider.setBounds(distortionGainSliderPosX, everyDistortionGainY, everyRotarySliderDim, everyRotarySliderDim); //x y w h
    distortionOffsetSlider.setBounds(distortionOffsetSliderPosX, everyDistortionGainY, everyRotarySliderDim, everyRotarySliderDim);
    distortionThresholdSlider.setBounds(treshHoldSliderPosX, everyDistortionGainY, everyRotarySliderDim, everyRotarySliderDim);
    distortionGainValue.setBounds(distortionGainLabelValueX, everyDistortionLabelBalueY, everyRotarySliderDim, everyRotarySliderDim);
    distortionOffsetValue.setBounds(distortionOffsetLabelValueX, everyDistortionLabelBalueY, everyRotarySliderDim, everyRotarySliderDim);
    distortionThresholdValue.setBounds(distortionThreshHoldLabelValueX, everyDistortionLabelBalueY, everyRotarySliderDim, everyRotarySliderDim);

    //Toggle Active Distortion
    toggleActiveDistotion.setBounds(15, -25, everyToggleSide, everyToggleSide);

    //Toggle DistortionType
    softDistortion.setBounds(softDistortionToggleX, everyDistortionToggleY, everyToggleSide, everyToggleSide);
    midDistortion.setBounds(midDistortionToggleX, everyDistortionToggleY, everyToggleSide, everyToggleSide);
    hardDistortion.setBounds(hardDistortionToggleX, everyDistortionToggleY, everyToggleSide, everyToggleSide);

    softDistortionLabel.setBounds(softDistortionTypeLabelX, everyDistortionTypeLabelY, everyToggleSide, everyToggleSide);
    midDistortionLabel.setBounds(midDistortionTypeLabelX, everyDistortionTypeLabelY, everyToggleSide, everyToggleSide);
    hardDistortionLabel.setBounds(hardDistortionTypeLabelX, everyDistortionTypeLabelY, everyToggleSide, everyToggleSide);

    //Distortion Utilities
    borderDistortionGain.setBounds(distortionGainBorderX, everyDistortionBorderY, dirtortionBorderWidth, distortionBorderHeight);
    borderDistortionOffset.setBounds(distortionOffsetBorderX, everyDistortionBorderY, dirtortionBorderWidth, distortionBorderHeight);
    borderDistortionTreshold.setBounds(distortionThresholdBorderX, everyDistortionBorderY, dirtortionBorderWidth, distortionBorderHeight);


    //-----------------------------------------------------DELAY---------------------------------------------------------------------
    
    //toggle attivazione delay
    toggleActiveDelay.setBounds(15 + DelayPositionOffSet, -25, everyToggleDelayHW, everyToggleDelayHW);

    //Bounds slider delay
    delayDryWetSlider.setBounds(75 + DelayPositionOffSet, 120, delayDryWetWidth, delayDryWetHeight);
    delayGainSlider.setBounds(43 + DelayPositionOffSet, everyDistortionGainY, RotarySliderDimHW, RotarySliderDimHW);
    delayTimeSlider.setBounds(147 + DelayPositionOffSet, everyDistortionGainY, RotarySliderDimHW, RotarySliderDimHW);
    delayLowPassFilter.setBounds(248 + DelayPositionOffSet, everyDistortionGainY, RotarySliderDimHW, RotarySliderDimHW);
    delayFilterFrequencyValue.setBounds(252 + DelayPositionOffSet, everyDistortionGainY +25, 100, 100);


    //toggle fD/FW
    isFeedback.setBounds(80 + DelayPositionOffSet, toggleFDFBPosY, toggleFDFBWidth, toggleFDFBHeight);
    isFeedForward.setBounds(180 + DelayPositionOffSet, toggleFDFBPosY, toggleFDFBWidth, toggleFDFBHeight);

    //Toggle
    toggleActiveDelayLowPass.setBounds(250 + DelayPositionOffSet, 165, everyToggleDelayHW, everyToggleDelayHW);

    //Delay Utilities
    borderDelayGain.setBounds(FirstLowerElementPosX, LowerElementsBorderPosY, LowerElementsBorderWidth, LowerElementsBorderHeight);
    borderDelayTime.setBounds(FirstLowerElementPosX + FirstLowerElementsPosOffsetX, LowerElementsBorderPosY, LowerElementsBorderWidth, LowerElementsBorderHeight);
    borderDelayLowPass.setBounds(FirstLowerElementPosX + SecondLowerElementPosOffsetX, LowerElementsBorderPosY, LowerElementsBorderWidth, LowerElementsBorderHeight);
    borderDelayDryWet.setBounds(425, 90, 220, 80);

    //Value Labels
    delayGainValue.setBounds(412, delayGain_Time_ValuePosY, delayGain_Time_ValueHW, delayGain_Time_ValueHW);
    delayTimeMsValue.setBounds(507, delayGain_Time_ValuePosY, delayGain_Time_ValueHW, delayGain_Time_ValueHW);
    delayDryWetValue.setBounds(515, 143, delayDryWetWidth, delayDryWetHeight);


    //--------------------------------------------------------------REVERB------------------------------------------------------
    //Toggle
    toggleActiveReverb.setBounds(15, 325, ReverbToggleHW, ReverbToggleHW);

    //Sliders
    reverbDryWetLevelSlider.setBounds(37, reverbLinearPosY, reverbLinearSlidersWidth, reverbLinearSlidersHeight);
    reverbRoomSizeSlider.setBounds(193, reverbLinearPosY, reverbLinearSlidersWidth, reverbLinearSlidersHeight);
    reverbDampingSlider.setBounds(75, LowerElReverbPosY, RotarySliderDimHW, RotarySliderDimHW);
    reverbWidthSlider.setBounds(212, LowerElReverbPosY, RotarySliderDimHW, RotarySliderDimHW);

    //Labels
    reverbDryWetValue.setBounds(83, UpperReverbLabelsPosY, everyReverbLabelWidth, everyReverbLabelHeight);
    reverbRoomSizeValue.setBounds(237, UpperReverbLabelsPosY, everyReverbLabelWidth, everyReverbLabelHeight);
    reverbDampingValue.setBounds(89, LowerReverbLabelsPosY, everyReverbLabelWidth, everyReverbLabelHeight);
    reverbWidthValue.setBounds(226, LowerReverbLabelsPosY, everyReverbLabelWidth, everyReverbLabelHeight);

    //Reverb Utilities
    borderReverbDamping.setBounds(65, LowerReverbBorderPosY, LowerReverbBorderWidth, LowerReverbBorderHeight);
    borderReverbWidth.setBounds(202, LowerReverbBorderPosY, LowerReverbBorderWidth, LowerReverbBorderHeight);
    borderReverbDryWet.setBounds(32, UpperReverbBorderPosY, UpperReverbBorderWidth, UpperReverbBorderHeight);
    borderReverbRoomSize.setBounds(185, UpperReverbBorderPosY, UpperReverbBorderWidth, UpperReverbBorderHeight);

    comboBox.setBounds(38, 397, 280, 30);


    //-----------------------------------------------EQ------------------------------------------
    eqLowSlider.setBounds(390, eqSlidereveryY, eqSliderEveryWidth, eqSliderEveryHeight);
    eqMidSlider.setBounds(470, eqSlidereveryY, eqSliderEveryWidth, eqSliderEveryHeight);
    eqHighSlider.setBounds(550, eqSlidereveryY, eqSliderEveryWidth, eqSliderEveryHeight);
    eqMasterOutSlider.setBounds(630, eqSlidereveryY, eqSliderEveryWidth, eqSliderEveryHeight);

    eqLowLabel.setBounds(392, qeLabelsEveryY, eqLabelsEveryEdge, eqLabelsEveryEdge);
    eqMidLabel.setBounds(473, qeLabelsEveryY, eqLabelsEveryEdge, eqLabelsEveryEdge);
    eqHighLabel.setBounds(550, qeLabelsEveryY, eqLabelsEveryEdge, eqLabelsEveryEdge);
    eqMasterOutLabel.setBounds(625, qeLabelsEveryY, eqLabelsEveryEdge, eqLabelsEveryEdge);

    eqLowValue.setBounds(392, eqBordersEveryY, eqBorderEveryEdge, eqBorderEveryEdge);
    eqMidValue.setBounds(473, eqBordersEveryY, eqBorderEveryEdge, eqBorderEveryEdge);
    eqHighValue.setBounds(550, eqBordersEveryY, eqBorderEveryEdge, eqBorderEveryEdge);
    eqMasterOutValue.setBounds(630, eqBordersEveryY, eqBorderEveryEdge, eqBorderEveryEdge);

    //-------------------------------------------SPECTRUM--------------------------------------------
    moreSamples.setBounds(1150, 375, 25, 25);
    lessSamples.setBounds(1030, 375, 25, 25);
    numSaples.setBounds(1057, 340, 100, 100);


    //-------------------------------------------------------------VIEWER-------------------------------------------------------
    audioProcessor.waveViewer.setBounds(742, 90, 430, 170);
    borderWV.setBounds(720, 63, 462, 220);
    audioProcessor.spectrum.setBounds(742, 420, 430, 170);
    borderSV.setBounds(720, 393, 462, 220);    

    
}








   