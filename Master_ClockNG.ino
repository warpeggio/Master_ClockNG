// Master_ClockNG: A poorly quantized, but swingin' version of Luther's Master Clock 
// Warpeggio takes responsibility for this mess. The displayed BPM doesn't always match the frequency.
// Based on Master_Clock by L.Geerinckx 28-1-2021 | https://github.com/PierreIsCoding

#include "MyTimer.h"
#include "MyRotary.h"
#include "MyButtons.h"
#include "MyScroll.h"
#include "MyShiftDisplay.h"
//#include "MyPulse.h"
#include "PulseNG.h"
#include "MeasureTime.h"

MyShiftDisplay myDisplay;

PulseNG pulse_1(9, 10, 11, 12, 720); // (1x, 2x, 4x, 8x output pins, buffer length)

MyTimer updatePulse(1, pulse_1.bufferLen); // Frequency, Scalar, where Scalar can be thought of
                                           // as subdivisions within the beat frequency.
MyTimer updateDisplay(150); // hz

MeasureTime tapTimer(6);

// ---------------------------- init  Scroll Functions ----------------------------
int scrollIndex = 0;
MyRotary myRotary(A3, A2);

void tempoHandler(int index)
{
    updatePulse.setFreq((float)index / 60);
}

MyScroll tempoScroll(701, tempoHandler, 1, 60); // max value, handler,min value=0, start=0, loop=0

void dutyHandler(int index)
{
    pulse_1.setDutyCycle((float)index / 100);
}
MyScroll dutyCycleScroll(96, dutyHandler, 5, 50);

void swingHandler(int index)
{
    pulse_1.setSwing(index); // The number on the display shows
                             // how many clockIndex cycles will be added or removed
                             // At bufferLength==720, this works well.
    //pulse_1.setSwing(1 - (float)index / 100);
}
MyScroll swingScroll(16, swingHandler);

MyScroll *allScrolls[3] = {&tempoScroll, &dutyCycleScroll, &swingScroll};

// ---------------------------- init  buttons ----------------------------

MyButtons pushButtons((int[5]){A1, 6, A4, A5, A0}, 5); // dutycycle, swing, rotary push, start/stop , tap-tempo

void checkButtons()
{
    if (pushButtons.on())
    {
        if (pushButtons.checkButton(2)) // Rotary Push
        {
            allScrolls[0]->toggleIncrement();
            allScrolls[1]->toggleIncrement();
            allScrolls[2]->toggleIncrement();
        }
        else if (pushButtons.checkButton(0)) // duty
        {
            scrollIndex = 1;
            myDisplay.setValue(allScrolls[1]->getIndex());
        }
        else if (pushButtons.checkButton(1)) // swing
        {
            scrollIndex = 2;
            myDisplay.setValue(allScrolls[2]->getIndex());
        }
        else if (pushButtons.checkButton(3)) // start/stop
        {
            updatePulse.toggleRun();
            if (!updatePulse.getState())
            {
                pulse_1.reset(false);
            }
        }
        else if (pushButtons.checkButton(4)) // tap tempo
        {
            //            Serial.println("tap");
            tapTimer.addSample();
            int newBPM = tapTimer.getTiming();
            if (newBPM != -1)
            {
                updatePulse.setFreq((float)newBPM / 60);
                pulse_1.reset(false);
                allScrolls[0]->setIndex(newBPM);
            }
        }
        else
        {
            scrollIndex = 0; // tempo
            myDisplay.setValue(allScrolls[0]->getIndex());
        }
    }

    if (myRotary.on())
    {
        if (myRotary.checkDirection() == 1)
        {
            allScrolls[scrollIndex]->UP();
        }
        else if (myRotary.checkDirection() == -1)
        {
            allScrolls[scrollIndex]->DOWN();
        }
        myDisplay.setValue(allScrolls[scrollIndex]->getIndex());
    }
}

// ---------------------------- SETUP & LOOP ----------------------------

void setup()
{
    Serial.begin(9600);
    Serial.println("start");
    myDisplay.begin();
    pulse_1.begin();
    myRotary.begin();
    pushButtons.begin();
    updatePulse.setFreq(1);
}

void loop()
{

    if (updateDisplay.onTime())
    {
        myDisplay.updateNext();
    }

    if (updatePulse.onTime())
    {
        pulse_1.next();
    }

    checkButtons();
}
