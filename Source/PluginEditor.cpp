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
    distortionGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    distortionGainSlider.setPaintingIsUnclipped(true);
    

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
    midDistortionLabel.setText("f(x) = sign (x)(e^-|x|)", juce::dontSendNotification);
    hardDistortionLabel.setText("f(x) = 1 -> x>1, -1 -> x<-1, x", juce::dontSendNotification);

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

    //condizioni di selezione tipo distorsione
    softDistortion.onClick = [this]() {
        audioProcessor.distortionType = 0;
        midDistortion.setToggleState(false, false);
        hardDistortion.setToggleState(false, false);

        };

    midDistortion.onClick = [this]() {
        audioProcessor.distortionType = 1;
        softDistortion.setToggleState(false, false);
        hardDistortion.setToggleState(false, false);
        };

    hardDistortion.onClick = [this]() {
        audioProcessor.distortionType = 2;
        softDistortion.setToggleState(false, false);
        midDistortion.setToggleState(false, false);
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
    delayGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    delayGainSlider.setDoubleClickReturnValue(true, .5f);
  
    //delayGainSlider.setSize(200.f, 200.f);

    //Time delay slider Setup
    delayTimeSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    delayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    

    //lowPass Filter Slider Setup
    delayLowPassFilter.setLookAndFeel(&myLookAndFeelDelayLine);
    delayLowPassFilter.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    delayLowPassFilter.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
   

    //dryWet slider Setup
    delayDryWetSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    delayDryWetSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    delayDryWetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    //delayDryWetLabel.setText("Dry / Wet", juce::dontSendNotification);
    delayDryWetSlider.setRange(0.0f, 1.0f);
    delayDryWetSlider.setValue(0.5f);


    //ToggleActibeDelay
    addAndMakeVisible(toggleActiveDelay);

    //Toggle Low Pass
    addAndMakeVisible(toggleActiveDelayLowPass);

    //DelaySliders
    addAndMakeVisible(delayGainSlider);
    addAndMakeVisible(delayTimeSlider);
    addAndMakeVisible(delayLowPassFilter);
    addAndMakeVisible(delayDryWetSlider);

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
    reverbDampingSlider.setLookAndFeel(&myLookAndFeelReverb);
    reverbDampingSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    reverbDampingSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
  

    //dry wet
    reverbDryWetLevelSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    reverbDryWetLevelSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    reverbDryWetLevelLabel.setText("Dry / Wet", juce::dontSendNotification);

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

    //Utilities
    addAndMakeVisible(borderReverbDamping);
    borderReverbDamping.setText("Damping");
    borderReverbDamping.setTextLabelPosition(juce::Justification::centredTop);
    addAndMakeVisible(borderReverbWidth);
    borderReverbWidth.setText("Width");
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
    eqLowSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    eqLowLabel.setText("Low", juce::dontSendNotification);

    eqMidSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    eqMidSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    eqMidSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    eqMidLabel.setText("Mid", juce::dontSendNotification);

    eqHighSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    eqHighSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    eqHighSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    eqHighLabel.setText("High", juce::dontSendNotification);

    eqMasterOutSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    eqMasterOutSlider.setLookAndFeel(&myLookAndFeelDelayLine);
    eqMasterOutSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 40, 20);
    eqMasterOutLabel.setText("Gain", juce::dontSendNotification);

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

    //attachment
    eqLowSliderAttachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "EqLOW", eqLowSlider);
    eqMidSliderAttachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "EqMID", eqMidSlider);
    eqHighSliderAttachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "EqHIGH", eqHighSlider);
    eqMasterOutSliderAttachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvt, "EqMASTEROUTGAIN", eqMasterOutSlider);

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

    //------------------------------------------------------HARD, MID, SOFT CLIPPING DISEGNO--------------------------------------------
    g.setColour(juce::Colours::darkgrey.withAlpha(0.9f).darker(0.7f));
    g.fillRect(25, 90, 95, 95);
    juce::Path xAxis;
    juce::Path yAxis;
    juce::Line<float> xLength = juce::Line<float>(37.0f, 175.0f, 37.0f, 97.0f); 
    juce::Line<float> yLength = juce::Line<float>(37.0f, 174.0f, 115.0f, 174.0f);
    xAxis.addArrow(xLength, 2.0f, 8.0f, 6.0f);
    yAxis.addArrow(yLength, 2.0f, 8.0f, 6.0f);

    g.setColour(juce::Colours::whitesmoke);
    g.drawLine(37.0f, 160.0f, 56.5f, 160.0f, 1.0f);
    g.drawLine(93.0f, 115.0f, 112.5f, 115.0f, 1.0f);
    g.drawLine(56.5f, 160.0f, 93.0f, 115.0f);

    g.setColour(juce::Colours::lightcyan);
    g.fillPath(xAxis);
    g.fillPath(yAxis);


}

