// Code by L.Geerinckx 28-1-2021 | https://github.com/PierreIsCoding
// Modified by Warpeggio
#include <SoftwareSerial.h>
#define MIDI_RX_PIN A6 // not actually used (should be an interruptable pin in theory)
#define MIDI_TX_PIN 13 // SoftwareSerial output port -- connect to MIDI jack pin 5 via 220 Ohm resistor
// more details on MIDI jack wiring: https://www.arduino.cc/en/uploads/Tutorial/MIDI_bb.png

class MyMidi
{
private:
    int bufferIndex = 0;
    int pin;
    void makePulse();
    void addPulse(int);
    float dutyCycle;
    int *pulseBuffer;
    float swing;
    SoftwareSerial* Midi;

public:
    MyMidi(int, int, float,float);
    int bufferLen;
    int pulseLen;
    void setDutyCycle(float);
    void setSwing(float);
    void next();
    int getCurrent();
    void begin();
    void reset(int);
};

MyMidi::MyMidi(int _pin, int len, float duty = 0.5, float _swing=1)
{
    pin = _pin;
    bufferLen = len;
    pulseLen=1;
    pulseBuffer = new int[bufferLen];
    dutyCycle = duty;
    swing=_swing;
}

void MyMidi::begin()
{
    Midi = new SoftwareSerial(MIDI_RX_PIN, MIDI_TX_PIN);
    Midi->begin(31250);
    makePulse();
}

void MyMidi::setDutyCycle(float _dutyCycle)
{
    dutyCycle = _dutyCycle;
    makePulse();
}

void MyMidi::setSwing(float _swing)
{
    swing=_swing;
    makePulse();
}

void MyMidi::makePulse()
{
    for (int i = 0; i < bufferLen ; i++)
    {
        pulseBuffer[i] = 0;
    }
    addPulse(0);
    int temp = (float)pulseLen*swing;
    addPulse(temp);
}

void MyMidi::addPulse(int offset)
{
    for (int i = offset; i < offset + pulseLen; i++)
    {
        pulseBuffer[i] = 1;
    }

}

void MyMidi::next()
{
    if (bufferIndex < bufferLen - 1)
    {
        bufferIndex++;
    }
    else
    {
        bufferIndex = 0;
    }
    if (pulseBuffer[bufferIndex]) {
        Midi->write(0xF8);
    }
    
}

int MyMidi::getCurrent()
{
    return pulseBuffer[bufferIndex];
}

void MyMidi::reset(int resetValue){
  bufferIndex = 0;
  digitalWrite(pin, resetValue);
}
