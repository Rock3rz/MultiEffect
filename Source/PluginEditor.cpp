/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define WindowWidth 1192
#define WindowHeight 696
#define RectBoundThickness 2
#define DelayRotaryLine 100
#define DelayPositionOffSet 360


//==============================================================================
MultiEffectAudioProcessorEditor::MultiEffectAudioProcessorEditor (MultiEffectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize(WindowWidth, WindowHeight);
    setResizable(false, false);
    setLookAndFeel(nullptr);


    //-----------------------------------------------DISTORTION--------------------------------------------------------------------
    //toggle aactive
    toggleActiveDistotion.setButtonText("Inactive");

    //Gain
    distortionGainSlider.setLookAndFeel(&myLookAndFeel1);
    distortionGainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    distortionGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    distortionGainSlider.setPaintingIsUnclipped(true);
    distortionGainLabel.setText("Gain", juce::dontSendNotification);

    //Offset
    distortionOffsetSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    distortionOffsetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    distortionOffsetLabel.setText("Offset", juce::dontSendNotification);


    //Treshold
    distortionThresholdSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    distortionThresholdSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    distortionThresholdLabel.setText("Treshold", juce::dontSendNotification);

    //Toggle active delay
    addAndMakeVisible(toggleActiveDistotion);

    //Sliders
    addAndMakeVisible(distortionGainSlider);
    addAndMakeVisible(distortionOffsetSlider);
    addAndMakeVisible(distortionThresholdSlider);

    //Labels
    addAndMakeVisible(distortionGainLabel);
    addAndMakeVisible(distortionOffsetLabel);
    addAndMakeVisible(distortionThresholdLabel);

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


    //-----------------------------------------------DELAY------------------------------------------------------------------------

        //Delay activate
    toggleActiveDelay.setButtonText("Inactive");

    //Toggle LowPass Filter
    toggleActiveDelayLowPass.setButtonText("Filter");

    //Delay ToggleFeedBack
    isFeedback.setButtonText("FeedBack");
    isFeedForward.setButtonText("FeedForward");

    //Delay slider Setup
    delayGainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    delayGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    delayGainSlider.setDoubleClickReturnValue(true, .5f);
    delayGainLabel.setText("Gain", juce::dontSendNotification);
    //delayGainSlider.setSize(200.f, 200.f);

    //Time delay slider Setup
    delayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    delayTimeLabel.setText("Time", juce::dontSendNotification);

    //lowPass Filter Slider Setup
    delayLowPassFilter.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    delayLowPassFilter.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    delayLowPassLabel.setText("Filter", juce::dontSendNotification);

    //dryWet slider Setup
    delayDryWetSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    delayDryWetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    delayDryWetLabel.setText("Dry / Wet", juce::dontSendNotification);


    //ToggleActibeDelay
    addAndMakeVisible(toggleActiveDelay);

    //Toggle Low Pass
    addAndMakeVisible(toggleActiveDelayLowPass);

    //DelaySliders
    addAndMakeVisible(delayGainSlider);
    addAndMakeVisible(delayTimeSlider);
    addAndMakeVisible(delayLowPassFilter);
    addAndMakeVisible(delayDryWetSlider);

    //DelayLabels
    addAndMakeVisible(delayGainLabel);
    addAndMakeVisible(delayTimeLabel);
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
    reverbRoomSizeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    reverbRoomSizeLabel.setText("Room size", juce::dontSendNotification);

    //Damping
    reverbDampingSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    reverbDampingSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    reverbDampingLabel.setText("Damping", juce::dontSendNotification);

    //dry wet
    reverbDryWetLevelSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    reverbDryWetLevelSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    reverbDryWetLevelLabel.setText("Dry / Wet", juce::dontSendNotification);

    //width
    reverbWidthSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    reverbWidthSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    reverbWidthLabel.setText("Width", juce::dontSendNotification);

    //Toggle active 
    addAndMakeVisible(toggleActiveReverb);

    //Sliders
    addAndMakeVisible(reverbRoomSizeSlider);
    addAndMakeVisible(reverbDampingSlider);
    addAndMakeVisible(reverbDryWetLevelSlider);
    addAndMakeVisible(reverbWidthSlider);

    //Labels
    addAndMakeVisible(reverbRoomSizeLabel);
    addAndMakeVisible(reverbDampingLabel);
    addAndMakeVisible(reverbDryWetLevelLabel);
    addAndMakeVisible(reverbWidthLabel);

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


    //--------------------------------------------------VIEWER----------------------------------------------------------------------------------
    addAndMakeVisible(audioProcessor.waveViewer);
    addAndMakeVisible(audioProcessor.spectrum);

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

    //divisione in 4
    juce::Rectangle<int> Upper = LeftHalf.removeFromTop(LeftHalf.getHeight() / 2);
    juce::Rectangle<int> BottomLeft = LeftHalf;
    juce::Rectangle<int> UpLeft = Upper.removeFromLeft(Upper.getWidth() / 2);
    juce::Rectangle<int> UpRight = Upper;
    juce::Rectangle<int> DownLeft = BottomLeft.removeFromLeft(BottomLeft.getWidth() / 2);
    juce::Rectangle<int> DownRight = BottomLeft;


    //creo per ogni quadrante delle FlexBox così da poter gestire i posizionamenti più facilmente
    juce::FlexBox fbRightHalf;
    juce::FlexBox fbUpLeft;
    juce::FlexBox fbUpRight;
    juce::FlexBox fbDownLeft;
    juce::FlexBox fbDownRight;

    juce::Font Titles_Font("North Carossela", 17.0f, juce::Font::bold);
    g.setColour(juce::Colours::deepskyblue);
    g.setFont(Titles_Font);
    g.drawText("Distortion", 132, 15, 90, 15, juce::Justification::centredTop, true);
    g.drawText("Delay Line", 490, 15, 90, 15, juce::Justification::centredTop, true);
    g.drawText("Riverbero", 132, 360, 90, 15, juce::Justification::centredTop, true);
    g.drawText("Master EQ", 490, 360, 90, 15, juce::Justification::centredTop, true);
    g.drawText("Grafici", RightHalf, juce::Justification::centred);
    g.drawText("Wave viewer", RightHalf, juce::Justification::centredTop);
}

