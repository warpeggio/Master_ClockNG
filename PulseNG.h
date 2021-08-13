// PulseNG: A poorly quantized, but swingin' version of Luther's Master Clock
// Based on Master_Clock::MyPulse.h by L.Geerinckx 28-1-2021 | https://github.com/PierreIsCoding
#include <SoftwareSerial.h>
#define MIDI_RX_PIN A6 // not actually used (should be an interruptable pin in theory)
#define MIDI_TX_PIN 13 // SoftwareSerial output port -- connect to MIDI jack pin 5 via 220 Ohm resistor
#define _BV(bit) (1 << (bit))
class PulseNG
{
private:
    int bufferIndex = 0;
    int clockIndex = 0;
    int clockCount;
    int _clockCount;
    int pulseIndex = 0;
    int stress = 1;
    int pin1;
    int pin2;
    int pin3;
    int pin4;
    float dutyCycle;
    int QLen;
    int ELen;
    int SLen;
    int TLen;
    int SStop1;
    int SStop2;
    int TStop1;
    int TStop2;
    int TStop3;
    int TStop4;
    float swing;
    SoftwareSerial* Midi;


public:
    PulseNG(int, int, int, int, int, float,float);
    int bufferLen;
    int pulseLen;
    void setDutyCycle(float);
    void setSwing(float);
    void next();
    int getCurrent();
    void begin();
    void reset(int);
};

PulseNG::PulseNG(int _pin1, int _pin2, int _pin3, int _pin4, int len, float duty = 0.5, float _swing=0)
{
    //TODO: BEWARE, this only effects the pinMode statements in ::begin,
    //      the pins are hardcoded byte values in ::next().
    pin1 = _pin1;
    pin2 = _pin2;
    pin3 = _pin3;
    pin4 = _pin4;
    bufferLen = len;
    pulseLen=(float)len/2;
    QLen=len;
    ELen=len/2;
    SLen=len/4;
    TLen=len/8;
    dutyCycle = duty;
    swing=_swing;
    clockCount=len/24;
}

void PulseNG::begin()
{
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    pinMode(pin3, OUTPUT);
    pinMode(pin4, OUTPUT);
    _clockCount = clockCount;
    SStop1 = (int)(SLen + (6 * (swing) ) )*dutyCycle;
    SStop2 = (SLen + (6 * (swing) ) ) + (int)(SLen - (6 * (swing) ) )*dutyCycle;
    TStop1 = (int)(TLen + (3 * (swing) ) )*dutyCycle;
    TStop2 = (TLen + (3 * (swing) ) ) + (int)(TLen + (3 * (swing) ) )*dutyCycle;
    TStop3 = (TLen + (3 * (swing) ) ) + (TLen + (3 * (swing) ) ) + (int)(TLen - (3 * (swing) ) )*dutyCycle;
    TStop4 = (TLen + (3 * (swing) ) ) + (TLen + (3 * (swing) ) ) + (TLen - (3 * (swing) ) ) + (int)(TLen - (3 * (swing) ) )*dutyCycle;
    Midi = new SoftwareSerial(MIDI_RX_PIN, MIDI_TX_PIN);
    Midi->begin(31250);
}

void PulseNG::setDutyCycle(float _dutyCycle)
{
    dutyCycle = _dutyCycle;
    SStop1 = (int)(SLen + (6 * (swing) ) )*dutyCycle;
    SStop2 = (SLen + (6 * (swing) ) ) + (int)(SLen - (6 * (swing) ) )*dutyCycle;
    TStop1 = (int)(TLen + (3 * (swing) ) )*dutyCycle;
    TStop2 = (TLen + (3 * (swing) ) ) + (int)(TLen + (3 * (swing) ) )*dutyCycle;
    TStop3 = (TLen + (3 * (swing) ) ) + (TLen + (3 * (swing) ) ) + (int)(TLen - (3 * (swing) ) )*dutyCycle;
    TStop4 = (TLen + (3 * (swing) ) ) + (TLen + (3 * (swing) ) ) + (TLen - (3 * (swing) ) ) + (int)(TLen - (3 * (swing) ) )*dutyCycle;
    reset(false);
}

void PulseNG::setSwing(float _swing)
{
    swing=_swing;
    setDutyCycle(dutyCycle);
}

