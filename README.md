# MasterClockNG for Synth DIY
Hi, i'm Warpeggio and i modified some code from @PierreIsCoding over at https://github.com/PierreIsCoding/sdiy. Most of this readme is from there as well :)
Master Clock is an Arduino Nano based Eurorack clock module. It is a fairly simple but usefull module for any DIY synthesizer enthusiast.
## Features
- 3 digit segment display
- 1 main output pulse and 3 multiplied clocked output pulses
  - First: the main "Beats Per Minute" tempo is displayed by the 3 segment display
  - Second: 2 times faster
  - Third: 4 times faster
  - Fourth: 8 times faster
- A rotary encoder to change the speed. By using the encoder push button the master clock toggles between fine or course increments
- Four utility buttons:
  - Start and stop the clock
  - Tap tempo. The clock will estimate your tapped speed
  - Duty Cycle. Press the Duty Cycle button and change the duty cycle of all of the output pulses
  - Swing. Change the time between pulses to get a more groovy feeling
## Files
The following files are included:
- PDF of schematic
- PDF of perfboard helper. It is not complete, but this will help you
- Arduino Files
- 3D print .STL files. I 3D print all my panels.
## Bugs & Fixes
Please be informed that @PierreIsCoding and @Warpeggio, although having a technical background, are hobbyists. We build and share this project for fun and learning, so please let us know if you have any good improvements in mind! Warpeggio had a blast playing with this code and hopes you do too. :)

New Feature: MIDI clock output on pin D13. Wire this to a MIDI jack in the standard way with a 220-ohm resistor.

Fixed: Swing sounds pretty good!

New bug: The frequency output is slightly slower than the displayed BPM. The quantization seems to indicate rounding errors, and is probably deeply related to Warpeggios beat subdivision scheme.

New bug: not all duty cycles are functioning for Quarter or Eighth notes, related again to rounding errors.

  
  