void MultiEffectAudioProcessorEditor::resized()
{
    //----------------------------------------------------DISTORTION---------------------------------------------------------------------

    //Sliders
    distortionGainSlider.setBounds(30, 230, 85, 85); //x y h w
    distortionOffsetSlider.setBounds(125, 225, 100, 100);
    distortionThresholdSlider.setBounds(225, 225, 100, 100);

    //Labels
    distortionGainLabel.setBounds(55, 160, 100, 100);
    distortionOffsetLabel.setBounds(152, 165, 100, 100);
    distortionThresholdLabel.setBounds(244, 165, 100, 100);

    //Toggle
    toggleActiveDistotion.setBounds(10, -30, 100, 100);


    //-----------------------------------------------------DELAY---------------------------------------------------------------------
    // 
        //toggle attivazione delay
    toggleActiveDelay.setBounds(10 + DelayPositionOffSet, -30, 100, 100);

    //Bounds slider delay
    delayDryWetSlider.setBounds(75 + DelayPositionOffSet, 90, 200, 100);
    delayGainSlider.setBounds(25 + DelayPositionOffSet, 225, 100, 100);
    delayTimeSlider.setBounds(127 + DelayPositionOffSet, 225, 100, 100);
    delayLowPassFilter.setBounds(225 + DelayPositionOffSet, 225, 100, 100);

    //Bounds label delay
    delayGainLabel.setBounds(55 + DelayPositionOffSet, 165, 100, 100);
    delayTimeLabel.setBounds(155 + DelayPositionOffSet, 165, 100, 100);
    delayLowPassLabel.setBounds(240 + DelayPositionOffSet, 165, 100, 100);
    delayDryWetLabel.setBounds(140 + DelayPositionOffSet, 50, 100, 100);

    //toggle fD/FW
    isFeedback.setBounds(80 + DelayPositionOffSet, 30, 200, 50);
    isFeedForward.setBounds(180 + DelayPositionOffSet, 30, 200, 50);

    //Toggle
    toggleActiveDelayLowPass.setBounds(235 + DelayPositionOffSet, 165, 100, 100);

    //--------------------------------------------------------------REVERB------------------------------------------------------
        //Toggle
    toggleActiveReverb.setBounds(10, 325, 100, 100);

    //Sliders
    reverbDryWetLevelSlider.setBounds(25, 400, 150, 100);
    reverbRoomSizeSlider.setBounds(175, 400, 150, 100);
    reverbDampingSlider.setBounds(50, 550, 100, 100);
    reverbWidthSlider.setBounds(200, 550, 100, 100);

    //Labels
    reverbDryWetLevelLabel.setBounds(65, 360, 100, 100);
    reverbRoomSizeLabel.setBounds(210, 360, 100, 100);
    reverbDampingLabel.setBounds(67, 490, 100, 100);
    reverbWidthLabel.setBounds(226, 490, 100, 100);
    //-------------------------------------------------------------VIEWER-------------------------------------------------------
    audioProcessor.waveViewer.setBounds(730, 20, 450,300);
    audioProcessor.spectrum.setBounds(730, 360, 450, 300);
}
