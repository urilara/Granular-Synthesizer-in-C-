/*
  ==============================================================================

    ReferenceCountedBuffer.h
    Created: 31 Aug 2022 4:51:51pm
    Author:  Usuario

  ==============================================================================
*/

#pragma once

class ReferenceCountedBuffer : public juce::ReferenceCountedObject
{
public:
    typedef juce::ReferenceCountedObjectPtr<ReferenceCountedBuffer> Ptr;

    ReferenceCountedBuffer (const juce::String& nameToUse,
                            int numChannels,
                            int numSamples)
        : name (nameToUse),
        buffer (numChannels, numSamples)
    {
        DBG (juce::String ("Buffer named '") + name + "' constructed. numChannels = " + juce::String (numChannels) + ", numSamples = " + juce::String (numSamples));
    }

    ~ReferenceCountedBuffer ()
    {
        DBG (juce::String ("Buffer named '") + name + "' destroyed");
    }

    juce::AudioSampleBuffer* getAudioSampleBuffer ()
    {
        return &buffer;
    }

    int time = 0;

private:
    juce::String name;
    juce::AudioSampleBuffer buffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReferenceCountedBuffer)
};
