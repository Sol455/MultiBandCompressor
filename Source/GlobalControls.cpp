/*
  ==============================================================================

    GlobalControls.cpp
    Created: 5 Nov 2022 7:31:11pm
    Author:  Sol Harter

  ==============================================================================
*/

#include "GlobalControls.h"
#include "Params.h"
#include "Utilities.h"

GlobalControls::GlobalControls(juce::AudioProcessorValueTreeState& apvts)
{
    using namespace Params;
    const auto& params = GetParams();
    
    auto getParamHelper = [&params, &apvts](const auto& name) -> auto&
    {
        return getParam(apvts, params, name);
    };
    
    auto& gainInParam = getParamHelper(Names::Gain_in);
    auto& lowMidParam = getParamHelper(Names::Low_Mid_Crossover_Freq);
    auto& midHighParam = getParamHelper(Names::Mid_High_Crossover_Freq);
    auto& GainOutParam = getParamHelper(Names::Gain_out);

    
    inGainSlider = std::make_unique<RSWL>(&gainInParam,
                                          "dB",
                                          "INPUT TRIM");
    lowMidXoverSlider = std::make_unique<RSWL>(&lowMidParam,
                                               "Hz",
                                               "LOW-MID X-OVER");
    
    midHighXoverSlider = std::make_unique<RSWL>(&midHighParam,
                                                "Hz",
                                                "MID-HI X-OVER");
    outGainSlider = std::make_unique<RSWL>(&GainOutParam,
                                           "dB",
                                           "OUTPUT TRIM");
    
    
    auto MakeAttachmentHelper = [&params, &apvts](auto& attachment,
                                                  const auto& name,
                                                  auto& slider)
    {
        makeAttachment(attachment, apvts, params, name, slider);
    };
    
    MakeAttachmentHelper(inGainSliderAttachment,
                         Names::Gain_in,
                         *inGainSlider);
    
    MakeAttachmentHelper(lowMidXoverSliderAttachment,
                         Names::Low_Mid_Crossover_Freq,
                         *lowMidXoverSlider);
    
    MakeAttachmentHelper(midHighXoverSliderAttachment,
                         Names::Mid_High_Crossover_Freq,
                         *midHighXoverSlider);
    
    MakeAttachmentHelper(outGainSliderAttachment,
                         Names::Gain_out,
                         *outGainSlider);
    
    addLabelPairs(inGainSlider->labels,
                  gainInParam,
                  "dB");
    
    addLabelPairs(lowMidXoverSlider->labels,
                  lowMidParam,
                  "Hz");
    
    addLabelPairs(midHighXoverSlider->labels,
                  midHighParam,
                  "Hz");
    
    addLabelPairs(outGainSlider->labels,
                  GainOutParam,
                  "dB");
    
    
    addAndMakeVisible(*inGainSlider);
    addAndMakeVisible(*lowMidXoverSlider);
    addAndMakeVisible(*midHighXoverSlider);
    addAndMakeVisible(*outGainSlider);

    
}

void GlobalControls::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds();
    drawModuleBackground(g, bounds);
}

void GlobalControls::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    using namespace juce;
    
    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::row;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;
    
    auto spacer = FlexItem().withWidth(4);
    auto endCap = FlexItem().withWidth(6);
    
    flexBox.items.add(endCap);
    flexBox.items.add(FlexItem(*inGainSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*lowMidXoverSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*midHighXoverSlider).withFlex(1.f));
    flexBox.items.add(spacer);
    flexBox.items.add(FlexItem(*outGainSlider).withFlex(1.f));
    flexBox.items.add(endCap);

    
    flexBox.performLayout(bounds);
}
