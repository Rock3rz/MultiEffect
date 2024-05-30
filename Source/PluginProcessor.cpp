/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include <cmath>
#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
MultiEffectAudioProcessor::MultiEffectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvt(*this, nullptr, "Parameters", createParameters()),
    waveViewer(1), spectrum()
#endif
{
}

MultiEffectAudioProcessor::~MultiEffectAudioProcessor()
{
    waveViewer.setRepaintRate(200);
    waveViewer.setBufferSize(480);
}

//==============================================================================
const juce::String MultiEffectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MultiEffectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MultiEffectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MultiEffectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MultiEffectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MultiEffectAudioProcessor::getNumPrograms()
{
    return 1;  
}

int MultiEffectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MultiEffectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MultiEffectAudioProcessor::getProgramName (int index)
{
    return {};
}

void MultiEffectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MultiEffectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    auto delayBufferSize = 3 * sampleRate;
    delayBuffer.setSize(getNumInputChannels(), (int)delayBufferSize);

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getNumInputChannels();
    //sampleRate = 44100.f;

    lowPassFilters.resize(getNumInputChannels());
    auto dCutOffLowPass = apvt.getRawParameterValue("DLOWFILTER")->load();
    for (auto& filter : lowPassFilters)
    {
        filter.reset();
        filter.prepare(spec);
        *filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, dCutOffLowPass);
    }

   
    juce::dsp::ProcessSpec eQSpec{ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2 };

    lowShelf.prepare(eQSpec);
    peak.prepare(eQSpec);
    highShelf.prepare(eQSpec);

    updateFilters();
   
   
    
}

void MultiEffectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MultiEffectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
   
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MultiEffectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    //Conversione parametro-variabile
   //Distortion
    auto disGainLevel = apvt.getRawParameterValue("DisGAIN")->load();
    auto disOffsetLevel = apvt.getRawParameterValue("DisOFFSET")->load();
    auto disTresholdLevel = apvt.getRawParameterValue("DisTRESHOLD")->load();


    //Delay
    auto dGainLevel = apvt.getRawParameterValue("DGAIN")->load();
    auto timeDelay = apvt.getRawParameterValue("DTIME")->load();
    auto dDryWet = apvt.getRawParameterValue("DDRYWET")->load();
    auto dCutOffLowPass = apvt.getRawParameterValue("DLOWFILTER")->load();

    //applicazione del filtro ad ogni chiamata processBlock
    if (isDelayLowPassActive) {
        for (auto& filter : lowPassFilters)
        {
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), dCutOffLowPass * 1000.0f, .6f);
        }
    }

    //Reverb
    auto revRoomSizeLevel = apvt.getRawParameterValue("RevROOMSIZE")->load();
    auto revDampingLevel = apvt.getRawParameterValue("RevDAMPING")->load();
    auto revDryLevel = apvt.getRawParameterValue("RevDRYWET")->load();
    auto revWidth = apvt.getRawParameterValue("RevWIDTH")->load();

    //MasterOut
    auto eqMasterOut = apvt.getRawParameterValue("EqMASTEROUTGAIN")->load();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {

        //------------------------------------------------DISTORSION ------------------------------------------------------------
        if (isDistortionActive) {
            auto* channelData = buffer.getWritePointer(channel);


            for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
                channelData[sample] *= disGainLevel;
                channelData[sample] += disOffsetLevel;

                if (channelData[sample] > disTresholdLevel) {
                    channelData[sample] = disTresholdLevel;
                }
                else if (channelData[sample] < -disTresholdLevel) {
                    channelData[sample] = -disTresholdLevel;
                }

                channelData[sample] -= disOffsetLevel;

                channelData[sample] = channelData[sample] - ((channelData[sample] * channelData[sample] * channelData[sample]) / 3.f);
            }
        }


        //----------------------------------------------------DELAY----------------------------------------------------------------------

        if (isDelayActive) {
            //scrive nel buffer circolare
            fillBuffer(buffer, channel, dGainLevel);

            //legge dal buffer in ritardo
            readFromBuffer(channel, buffer, delayBuffer, dGainLevel, timeDelay, dDryWet);

            if (isFeedbackActive)
            {
                fillBuffer(buffer, channel, dGainLevel);
            }
        }
    }
    if (isDelayActive) {
        updateBufferPosition(buffer, delayBuffer);
    }

    // aggiunta
    if (isDelayLowPassActive) {
        juce::dsp::AudioBlock<float> audioBlock(delayBuffer);
        juce::dsp::ProcessContextReplacing<float> context(audioBlock);
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            lowPassFilters[channel].process(context);
        }
    }
   

    //----------------------------------------------------RIVERBERO------------------------------------------------------------------
    if (isReverbActive) {


        juce::Reverb::Parameters reverbParams;
        reverbParams.roomSize = *apvt.getRawParameterValue("RevROOMSIZE");
        reverbParams.damping = *apvt.getRawParameterValue("RevDAMPING");
        reverbParams.wetLevel = *apvt.getRawParameterValue("RevDRYWET");
        reverbParams.dryLevel = 1 - reverbParams.wetLevel;
        reverbParams.width = *apvt.getRawParameterValue("RevWIDTH");

        //reverb.setSampleRate(48000);
        reverb.setParameters(reverbParams);


        auto* channelData1 = buffer.getWritePointer(0);
        auto* channelData = buffer.getWritePointer(1);

        reverb.processStereo(channelData, channelData1, buffer.getNumSamples());
        
    }

   

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    lowShelf.process(context);
    peak.process(context);
    highShelf.process(context);

    updateFilters();

    

    //----------------------------------------VIEWER-----------------------------------------
   
    waveViewer.pushBuffer(buffer);

    //---------------------------------------SPECTRUM ANALYZER---------------------------------------------------------------------------

    for (int channel = 0; channel < getTotalNumOutputChannels(); ++channel) {
        auto* spectrumReadPointer = buffer.getReadPointer(channel);
        auto* channelData = buffer.getWritePointer(channel);
        for (int samples = 0; samples < buffer.getNumSamples(); ++samples) {
            spectrum.pushNextSampleIntoFifo(spectrumReadPointer[samples]);
            channelData[samples] *= eqMasterOut;
        }
    }
    
}

