# Application_1_Modified README

This project uses the C5535 eZdsp from Spectrum Digital, and Code Composer Studio Version 4.2.4.00033. 
The purpose of this project is to take in an audio signal of a monophonic audio signal, perform an FFT, 
do a peak search in order to find the fundamental frequency in windows of around 30 miliseconds - in real time. This fundamental frequency
is then turned into a MIDI number, and the MIDI ON/OFF messages that correspond to the frequency found in the FFT are output
serially via UART. 

Essentially, the project is an automatic transcriber for monophonic instruments. 

LICENSE:
--------
Permission to use, copy, modify, and/or distribute this software for any 
purpose with or without fee is hereby granted, provided that the above 
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES 
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY 
SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER 
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE 
USE OR PERFORMANCE OF THIS SOFTWARE.





