#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------------- OLED Setup ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------- Pin Definitions ----------------
#define RED_LED     23
#define GREEN_LED   25
#define BUZZER      4

#define PIR_PIN     19

#define TRIG_PIN    5
#define ECHO_PIN    18

#define MQ2_PIN     34

// ---------------- Thresholds ----------------
#define DIST_THRESHOLD 100
#define SMOKE_THRESHOLD 1800

long duration;
int distance;
int smokeLevel;
bool motionDetected;

// ---------------- Functions ----------------

int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

void showDisplay(String status) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);
  display.println("Smart Home Shield");

  display.setCursor(0, 18);
  display.print("Distance : ");
  display.print(distance);
  display.println(" cm");

  display.setCursor(0, 32);
  display.print("Smoke    : ");
  display.println(smokeLevel);

  display.setCursor(0, 50);
  display.print("Status   : ");
  display.println(status);

  display.display();
}

void beep(int delayTime) {
  digitalWrite(BUZZER, HIGH);
  delay(delayTime);
  digitalWrite(BUZZER, LOW);
  delay(delayTime);
}

// ---------------- Setup ----------------
void setup() {
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  pinMode(PIR_PIN, INPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while (true);
  }

  display.clearDisplay();
  display.setCursor(10, 20);
  display.println("Initializing...");
  display.display();
  delay(1500);
}

// ---------------- Loop ----------------
void loop() {

  motionDetected = digitalRead(PIR_PIN);
  distance = getDistance();
  smokeLevel = analogRead(MQ2_PIN);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Smoke: ");
  Serial.println(smokeLevel);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BUZZER, LOW);

  // ---- Smoke Priority ----
  if (smokeLevel > SMOKE_THRESHOLD) {

    digitalWrite(RED_LED, HIGH);
    showDisplay("SMOKE ALERT");

    for (int i = 0; i < 2; i++) beep(100);
  }

  // ---- Intrusion ----
  else if (distance <= DIST_THRESHOLD) {

    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER, HIGH);

    showDisplay("INTRUSION");
  }

  // ---- Motion ----
  else if (motionDetected) {

    digitalWrite(RED_LED, HIGH);
    showDisplay("MOTION");

    beep(200);
  }

  // ---- Safe ----
  else {

    digitalWrite(GREEN_LED, HIGH);
    showDisplay("SAFE");
  }

  delay(300);
}