void MultiEffectAudioProcessorEditor::resized()
{
    //----------------------------------------------------DISTORTION---------------------------------------------------------------------

    //Sliders
    distortionGainSlider.setBounds(UpRotarySlidersPosX, UpRotarySlidersPosY, RotarySliderDimHW, RotarySliderDimHW); //x y w h
    distortionOffsetSlider.setBounds(UpRotarySlidersPosX + 100, UpRotarySlidersPosY, RotarySliderDimHW, RotarySliderDimHW);
    distortionThresholdSlider.setBounds(UpRotarySlidersPosX + 200, UpRotarySlidersPosY, RotarySliderDimHW, RotarySliderDimHW);


    //Toggle Active Distortion
    toggleActiveDistotion.setBounds(10, -30, 100, 100);

    //Toggle DistortionType
    softDistortion.setBounds(30, 20, 100, 100);
    midDistortion.setBounds(140, 20, 100, 100);
    hardDistortion.setBounds(250, 20, 100, 100);

    softDistortionLabel.setBounds(30, 90, 100, 100);
    midDistortionLabel.setBounds(140, 90, 100, 100);
    hardDistortionLabel.setBounds(250, 90, 100, 100);

    //Distortion Utilities
    borderDistortionGain.setBounds(30, 205, 85, 120);
    borderDistortionOffset.setBounds(130, 205, 85, 120);
    borderDistortionTreshold.setBounds(230, 205, 85, 120);




    //-----------------------------------------------------DELAY---------------------------------------------------------------------
    // 
        //toggle attivazione delay
    toggleActiveDelay.setBounds(10 + DelayPositionOffSet, -30, 100, 100);

    //Bounds slider delay
    delayDryWetSlider.setBounds(75 + DelayPositionOffSet, 90, 200, 30);
    delayGainSlider.setBounds(25 + DelayPositionOffSet, UpRotarySlidersPosY, RotarySliderDimHW, RotarySliderDimHW);
    delayTimeSlider.setBounds(127 + DelayPositionOffSet, UpRotarySlidersPosY, RotarySliderDimHW, RotarySliderDimHW);
    delayLowPassFilter.setBounds(229 + DelayPositionOffSet, UpRotarySlidersPosY, RotarySliderDimHW, RotarySliderDimHW);

    //Bounds label delay
    delayDryWetLabel.setBounds(140 + DelayPositionOffSet, 50, 100, 100);

    //toggle fD/FW
    isFeedback.setBounds(80 + DelayPositionOffSet, 30, 200, 50);
    isFeedForward.setBounds(180 + DelayPositionOffSet, 30, 200, 50);

    //Toggle
    toggleActiveDelayLowPass.setBounds(240 + DelayPositionOffSet, 165, 100, 100);

    //Delay Utilities
    borderDelayGain.setBounds(385, 205, 85, 120);
    borderDelayTime.setBounds(487, 205, 85, 120);
    borderDelayLowPass.setBounds(589, 205, 85, 120);

    //--------------------------------------------------------------REVERB------------------------------------------------------
    //Toggle
    toggleActiveReverb.setBounds(10, 325, 100, 100);

    //Sliders
    reverbDryWetLevelSlider.setBounds(25, 400, 150, 100);
    reverbRoomSizeSlider.setBounds(175, 400, 150, 100);
    reverbDampingSlider.setBounds(58, 550, RotarySliderDimHW, RotarySliderDimHW);
    reverbWidthSlider.setBounds(208, 550, RotarySliderDimHW, RotarySliderDimHW);

    //Labels
    reverbDryWetLevelLabel.setBounds(65, 360, 100, 100);
    reverbRoomSizeLabel.setBounds(210, 360, 100, 100);
    
    //Reverb Utilities
    borderReverbDamping.setBounds(58, 525, 85, 120);
    borderReverbWidth.setBounds(207, 525, 85, 120);

    //-------------------------------------------------------------VIEWER-------------------------------------------------------
    audioProcessor.waveViewer.setBounds(730, 20, 450,300);
    audioProcessor.spectrum.setBounds(730, 360, 450, 300);

    //-----------------------------------------------EQ---------------------------------------
    eqLowSlider.setBounds(390, 430, 40, 200);
    eqMidSlider.setBounds(470, 430, 40, 200);
    eqHighSlider.setBounds(550, 430, 40, 200);
    eqMasterOutSlider.setBounds(630, 430, 40, 200);

    eqLowLabel.setBounds(395, 360, 100, 100);
    eqMidLabel.setBounds(475, 360, 100, 100);
    eqHighLabel.setBounds(555, 360, 100, 100);
    eqMasterOutLabel.setBounds(635, 360, 100, 100);
    

}
