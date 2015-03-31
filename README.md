# arduino-bezier
Translated C# Bezier code into C++ for Arduino to smooth throttle control of differential robot steering system

## Caveat

This initial commit was just a cut-n-paste from Xcode, so it hasn't been tested outside of an Arduino Mega 2560 board environment. If you need a generic Bezier class in C++ or a more polished one for your Arduino project, this one needs to be cleaned up, as it was an experiment.

For instance, the BezierThrottleControl class AND the main Arduino loop function are all in the same file. That's silly, but that's how I test concepts, sometimes. Ideally, that class has a proper .cpp and .h file in your project.

## Adaptation

This code was adapted from code found here:

http://www.codeproject.com/Articles/25237/Bezier-Curves-Made-Simple

