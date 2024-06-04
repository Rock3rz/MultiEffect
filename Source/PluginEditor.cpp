/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utilities.h"

#define WindowWidth 1192
#define WindowHeight 696
#define RectBoundThickness 2
#define DelayRotaryLine 100
#define DelayPositionOffSet 360
#define RotarySliderDimHW 85
#define UpRotarySlidersPosY 230
#define UpRotarySlidersPosX 30


//==============================================================================
MultiEffectAudioProcessorEditor::MultiEffectAudioProcessorEditor (MultiEffectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize(WindowWidth, WindowHeight);
    setResizable(false, false);
    setLookAndFeel(nullptr);

    //-----------------------------------------------MY LOOK&FEEL COLOURS----------------------------------------------------------
    myLookAndFeelDistortion.SetValueArcColour(juce::Colours::mediumaquamarine);
    myLookAndFeelDelayLine.SetValueArcColour(juce::Colours::turquoise);
    myLookAndFeelReverb.SetValueArcColour(juce::Colours::deepskyblue);
    myLookAndFeelEQ.SetValueArcColour(juce::Colours::azure);


    //-----------------------------------------------DISTORTION--------------------------------------------------------------------
    //toggle active
    toggleActiveDistotion.setButtonText("Inactive");

    //Gain
    distortionGainSlider.setLookAndFeel(&myLookAndFeelDistortion);
    distortionGainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    distortionGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0,0);
    distortionGainSlider.setPaintingIsUnclipped(true);
    distortionGainValue.setText(juce::String(Utilities::linearToDb(distortionGainSlider.getValue()), 1) + " Db", juce::dontSendNotification);
    distortionGainSlider.onValueChange = [this]() {
        distortionGainValue.setText(juce::String(Utilities::linearToDb(distortionGainSlider.getValue()), 1) + " Db", juce::dontSendNotification);
        };
    
    

    //Offset
    distortionOffsetSlider.setLookAndFeel(&myLookAndFeelDistortion);
    distortionOffsetSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    distortionOffsetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    


    //Treshold
    distortionThresholdSlider.setLookAndFeel(&myLookAndFeelDistortion);
    distortionThresholdSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    distortionThresholdSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
   

    //Distortion
    //softDistortionLabel.setText("f(x) = tanH(x)", juce::dontSendNotification);
    //midDistortionLabel.setText("f(x) = sign (x)(e^-|x|)", juce::dontSendNotification);
    //hardDistortionLabel.setText("f(x) = 1 -> x>1, -1 -> x<-1, x", juce::dontSendNotification);

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
        }
        else if (!toggleActiveDistotion.getToggleState())
        {
            audioProcessor.isDistortionActive = false;
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
    isFeedback.setButtonText("FeedBack");
    isFeedForward.setButtonText("FeedForward");

    //Delay slider Setup
    delayGainSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    delayGainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    delayGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0, 0);
    delayGainSlider.setDoubleClickReturnValue(true, .5f);
    delayGainValue.setText(juce::String(Utilities::linearToDb(delayGainSlider.getValue())), juce::dontSendNotification);
    delayGainSlider.onValueChange = [this]() {
        delayGainValue.setText(juce::String(Utilities::linearToDb(delayGainSlider.getValue()), 1) + " Db", juce::dontSendNotification);
        };
    

    //Time delay slider Setup
    delayTimeSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    delayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0, 0);
    delayTimeMsValue.setText(juce::String(Utilities::linearToMs(delayTimeSlider.getValue()), 0) + " Ms", juce::dontSendNotification);
    delayTimeSlider.onValueChange = [this]() {
        delayTimeMsValue.setText(juce::String(Utilities::linearToMs(delayTimeSlider.getValue()), 0) + " Ms", juce::dontSendNotification);
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
        }
        else if (!toggleActiveDelay.getToggleState())
        {
            audioProcessor.isDelayActive = false;
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
    reverbRoomSizeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    reverbRoomSizeSlider.setLookAndFeel(&myLookAndFeelReverb);
    reverbRoomSizeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

    //Damping
    reverbDampingSlider.setLookAndFeel(&myLookAndFeelReverb);
    reverbDampingSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    reverbDampingSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
  

    //dry wet
    reverbDryWetLevelSlider.setLookAndFeel(&myLookAndFeelReverb);
    reverbDryWetLevelSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    reverbDryWetLevelSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    ReverbDryWetValue.setText(juce::String(reverbDryWetLevelSlider.getValue(), 1), juce::dontSendNotification);
    reverbDryWetLevelSlider.onValueChange = [this]() {
        ReverbDryWetValue.setText(juce::String(reverbDryWetLevelSlider.getValue(), 2), juce::dontSendNotification);
        };
    

    //width
    reverbWidthSlider.setLookAndFeel(&myLookAndFeelReverb);
    reverbWidthSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    reverbWidthSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    

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
    addAndMakeVisible(ReverbDryWetValue);
    addAndMakeVisible(ReverbRoomSizeValue);

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
        }
        else if (!toggleActiveReverb.getToggleState())
        {
            audioProcessor.isReverbActive = false;
        }
        };

    //-------------------------------------------------EQ---------------------------------
    eqLowSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    eqLowSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    eqLowSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0, 0); // a zero per non sballare le misure
    eqLowLabel.setText("Low", juce::dontSendNotification);
    eqLowValue.setText(juce::String(Utilities::linearToDb(eqLowSlider.getValue()), 1) + " Db", juce::dontSendNotification);

    //Converto i valori in decibel per Low
    eqLowSlider.onValueChange = [this]() {
        eqLowValue.setText(juce::String(Utilities::linearToDb(eqLowSlider.getValue()), 1) + " Db", juce::dontSendNotification);
        };



    eqMidSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    eqMidSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    eqMidSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0,0); // a zero per non sballare le misure
    eqMidLabel.setText("Mid", juce::dontSendNotification);
    eqMidValue.setText("Prova", juce::dontSendNotification);

    //Converto i valori in decibel per mid
    eqMidSlider.onValueChange = [this]() {
        eqMidValue.setText(juce::String(Utilities::linearToDb(eqMidSlider.getValue()), 1) + " Db", juce::dontSendNotification);
        };



    eqHighSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    eqHighSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    eqHighSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0,0); // a zero per non sballare le misure
    eqHighLabel.setText("High", juce::dontSendNotification);
    eqHighValue.setText("Prova", juce::dontSendNotification);

    //Converto i valori in decibel per High
    eqHighSlider.onValueChange = [this]() {
        eqHighValue.setText(juce::String(Utilities::linearToDb(eqHighSlider.getValue()), 1) + " Db", juce::dontSendNotification);
        };



    eqMasterOutSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    eqMasterOutSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    eqMasterOutSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 0,0); // a zero per non sballare le misure
    eqMasterOutLabel.setText("Master\n  out", juce::dontSendNotification);
    eqMasterOutValue.setText("Prova", juce::dontSendNotification);


    //Converto i valori in decibel per masterOut
    eqMasterOutSlider.onValueChange = [this]() {
        eqMasterOutValue.setText(juce::String(Utilities::linearToDb(eqMasterOutSlider.getValue()), 1) + " Db", juce::dontSendNotification);
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
    
    Background = juce::ImageCache::getFromMemory(BinaryData::Sfondo_PlugIn5_png, BinaryData::Sfondo_PlugIn5_pngSize);

    if (Background.isNull())
    {
        DBG("Failed to load background image");
    }
    
}

