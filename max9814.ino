#include <SPI.h>
#include <Wire.h>

const int sampleWindow = 250; // Sample window width in mS
unsigned int sample;


void setup(){
 Serial.begin(9600);
}

void loop(){
  
 unsigned long startMillis= millis(); // Start of sample window
 unsigned int peakToPeak = 0; // peak-to-peak level
 unsigned int signalMax = 0;
 unsigned int signalMin = 1024;
 
 // collect data for 250 mS
 while (millis() - startMillis < sampleWindow){
  sample = analogRead(0);
   if (sample < 1024){ // toss out spurious readings
    if (sample > signalMax){
      signalMax = sample; // save just the max levels
    }else if (sample < signalMin){
      signalMin = sample; // save just the min levels
    }
   }
 }
 
 peakToPeak = signalMax - signalMin; // max - min = peak-peak amplitude
 double volts = (peakToPeak * 10.0) / 1024; // convert to volts
 double dBW = 85;
 Serial.println(volts);

 if(volts<2.3){
    dBW = 20*log10(volts*100)+18.5;
    Serial.print(String(volts)+", medio alto, ");
    Serial.println(String(dBW)+"dB");
  
 }else if (volts<2.55){
    dBW = 20*log10(volts*200)+13;
    Serial.print(String(volts)+", alto, ");
    Serial.println(String(dBW)+"dB");
    
 }else if(volts<3.2){
    dBW = 20.0*log10(volts*200)+15;
    Serial.print(String(volts)+", muy alto, ");
    Serial.println(String(dBW)+"dB");
    
 }else if(volts<4.2){
    dBW = 20.0*log10(volts*200)+20;
    Serial.print(String(volts)+", muy muy alto, ");
    Serial.println(String(dBW)+"dB");
 }
 
}
