int Led = 13;
int ledPin = 12;
int buttonPin = 7;
int soundSensorPin = A0;  // Sound sensor connected to analog pin A0
int sensorValue = 0;
float voltage;
float dB;


#define REDPIN 5
#define GREENPIN 6
#define BLUEPIN 3
 
#define FADESPEED 20

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(soundSensorPin, INPUT);

  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);

}

void loop() {

  int g = 100;

  sensorValue = analogRead(soundSensorPin);
  voltage = sensorValue * (5.0 / 1023.0); // Convert analog value to voltage (assuming 5V Arduino)

  // Your calibration and dB calculation logic goes here based on your sensor's specifications
  // This formula is hypothetical and requires proper calibration
  // Replace V_REF and dB_SPL_REF with your sensor's actual reference values
  float V_REF = 5.0; // Replace this with your sensor's reference voltage
  float dB_SPL_REF = 94.0; // Replace this with the reference dB value at V_REF

  // Calculate dB value using voltage ratio
  float ratio = voltage / V_REF;
  dB = dB_SPL_REF + 20 * log10(ratio);

  Serial.print("dB Value: ");
  Serial.println(dB);

  // Control LED based on the dB value
  if (dB > 63) {
    digitalWrite(ledPin, HIGH);
    analogWrite(GREENPIN, 255);
  } else {
    digitalWrite(ledPin, LOW);
    analogWrite(GREENPIN, 0);
  }

  delay(50); // Adjust delay as needed

}
