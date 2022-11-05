/*
  ==============================================================================

    CompressorBand.cpp
    Created: 5 Nov 2022 7:36:20pm
    Author:  Sol Harter

  ==============================================================================
*/

#include "CompressorBand.h"

void CompressorBand::prepare(const juce::dsp::ProcessSpec& spec) {
    compressor.prepare(spec);
}
 
void CompressorBand::updateCompressorSettings() {
    compressor.setAttack(attack->get());
    compressor.setRelease(release ->get());
    compressor.setThreshold(threshold->get());
    compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());
}

void CompressorBand::process(juce::AudioBuffer<float> &buffer) {
    auto block= juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);
    
    context.isBypassed = bypassed->get();
    compressor.process(context);
    
}
