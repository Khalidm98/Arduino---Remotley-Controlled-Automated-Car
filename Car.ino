// Ultrasonic
#include <NewPing.h>          // Header library for ultrasonic sensor
NewPing ultra(13, 12, 20);    // (TriggPin, EchoPin, MaxDistance)
unsigned int U;

// Motor Driver
#define ENB 6                 // right speed
#define RF 7                  // right forward
#define RB 8                  // right backward
#define LF 9                  // left forward
#define LB 10                 // left backward
#define ENA 11                // left speed
#define Speed 120             // PWM (MAX = 255)

// Mode
#define MODE 2                // Bluetooth (HIGH) - InfraRed (LOW)

// Bluetooth
char blue;                    // Bluetooth Command

// Infrared
#define IRL 3                 // left sensor
#define IRM 4                 // middle sensor
#define IRR 5                 // right sensor
#define W 1                   // white
#define B 0                   // black
int SL;                       // left sensor reading
int SM;                       // middle sensor reading
int SR;                       // right sensor reading
char lastState = 'F';         // froward(F), right(R), left(L)
char obstacle;                // infrared obstacle control


// Movement Functions
void forward() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(RF, HIGH);
  digitalWrite(RB, LOW);
  digitalWrite(LF, HIGH);
  digitalWrite(LB, LOW);
}

void backward() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(RF, LOW);
  digitalWrite(RB, HIGH);
  digitalWrite(LF, LOW);
  digitalWrite(LB, HIGH);
}

void right() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(RF, LOW);
  digitalWrite(RB, HIGH);
  digitalWrite(LF, HIGH);
  digitalWrite(LB, LOW);
}

void left() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(RF, HIGH);
  digitalWrite(RB, LOW);
  digitalWrite(LF, LOW);
  digitalWrite(LB, HIGH);
}

void stop_motor() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(RF, LOW);
  digitalWrite(RB, LOW);
  digitalWrite(LF, LOW);
  digitalWrite(LB, LOW);
}


void setup() {
  pinMode(RF, OUTPUT);
  pinMode(RB, OUTPUT);
  pinMode(LF, OUTPUT);
  pinMode(LB, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(IRL, INPUT);
  pinMode(IRM, INPUT);
  pinMode(IRR, INPUT);

  pinMode(MODE, INPUT);
  Serial.begin(9600);
}

void loop() {
  // Ultrasonic
  U = ultra.ping_cm();                // Compare measured distance with max distance

  // Bluetooth
  if (digitalRead(MODE) == 1) {
    while (Serial.available()) {
      blue = Serial.read();
      if (U != 0) {
        if (blue == 'B')              backward();
        else                          stop_motor();
      }

      else {
        if (blue == 'S')              stop_motor();
        else if (blue == 'B')         backward();
        else if (blue == 'F')         forward();
        else if (blue == 'R')         right();
        else if (blue == 'L')         left();
      }
    }
  }

  // InfraRed
  else {
    SL = digitalRead(IRL);
    SM = digitalRead(IRM);
    SR = digitalRead(IRR);
    
    if (U != 0) {
      if (obstacle == 'R') {
        right();
        delay(1500);
        lastState = 'F';
      }

      else if (obstacle == 'L') {
        left();
        delay(1500);
        lastState = 'F';
      }
    }

    else {
      if ((SL == W && SM == B && SR == W) || (SL == B && SM == B && SR == B)) {
        forward();
        lastState = 'F';
      }

      else if ((SL == W && SM == W && SR == B) || (SL == W && SM == B && SR == B)) {
        right();
        lastState = 'R';
        obstacle = 'R';
      }

      else if ((SL == B && SM == W && SR == W) || (SL == B && SM == B && SR == W)) {
        left();
        lastState = 'L';
        obstacle = 'L';
      }

      else if (SL == W && SM == W && SR == W) {
        if (lastState == 'F')                 forward();
        else if (lastState == 'R')            right();
        else if (lastState == 'L')            left();
      }
    }
  }
}
