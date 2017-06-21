#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

//Pitch and in Tune Libraries-----------------------------------------------------------------
#include "pitch_utils.h"
#include "note_name.h"
#include "tuned_note.h"
//--------------------------------------------------------------------------------------
#define I0(A0);

//Define Variables
AudioAnalyzeNoteFrequency notefreq;
AudioInputAnalog          adc1(A2);
AudioMixer4               mixer;
AudioOutputUSB            usb1;

//---------------------------------------------------------------------------------------
AudioConnection patchCord0(adc1, 0, notefreq, 0);
AudioConnection patchCord1(adc1, 0, usb1, 0 );

int channel = 1; // Defines the MIDI channel to send messages on (values from 1-16)
int velocity = 100; // Defines the velocity that the note plays at (values from 0-127)
int noteValue;  // Defines what note value to send to the midi channel

//---------------------------------------------------------------------------------------

// Global access
std::array<float, 88> pitch_freqs;
std::array<note_name *, 88> pitch_names;
//


void setup() {
  // put your setup code here, to run once:
 AudioMemory(30);
 notefreq.begin(.15);
 
 //usbMIDI.begin(1);

 // Pitch detection: gets pitch frequencies and outputs the
 // the range of freqnuencies closes to that of the 88
 // keys on a keyboard
  float reference = 440.0;
  pitch_freqs = get_pitch_freqs(reference);
  pitch_names = get_pitch_names();
 
}

void loop() {
  //Add Delay to not get a million values
  delay(100);
  
  // put your main code here, to run repeatedly:
   if (notefreq.available()) {
     float note = notefreq.read();
     float prob = notefreq.probability();

     noteValue = 69 + 12*log(note/440);

     //noteValue = 5;
 

      tuned_note n = freq_to_note(note, pitch_freqs);

  //get the pitch value   
    int index = n.getPitch();
  //see how far off pitch the value is
    double distance = n.getDistance();

  // get the closest note value
    note_name closest_note = *pitch_names[index];

  // Send correct note to through the midi channel
  usbMIDI.sendNoteOn(noteValue,velocity,channel);   // Turn the note ON
  //usbMIDI.sendNoteOff(noteValue,velocity,channel);  // Turn the note OFF


   //Check if note is valid
   
   Serial.printf("Note: %3.2f | Probability: %.2f \n", note, prob);
   Serial.printf("Closest Midi Note: %i\n",noteValue);
   Serial.printf("closest tuned_note %c , %c\n", closest_note.getName(), closest_note.getModifier());
 }
 
}
