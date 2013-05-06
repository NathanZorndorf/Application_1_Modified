# Application_1_Modified README

This project uses the C5535 eZdsp from Spectrum Digital, and Code Composer Studio Version 4.2.4.00033. 
The purpose of this project is to take in an audio signal of a monophonic audio signal, perform an FFT, 
do a peak search in order to find the fundamental frequency in windows of around 30 miliseconds - in real time. This fundamental frequency
is then turned into a MIDI number, and the MIDI ON/OFF messages that correspond to the frequency found in the FFT are output
serially via UART. 

Essentially, the project is an automatic transcriber for monophonic instruments. 






