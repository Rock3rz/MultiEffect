/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include <cmath>
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utilities.h"
#include "Defines.h"


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
    waveViewer.setRepaintRate(180);
    waveViewer.setBufferSize(1000);
}

MultiEffectAudioProcessor::~MultiEffectAudioProcessor()
{
   
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
    delayBuffer = Utilities::zeroBuffer(delayBuffer, getTotalNumOutputChannels());
    
    //preparazione del lowPass delay
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getNumInputChannels();
    
    //preparazione del lowPass delay
    DelayLowPassFilters.resize(getNumInputChannels());
    auto dCutOffLowPass = apvt.getRawParameterValue("DLOWFILTER")->load();
    for (auto& filter : DelayLowPassFilters)
    {
        filter.reset();
        filter.prepare(spec);
        *filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, dCutOffLowPass * 1000.0f);
    }

    /*
   //preparazione dei filtri distorsione (non in uso)
    distorsionSpec.sampleRate = sampleRate;
    distorsionSpec.maximumBlockSize = samplesPerBlock;
    distorsionSpec.numChannels = getNumInputChannels();

    distortionLowPass.reset();
    distortionHighPass.reset();

    distortionLowPass.prepare(distorsionSpec);
    distortionHighPass.prepare(distorsionSpec);

    *distortionLowPass.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 10000,0.6f);
    *distortionHighPass.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 50,0.6f);
   */
    
    eQSpec.sampleRate = sampleRate;
    eQSpec.maximumBlockSize = samplesPerBlock;
    eQSpec.numChannels = getNumInputChannels();

    lowShelf.prepare(eQSpec);
    peak.prepare(eQSpec);
    highShelf.prepare(eQSpec);

    updateFilters();

    DBG("SamplesPerBlock" );
    DBG(samplesPerBlock );
    DBG("\nSamplerate" );
    DBG(sampleRate );

     
    
}

