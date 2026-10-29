#include <Wire.h>
#include <math.h>

#define VIB_PIN 3

float pitchOffset = 0;
float tolerance = 15.0;

float readPitch() {
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);

  Wire.requestFrom(0x68, 6);

  if (Wire.available() >= 6) {
    int16_t ax = (Wire.read() << 8) | Wire.read();
    int16_t ay = (Wire.read() << 8) | Wire.read();
    int16_t az = (Wire.read() << 8) | Wire.read();

    return atan2(
             ax,
             sqrt((long)ay * ay + (long)az * az)
           ) * 180.0 / PI;
  }

  return 0;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  pinMode(VIB_PIN, OUTPUT);
  digitalWrite(VIB_PIN, LOW);

  // Wake up MPU6050
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission();

  delay(2000);

  Serial.println("Kalibrasi...");
  pitchOffset = readPitch();

  Serial.print("Posisi tegak = ");
  Serial.println(pitchOffset);

  Serial.println("Monitoring posture...");
}

void loop() {

  float pitch = readPitch();
  float correctedPitch = pitch - pitchOffset;

  Serial.print("Pitch : ");
  Serial.println(correctedPitch);

  if (abs(correctedPitch) > tolerance) {

    Serial.println("POSTURE WARNING");

    digitalWrite(VIB_PIN, HIGH);
    delay(300);

    digitalWrite(VIB_PIN, LOW);
    delay(300);
  }
  else {

    digitalWrite(VIB_PIN, LOW);
    delay(100);
  }
}