MultiEffectAudioProcessorEditor::~MultiEffectAudioProcessorEditor()
{
}

//==============================================================================
void MultiEffectAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
    g.drawImage(Background, 0, 0, WindowWidth, WindowHeight, 0, 0, WindowWidth, WindowHeight, false);

    //Calcolo i limiti della finestra
    juce::Rectangle<int> MainWindow(WindowWidth, WindowHeight);

    //dimensionamento dei rettangoli principali
    juce::Rectangle<int> LeftHalf = MainWindow.removeFromLeft(WindowWidth - WindowWidth / 2.5);
    juce::Rectangle<int> RightHalf = MainWindow;


    juce::Font Titles_Font("North Carossela", 17.0f, juce::Font::bold);
    g.setColour(juce::Colours::deepskyblue);
    g.setFont(Titles_Font);
    g.drawText("Distortion", 132, 15, 90, 15, juce::Justification::centredTop, true);
    g.drawText("Delay Line", 490, 15, 90, 15, juce::Justification::centredTop, true);
    g.drawText("Riverbero", 132, 360, 90, 15, juce::Justification::centredTop, true);
    g.drawText("Master EQ", 490, 360, 90, 15, juce::Justification::centredTop, true);


    //------------------------------------------------------HARD, MID, SOFT CLIPPING DISEGNO--------------------------------------------

    //------------------------------------------------------SOFT------------------------------------------------------------------------
    g.setColour(juce::Colours::darkgrey.withAlpha(0.9f).darker(0.7f));
    g.fillRoundedRectangle(35, 110, 75, 75, 4);
    Utilities::drawTanhFunction(g, 35, 110, 75, 75);

    //-----------------------------------------MID-----------------------------------------------------//

    g.setColour(juce::Colours::darkgrey.withAlpha(0.9f).darker(0.7f));
    g.fillRoundedRectangle(135, 110, 75, 75, 4);
    Utilities::drawMidClippingFunction(g, 135, 110, 75, 75);
    
   
    //-------------------------------HARD---------------------------------------------------------------//
   
    g.setColour(juce::Colours::darkgrey.withAlpha(0.9f).darker(0.7f));
    g.fillRoundedRectangle(235, 110, 75, 75, 4);
    Utilities::drawHardClippingFunction(g, 235, 110, 75, 75);
   
  
    //--------------------------------DISEGNO I CONTORNI IMMAGINE-----------------------
    g.setColour(juce::Colours::white);
    if (isSoftDistortionSelected) {
        g.drawRoundedRectangle(35, 110, 75, 75, 4, 3);
        g.drawRoundedRectangle(135, 110, 75, 75, 4, 0);
        g.drawRoundedRectangle(235, 110, 75, 75, 4, 0);
    }
    else if (isMidDistortionSelected) {
        g.drawRoundedRectangle(35, 110, 75, 75, 4, 0);
        g.drawRoundedRectangle(135, 110, 75, 75, 4, 3);
        g.drawRoundedRectangle(235, 110, 75, 75, 4, 0);
    }
    else if (isHardDistortionSelected) {
        g.drawRoundedRectangle(35, 110, 75, 75, 4, 0);
        g.drawRoundedRectangle(135, 110, 75, 75, 4, 0);
        g.drawRoundedRectangle(235, 110, 75, 75, 4, 3);
    }

    //-----------------------ROUND RECTANGLES PER I VIEWER------------------------------
    g.setColour(juce::Colours::black.withAlpha(0.9f));
    g.fillRoundedRectangle(729, 90, 445, 183, 4);
    g.fillRoundedRectangle(729, 420, 445, 183, 4);

    juce::Path xAxisWV;
    juce::Path yAxisWV;
    juce::Line<float> xLengthWV = juce::Line<float>(739, 265, 1161, 265);
    juce::Line<float> yLengthWV = juce::Line<float>(739, 265, 739, 100);
    xAxisWV.addArrow(xLengthWV, 1.0, 7.0f, 3.5f);
    yAxisWV.addArrow(yLengthWV, 1.0f, 7.0f, 3.5f);

    juce::Font Axis_Font("Abadi", 10.0f, 0);
    g.setFont(Axis_Font);
    g.setColour(juce::Colours::white);
    g.drawText("-1", 728, 259, 10, 10, juce::Justification::centred);
    g.drawText("1", 728, 100, 10, 10, juce::Justification::centred);

    g.setColour(juce::Colours::lightcyan);
    g.fillPath(xAxisWV);
    g.fillPath(yAxisWV);


    juce::Path xAxisSV;
    juce::Path yAxisSV;
    juce::Line<float> xLengthSV = juce::Line<float>(739, 595, 1161, 595);
    juce::Line<float> yLengthSV = juce::Line<float>(739, 595, 739, 430);
    xAxisSV.addArrow(xLengthSV, 1.0, 7.0f, 3.5f);
    yAxisSV.addArrow(yLengthSV, 1.0f, 7.0f, 3.5f);

    g.fillPath(xAxisSV);
    g.fillPath(yAxisSV);

}

