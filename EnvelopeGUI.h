/*
  ==============================================================================

    EnvelopeGUI.h
    Created: 17 Sep 2022 11:25:30am
    Author:  Usuario

  ==============================================================================
*/

#pragma once
#include<JuceHeader.h>

class Node : public juce::Component {

public:

    Node () {

        constrainer.setMinimumOnscreenAmounts (5000, 5000, 5000, 5000);
        
    }

    ~Node() {

    }

    void paint (juce::Graphics& g) override 
    {

        g.setColour (juce::Colours::lightsteelblue);
        g.fillEllipse (0.0, 0.0, getWidth (), getHeight ());
    }

    void resized () override
    {

    }

    void mouseDrag (const juce::MouseEvent& e) override {

        myDragger.dragComponent (this, e, &constrainer);
        constrainer.applyBoundsToComponent (*this, this->getBoundsInParent ().withY (getParentHeight() / 6));
    }

    void mouseDown (const juce::MouseEvent& e) override {

        myDragger.startDraggingComponent (this, e);

    }



private:
    juce::ComponentDragger myDragger;
    juce::ComponentBoundsConstrainer constrainer;

};

class Envelope : public juce::Component {

public:

    Envelope () {

        addAndMakeVisible (attackNode);
        addAndMakeVisible (decayNode);
    }

    ~Envelope () {

    }

    float getAttackValueX () 
    {
        return attackNode.getX ();
    }

    float getDecayValueX ()
    {
        return decayNode.getX ();
    }

    void paint (juce::Graphics& g) override 
    {

        g.setColour (juce::Colours::slategrey);
        g.fillAll ();
        g.drawRect (getLocalBounds());
        
        g.setColour (juce::Colours::lightblue);
        g.strokePath (myPath, juce::PathStrokeType (1.0f));
        
    }

    void drawPath (float curve) {

        myPath.clear ();

        myPath.startNewSubPath (0.0f, getHeight ());

        //myPath.quadraticTo ((curve * 10 + getWidth() / 8), (getHeight () - getHeight () / 6) / 2, attackNode.getX () + attackNode.getWidth () / 2, attackNode.getY () + attackNode.getHeight () / 2);
        //myPath.quadraticTo ((float)getWidth() / 8.0f, ((float)getHeight() - (float)getHeight() / 6.0f) / 2.0f, attackNode.getX () + attackNode.getWidth () / 2, attackNode.getY () + attackNode.getHeight () / 2);

        myPath.lineTo (attackNode.getX () + attackNode.getWidth () / 2, attackNode.getY () + attackNode.getHeight () / 2);

        myPath.startNewSubPath (attackNode.getX () + attackNode.getWidth () / 2, attackNode.getY () + attackNode.getHeight () / 2);

        myPath.lineTo (decayNode.getX () + decayNode.getWidth () / 2, decayNode.getY () + decayNode.getWidth () / 2);

        myPath.startNewSubPath (decayNode.getX () + decayNode.getWidth () / 2, decayNode.getY () + decayNode.getWidth () / 2);

        //myPath.quadraticTo ((float)getWidth () * (3.0 / 8.0), ((float)getHeight () - (float)getHeight () / 6.0f) / 2.0f, getHeight (), getWidth ());
        myPath.lineTo (getWidth (), getHeight ());

        repaint ();

    }



    void resized () override
    {

    }

    void loadNodes () 
    {
        attackNode.setBounds (getWidth () / 4.0, getHeight () / 6, getWidth () / 15.0, getWidth () / 15.0);
        decayNode.setBounds (getWidth () - (getWidth () / 4.0), getHeight () / 6, getWidth () / 15.0, getWidth () / 15.0);
    }

    void clearGui ()
    {
        myPath.clear ();
        attackNode.setBounds (getWidth () / 4.0, getHeight () / 6, 0.0f, 0.0f);
        decayNode.setBounds (getWidth () - (getWidth () / 4.0), getHeight () / 6, 0.0f, 0.0f);
        repaint ();
    }

private:

    Node attackNode;
    Node decayNode;
    juce::Path myPath;
};