/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MultibandCompressorAudioProcessor::MultibandCompressorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    //Retrieve APVTS stored paramaters and assign them to cached pointers
    attack = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Attack"));
    //Assert that parameter names are spelt correctly
    jassert(attack !=nullptr);
    
    release = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Release"));
    jassert(release !=nullptr);
    
    threshold = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Threshold"));
    jassert(threshold !=nullptr);
    
    ratio = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("Ratio"));
    jassert(ratio !=nullptr);

}

MultibandCompressorAudioProcessor::~MultibandCompressorAudioProcessor()
{
}

//==============================================================================
const juce::String MultibandCompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MultibandCompressorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MultibandCompressorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MultibandCompressorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MultibandCompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MultibandCompressorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MultibandCompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MultibandCompressorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MultibandCompressorAudioProcessor::getProgramName (int index)
{
    return {};
}

void MultibandCompressorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MultibandCompressorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
//
    juce::dsp::ProcessSpec spec; // Create process spec object --> needed to initialised DSP -->
    spec.maximumBlockSize = samplesPerBlock; // set max number of samples to be processed
    spec.numChannels = getTotalNumOutputChannels();  //Number of channels to be configured to compressed
    spec.sampleRate = sampleRate; //Sample rate
    
    compressor.prepare(spec); // send spec object to the compressor to prepare to play audio;
    
    
}

void MultibandCompressorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MultibandCompressorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void MultibandCompressorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    //retrive parameter values from private cache and asign to compressor controls
    compressor.setAttack(attack->get());
    compressor.setRelease(release ->get());
    compressor.setThreshold(threshold->get());
    //use helper function to retrieve the string currently set in the string array then get calculate float value from this string
    //compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());
    auto block= juce::dsp::AudioBlock<float>(buffer); //create audio block feed in buffer input
    auto context = juce::dsp::ProcessContextReplacing<float>(block); //replaces audio in the buffer with processed audio --> creates context
    
    compressor.process(context); // process compressor audio from context on audio buffer
    
}

//==============================================================================
bool MultibandCompressorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MultibandCompressorAudioProcessor::createEditor()
{
//    return new MultibandCompressorAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this); 
    
}

//==============================================================================
void MultibandCompressorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    //get the state information from parameters
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}


void MultibandCompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
// You should use this method to restore your parameters from this memory block,
// whose contents will have been created by the getStateInformation() call.
    
    //update the state information from audio parqamaters value tree
 auto tree = juce::ValueTree::readFromData(data, sizeInBytes);

    if (tree.isValid()) {
        apvts.replaceState(tree);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout MultibandCompressorAudioProcessor::createParameterLayout()
{
    //define and add parameters
    APVTS::ParameterLayout layout;

    using namespace juce;

    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID {"Threshold", 1},
                                                    "Threshold",
                                                    juce::NormalisableRange<float>(-60, 12, 1, 1),
                                                    0 ));


    layout.add(std::make_unique<AudioParameterFloat> (ParameterID { "Gain", 1 }, "Gain", NormalisableRange<float> (0.0f, 1.0f), 0.9f));


    auto attackReleaseRange = juce::NormalisableRange<float>(5, 500, 1, 1);

    layout.add(std::make_unique<AudioParameterFloat>(ParameterID {"Attack",1},
                                                    "Attack",
                                                    attackReleaseRange,
                                                    50));

    layout.add(std::make_unique<AudioParameterFloat>(ParameterID {"Release", 1},
                                                    "Release",
                                                     attackReleaseRange,
                                                    250));
    

    auto choices = std::vector<double>{1,1.5,2,3,4,5,6,7,8,10,15,20,50,100};

    juce::StringArray sa;
    for (auto choice : choices)
    {
        sa.add(juce::String(choice, 1));
    }

    layout.add(std::make_unique<AudioParameterChoice>(ParameterID {"Ratio", 1},
                                                      "Ratio",
                                                      sa,
                                                      3 ));
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MultibandCompressorAudioProcessor();
}
 