void MultiEffectAudioProcessor::releaseResources()
{
    
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
        for (auto& filter : DelayLowPassFilters)
        {
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), dCutOffLowPass * 1000.0f);
            
        }
    }

    
    //MasterOut
    auto eqMasterOut = apvt.getRawParameterValue("EqMASTEROUTGAIN")->load();

    //Roba dei filtri che ronzano =(
    /*
    juce::dsp::AudioBlock<float> audioBlock(buffer);
    juce::dsp::ProcessContextReplacing<float> context1(audioBlock);
    */

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
        delayBuffer.clear(i, 0, delayBuffer.getNumSamples());
    }

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {

        //------------------------------------------------DISTORSION ------------------------------------------------------------
        if (isDistortionActive) {
            auto* channelData = buffer.getWritePointer(channel);

                       
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
                switch (distortionType)
                {
                case 0:
                    
                    channelData[sample] *= disGainLevel;
                    channelData[sample] += disOffsetLevel;

                    if (channelData[sample] > disTresholdLevel) {
                        channelData[sample] = disTresholdLevel;
                    }
                    else if (channelData[sample] < -disTresholdLevel) {
                        channelData[sample] = -disTresholdLevel;
                    }

                    channelData[sample] = tanh(channelData[sample]);            //tanH
                    //channelData[sample] -= disOffsetLevel;
                    
                    break;

                case 1:
                    
                    channelData[sample] *= disGainLevel;
                    channelData[sample] += disOffsetLevel;

                    if (channelData[sample] > disTresholdLevel) {
                        channelData[sample] = disTresholdLevel;
                    }
                    else if (channelData[sample] < -disTresholdLevel) {
                        channelData[sample] = -disTresholdLevel;
                    }

                    channelData[sample] = ((channelData[sample] > 0.f) - (channelData[sample] < 0.f)) * (1.f-std::exp(-std::abs(channelData[sample]))); //sign(x)*(1-e^-|x|);
                    //channelData[sample] -= disOffsetLevel;
                    break;

                case 2:
                    
                    channelData[sample] *= disGainLevel;                     // 1 se > 1
                    channelData[sample] += disOffsetLevel;                  // -1 se < 1
                                                                            // x altrimenti
                    if (channelData[sample] > disTresholdLevel) {
                        channelData[sample] = disTresholdLevel;
                    }
                    else if (channelData[sample] < -disTresholdLevel) {
                        channelData[sample] = -disTresholdLevel;
                    }

                    //channelData[sample] -= disOffsetLevel;

                    break;
                default:
                    break;
                }
            }
           
            //RONZA DA MORIRE =(
            //distortionLowPass.process(context1);
           //distortionHighPass.process(context1);
            
           
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
    
    if (isDelayLowPassActive) {
        juce::dsp::AudioBlock<float> audioBlock(delayBuffer);
        juce::dsp::ProcessContextReplacing<float> context(audioBlock);
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            DelayLowPassFilters[channel].process(context);
            
        }
    }
   

    //----------------------------------------------------RIVERBERO------------------------------------------------------------------
    if (isReverbActive) { // Controlla se il riverbero � attivo

        juce::Reverb::Parameters reverbParams; // Crea un oggetto per i parametri del riverbero
        if (presetIndex < 6) { // Controlla se l'indice del preset � inferiore a 6 
            if (tempIndex != presetIndex) { // Controlla se l'indice temporaneo � diverso dall'indice del preset
                auto& preset = presets[presetIndex]; // Ottiene il preset corrispondente all'indice attuale

                // Imposta i parametri del riverbero in base al preset selezionato
                reverbParams.roomSize = preset.roomSize;
                reverbParams.damping = preset.damping;
                reverbParams.wetLevel = preset.wetLevel;
                reverbParams.dryLevel = 1 - reverbParams.wetLevel; // Calcola il livello dry come complementare al livello wet
                reverbParams.width = preset.width;
            }
        }
        else {
            // Se l'indice del preset � 6 (none) o superiore, ottiene i parametri direttamente dai valori impostati dall'utente
            reverbParams.roomSize = *apvt.getRawParameterValue("RevROOMSIZE");
            reverbParams.damping = *apvt.getRawParameterValue("RevDAMPING");
            reverbParams.wetLevel = *apvt.getRawParameterValue("RevDRYWET");
            reverbParams.dryLevel = 1 - reverbParams.wetLevel;
            reverbParams.width = *apvt.getRawParameterValue("RevWIDTH");
            
        }

        // Imposta i parametri del riverbero nell'oggetto reverb
        reverb.setParameters(reverbParams);

        // Ottiene i puntatori ai dati dei canali sinistro e destro del buffer audio
        auto* channelData1 = buffer.getWritePointer(0);
        auto* channelData = buffer.getWritePointer(1);

        // Applica il riverbero ai dati audio dei canali stereo
        reverb.processStereo(channelData, channelData1, buffer.getNumSamples());
        
    }

   //--------------------------------------EQ----------------------------------------------------------------------

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
        
        float* bufferData = buffer.getWritePointer(channel);
        const float* delayBufferData = delayBuffer.getReadPointer(channel, readPosition);
        for (int i = 0; i < bufferSize; ++i)
        {
       
            bufferData[i] = ((bufferData[i] * dry) + (delayBufferData[i] * wet * gain));
        }
    }
    else
    {
        auto numSamplesToEnd = delayBufferSize - readPosition;
        auto numSamplesToStart = bufferSize - numSamplesToEnd;

        
        float* bufferData = buffer.getWritePointer(channel);
        const float* delayBufferData = delayBuffer.getReadPointer(channel, readPosition);
        for (int i = 0; i < numSamplesToEnd; ++i)
        {

            bufferData[i] = ((bufferData[i] * dry) + (delayBufferData[i] * wet * gain));
            
        }

        
        const float* delayBufferDataStart = delayBuffer.getReadPointer(channel, 0);
        for (int i = 0; i < numSamplesToStart; ++i)
        {
            
            bufferData[numSamplesToEnd + i] = ((bufferData[numSamplesToEnd + i] * dry) + (delayBufferDataStart[i] * wet * gain));
            
        }  
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

    *lowShelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(getSampleRate(), 400.0f, 2.5f, lowGain);
    *peak.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), 1600.0f, 1.5f, midGain);
    *highShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(getSampleRate(), 3200.0f, 2.5f, highGain);
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
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DisGAIN", "disGain", 0.001f, 2.f, 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DisOFFSET", "disOffset", -.5f, .5f, 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DisTRESHOLD", "disTreshold", .0f, 1.0f, .5f));

    //-------------------------------------------------DELAY---------------------------------------------------------
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DGAIN", "dGain", 0.001f, 1.f, .5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DTIME", "dTime", .01f, 2.0f, 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DDRYWET", "dDryWet", .0f, 1.0f, .5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DLOWFILTER", "dLowFilter", .01f, 10.f, 5.f));

    //-----------------------------------------------REVERB-------------------------------------------------
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RevROOMSIZE", "revRoomSize", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RevDAMPING", "revDamping", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RevDRYWET", "revDryWet", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RevWIDTH", "revWidth", 0.0f, 1.0f, 0.5f));

    //-----------------------------------------------EQ-------------------------------------------------------
    params.push_back(std::make_unique<juce::AudioParameterFloat>("EqLOW", "LowCutFreq", 0.001f, 2.f, 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("EqMID", "MidCutFreq", 0.001f, 2.f, 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("EqHIGH", "HighCutFreq", 0.001f, 2.f, 1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("EqMASTEROUTGAIN", "MasterOutGain", 0.001f, 5.f, 1.f));

    //ritorno il vettore da inizio a fine
    return { params.begin(), params.end() };
}

// Definizione dei preset
const ReverbPreset MultiEffectAudioProcessor::presets[] = {
    { "Small Space", SMALL_SPACE_ROOM_SIZE, SMALL_SPACE_DAMPING, SMALL_SPACE_WET_LEVEL, SMALL_SPACE_WIDTH},              // Stanza piccola
    { "Cavern", CAVERN_ROOM_SIZE, CAVERN_DAMPING, CAVERN_WET_LEVEL, CAVERN_WIDTH},                                       // Caverna
    { "Empty Room", EMPTY_ROOM_SIZE, EMPTY_ROOM_DAMPING, EMPTY_ROOM_WET_LEVEL, EMPTY_ROOM_WIDTH},                        // Stanza vuota
    { "Furnished Room", FURNISHED_ROOM_SIZE, FURNISHED_ROOM_DAMPING, FURNISHED_ROOM_WET_LEVEL, FURNISHED_ROOM_WIDTH},    // Stanza arredata 
    { "Rubber Room", RUBBER_ROOM_SIZE, RUBBER_ROOM_DAMPING, RUBBER_ROOM_WET_LEVEL, RUBBER_ROOM_WIDTH},                   //Stanza di gomma
    { "Glass Room", GLASS_ROOM_SIZE, GLASS_ROOM_DAMPING, GLASS_ROOM_WET_LEVEL,GLASS_ROOM_WIDTH} ,                        // Stanza di vetro
    { "None", 0.5f,0.5f,0.5f,0.5f}                                                                                       //None
};

void MultiEffectAudioProcessor::loadPreset(int index)
{
    auto& preset = presets[index];
    *apvt.getRawParameterValue("RevROOMSIZE") = preset.roomSize;
    *apvt.getRawParameterValue("RevDAMPING") = preset.damping;
    *apvt.getRawParameterValue("RevDRYWET") = preset.wetLevel;
    *apvt.getRawParameterValue("RevWIDTH") = preset.width;
}



