#include  "arduinoFFT.h"
 
#define SAMPLES 128             //SAMPLES-pt FFT. Must  be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 2048  //Ts = Based on Nyquist, must be 2 times the highest expected frequency.

int soundSensorPin = A0;

#define REDPIN 5
#define GREENPIN 6
#define BLUEPIN 3

int redValue = 0;
int greenValue = 0;
int blueValue = 0;

bool on = true;

 
arduinoFFT FFT = arduinoFFT();
 
unsigned int samplingPeriod;
unsigned long microSeconds;
  
double vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
double  vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values


// Czujnik podczerwieni
#include <IRremote.hpp>
#define IR_RECEIVE_PIN 2

unsigned long keyValue = 0;


void  setup() 
{
    Serial.begin(115200);
    samplingPeriod = round(1000000*(1.0/SAMPLING_FREQUENCY)); //Period in microseconds  
    
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
}
 
void loop() 
{  

    if (keyValue == 0xBE41FF00){
      
      /*Sample SAMPLES times*/
      for(int  i=0; i<SAMPLES; i++)
      {
          microSeconds = micros();    //Returns the  number of microseconds since the Arduino board began running the current script.  
      
          vReal[i] = analogRead(0); //Reads the value from analog pin  0 (A0), quantize it and save it as a real term.
          vImag[i] = 0; //Makes  imaginary term 0 always

          /*remaining wait time between samples if  necessary*/
          while(micros() < (microSeconds + samplingPeriod))
          {
            //do nothing
          }
      }
  
      /*Perform FFT on samples*/
      FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
      FFT.Compute(vReal,  vImag, SAMPLES, FFT_FORWARD);
      FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

      /*Find peak frequency and print peak*/
      double peak = FFT.MajorPeak(vReal,  SAMPLES, SAMPLING_FREQUENCY);
      Serial.println(peak);     //Print out the most  dominant frequency.

      // Map the dB values to a color range from green to red
      int rValue = map(peak, 50, 1024, 255, 0);
      int gValue = map(peak, 50, 1024, 0, 255);

      analogWrite(REDPIN, rValue);
      analogWrite(GREENPIN, gValue);
      analogWrite(BLUEPIN, 0); // Set blue to 0 for simplicity (you can adjust as needed)

      delay(100);
    }
    
    if (IrReceiver.decode()) {
      keyValue = IrReceiver.decodedIRData.decodedRawData;
      Serial.println(keyValue, HEX); // Print "old" raw data
      switch(keyValue){
        case 0xA35CFF00:
          Serial.println("OP+");
          if (redValue <= 205 && greenValue == 0 && blueValue == 0){
            redValue += 50;
          }
          else if (redValue == 0 && greenValue <= 205 && blueValue == 0){
            greenValue += 50;
          }
          else if (redValue == 0 && greenValue == 0 && blueValue <= 205){
            blueValue += 50;
          }
          else if (redValue <= 205 && redValue == greenValue){
            redValue +=50;
            greenValue += 50;
            //blueValue += 50;
          }
          Serial.println(redValue);
          Serial.println(greenValue);
          Serial.println(blueValue);
          analogWrite(REDPIN, redValue);
          analogWrite(GREENPIN, greenValue);
          analogWrite(BLUEPIN, blueValue);
          break;
        case 0xA25DFF00:
          Serial.println("OP-");
          if (redValue > 50 && greenValue == 0 && blueValue == 0){
            redValue -= 50;
          }
          else if (redValue == 0 && greenValue > 50 && blueValue == 0){
            greenValue -= 50;
          }
          else if (redValue == 0 && greenValue == 0 && blueValue > 50){
            blueValue -= 50;
          }
          else if (redValue > 50 && redValue == greenValue){
            redValue -=50;
            greenValue -= 50;
            //blueValue -= 50;
          }
          Serial.println(redValue);
          Serial.println(greenValue);
          Serial.println(blueValue);
          analogWrite(REDPIN, redValue);
          analogWrite(GREENPIN, greenValue);
          analogWrite(BLUEPIN, blueValue);
          break;
        case 0xBE41FF00:
          Serial.println("PLAY");
          break;
        case 0xBF40FF00:
          Serial.println("ON/OFF");
          if (on == true){
            on = false;
            redValue = 0;
            greenValue = 0;
            blueValue = 0;
          } 
          else{
            on = true;
            redValue = 255;
            greenValue = 255;
            blueValue = 255;
          }
            analogWrite(REDPIN, redValue);
            analogWrite(GREENPIN, greenValue);
            analogWrite(BLUEPIN, blueValue);
          break;
        case 0xA758FF00:
          Serial.println("R");
          redValue = 255;
          greenValue= 0;
          blueValue = 0;
          analogWrite(REDPIN, redValue);
          analogWrite(GREENPIN, greenValue);
          analogWrite(BLUEPIN, blueValue);
          break;
        case 0xA659FF00:
          Serial.println("G");
          redValue = 0;
          greenValue= 255;
          blueValue = 0;
          analogWrite(REDPIN, redValue);
          analogWrite(GREENPIN, greenValue);
          analogWrite(BLUEPIN, blueValue);
          break;
        case 0xBA45FF00:
          Serial.println("B");
          redValue = 0;
          greenValue= 0;
          blueValue = 255;
          analogWrite(REDPIN, redValue);
          analogWrite(GREENPIN, greenValue);
          analogWrite(BLUEPIN, blueValue);
          break;
        case 0xBB44FF00:
          Serial.println("W");
          redValue = 255;
          greenValue= 255;
          blueValue = 255;
          analogWrite(REDPIN, 255);
          analogWrite(GREENPIN, 255);
          analogWrite(BLUEPIN, 255);
          break;
      }
      
      IrReceiver.resume(); // Enable receiving of the next value
    }
}