void MultiEffectAudioProcessorEditor::resized()
{
    //----------------------------------------------------DISTORTION---------------------------------------------------------------------

    //Sliders
    distortionGainSlider.setBounds(UpRotarySlidersPosX+10, UpRotarySlidersPosY+2, 65,65); //x y w h
    distortionOffsetSlider.setBounds(UpRotarySlidersPosX + 100, UpRotarySlidersPosY, RotarySliderDimHW, RotarySliderDimHW);
    distortionThresholdSlider.setBounds(UpRotarySlidersPosX + 200, UpRotarySlidersPosY, RotarySliderDimHW, RotarySliderDimHW);
    distortionGainValue.setBounds(UpRotarySlidersPosX + 15, UpRotarySlidersPosY +42, 65, 65);

    //Toggle Active Distortion
    toggleActiveDistotion.setBounds(10, -30, 100, 100);

    //Toggle DistortionType
    softDistortion.setBounds(35, 40, 100, 100);
    midDistortion.setBounds(135, 40, 100, 100);
    hardDistortion.setBounds(235, 40, 100, 100);

    softDistortionLabel.setBounds(30, 90, 100, 100);
    midDistortionLabel.setBounds(140, 90, 100, 100);
    hardDistortionLabel.setBounds(250, 90, 100, 100);

    //Distortion Utilities
    borderDistortionGain.setBounds(30, 205, 85, 120);
    borderDistortionOffset.setBounds(130, 205, 85, 120);
    borderDistortionTreshold.setBounds(230, 205, 85, 120);


    //-----------------------------------------------------DELAY---------------------------------------------------------------------
    
        //toggle attivazione delay
    toggleActiveDelay.setBounds(10 + DelayPositionOffSet, -30, 100, 100);

    //Bounds slider delay
    delayDryWetSlider.setBounds(75 + DelayPositionOffSet, 120, 200, 15);
    delayGainSlider.setBounds(43 + DelayPositionOffSet, UpRotarySlidersPosY, 65, 65);
    delayTimeSlider.setBounds(140 + DelayPositionOffSet, UpRotarySlidersPosY, 65,65);
    delayLowPassFilter.setBounds(248 + DelayPositionOffSet, UpRotarySlidersPosY+5, 65,65);
    delayFilterFrequencyValue.setBounds(250 + DelayPositionOffSet, UpRotarySlidersPosY +30, 100, 100);


    //toggle fD/FW
    isFeedback.setBounds(80 + DelayPositionOffSet, 45, 200, 50);
    isFeedForward.setBounds(180 + DelayPositionOffSet, 45, 200, 50);

    //Toggle
    toggleActiveDelayLowPass.setBounds(250 + DelayPositionOffSet, 165, 100, 100);

    //Delay Utilities
    borderDelayGain.setBounds(393, 205, 85, 120);
    borderDelayTime.setBounds(495, 205, 85, 120);
    borderDelayLowPass.setBounds(597, 205, 85, 120);
    borderDelayDryWet.setBounds(425, 90, 220, 80);
    delayGainValue.setBounds(412,255,100,100);
    delayTimeMsValue.setBounds(507, 255, 100, 100);
    delayDryWetValue.setBounds(515, 143, 200, 15);


    //--------------------------------------------------------------REVERB------------------------------------------------------
    //Toggle
    toggleActiveReverb.setBounds(10, 325, 100, 100);

    //Sliders
    reverbDryWetLevelSlider.setBounds(37, 450, 130, 15);
    reverbRoomSizeSlider.setBounds(190, 450, 130, 15);
    reverbDampingSlider.setBounds(65, 550, RotarySliderDimHW, RotarySliderDimHW);
    reverbWidthSlider.setBounds(202, 550, RotarySliderDimHW, RotarySliderDimHW);

    //Labels
    ReverbDryWetValue.setBounds(85, 467, 60, 15);
    
    //Reverb Utilities
    borderReverbDamping.setBounds(65, 525, 85, 120);
    borderReverbWidth.setBounds(202, 525, 85, 120);
    borderReverbDryWet.setBounds(32, 420, 140, 70);
    borderReverbRoomSize.setBounds(185, 420, 140, 70);

    //-------------------------------------------------------------VIEWER-------------------------------------------------------
    audioProcessor.waveViewer.setBounds(742, 90, 430,170);
    borderWV.setBounds(720, 63, 462, 220);
    audioProcessor.spectrum.setBounds(742, 420, 430, 170);
    borderSV.setBounds(720, 393, 462, 220);

    //-----------------------------------------------EQ---------------------------------------
    eqLowSlider.setBounds(390, 425, 40, 176);
    eqMidSlider.setBounds(470, 425, 40, 176);
    eqHighSlider.setBounds(550, 425, 40, 176);
    eqMasterOutSlider.setBounds(630, 425, 40, 176);

    eqLowLabel.setBounds(392, 360, 100, 100);
    eqMidLabel.setBounds(473, 360, 100, 100);
    eqHighLabel.setBounds(550, 360, 100, 100);
    eqMasterOutLabel.setBounds(630, 360, 100, 100);

    eqLowValue.setBounds(392, 560, 100, 100);
    eqMidValue.setBounds(473, 560, 100, 100);
    eqHighValue.setBounds(550, 560, 100, 100);
    eqMasterOutValue.setBounds(630, 560, 100, 100);
    

}








   