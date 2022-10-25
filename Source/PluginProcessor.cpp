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
    //Retrieve APVTS stored paramaters and assign them to cached pointers    //Assert that parameter names are spelt correctly
    
    using namespace Params;
    const auto& params = GetParams();
    
    auto floatHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));
        jassert(param !=nullptr);
    };
    
    floatHelper(compressor.attack, Names::Attack_Low_Band);
    floatHelper(compressor.release, Names::Release_Low_Band);
    floatHelper(compressor.threshold, Names::Threshold_Low_Band);
    
    
    auto choiceHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(params.at(paramName)));
        jassert(param !=nullptr);
    };
    
    choiceHelper(compressor.ratio, Names::Ratio_Low_Band);
    

    auto boolHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
        jassert(param !=nullptr);
    };
    
    boolHelper(compressor.bypassed, Names::Bypassed_Low_Band);
    
    floatHelper(lowCrossover, Names::Low_Mid_Crossover_Freq);
    
    LP.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    

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
    
    LP.prepare(spec);
    HP.prepare(spec);
    
    for(auto& buffer: filterBuffers)
    {
        buffer.setSize(spec.numChannels, samplesPerBlock);
    }
    
    
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
    
//    compressor.updateCompressorSettings();
//    compressor.process(buffer);
    
    for(auto& fb: filterBuffers)
    {
        fb = buffer;
    }
    
    auto cutoff = lowCrossover->get();
    LP.setCutoffFrequency(cutoff);
    HP.setCutoffFrequency(cutoff);
    
    auto fb0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);
    auto fb1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);
    
    auto fb0Ctx = juce::dsp::ProcessContextReplacing<float>(fb0Block);
    auto fb1Ctx = juce::dsp::ProcessContextReplacing<float>(fb1Block);
    
    LP.process(fb0Ctx);
    HP.process(fb1Ctx);
    
    auto numSamples = buffer.getNumSamples();
    auto NumChannels = buffer.getNumChannels();
    
    buffer.clear();
    
    auto addFilterBand = [nc = NumChannels, ns = numSamples](auto& inputBuffer, const auto& source)
    {
        for( auto i = 0; i < nc; ++i)
        {
            inputBuffer.addFrom(i, 0, source, i, 0, ns);
        }
    };
    
    addFilterBand(buffer, filterBuffers[0]);
    addFilterBand(buffer, filterBuffers[1]);

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
    using namespace Params;
    const auto& params = GetParams();

    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID {params.at(Names::Threshold_Low_Band), 1},
                                                    params.at(Names::Threshold_Low_Band),
                                                    juce::NormalisableRange<float>(-60, 12, 1, 1),
                                                    0 ));


    auto attackReleaseRange = juce::NormalisableRange<float>(5, 500, 1, 1);

    layout.add(std::make_unique<AudioParameterFloat>(ParameterID {params.at(Names::Attack_Low_Band),1},
                                                    params.at(Names::Attack_Low_Band),
                                                    attackReleaseRange,
                                                    50));

    layout.add(std::make_unique<AudioParameterFloat>(ParameterID {params.at(Names::Release_Low_Band), 1},
                                                    params.at(Names::Release_Low_Band),
                                                    attackReleaseRange,
                                                    250));
    

    auto choices = std::vector<double>{1,1.5,2,3,4,5,6,7,8,10,15,20,50,100};

    juce::StringArray sa;
    for (auto choice : choices)
    {
        sa.add(juce::String(choice, 1));
    }

    layout.add(std::make_unique<AudioParameterChoice>(ParameterID {params.at(Names::Ratio_Low_Band), 1},
                                                      params.at(Names::Ratio_Low_Band),
                                                      sa,
                                                      3 ));
    
    layout.add(std::make_unique<AudioParameterBool>(ParameterID {params.at(Names::Bypassed_Low_Band), 1},
                                                    params.at(Names::Bypassed_Low_Band),
                                                    false));
    
    layout.add(std::make_unique<AudioParameterFloat>(ParameterID {params.at(Names::Low_Mid_Crossover_Freq)}, params.at(Names::Low_Mid_Crossover_Freq), NormalisableRange<float>(20, 20000, 1, 1), 500));
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MultibandCompressorAudioProcessor();
}
 



