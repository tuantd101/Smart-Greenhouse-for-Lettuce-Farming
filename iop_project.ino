/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL63lb6agxb"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "2PgUZ3G_ehbj96IyesptKLeFDuMX4oXm"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "DHTesp.h"

BlynkTimer timer;
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "V2027";
char pass[] = "1234567890";

DHTesp dht;
float temp, humi;
int number = 0;
int valueSoilMoi, percent;
bool autoMode = false;
boolean online = true;
BLYNK_WRITE(V0) {       //auto mode
  int buttonValue = param.asInt();
  if (buttonValue == 1) {
    // Chế độ tu dong
    autoMode = true;
  } else {
    // Chế độ thu cong
    autoMode = false;
  }
}
BLYNK_WRITE(V7) {     //Tuoi
  if (!autoMode) {
    int toggleState_0 = param.asInt();
    if (toggleState_0 == 0) {
      digitalWrite(D1, LOW);
    } else {
      digitalWrite(D1 , HIGH);
    }
  }
}
BLYNK_WRITE(V1) {     //Quat
  if (!autoMode) {
    int toggleState_1 = param.asInt();
    if (toggleState_1 == 0) {
      digitalWrite(D2, LOW);
    } else {
      digitalWrite(D2 , HIGH);
    }
  }
}
BLYNK_WRITE(V2) {    // LED
  if (!autoMode) {
    int toggleState_2 = param.asInt();
    if (toggleState_2 == 0) {
      digitalWrite(D7, LOW);
    } else {
      digitalWrite(D7 , HIGH);
    }
  }
}
BLYNK_WRITE(V4) {    //Phun suong
  if (!autoMode) {
    int toggleState_3 = param.asInt();
    if (toggleState_3 == 0) {
      digitalWrite(D0, LOW);
    } else {
      digitalWrite(D0 , HIGH);
    }
  }
}
void syncDataSet() {
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V7);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V4);
}
void setup()
{
  // Setup relay mode
  pinMode(D1, OUTPUT);
  pinMode(D0, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D7, OUTPUT);
  // Set up dht11
  dht.setup(D6, DHTesp::DHT11);
  // Set up manual mode at start
  autoMode = false;
  // Turn off all relay
  digitalWrite(D1, LOW);
  digitalWrite(D0, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D7, LOW);
  // Debug console
  Serial.begin(115200);
  // Connect controller to Wifi and Blynk server
  WiFi.begin(ssid, pass);
  Blynk.config(BLYNK_AUTH_TOKEN);
  timer.setInterval(3000L, syncDataSet);
  timer.setInterval(5000, getTempAndHumi);
  timer.setInterval(5000, getSoilMois);
}

void getTempAndHumi() {
  temp = dht.getTemperature();
  humi = dht.getHumidity();
  Serial.println(temp);
  Serial.println(humi);
}
void getSoilMois() {
  int valueSoilMoi = analogRead(A0);
  percent = 100 - map(valueSoilMoi, 0, 1023, 0, 100);
  Serial.println(percent);
}
void autoModeControllByTemp(float x, int y) {
  if (x < 28.0) {
    if (digitalRead(D0) == LOW) {
      digitalWrite(D0, HIGH);
    }
    if (digitalRead(D2) == LOW) {
      digitalWrite(D2, HIGH);
    }
  } else {
    if (digitalRead(D0) == HIGH) {
      digitalWrite(D0, LOW);
    }
    if (digitalRead(D2) == HIGH) {
      digitalWrite(D2, LOW);
    }
  }

  if (y >= 55) {
    if (digitalRead(D1) == LOW) {
      digitalWrite(D1, HIGH);
    }
  } else {
    if (y != 0) {
      if (digitalRead(D1) == HIGH) {
        digitalWrite(D1, LOW);
      }
    }
  }
}
void checkOnOrOff() {
  if (WiFi.status() != WL_CONNECTED || !Blynk.connected()) {
    Serial.println("Lost connection. Switching to offline mode.");
    online = false;
  } else {
    online = true;
  }
}
void loop() {
  Blynk.run();
  timer.run();

  // up data to Blynk
  Blynk.virtualWrite(V5, temp);
  Blynk.virtualWrite(V6, humi);
  Blynk.virtualWrite(V8, percent);

  // check wifi status
  checkOnOrOff();

  // Set mode auto for device
  if (!online) {
    autoModeControllByTemp(temp, percent);
  }
  else if (online && autoMode) {
    autoModeControllByTemp(temp, percent);
  }
}
