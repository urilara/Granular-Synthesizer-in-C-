#pragma once

#include <JuceHeader.h>
#include "Grain.h"
#include "ReferenceCountedBuffer.h"
#include "EnvelopeGUI.h"

class Ralla : public juce::Component
{
public:

    Ralla ()
    {

    };

    Ralla (float position, float length)
        :pos(position), len(length)
    {

    };

    void paintAgain (float currentPos, float audioLength)
    {
        pos = currentPos;
        len = audioLength;
        repaint ();
    }

    void paint (juce::Graphics& g) override
    {
        drawPosition = (float)(pos / len) * (float)getWidth ();
        g.setColour (juce::Colours::red);
        g.drawLine (drawPosition, 0.0f, drawPosition, (float)getHeight (), 2.0f);
    }

private:
    float pos = 0.0f;
    float len = 0.0f;
    float drawPosition = 0.0f;
};

class MainComponent : public juce::AudioAppComponent, public juce::Thread, public juce::ChangeListener
{
public:
    //==============================================================================
    MainComponent () : Thread ("Background Thread"), thumbnailCache(5), thumbnail(512, formatManager, thumbnailCache)
                        , keyboardComponent( keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
    {
        addAndMakeVisible (openButton);
        openButton.setButtonText ("Open");
        openButton.onClick = [this] { openButtonClicked (); };

        addAndMakeVisible (clearButton);
        clearButton.setButtonText ("Clear");
        clearButton.onClick = [this] { clearButtonClicked (); };
		
        //parameters
        //=========================================================================================
		addAndMakeVisible (grainDensityRandom);
		grainDensityRandom.setRange (0.0, 1.0);
		grainDensityRandom.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
		grainDensityRandom.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
        
        addAndMakeVisible (grainDensity);
        grainDensity.setRange (20.0, 80.0);
		grainDensity.setValue (50.0, juce::NotificationType::dontSendNotification);
        grainDensity.setSkewFactorFromMidPoint (50.0);
        grainDensity.setSliderStyle (juce::Slider::SliderStyle::Rotary);
        grainDensity.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
			
        addAndMakeVisible (grainDensityLabel);
        grainDensityLabel.setText ("Grain Rate", juce::dontSendNotification);
        grainDensityLabel.attachToComponent (&grainDensity, false);

        addAndMakeVisible (grainLengthRandom);
        grainLengthRandom.setRange (0.0, 1.0);
        grainLengthRandom.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
        grainLengthRandom.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

		addAndMakeVisible (grainLength);
		grainLength.setRange (1000.0, 100000.0);
		grainLength.setSkewFactorFromMidPoint (5000.0);
		grainLength.setValue (5000.0, juce::NotificationType::dontSendNotification);
		grainLength.setSliderStyle (juce::Slider::SliderStyle::Rotary);
		grainLength.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

		addAndMakeVisible (grainLengthLabel);
		grainLengthLabel.setText ("Grain Length", juce::dontSendNotification);
		grainLengthLabel.attachToComponent (&grainLength, false);
        
        addAndMakeVisible (grainPitchRandom);
        grainPitchRandom.setRange (0.0, 24.0);
        grainPitchRandom.setSkewFactorFromMidPoint (0.5);
        grainPitchRandom.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
        grainPitchRandom.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

		addAndMakeVisible (grainPitch);
		grainPitch.setRange (-48.0, 48.0);
		grainPitch.setSkewFactorFromMidPoint (0.0);
		grainPitch.setValue (0.0, juce::NotificationType::dontSendNotification);
		grainPitch.setSliderStyle (juce::Slider::SliderStyle::Rotary);
		grainPitch.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

		addAndMakeVisible (grainPitchLabel);
		grainPitchLabel.setText ("Grain Pitch", juce::dontSendNotification);
		grainPitchLabel.attachToComponent (&grainPitch, false);

        addAndMakeVisible (grainVolume);
        grainVolume.setRange (-12.0, 6.0);
        grainVolume.setSkewFactorFromMidPoint (0.0);
        grainVolume.setValue (0.0, juce::NotificationType::dontSendNotification);
        grainVolume.setSliderStyle (juce::Slider::SliderStyle::Rotary);
        grainVolume.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

        addAndMakeVisible (grainVolumeRandom);
        grainVolumeRandom.setRange (0.0, 1.0);
        grainVolumeRandom.setSkewFactorFromMidPoint (0.5);
        grainVolumeRandom.setValue (0.0);
        grainVolumeRandom.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
        grainVolumeRandom.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

        addAndMakeVisible (grainVolumeLabel);
        grainVolumeLabel.setText ("Grain output level",juce::NotificationType::dontSendNotification);
        grainVolumeLabel.attachToComponent (&grainVolume, false);

		addAndMakeVisible (grainPositionRandom);
		grainPositionRandom.setRange (0.0, 0.5);
        grainPositionRandom.setSkewFactorFromMidPoint (0.1);
		grainPositionRandom.setSliderStyle (juce::Slider::SliderStyle::LinearVertical);
		grainPositionRandom.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

		addAndMakeVisible (grainPosition);
		grainPosition.setRange (0.0, 1.0);
		grainPosition.setSliderStyle (juce::Slider::SliderStyle::LinearHorizontal);
		grainPosition.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

        addAndMakeVisible (envelopeGui);
        
        addAndMakeVisible (envelopeCurve);
        envelopeCurve.setRange (-3.0, 3.0);
        envelopeCurve.setSkewFactorFromMidPoint (0.0);
        envelopeCurve.setSliderStyle (juce::Slider::SliderStyle::Rotary);
        envelopeCurve.setTextBoxStyle (juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

        addAndMakeVisible (envelopeCurveLabel);
        envelopeCurveLabel.setText ("Envelope Curve", juce::dontSendNotification);
        envelopeCurveLabel.attachToComponent (&envelopeCurve, false);


        //=====================================================================================================
        
        addAndMakeVisible (ralla);
        thumbnail.addChangeListener (this);

        //Midi handling 
        //=====================================================================================================
        
        addAndMakeVisible (midiInputList);
        midiInputList.setTextWhenNoChoicesAvailable ("No MIDI Inputs Enabled");
        auto midiInputs = juce::MidiInput::getAvailableDevices ();

        addAndMakeVisible (midiInputListLabel);
        midiInputListLabel.setText ("MIDI Input:", juce::dontSendNotification);
        midiInputListLabel.attachToComponent (&midiInputList, true);

        juce::StringArray midiInputNames;

        for (auto input : midiInputs)
            midiInputNames.add (input.name);

        midiInputList.addItemList (midiInputNames, 1);
        midiInputList.onChange = [this] { setMidiInput (midiInputList.getSelectedItemIndex ()); };

        for (auto input : midiInputs)
        {
            if (deviceManager.isMidiInputDeviceEnabled (input.identifier))
            {
                setMidiInput (midiInputs.indexOf (input));
                break;
            }
        }

        if (midiInputList.getSelectedId () == 0)
            setMidiInput (0);

        addAndMakeVisible (keyboardComponent);

        //================================================================================================================================

        formatManager.registerBasicFormats ();
        setSize (800, 600);
        setAudioChannels (0, 2); // inicialitzem el audio per obtenir el sampleRate i poder resamplejar l'arxiu abans de guardar-lo en fileBuffer.
        startThread ();

    }

    ~MainComponent () override
    {   
        stopThread (4000);
        shutdownAudio ();
    }


    void run () override
    {
        while (!threadShouldExit ()) //aquest es el background thread
        {

            checkForPathToOpen ();
            checkForBuffersToFree (); //primer mira si hi ha buffers que alliberar i llavors genera els grains.
            grainScheduling ();
            wait (dur);
        }
    }

    void setMidiInput(int index)
    {
        auto list = juce::MidiInput::getAvailableDevices ();

        auto newInput = list[index];

        deviceManager.removeMidiInputDeviceCallback (list[lastInputIndex].identifier,
                                                     &midiCollector);

        if (!deviceManager.isMidiInputDeviceEnabled (newInput.identifier))
            deviceManager.setMidiInputDeviceEnabled (newInput.identifier, true);

        deviceManager.addMidiInputDeviceCallback (newInput.identifier, &midiCollector);
        midiInputList.setSelectedId (index + 1, juce::dontSendNotification);

        lastInputIndex = index;
    }


   void handleMidiMessage (juce::MidiBuffer& midiBuffer)
    {

        juce::MidiBuffer::Iterator i (midiBuffer);
        juce::MidiMessage message;
        int position;

        while (i.getNextEvent (message, position))
        {
            if (message.isNoteOn ()) {
                midiNotes[message.getNoteNumber ()] = message.getVelocity ();
                notify ();
            }
            if (message.isNoteOff ()) {
                midiNotes[message.getNoteNumber ()] = 0;
            }
            if (message.isAllNotesOff ()) {
                for (int i = 0; i < 128; ++i)
                    midiNotes[i] = 0;
            }
        }
    }


    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        hostSampleRate = sampleRate;
        midiCollector.reset (sampleRate);
    }

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override 
    {
        auto retainedCurrentBuffer = [&]() -> ReferenceCountedBuffer::Ptr
        {
            const juce::SpinLock::ScopedTryLockType lock (mutex);

            if (lock.isLocked ()) return currentBuffer;

            return nullptr;
        }();

        if (retainedCurrentBuffer == nullptr)
        {
            bufferToFill.clearActiveBufferRegion ();
            return;
        }

        auto* currentAudioSampleBuffer = retainedCurrentBuffer->getAudioSampleBuffer ();

        const int numSamplesInBlock = bufferToFill.buffer->getNumSamples(); 
        const int numSamplesInFile = currentAudioSampleBuffer->getNumSamples ();

        juce::MidiBuffer incomingMidi;

        midiCollector.removeNextBlockOfMessages (incomingMidi, numSamplesInBlock);
        keyboardState.processNextMidiBuffer (incomingMidi, bufferToFill.startSample, numSamplesInBlock, true);
        handleMidiMessage (incomingMidi);

        const juce::Array<Grain> localStack = grainStack; //possiblement s'haurà d'implementar mutex.
       
        for (int sample = 0; sample < numSamplesInBlock; ++sample) {
            for (int i = 0; i < localStack.size (); i++) {
                if (localStack[i].onset < time) { //aixo es el que ens permet retrassar la reproducció del grain.
                    if (time < (localStack[i].onset + localStack[i].length)) { //aixo es el que fa que pari la reproducció del grain. 
                        localStack[i].process (bufferToFill.buffer, currentAudioSampleBuffer, bufferToFill.buffer->getNumChannels (), numSamplesInBlock, numSamplesInFile, time);
                    }
                }
            }

            time++;
        }
    }

    void changeListenerCallback (juce::ChangeBroadcaster* source) override
    {
        const juce::MessageManagerLock msgLock;

        if(source == &thumbnail)
            repaint ();      
        
    }

    void paint (juce::Graphics& g) override
    {
        const juce::MessageManagerLock msgLock;
        juce::Rectangle<int> thumbnailBounds (10, getHeight () / 4, getWidth () - 60, 200);

        if (thumbnail.getNumChannels () == 0)
            paintIfNoFileLoaded (g, thumbnailBounds);
        else
            paintIfFileLoaded (g, thumbnailBounds);

    }

    void paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
    {
        g.setColour (juce::Colours::white);
        g.fillRect (thumbnailBounds);
        g.setColour (juce::Colours::blue);
        thumbnail.drawChannels (g, thumbnailBounds, 0.0, thumbnail.getTotalLength (), 1.0f);
    }

    void paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
    {
        g.setColour (juce::Colours::darkgrey);
        g.fillRect (thumbnailBounds);
        g.setColour (juce::Colours::white);
        g.drawFittedText ("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
    }

    void releaseResources () override
    {
        const juce::SpinLock::ScopedLockType lock (mutex);
        currentBuffer = nullptr;
    }

    //==============================================================================

    void resized () override
    {
        midiInputList.setBounds (getWidth() / 2 - 10, 10, getWidth () / 2, getHeight () / 20);
        openButton.setBounds (10, midiInputList.getY() + getHeight() / 20 + 10, getWidth () - 20, getHeight () / 15);
        clearButton.setBounds (10, openButton.getY() + getHeight() / 15 + 10, getWidth () - 20, getHeight () / 15);

        keyboardComponent.setBounds (10, 9 * getHeight() / 10 , getWidth () - 20, getHeight () / 10);

        grainDensity.setBounds (10, 7.0 * getHeight () / 10.0, getWidth () / 8, getHeight () / 8);
		grainDensityRandom.setBounds (10, 7.0 * getHeight () / 10.0 + getHeight () / 8 + 5, getWidth () / 8, getHeight () / 32);

		grainLength.setBounds (getWidth() / 6, 7.0 * getHeight () / 10.0, getWidth () / 8, getHeight () / 8);
        grainLengthRandom.setBounds (getWidth () / 6, 7.0 * getHeight () / 10.0 + getHeight () / 8 + 5, getWidth () / 8, getHeight () / 32);

		grainPitch.setBounds (getWidth() / 3, 7.0 * getHeight () / 10.0, getWidth () / 8, getHeight () / 8);
        grainPitchRandom.setBounds (getWidth () / 3, 7.0 * getHeight () / 10.0 + getHeight () / 8 + 5, getWidth () / 8, getHeight () / 32);

		grainPosition.setBounds (10, (7.0 * getHeight ()) / 12.0, getWidth () - 60, 40);
		grainPositionRandom.setBounds (10 + getWidth () - 60, getHeight () / 3, 50, 100);

        grainVolume.setBounds (getWidth () / 2 - 20, 7.0 * getHeight () / 10.0, getWidth () / 8, getHeight () / 8);
        grainVolumeRandom.setBounds (getWidth () / 2 - 20, 7.0 * getHeight () / 10.0 + getHeight () / 8 + 5, getWidth () / 8, getHeight () / 32);

        envelopeGui.setBounds (getWidth () * (2.0 / 3.0), getHeight() * (2.0/3.0), (getWidth() / 8.0) * 1.5, getWidth () / 8.0);

        envelopeCurve.setBounds (getWidth () * (7.0 / 8.0), 7.0 * getHeight () / 10.0, getWidth () / 8, getHeight () / 8);

        ralla.setBounds (10, getHeight () / 4, getWidth () - 60, 200);
    }


private:
    //==============================================================================
    // Your private member variables go here...

    
    int wrap (int val, const int low, const int high) //Si val es menor que low es resta a high, si val es major que high, es suma a low. 
    {
        int range_size = high - low + 1;

        if (val < low)
            val += range_size * ((low - val) / range_size + 1); 

        return low + (val - low) % range_size; 
    }


    void grainScheduling () { 


        juce::Array<juce::Array<int>> activeNotes;

        for (int i = 0; i < 128; i++) {
            if (midiNotes[i] > 0) {
                activeNotes.add (juce::Array<int> {i, midiNotes[i] });
            }
        }


        if (grainStack.size () > 0) {
            for (int i = grainStack.size () - 1; i >= 0; i--)
            {
                int grainEnd = grainStack[i].onset + grainStack[i].length;
                bool hasEnded = grainEnd < time;
                if (hasEnded) grainStack.remove (i); //s'elimina el grain que ja s'ha reproduït.
            }
        }


        if (currentBuffer != nullptr) 
        {   
            {
                juce::MessageManagerLock msgLock;
                float curve = envelopeCurve.getValue ();
                envelopeGui.drawPath (curve);
            }
            
            
            if (activeNotes.size () > 0)
            {

                dur = grainDensity.getValue () * (1 + grainDensityRandom.getValue () * (juce::Random::getSystemRandom ().nextFloat () * 2.0 - 1.0));
                int length = grainLength.getValue () * (1 + grainLengthRandom.getValue () * (juce::Random::getSystemRandom ().nextFloat () * 2.0 - 1.0));

                float midiNote = 60;
                midiNote = activeNotes[juce::Random::getSystemRandom ().nextInt (activeNotes.size ())][0]; // aaah de les notes actives tria la que sigui
                midiNote = (midiNote - 61);

                float pitch = grainPitch.getValue () + midiNote; //* (1 + grainPitchRandom.getValue () * (juce::Random::getSystemRandom ().nextFloat () * 2.0 - 1.0));
                pitch += 1 + grainPitchRandom.getValue () * (juce::Random::getSystemRandom ().nextFloat () * 2 - 1);

                float rate = std::pow (2.0, pitch / 12.0);

                float position = grainPosition.getValue() + grainPositionRandom.getValue () * (juce::Random::getSystemRandom ().nextFloat () * 2.0 - 1.0);

                float volume = std::pow (10, grainVolume.getValue () / 10) * (1 + grainVolumeRandom.getValue () * (juce::Random::getSystemRandom ().nextFloat () * 2.0 - 1.0));

                length = std::abs (length);
                dur = std::abs (dur);
                volume = std::abs (volume);

                int numSamples = currentBuffer->getAudioSampleBuffer ()->getNumSamples ();
                int startPosition = position * numSamples;

                float envelopeAttack = envelopeGui.getAttackValueX () / (float)envelopeGui.getWidth();
                float envelopeDecay = envelopeGui.getDecayValueX () / (float)envelopeGui.getWidth();
                float curve = envelopeCurve.getValue ();

                {
                    juce::MessageManagerLock msgLock;
                    envelopeGui.drawPath (curve);
                    ralla.paintAgain (wrap (startPosition, 0, numSamples), numSamples);
                }
            
                grainStack.add (Grain (time, length, wrap (startPosition, 0, numSamples), envelopeAttack, envelopeDecay, curve, rate, volume));

            }

           
            
        }
        
    }

    void checkForBuffersToFree ()
    {
        for (auto i = buffers.size (); --i >= 0;)                           
        {
            ReferenceCountedBuffer::Ptr buffer (buffers.getUnchecked (i)); 

            if (buffer->getReferenceCount () == 2)
            {
                buffers.remove (i);
            }
                
                
        }
    }


    void openButtonClicked ()
    {                                                                          

        chooser = std::make_unique<juce::FileChooser> ("Select a Wave file shorter than 5 seconds to play...",
                                                       juce::File{},
                                                       "*.wav");
        auto chooserFlags = juce::FileBrowserComponent::openMode
            | juce::FileBrowserComponent::canSelectFiles;

        chooser->launchAsync (chooserFlags, [this](const juce::FileChooser& fc)
                              {
                                  auto file = fc.getResult ();

                                  if (file == juce::File{})
                                      return;

                                  auto path = file.getFullPathName ();

                                  {
                                      const juce::ScopedLock lock (pathMutex);
                                      chosenPath.swapWith (path);
                                  }

                                  notify (); //força a obrir la funció checkForPathToOpen encara que estiguem en wait().
                              });
    }

    void checkForPathToOpen ()
    {
        juce::String pathToOpen;
        {
            const juce::ScopedLock lock (pathMutex);
            pathToOpen.swapWith (chosenPath);
        }

        if (pathToOpen.isNotEmpty ())
        {
            juce::File file (pathToOpen);
            //std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (file));

            auto* reader = formatManager.createReaderFor (file); // si fem servir audioformatreadersource no podem fer servir smart pointers. 

            if (reader!= nullptr)
            {

                ReferenceCountedBuffer::Ptr newBuffer = new ReferenceCountedBuffer (file.getFileName (), (int)reader->numChannels, (int)reader->lengthInSamples); //when using new we have to use delete or manage it with reference counting.

                if (reader->sampleRate != hostSampleRate) { //resampling
          
                    juce::AudioSampleBuffer temp;

                    temp.clear ();

                    double ratio = reader->sampleRate / hostSampleRate;

                    temp.setSize ((int)reader->numChannels, (int)reader->lengthInSamples);
                    newBuffer->getAudioSampleBuffer ()->setSize ((int)reader->numChannels, ((double)((int)reader->lengthInSamples) / ratio));

                    reader->read (&temp, 0, (int)reader->lengthInSamples, 0, true, true);

                    std::unique_ptr<juce::LagrangeInterpolator> resampler (new juce::LagrangeInterpolator ());
                    const float** inputs = temp.getArrayOfReadPointers ();
                    float** outputs = newBuffer->getAudioSampleBuffer ()->getArrayOfWritePointers ();

                    for (int c = 0; c < newBuffer->getAudioSampleBuffer ()->getNumChannels (); c++)
                    {
                        resampler->reset ();
                        resampler->process (ratio, inputs[c], outputs[c], newBuffer->getAudioSampleBuffer ()->getNumSamples ()); //equival al read normal.
                    }

                }

                else {
                    reader->read (newBuffer->getAudioSampleBuffer (), 0, (int)reader->lengthInSamples, 0, true, true);
                }
                
                {
                    const juce::MessageManagerLock msgLock;
                    thumbnail.setSource (new juce::FileInputSource (file));   
                    envelopeGui.loadNodes ();
                }

                { //quan copiem el buffer hem de crear un mutex perque no modifiqui cap altre thread el buffer que anem a copiar.
                    const juce::SpinLock::ScopedLockType lock (mutex);
                    currentBuffer = newBuffer;
                }
                
                buffers.add (newBuffer); //això per anar fent reference counting i elminar-lo quan no el necessitem.
            }
        }
    }

    void clearButtonClicked ()
    {
        const juce::SpinLock::ScopedLockType lock (mutex);
        currentBuffer = nullptr;
        envelopeGui.clearGui();
    }

    std::unique_ptr<juce::FileChooser> chooser;
    juce::TextButton clearButton;
    juce::SpinLock mutex;
    juce::CriticalSection pathMutex;
    juce::String chosenPath;
    juce::ReferenceCountedArray<ReferenceCountedBuffer> buffers;
    ReferenceCountedBuffer::Ptr currentBuffer;
    juce::TextButton openButton;
    juce::AudioFormatManager formatManager;
    juce::Array<Grain> grainStack;

    juce::AudioDeviceManager deviceManager;
    juce::ComboBox midiInputList;
    juce::Label midiInputListLabel;
    int lastInputIndex = 0;
    bool isAddingFromMidiInput = false;
    int midiNotes[128] = { 0 };
    juce::MidiMessageCollector midiCollector;

    juce::MidiKeyboardState keyboardState; // es necesari??
    juce::MidiKeyboardComponent keyboardComponent;

    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;


    long long int time = 0; //representa samples, no segons.
    float angle = 0;
    float hostSampleRate = 0;
    int dur = 1000;

    juce::Slider grainDensity;
    juce::Label grainDensityLabel;

	juce::Slider grainLength;
	juce::Label grainLengthLabel;

	juce::Slider grainPitch;
	juce::Label grainPitchLabel;

    juce::Slider envelopeCurve;
    juce::Label  envelopeCurveLabel;

    juce::Slider grainVolume;
    juce::Label grainVolumeLabel;
    
    juce::Slider grainPosition;
	
	juce::Slider grainDensityRandom;
	juce::Slider grainLengthRandom;
	juce::Slider grainPitchRandom;
	juce::Slider grainPositionRandom;
    juce::Slider grainVolumeRandom;

    Envelope envelopeGui;

    Ralla ralla;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