void PulseNG::next()
{
    // First, determine if we are on a Long or Short stressed pulse as part of Swing
    if (pulseIndex < 6) 
    {
        stress = 1;
        // Assuming a buffer length of 720 in Master_Clock,
        // A pulse is nominally 30 clockindexes long, UNLESS swing is applied.
        // The swing value is ADDED to the Long pulses, 
        // and SUBTRACTED from the shorts.
        _clockCount = clockCount+swing;
    }
    
    if ((pulseIndex>=6) && (pulseIndex<12))
    {
        stress = -1;
        _clockCount = clockCount-swing;
    }
    
    if ((pulseIndex>=12) && (pulseIndex<18))
    {
        stress = 1;
        _clockCount = clockCount+swing;
    }
    
    if ((pulseIndex>=18) && (pulseIndex<24))
    {
        stress = -1;
        _clockCount = clockCount-swing;
    }
    // Send the MIDI frame
    if (clockIndex == 0 )
    {
        Midi->write(0xF8);
    }
    // Quarter Note Start
    if ((clockIndex == 0) && (pulseIndex == 0))
    {
        PORTB |= _BV(1); // TODO This is a hardcoded pin value and should
                         // instead reference the local variables.
    }
    // Quarter Note Stop
    //TODO: Due to rounding errors, this output will only
    //      function when dutyCycle is divisable by 5...
    if (bufferIndex == (int)QLen*dutyCycle)
    {
        PORTB &= ~(_BV(1));
    }
    // Eighth Note Start
    if ( (clockIndex == 0) && ( (pulseIndex == 0) || (pulseIndex == 12) ) )
    {
        PORTB |= _BV(2);
    }
    // Eighth Note Stop
    //TODO: Due to rounding errors, this output will only
    //      function when dutyCycle is divisable by 5...
    if ((bufferIndex == (int)ELen*dutyCycle) || (bufferIndex == ((int)ELen*dutyCycle)+ELen))
    {
        PORTB &= ~(_BV(2));
    }
    // Sixteenth Note Start (4x)
    if ((clockIndex == 0) && ( (pulseIndex == 0) || (pulseIndex == 6) || (pulseIndex == 12) || (pulseIndex == 18) ))
    {
        PORTB |= _BV(3);
    }
    // Sixteenth Note Stop (4x)
    if ((bufferIndex == SStop1) || (bufferIndex == SStop2) || (bufferIndex == SStop1 + ELen) || (bufferIndex == SStop2 + ELen))
    {
        PORTB &= ~(_BV(3));
    }
    // 32nd Note Start (8x)
    if ( (clockIndex == 0) && ( (pulseIndex == 0) || (pulseIndex == 3) || (pulseIndex == 6) || (pulseIndex == 9) || (pulseIndex == 12) || (pulseIndex == 15) || (pulseIndex == 18) || (pulseIndex == 21) ) )
    {
        PORTB |= _BV(4);
    }
    // 32nd Note Stop (8x)
    if ((bufferIndex == TStop1) || (bufferIndex == TStop2) || (bufferIndex == TStop3) || (bufferIndex == TStop4) || (bufferIndex == TStop1 + ELen) || (bufferIndex == TStop2 + ELen) || (bufferIndex == TStop3 + ELen) || (bufferIndex == TStop4 + ELen))
    {
        PORTB &= ~(_BV(4));
    }

    // The clockIndex increments every time the updatePulse timer allows.
    // We START stuff (pulses, midi clock bytes) on clockIndex == 0,
    // which allows the number of clockIndex cycles to control timing.
    // ALL of the STOP events must be calculated as bufferIndex positions,
    // since they are not related to the current active Pulse.
    if (clockIndex < _clockCount - 1)
    {
        clockIndex++;
    }
    else
    {
        clockIndex = 0;
        if (pulseIndex < 23)
        {
            pulseIndex++;
        }
        else
        {
            pulseIndex = 0;
        }
    }
    if (bufferIndex < bufferLen - 1)
    {
        bufferIndex++;
    }
    else
    {
        bufferIndex = 0;
    }
}

int PulseNG::getCurrent()
{
    // Unused?
    return bufferIndex;
}

void PulseNG::reset(int resetValue){
  // Setting the Swing can cause the clockIndex  == 0 event to become
  // unsynchronized with bufferIndex=0. As such, changing either the
  // duty or swing will cause the beat clock to reset.
  bufferIndex = 0;
  pulseIndex = 0;
  clockIndex = 0;
  //digitalWrite(pin1, resetValue);
}
