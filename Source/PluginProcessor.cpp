
#include "PluginProcessor.h"
#include "PluginEditorGui.h"

//==============================================================================
RingWormModulatorAudioProcessor::RingWormModulatorAudioProcessor() :
parameters(*this, nullptr, Identifier("RingWormParameters"), {
        std::make_unique<AudioParameterFloat>(NAME_RATE, "Rate", NormalisableRange<float>(0.1f, 127.0f, 0.1f, 0.5f), DEFAULT_RATE),
        std::make_unique<AudioParameterFloat>(NAME_FREQ, "Frequency", NormalisableRange<float>(0.1f, 127.0f, 0.1f, 0.5f), DEFAULT_FREQ),
        std::make_unique<AudioParameterChoice>(NAME_WF, "Mode", StringArray{ "Sine", "Step squared", "Squared", "Random", "Envelope Follower" }, DEFAULT_WF),
        std::make_unique<AudioParameterFloat>(NAME_WT, "Width", 0.0f, 100.0f, DEFAULT_WT),
        std::make_unique<AudioParameterFloat>(NAME_DW, "Blend", 0.0f, 1.0f, DEFAULT_DW),
    
        })
{
    parameters.addParameterListener(NAME_RATE, &lfo);
    parameters.addParameterListener(NAME_FREQ, &osc);
    parameters.addParameterListener(NAME_WF, &lfo);
    parameters.addParameterListener(NAME_DW, &dryWet);
    parameters.addParameterListener(NAME_WT, this);
    width.setCurrentAndTargetValue(DEFAULT_DW);
}

RingWormModulatorAudioProcessor::~RingWormModulatorAudioProcessor()
{
}

void RingWormModulatorAudioProcessor::parameterChanged(const String& paramID, float newValue)
{
    if (paramID == NAME_WT)
        width.setTargetValue(newValue);
}
//==============================================================================
const juce::String RingWormModulatorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RingWormModulatorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RingWormModulatorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RingWormModulatorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RingWormModulatorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RingWormModulatorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RingWormModulatorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RingWormModulatorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String RingWormModulatorAudioProcessor::getProgramName (int index)
{
    return {};
}

void RingWormModulatorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void RingWormModulatorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    lfo.prepareToPlay(sampleRate, samplesPerBlock);
    osc.prepareToPlay(sampleRate, samplesPerBlock);
    dryWet.prepareToPlay(samplesPerBlock);
    
    
    
    lfoBuffer.setSize(1, samplesPerBlock);
    modulator.setSize(1, samplesPerBlock);
    
    
    width.reset(sampleRate, RATE_SMTH); //guardare meglio
    
    
}

void RingWormModulatorAudioProcessor::releaseResources()
{
    lfoBuffer.setSize(0, 0);
    modulator.setSize(0, 0);
    dryWet.releaseResources();
    
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RingWormModulatorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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

void RingWormModulatorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    const auto numSamples = buffer.getNumSamples();
    
    dryWet.storeFrom(buffer);
    
    lfo.getNextAudioBlock(buffer, lfoBuffer, numSamples);
    
    widthLfoModulation(numSamples);
    
    osc.getNextAudioBlock(lfoBuffer, modulator, numSamples);
    
    ApplyRingModulation(buffer, numSamples);
    
    dryWet.mergeTo(buffer);
    
}
//applico width a lfoBuffer
void RingWormModulatorAudioProcessor::widthLfoModulation(int numSamples)
{
    auto* lfoBufferData = lfoBuffer.getArrayOfWritePointers();
    
    FloatVectorOperations::add(lfoBufferData[0], 1.0f, numSamples);
    FloatVectorOperations::multiply(lfoBufferData[0], 0.5f, numSamples);
    width.applyGain(lfoBufferData[0], numSamples);
}

//applico ring modulation a buffer input
void RingWormModulatorAudioProcessor::ApplyRingModulation(AudioBuffer<float>& buffer, int numSamples)
{
    const auto numChannels = buffer.getNumChannels();
    auto* bufferData = buffer.getArrayOfWritePointers();
    auto* modulatorData = modulator.getArrayOfReadPointers();
    
    for (int ch = 0; ch < numChannels; ++ch)
    {
        FloatVectorOperations::multiply(bufferData[ch], modulatorData[0], numSamples);
    }
}





//==============================================================================
bool RingWormModulatorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
    //return false;
}

juce::AudioProcessorEditor* RingWormModulatorAudioProcessor::createEditor()
{
    return new PluginEditorGui (*this, parameters);
    //return nullptr;
}

//==============================================================================
void RingWormModulatorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RingWormModulatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RingWormModulatorAudioProcessor();
}