void MultiEffectAudioProcessor::fillBuffer(juce::AudioBuffer<float>& buffer, int channel, float g)
{
    const float* channelData = buffer.getReadPointer(channel);
    int bufferSize = buffer.getNumSamples();
    int delayBufferSize = delayBuffer.getNumSamples();

    if (delayBufferSize > bufferSize + mWritePosition)
    {
        delayBuffer.copyFromWithRamp(channel, mWritePosition, channelData, bufferSize, g, g);

    }
    else
    {
        auto numSamplesToEnd = delayBufferSize - mWritePosition;
        auto numSamplesAtBeginning = bufferSize - numSamplesToEnd;

        delayBuffer.copyFromWithRamp(channel, mWritePosition, channelData, numSamplesToEnd, g, g);

        delayBuffer.copyFromWithRamp(channel, 0, channelData + numSamplesToEnd, numSamplesAtBeginning, g, g);

    }
}

void MultiEffectAudioProcessor::readFromBuffer(int channel, juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer, float gain, float t, float dryWet)
{
    auto readPosition = mWritePosition - static_cast<int>(getSampleRate() * t);
    int bufferSize = buffer.getNumSamples();
    int delayBufferSize = delayBuffer.getNumSamples();
    float dry = 1 - dryWet;
    float wet = dryWet;

    if (readPosition < 0)
    {
        readPosition += delayBufferSize;
    }




    if ((readPosition + bufferSize) < delayBufferSize)
    {
        // Somma diretta senza rampa di gain
        float* bufferData = buffer.getWritePointer(channel);
        const float* delayBufferData = delayBuffer.getReadPointer(channel, readPosition);
        for (int i = 0; i < bufferSize; ++i)
        {
            //bufferData[i] += delayBufferData[i] * gain;
            bufferData[i] = (bufferData[i] * dry) + (delayBufferData[i] * wet * gain);
        }
    }
    else
    {
        auto numSamplesToEnd = delayBufferSize - readPosition;
        auto numSamplesToStart = bufferSize - numSamplesToEnd;

        // Somma diretta senza rampa di gain per la prima parte
        float* bufferData = buffer.getWritePointer(channel);
        const float* delayBufferData = delayBuffer.getReadPointer(channel, readPosition);
        for (int i = 0; i < numSamplesToEnd; ++i)
        {

            //bufferData[i] += delayBufferData[i] * gain;
            bufferData[i] = (bufferData[i] * dry) + (delayBufferData[i] * wet * gain);

        }

        // Somma diretta senza rampa di gain per la seconda parte
        const float* delayBufferDataStart = delayBuffer.getReadPointer(channel, 0);
        for (int i = 0; i < numSamplesToStart; ++i)
        {
            // bufferData[numSamplesToEnd + i] += delayBufferDataStart[i] * gain;
            bufferData[numSamplesToEnd + i] = (bufferData[numSamplesToEnd + i] * dry) + (delayBufferDataStart[i] * wet * gain);
        }
    }
    //applicazione del lowPass nel buffer di delay
    if (isDelayLowPassActive) {
        juce::dsp::AudioBlock<float> audioBlock(delayBuffer); //creazione di "blocchi"
        juce::dsp::ProcessContextReplacing<float> context(audioBlock); //sostituisce il contenuto del blocco con i nuovi dati filtrati
        lowPassFilters[channel].process(context); //filtra il contenuto dei nuovi dati
    }
}

