#pragma once
#include <JuceHeader.h>



#define NAME_RATE      "rate"
#define DEFAULT_RATE   50.0f //frequenza dell'lfo
#define RATE_SMTH      0.05f


#define NAME_WF        "waveform"
#define DEFAULT_WF     0


#define TGT_LVL          0.36f


class NaiveOscillator : public AudioProcessorValueTreeState::Listener
{
public:
    NaiveOscillator()
    {
        frequency.setCurrentAndTargetValue(DEFAULT_RATE);
    }

    ~NaiveOscillator() {};

    void prepareToPlay(double sr, const int maxSamplesPerBlock)
    {
        sampleRate = sr;
        frequency.reset(sr, RATE_SMTH);
        
    }

    void getNextAudioBlock(AudioBuffer<float>& bufferInput, AudioBuffer<float>& lfoBuffer, const int numSamples)
    {
        auto* bufferInputData = bufferInput.getWritePointer(0);
        auto* lfoBufferData = lfoBuffer.getWritePointer(0);
        
        for (int smp = 0; smp < numSamples; ++smp)
        {
            getNextAudioSample(bufferInputData[smp]);
            lfoBufferData[smp] = sampleValue;
        }
    
    }

    void getNextAudioSample(float smp)
    {
        
        switch (waveform)
        {
            case 0: //sinusoide
                sampleValue = (sin(2.0f * float_Pi * normalizedPhaseValue)/2.0f)+0.5f;
                
                break;
            case 1: { //gradino
                auto triang = round((2.0f *abs(normalizedPhaseValue - 0.5f)) * 5.0f);
                sampleValue = triang / 5.0f;
                break;
            };
            case 2: //quadra
                sampleValue = (((normalizedPhaseValue > 0.5) - (normalizedPhaseValue < 0.5))/2.0f) + 0.5f;
                break;
            case 3: {//sample and hold
                if(oldNormalizedPhaseValue > normalizedPhaseValue)
                {
                    sampleValue = rand.nextFloat();
                }
                break;
            };
            case 4:{ //envelope follower
                float frequencyValue = 440.0f * std::pow(2.0,(frequency.getCurrentValue() - 69.0f) / 12.0f);
                sampleValue = envelopeFollowerSampleValue(smp, frequencyValue); //calcola un solo valore di envelope
               
                break;
            }
            default:
                sampleValue = 0.0f;
        }
        
        const float phaseIncrement = frequency.getNextValue() / sampleRate;
        
        oldNormalizedPhaseValue = normalizedPhaseValue;
        normalizedPhaseValue += phaseIncrement;
        normalizedPhaseValue -= static_cast<int>(normalizedPhaseValue);
    }

    
private:
    void parameterChanged(const String& paramID, float newValue) override
    {
        
        if (paramID == NAME_RATE)
            frequency.setTargetValue(newValue);
            
        
        if (paramID == NAME_WF)
            waveform = roundToInt(newValue);
    
    }
    
    
    float envelopeFollowerSampleValue(float sampleValue, float freq)
    {
        
        updateAlpha(freq);
        envelopeValue = jmax(sampleValue, envelopeValue * alpha);
        return envelopeValue;
        
    }
    
    void updateAlpha(float freq)
    {
        float release = 1.0f / freq;
        const float n = jmax(1.0f, (float)(release * sampleRate));
        alpha = pow(TGT_LVL, 1.0f / n);
    }
    
    int waveform = DEFAULT_WF;
    
    double sampleRate = 1.0;
    
    float sampleValue = 0.0;
    
    
    
    float normalizedPhaseValue = 0.0f;
    float oldNormalizedPhaseValue = 0.0f;
    
    SmoothedValue<float, ValueSmoothingTypes::Multiplicative> frequency;
    
    Random rand;
    
    //parametri envelope follower
    float envelopeValue = 0.0f;
    float alpha;
};
