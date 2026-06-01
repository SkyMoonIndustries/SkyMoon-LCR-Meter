#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define MODE_R 0
#define MODE_C 1
#define MODE_L 2

const int modeButtonPin = 2;
const int measureButtonPin = 3;

const int analogPin = A0;
const int dischargePin = 7;

const float Vcc = 4.72;
const float R_ref = 9800.0;

const float R_charge = 9800.0;
const float threshold = 3.2;

double pulse, frequency, inductance;
const int inductanceChargePin = 13;
const int inductanceMeasurePin = 11;
double capacitance = 200.0E-9;

int mode = MODE_R;
bool lastModeButtonState = HIGH;
bool lastMeasureButtonState = HIGH;

void setup() {
  Serial.begin(115200);
  pinMode(modeButtonPin, INPUT_PULLUP);
  pinMode(measureButtonPin, INPUT_PULLUP);
  pinMode(dischargePin, OUTPUT);
  pinMode(inductanceChargePin, OUTPUT);
  pinMode(inductanceMeasurePin, INPUT);
  digitalWrite(dischargePin, LOW);

  // OLED başlat
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 bulunamadi"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println("SkyMoon Industries");
  display.println("LCR Meter");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  bool currentModeButtonState = digitalRead(modeButtonPin);
  if (lastModeButtonState == HIGH && currentModeButtonState == LOW) {
    mode = (mode + 1) % 3;
    Serial.print("Mod secildi: ");
    if (mode == MODE_R){ 
    Serial.println("Resistance");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Resistance Mode");
    display.display();
    }
    else if (mode == MODE_C){ 
    Serial.println("Capacitance");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Capacitance Mode");
    display.display();
    }
    else { 
    Serial.println("Inductance");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Inductance Mode");
    display.display();
    }
    delay(300);
  }
  lastModeButtonState = currentModeButtonState;

  bool currentMeasureButtonState = digitalRead(measureButtonPin);
  if (lastMeasureButtonState == HIGH && currentMeasureButtonState == LOW) {
    if (mode == MODE_R) measureResistance();
    else if (mode == MODE_C) measureCapacitance();
    else measureInductance();
    delay(300);
  }
  lastMeasureButtonState = currentMeasureButtonState;
}

void measureResistance() {
  pinMode(analogPin, INPUT);
  int adcValue = analogRead(analogPin);
  float Vout = (adcValue / 1023.0) * Vcc;
  float R_unknown = R_ref * (Vout / (Vcc - Vout));

  Serial.println("Direnc olculuyor...");
  Serial.print("Direnc: ");
  Serial.print(R_unknown);
  Serial.println(" ohm");

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Resistance Mode");
  display.print("R = ");
  display.print(R_unknown, 2);
  display.println(" ohm");
  display.display();
}

void measureCapacitance() {
  Serial.println("Kapasitans olculuyor...");

  pinMode(analogPin, OUTPUT);
  digitalWrite(analogPin, LOW);
  digitalWrite(dischargePin, HIGH);
  delay(100);
  digitalWrite(dischargePin, LOW);
  pinMode(analogPin, INPUT);

  unsigned long startTime = micros();
  while (analogRead(analogPin) < (threshold / 5.0 * 1023)) {
    // bekle
  }
  unsigned long elapsedTime = micros() - startTime;

  float capacitance = (float)elapsedTime / (R_charge * 1e6);
  float capacitance_uF = capacitance * 1e6;

  Serial.print("Kapasitans: ");
  Serial.print(capacitance_uF);
  Serial.println(" uF");

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Capacitance Mode");
  display.print("C = ");
  display.print(capacitance_uF, 2);
  display.println(" uF");
  display.display();
}

void measureInductance() {
  Serial.println("Enduktans olculuyor...");
  digitalWrite(inductanceChargePin, HIGH);
  delay(5);
  digitalWrite(inductanceChargePin,LOW);
  delayMicroseconds(100);
  pulse = pulseIn(inductanceMeasurePin,HIGH,5000);

  if(pulse > 0.1){
    frequency = 1.E6/(2*pulse);
    inductance = 1./(capacitance*frequency*frequency*4.*3.14159*3.14159);
    inductance *= 1E6;

    Serial.print("High for uS:");
    Serial.print(pulse);
    Serial.print("\tfrequency Hz:");
    Serial.print(frequency);
    Serial.print("\tinductance uH:");
    Serial.println(inductance);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Inductance Mode");
    display.print("L = ");
    display.print(inductance, 2);
    display.println(" uH");
    display.display();
  } else {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Inductance Mode");
    display.println("Okunamadi");
    display.display();
  }
}