void MultiEffectAudioProcessor::updateBufferPosition(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& delayBuffer)
{
    int bufferSize = buffer.getNumSamples();
    int delayBufferSize = delayBuffer.getNumSamples();

    mWritePosition += bufferSize;
    mWritePosition %= delayBufferSize;
}

void MultiEffectAudioProcessor::updateFilters() {

    auto lowGain = apvt.getRawParameterValue("EqLOW")->load();
    auto midGain = apvt.getRawParameterValue("EqMID")->load();
    auto highGain = apvt.getRawParameterValue("EqHIGH")->load();

    *lowShelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(getSampleRate(), 200.0f, 2, lowGain);
    *peak.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), 1000.0f, 2, midGain);
    *highShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(getSampleRate(), 5000.0f, 2, highGain);
}



//==============================================================================
bool MultiEffectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MultiEffectAudioProcessor::createEditor()
{
    return new MultiEffectAudioProcessorEditor (*this);
}

//==============================================================================
void MultiEffectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
   
}

void MultiEffectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MultiEffectAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout MultiEffectAudioProcessor::createParameters()
{
    //creazione del vettore parametri
    std::vector<std::unique_ptr<juce::RangedAudioParameter>>params;

    //assegnazione dei parametri al vettore creato

//-------------------------------------------------DISTORTION--------------------------------------
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DisGAIN", "disGain", 0.f, 2.f, 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DisOFFSET", "disOffset", -.5f, .5f, 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DisTRESHOLD", "disTreshold", .0f, 1.0f, .5f));

    //-------------------------------------------------DELAY---------------------------------------------------------
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DGAIN", "dGain", 0.f, 2.f, 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DTIME", "dTime", .1f, 2.0f, .5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DDRYWET", "dDryWet", .0f, 1.0f, .5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DLOWFILTER", "dLowFilter", .1f, 20.f, 5.f));

    //-----------------------------------------------REVERB-------------------------------------------------
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RevROOMSIZE", "revRoomSize", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RevDAMPING", "revDamping", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RevDRYWET", "revDryWet", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RevWIDTH", "revWidth", 0.0f, 1.0f, 0.5f));

    //-----------------------------------------------EQ-------------------------------------------------------
    params.push_back(std::make_unique<juce::AudioParameterFloat>("EqLOW", "LowCutFreq", 0.f, 2.f, 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("EqMID", "MidCutFreq", 0.f, 2.f, 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("EqHIGH", "HighCutFreq", 0.f, 2.f, 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("EqMASTEROUTGAIN", "MasterOutGain", 0.f, 2.f, 1.f));

    //ritorno il vettore da inizio a fine
    return { params.begin(), params.end() };
}

float MultiEffectAudioProcessor::linearToDb(float linear) {
    return 20.f * std::log10(linear);
}
