/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once
/*
 1) Global controls (x-over sliders
 2) Main Band controls (attack,release threshold sliders)
 3) Ass solo/mute/bypass buttons
 4) Band select functionality
 5) Ban select buttons reflect the solo, mute bypass button state implement logic that prevents you from having both mute & solo button at the same time
 6) custom look & feel for sliders and toggle buttons
 7) spectrum analyser overview
 8) Data structure for the spectrum analyzer
 9) Fifo usage in plugin processor process block, need a bit of dsp modification
 10) Implementation of the analyser rendering pre-processing paths
 11) Drawing crossovers on top of the analyzer plot
 12) Drawing gain reduction on top of the analyzer
 13) analyzer Bypass
 14) Global bypass button
 */
#include <JuceHeader.h>
#include "DSP/CompressorBand.h"



//==============================================================================
/**
*/
class MultibandCompressorAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    MultibandCompressorAudioProcessor();
    ~MultibandCompressorAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //Assing shorthand to audioProcessorValueTreeState
    using APVTS = juce::AudioProcessorValueTreeState;
    //create parameterlayout -- function definition, define function to create parameter layout
    static APVTS::ParameterLayout createParameterLayout();

    //Class initilisation for APVTS
    APVTS apvts{* this, nullptr, "Parameters", createParameterLayout() };
private:
    
    std::array<CompressorBand, 3> compressors;
    CompressorBand& lowBandComp = compressors[0];
    CompressorBand& midBandComp = compressors[1];
    CompressorBand& highBandComp = compressors[2];
    
    using Filter = juce::dsp::LinkwitzRileyFilter<float>;
    
    Filter LP1, AP2,
           HP1, LP2,
                HP2;
    
//    Filter invAP1, invAP2;
//    juce::AudioBuffer<float> invAPBuffer;
    
    juce::AudioParameterFloat* lowMidCrossover {nullptr};
    juce::AudioParameterFloat* midHighCrossover {nullptr};
    
    std::array<juce::AudioBuffer<float>, 3> filterBuffers;
    
    juce::dsp::Gain<float> inputGain, outputGain;
    juce::AudioParameterFloat* inputGainParam {nullptr};
    juce::AudioParameterFloat* outputGainParam {nullptr};
    
    template<typename T, typename U>
    void applyGain(T& buffer, U& gain)
    {
        auto block = juce::dsp::AudioBlock<float>(buffer);
        auto ctx = juce::dsp::ProcessContextReplacing<float>(block);
        gain.process(ctx);
    }
    
    void updateState();
    void splitBands(const juce::AudioBuffer<float>& inputBuffer);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultibandCompressorAudioProcessor)
};
 
