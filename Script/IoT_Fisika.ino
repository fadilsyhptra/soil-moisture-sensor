#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6AiDdljwc"
#define BLYNK_TEMPLATE_NAME "Soil Moisture SMANJA"
#define BLYNK_AUTH_TOKEN "fmPeXOVm-X03YWoqusE06IgzC-midy-a"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Blynk.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "SISTEL";
char pass[] = "adnifirda";

BlynkTimer timer;

#define buzzer D0
#define redLight D1
#define yellowLight D2
#define greenLight D3
#define soilSensor A0

bool isActive = false;
bool isGreen = false;
bool isYellow = false;
bool isRed = false;
bool isNotifTinggi = false;
bool isNotifRendah = false;

unsigned long delayLoop;
unsigned long delayBuzzerOn;
unsigned long delayBuzzerOff;

bool waitMillis(unsigned long &prev, unsigned long interval){
  unsigned long start = millis();
  if(start - prev >= interval){
    prev = start;
    return true;
  }
  return false;
}

void sendSoil(){
  int rawValue = analogRead(soilSensor);
  int soilPercent = map(rawValue, 720, 360, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  Blynk.virtualWrite(V0, soilPercent);

  String kondisi;

  if (soilPercent < 60) {
    isRed = true;
    isYellow = false;
    isGreen = false;
    kondisi = "KERING";

    if(!isNotifRendah){
      Blynk.logEvent("sangat_kering",
      "🚨 CRITIAL! SIRAM TANAMANU SEKARANG! Tingkat kelembapan: " + String(soilPercent) + "%");
      isNotifRendah = true;
    }

    isNotifTinggi = false;
  } else if (soilPercent <= 80) {
    isRed = false;
    isYellow = false;
    isGreen = true;
    kondisi = "NORMAL";
  } else {
    isRed = false;
    isYellow = true;
    isGreen = false;
    kondisi = "TERLALU BASAH";

    if(!isNotifTinggi){
      Blynk.logEvent("sangat_basah",
      "WARNING! Tanamanmu terlalu basah, jangan disiram lagi! Tingkat kelembapan: " + String(soilPercent) + "%");
      isNotifTinggi = true;
    }

    isNotifRendah = false;
  }

  Blynk.virtualWrite(V1, kondisi);
}


void setup() {
  pinMode(buzzer, OUTPUT);
  pinMode(redLight, OUTPUT);
  pinMode(yellowLight, OUTPUT);
  pinMode(greenLight, OUTPUT);

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000, sendSoil);
}

void loop() {
  if(waitMillis(delayLoop, 500)){
    settingBuzzer();
    Blynk.run();
    timer.run();
    settingLED();
  }
}

void settingBuzzer(){
  if(isActive) {
    digitalWrite(buzzer, HIGH);
    if(waitMillis(delayBuzzerOff, 1000)){
      digitalWrite(buzzer, LOW);
    }

    if(waitMillis(delayBuzzerOff, 1000)){
      digitalWrite(buzzer, HIGH);
    }
  }
  
  if(!isActive) digitalWrite(buzzer, LOW);
}

void settingLED(){
  if(isGreen){
    isActive = false;
    digitalWrite(redLight, LOW);
    digitalWrite(yellowLight, LOW);
    digitalWrite(greenLight, HIGH);
  }
  
  if(isYellow){
    isActive = false;
    digitalWrite(redLight, LOW);
    digitalWrite(yellowLight, HIGH);
    digitalWrite(greenLight, LOW);
  }

  if(isRed){
    isActive = true;
    digitalWrite(redLight, HIGH);
    digitalWrite(yellowLight, LOW);
    digitalWrite(greenLight, LOW);
  }
}

