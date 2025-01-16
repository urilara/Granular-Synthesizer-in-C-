/*
  ==============================================================================

    Grain.h
    Created: 31 Aug 2022 12:26:31pm
    Author:  Usuario

  ==============================================================================
*/

#pragma once

class Grain
{
public:
    int onset; //start time of the grain in samples, it has to be smaller than the external time variable.
    int length;
    int startPosition;
    float envAttack, envAttackRecip; //Recip because multiplicatiom is cheaper than division.
    float envRelease, envReleaseRecip;
    float envCurve;
    float lengthRecip;
    float rate;
    float volume;

    Grain (long long int onset, int length, int startPos, float attackX, float decayX, float envCurve, float rate, float volume)
        :onset (onset), length (length), startPosition (startPos),
        envAttack (attackX), envAttackRecip (1 / envAttack),
        envRelease (decayX), envReleaseRecip (1 / (1 - envRelease)),
        lengthRecip (1 / (float)length), envCurve(envCurve), rate(rate), volume(volume)
    {

    }


    Grain ()
    {
        onset = 0;
        length = 100;
        startPosition = 0;
        envAttack = 0.3;
        envAttackRecip = 1 / envAttack;
        envRelease = 0.6;
        envCurve = 0.0;
        rate = 1.0;
        envReleaseRecip = 1 / (1 - envRelease);
        lengthRecip = 1 / (float)length;
    }

    ~Grain () {}

    void process (juce::AudioSampleBuffer* currentBlock, juce::AudioSampleBuffer* fileBuffer, int numChannels, int numSamplesInBlock, int numSamplesInFile, int time)
    {

        for (int channel = 0; channel < numChannels; ++channel) {

            const float gain = envelope (time);

            float* channelData = currentBlock->getWritePointer (channel);
            const float* fileData = fileBuffer->getReadPointer (channel % fileBuffer->getNumChannels ());

            const float position = (time - onset) * rate;
            const int iPosition = (int)std::ceil (position);

            const float fracPos = position - iPosition;

            float currentSample = fileData[(iPosition + startPosition) % numSamplesInFile];
            float a = fileData[(iPosition + startPosition - 3) % numSamplesInFile]; 
            float b = fileData[(iPosition + startPosition - 2) % numSamplesInFile];
            float c = fileData[(iPosition + startPosition - 1) % numSamplesInFile];

            currentSample = cubicinterpolation (fracPos, a, b, c, currentSample);

            channelData[time % numSamplesInBlock] += currentSample * gain * volume; //si la startPosition es al final de l'arxiu continuara pel principi
        }                                                                                          //quan time sobrepassi el límit de numSamplesInFile.

        //time % numSamplesInBlock ens assegura el bucle

    }

    float envelope (int time) // es podria provar a fer una wavetable del envelope?
    {
        float gain;
        float envPos;

        envPos = (time - onset) * lengthRecip; //va de 0 a 1 la posició, al restar onset i ser reassignat cada trucada de run a time + onset, sempre es reseteja a 0 al principi.
        

        if (envPos <= envAttack) { //attack
            if (std::abs (envCurve) > 0.001) 
            {
                float aPos;

                aPos = envPos * envAttackRecip;

                double denom = 1.0f - exp (envCurve);
                double numer = 1.0f - exp (aPos * envCurve);

                gain = (numer / denom);
            }

            else
            {
                float aPos;
                aPos = envPos * envAttackRecip; //quan envPos == envAttackRecip tenim gain = 1.
                gain = aPos;
            }
            
        }

        else if (envPos < envRelease) { //sustain
            gain = 1.0;
        }

        else if (envPos >= envRelease) { //release

            if (std::abs (envCurve) > 0.001)
            {
                float rPos;

                rPos = (envPos - envRelease) * envReleaseRecip;

                double denom = 1.0f - exp (-envCurve);
                double numer = 1.0f - exp (rPos * -envCurve);

                gain = (numer / denom) * (-1) + 1;
            }
            
            else
            {
                float rPos;

                rPos = (envPos - envRelease) * envReleaseRecip;
                gain = rPos * (-1) + 1; // va de 1 a 0. Com envReleaseRecip es defineix com 1/(1-envRelease) quan envPos sigui 1 tindrem (1-envRelease)/(1-envRelease) = 1 -> * (-1) + 1 = 0.
        
            }
        }
            

        return gain;

    }
    
    inline float cubicinterpolation (float x, float y0, float y1, float y2, float y3)
    {
        float c0 = y1;
        float c1 = 0.5f * (y2 - y0);
        float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
        float c3 = 0.5f * (y3 - y0) + 1.5 * (y1 - y2);

        return((c3 * x + c2) * x + c1) * x + c0;
    }

};

