/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 7.0.12

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MyLookAndFeel.h"


typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class PluginEditorGui  : public juce::AudioProcessorEditor
{
public:
    //==============================================================================
    PluginEditorGui (RingWormModulatorAudioProcessor &p, AudioProcessorValueTreeState& vst);
    ~PluginEditorGui() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    RingWormModulatorAudioProcessor& audioProcessor;
    AudioProcessorValueTreeState& valueTreeState;

    MyLookAndFeel themeModParams;
    MyLookAndFeel themeLfoParams;
    MyLookAndFeel themeWaveformParam;

    std::unique_ptr<SliderAttachment> freqAttachment;
    std::unique_ptr<SliderAttachment> blendAttachment;
    std::unique_ptr<SliderAttachment> modeAttachment;
    std::unique_ptr<SliderAttachment> rateAttachment;
    std::unique_ptr<SliderAttachment> widthAttachment;
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;

    // Binary resources:
    static const char* img_0912_png;
    static const int img_0912_pngSize;
    static const char* img_0914_png;
    static const int img_0914_pngSize;
    static const char* img_0916_png;
    static const int img_0916_pngSize;
    static const char* img_0918_png;
    static const int img_0918_pngSize;
    static const char* img_0921_png;
    static const int img_0921_pngSize;
    static const char* logoFullBlack_png;
    static const int logoFullBlack_pngSize;
    static const char* img_0954_png;
    static const int img_0954_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::Slider> freqSlider;
    std::unique_ptr<juce::Slider> blendSlider;
    std::unique_ptr<juce::Slider> modeSlider;
    std::unique_ptr<juce::Slider> rateSlider;
    std::unique_ptr<juce::Slider> widthSlider;
    juce::Image cachedImage_img_0912_png_1;
    juce::Image cachedImage_img_0914_png_2;
    juce::Image cachedImage_img_0916_png_3;
    juce::Image cachedImage_img_0918_png_4;
    juce::Image cachedImage_img_0921_png_5;
    juce::Image cachedImage_logoFullBlack_png_6;
    juce::Image cachedImage_img_0954_png_7;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditorGui)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

