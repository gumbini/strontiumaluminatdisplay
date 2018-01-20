// Strontiumaluminat-Display

// (c) 2017 - 2018 Martin Weinzierl und Raphael Schuster
// für Jugend Forscht 2018


// [RGB]   TIME - Description

// [RXX]   2000 - Character overflow (too many characters for the display to show)
// [RXX]  FLASH - Processing servo test
// [XGX]   1000 - Function test complete
// [XGX]   3000 - Text completely printed
// [XXB] delay6 - Waiting for input to print (only at startup)
// [XXB]  BLINK - Printing

// ################################################################# Program #################################################################
#include <Servo.h>
Servo servo;

#define red 10
#define green 11
#define blue 12
#define waitpin 13

bool stoploop = false;
char char1;
int angle = 0;
String string1;


// Changeable parameters

unsigned int angle1 = 5; // Distance between rows
unsigned int angle2 = 12; // Distance between characters
unsigned int delay1 = 750; // Exposure time
unsigned int delay2 = 0; // Delay after one charater was printed
unsigned int delay3 = 120000; // Delay before the text is going to be written again (refresh glow)
unsigned int delay4 = 100; // Delay for function test (100 = standard)
unsigned int delay5 = 500; // Delay after one row of dots is printed (servo moving delay)
unsigned int delay6 = 30000; // Program waits ### milliseconds until printing the test string
unsigned int servolimit1 = 15; // Lower servo limit (black bar in the middle is hiding characters if set to low)
unsigned int servolimit2 = 165; // Upper servo limit (black bar in the middle is hiding characters if set to high)
String string2 = "J u Fo"; // test string

// Main code (do not change!)

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(1000);

  servo.attach(8); // Servo PIN
  SERVORESET(0);

  pinMode(1, INPUT_PULLUP); // Function Test PIN
  pinMode(2, OUTPUT); // LED 2
  pinMode(3, OUTPUT); // LED 3
  pinMode(4, OUTPUT); // LED 4
  pinMode(5, OUTPUT); // LED 5
  pinMode(6, OUTPUT); // LED 6
  pinMode(7, OUTPUT); // LED 7
  pinMode(9, OUTPUT); // LED 1
  pinMode(red, OUTPUT); // Status LED red
  pinMode(green, OUTPUT); // Status LED green
  pinMode(blue, OUTPUT); // Status LED blue
  pinMode(waitpin, OUTPUT); // Gets pulled HIGH if device is ready to receive serial data

  if (digitalRead(1) == LOW) {
    FUNCTIONTEST();
  }

  PORTB = 0;
  PORTD = 0;

  SERIALWAIT();
}

void loop() {
  CHECKCHAR();
  READABILITY();
  FINISHED();
  SERVORESET(1500);
}

void FUNCTIONTEST() {
  LEDTEST(10);
  SERVOTEST();
  digitalWrite(green, HIGH);
  delay(1000);
  digitalWrite(green, LOW);
}

void LEDTEST(int test) {
  servo.write(180);
  for (int i = 0; i < test; i++) {
    PORTB |= B00000010; // Testing LED 1
    delay(delay4);
    PORTB ^= B00000010;
    PORTD = B00000100; // Testing LED 2
    delay(delay4);
    PORTD = B00001000; // Testing LED 3
    delay(delay4);
    PORTD = B00010000; // Testing LED 4
    delay(delay4);
    PORTD = B00100000; // Testing LED 5
    delay(delay4);
    PORTD = B01000000; // Testing LED 6
    delay(delay4);
    PORTD = B10000000; // Testing LED 7
    delay(delay4);
    PORTD = 0;
    delay(delay4);
  }
}

void SERVOTEST() {
  for (int i = 0; i <= 180; i++) {
    delay(0.25 * delay4);
    int invangle = MAP(i, 0, 180, 180, 0);
    servo.write(invangle);
    digitalWrite(red, HIGH);
    delay(0.25 * delay4);
    digitalWrite(red, LOW);
  }
  servo.write(servolimit2);
  delay(10 * delay4);
}

void SERIALWAIT() {
  bool wait = true;
  long beginmillis = millis();
  digitalWrite(blue, HIGH);
  Serial.println("Device ready!");
  Serial.println("Waiting for transmission...");
  do {
    if (Serial.available() > 0) {
      string1 = Serial.readString();
      Serial.println("Display received the following text: " + string1);
      Serial.println("Transmission successfull!");
      Serial.println("You can now disconnect the FTDI-board from the microcontroller!");
      wait = !wait;
      digitalWrite(blue, LOW);
    }
  } while (wait == true && millis() - beginmillis <= delay6);
  if (millis() - beginmillis >= delay6) {
    Serial.println("Timeout!");
    Serial.println("Printing test string: " + string2);
    string1 = string2;
  }
  digitalWrite(waitpin, LOW);
  Serial.end();
}

void READABILITY() {
  if (angle <= 45) {
    MOVE(66);
  } else if (angle <= 90) {
    MOVE(21);
  } else if (angle <= 135) {
    MOVE(21);
  } else if (angle <= 180) {
    MOVE(66);
  }
}

void FINISHED() {
  digitalWrite(green, HIGH);
  delay(3000);
  digitalWrite(green, LOW);
  delay(delay3);
}

void PRINT(byte dots) {
  dots <<= 1;
  if (dots & B00000010) {
    PORTB |= B00000010;
  }
  PORTD = dots & B11111100;
  digitalWrite(blue, HIGH);
  delay(delay1);
  digitalWrite(blue, LOW);
  PORTD = 0;
  if (PORTB & B00000010) {
    PORTB ^= B00000010;
  }
  MOVE(angle1);
}

void MOVE(int addangle) {
  int newangle = angle + addangle;
  if (newangle > 180) {
    newangle = 180;
  } else if (newangle < 0) {
    newangle = 0;
  }
  int invangle = MAP(newangle, 0, 180, servolimit2, servolimit1);
  servo.write(invangle);
  delay(delay5);
  angle = newangle;
}

void SERVORESET(int del) {
  servo.write(servolimit2);
  angle = 0;
  delay(del);
}

void NEWCHAR() {
  MOVE(angle2);
  delay(delay2);
}

void OVERFLOW() {
  digitalWrite(red, HIGH);
  delay(2000);
  digitalWrite(red, LOW);
  READABILITY();
  FINISHED();
  stoploop = true;
}

void CHECKLEN(int pnumb, int ci) {
  int needangle = (pnumb * angle1) + angle2;
  int leftangle = angle + needangle;
  if (leftangle > 180) {
    OVERFLOW();
    if (stoploop == true) {
      SERVORESET(1500);
    }
  }
}

void CHECKCHAR() {
  for (int i = 0; i <= string1.length(); i++) {
    if (stoploop == true) {
      i = 1;
      SERVORESET(1500);
    }
    char1 = string1.charAt(i);
    switch (char1) {
      case 'A': case 'a': {
          CHECKLEN(5, i);
          PRINT(B0111111);
          PRINT(B1000100);
          PRINT(B1000100);
          PRINT(B1000100);
          PRINT(B0111111);
          NEWCHAR();
        } break;
      case 'B': case 'b': {
          CHECKLEN(5, i);
          PRINT(B1111111);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B0110110);
          NEWCHAR();
        } break;
      case 'C': case 'c': {
          CHECKLEN(5, i);
          PRINT(B0111110);
          PRINT(B1000001);
          PRINT(B1000001);
          PRINT(B1000001);
          PRINT(B0100010);
          NEWCHAR();
        } break;
      case 'D': case 'd': {
          CHECKLEN(5, i);
          PRINT(B1111111);
          PRINT(B1000001);
          PRINT(B1000001);
          PRINT(B1000001);
          PRINT(B0111110);
          NEWCHAR();
        } break;
      case 'E': case 'e': {
          CHECKLEN(5, i);
          PRINT(B1111111);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B1000001);
          NEWCHAR();
        } break;
      case 'F': case 'f': {
          CHECKLEN(5, i);
          PRINT(B1111111);
          PRINT(B1001000);
          PRINT(B1001000);
          PRINT(B1001000);
          PRINT(B1000000);
          NEWCHAR();
        } break;
      case 'G': case 'g': {
          CHECKLEN(5, i);
          PRINT(B0111110);
          PRINT(B1000001);
          PRINT(B1000101);
          PRINT(B1000101);
          PRINT(B0100110);
          NEWCHAR();
        } break;
      case 'H': case 'h': {
          CHECKLEN(5, i);
          PRINT(B1111111);
          PRINT(B0001000);
          PRINT(B0001000);
          PRINT(B0001000);
          PRINT(B1111111);
          NEWCHAR();
        } break;
      case 'I': case 'i': {
          CHECKLEN(3, i);
          PRINT(B1000001);
          PRINT(B1111111);
          PRINT(B1000001);
          NEWCHAR();
        } break;
      case 'J': case 'j': {
          CHECKLEN(5, i);
          PRINT(B0000110);
          PRINT(B0000001);
          PRINT(B0000001);
          PRINT(B0000001);
          PRINT(B1111110);
          NEWCHAR();
        } break;
      case 'K': case 'k': {
          CHECKLEN(5, i);
          PRINT(B1111111);
          PRINT(B0001000);
          PRINT(B0010100);
          PRINT(B0100010);
          PRINT(B1000001);
          NEWCHAR();
        } break;
      case 'L': case 'l': {
          CHECKLEN(5, i);
          PRINT(B1111111);
          PRINT(B0000001);
          PRINT(B0000001);
          PRINT(B0000001);
          PRINT(B0000001);
          NEWCHAR();
        } break;
      case 'M': case 'm': {
          CHECKLEN(5, i);
          PRINT(B1111111);
          PRINT(B0100000);
          PRINT(B0010000);
          PRINT(B0100000);
          PRINT(B1111111);
          NEWCHAR();
        } break;
      case 'N': case 'n': {
          CHECKLEN(5, i);
          PRINT(B1111111);
          PRINT(B0010000);
          PRINT(B0001000);
          PRINT(B0000100);
          PRINT(B1111111);
          NEWCHAR();
        } break;
      case 'O': case 'o': {
          CHECKLEN(5, i);
          PRINT(B0111110);
          PRINT(B1000001);
          PRINT(B1000001);
          PRINT(B1000001);
          PRINT(B0111110);
          NEWCHAR();
        } break;
      case 'P': case 'p': {
          CHECKLEN(5, i);
          PRINT(B1111111);
          PRINT(B1000100);
          PRINT(B1000100);
          PRINT(B1000100);
          PRINT(B0111000);
          NEWCHAR();
        } break;
      case 'Q': case 'q': {
          CHECKLEN(5, i);
          PRINT(B0111110);
          PRINT(B1000001);
          PRINT(B1000101);
          PRINT(B1000011);
          PRINT(B0111111);
          NEWCHAR();
        } break;
      case 'R': case 'r': {
          CHECKLEN(5, i);
          PRINT(B1111111);
          PRINT(B1000100);
          PRINT(B1000100);
          PRINT(B1000100);
          PRINT(B0111011);
          NEWCHAR();
        } break;
      case 'S': case 's': {
          CHECKLEN(5, i);
          PRINT(B0110010);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B0100110);
          NEWCHAR();
        } break;
      case 'T': case 't': {
          CHECKLEN(5, i);
          PRINT(B1000000);
          PRINT(B1000000);
          PRINT(B1111111);
          PRINT(B1000000);
          PRINT(B1000000);
          NEWCHAR();
        } break;
      case 'U': case 'u': {
          CHECKLEN(5, i);
          PRINT(B1111110);
          PRINT(B0000001);
          PRINT(B0000001);
          PRINT(B0000001);
          PRINT(B1111110);
          NEWCHAR();
        } break;
      case 'V': case 'v': {
          CHECKLEN(5, i);
          PRINT(B1111100);
          PRINT(B0000010);
          PRINT(B0000001);
          PRINT(B0000010);
          PRINT(B1111100);
          NEWCHAR();
        } break;
      case 'W': case 'w': {
          CHECKLEN(5, i);
          PRINT(B1111111);
          PRINT(B0000010);
          PRINT(B0000100);
          PRINT(B0000010);
          PRINT(B1111111);
          NEWCHAR();
        } break;
      case 'X': case 'x': {
          CHECKLEN(5, i);
          PRINT(B1100011);
          PRINT(B0010100);
          PRINT(B0001000);
          PRINT(B0010100);
          PRINT(B1100011);
          NEWCHAR();
        } break;
      case 'Y': case 'y': {
          CHECKLEN(5, i);
          PRINT(B1110000);
          PRINT(B0001000);
          PRINT(B0000111);
          PRINT(B0001000);
          PRINT(B1110000);
          NEWCHAR();
        } break;
      case 'Z': case 'z': {
          CHECKLEN(5, i);
          PRINT(B1000011);
          PRINT(B1000101);
          PRINT(B1001001);
          PRINT(B1010001);
          PRINT(B1100001);
          NEWCHAR();
        } break;
      case '?': {
          CHECKLEN(5, i);
          PRINT(B0100000);
          PRINT(B1000000);
          PRINT(B1000101);
          PRINT(B1001000);
          PRINT(B0110000);
          NEWCHAR();
        } break;
      case '!': {
          CHECKLEN(1, i);
          PRINT(B1111101);
          NEWCHAR();
        } break;
      case '.': {
          CHECKLEN(1, i);
          PRINT(B0000001);
          NEWCHAR();
        } break;
      case ':': {
          CHECKLEN(1, i);
          PRINT(B0100010);
          NEWCHAR();
        } break;
      case '(': {
          CHECKLEN(2, i);
          PRINT(B0111110);
          PRINT(B1000001);
          NEWCHAR();
        } break;
      case ')': {
          CHECKLEN(2, i);
          PRINT(B1000001);
          PRINT(B0111110);
          NEWCHAR();
        } break;
      case '%': {
          CHECKLEN(5, i);
          PRINT(B1100010);
          PRINT(B1100100);
          PRINT(B0001000);
          PRINT(B0010011);
          PRINT(B0100011);
          NEWCHAR();
        } break;
      case '$': {
          CHECKLEN(5, i);
          PRINT(B0111010);
          PRINT(B0101010);
          PRINT(B1101011);
          PRINT(B0101010);
          PRINT(B0101110);
          NEWCHAR();
        } break;
      case '+': {
          CHECKLEN(5, i);
          PRINT(B0001000);
          PRINT(B0001000);
          PRINT(B0111110);
          PRINT(B0001000);
          PRINT(B0001000);
          NEWCHAR();
        } break;
      case '-': {
          CHECKLEN(5, i);
          PRINT(B0001000);
          PRINT(B0001000);
          PRINT(B0001000);
          PRINT(B0001000);
          PRINT(B0001000);
          NEWCHAR();
        } break;
      case '=': {
          CHECKLEN(5, i);
          PRINT(B0010100);
          PRINT(B0010100);
          PRINT(B0010100);
          PRINT(B0010100);
          PRINT(B0010100);
          NEWCHAR();
        } break;
      case '€': { // May cause problem!
          CHECKLEN(5, i);
          PRINT(B0010100);
          PRINT(B0111110);
          PRINT(B1010101);
          PRINT(B1010101);
          PRINT(B1000001);
          NEWCHAR();
        } break;
      case '°': { // May cause problem!
          CHECKLEN(4, i);
          PRINT(B0110000);
          PRINT(B1001000);
          PRINT(B1001000);
          PRINT(B0110000);
          NEWCHAR();
        } break;
      case '0': {
          CHECKLEN(5, i);
          PRINT(B1111111);
          PRINT(B1000001);
          PRINT(B1000001);
          PRINT(B1000001);
          PRINT(B1111111);
          NEWCHAR();
        } break;
      case '1': {
          CHECKLEN(2, i);
          PRINT(B0100000);
          PRINT(B1111111);
          NEWCHAR();
        } break;
      case '2': {
          CHECKLEN(5, i);
          PRINT(B1001111);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B1111001);
          NEWCHAR();
        } break;
      case '3': {
          CHECKLEN(5, i);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B1111111);
          NEWCHAR();
        } break;
      case '4': {
          CHECKLEN(5, i);
          PRINT(B1111000);
          PRINT(B0001000);
          PRINT(B0001000);
          PRINT(B0001000);
          PRINT(B1111111);
          NEWCHAR();
        } break;
      case '5': {
          CHECKLEN(5, i);
          PRINT(B1111001);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B1001111);
          NEWCHAR();
        } break;
      case '6': {
          CHECKLEN(5, i);
          PRINT(B1111111);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B1001111);
          NEWCHAR();
        } break;
      case '7': {
          CHECKLEN(5, i);
          PRINT(B1000000);
          PRINT(B1000000);
          PRINT(B1000111);
          PRINT(B1001000);
          PRINT(B1110000);
          NEWCHAR();
        } break;
      case '8': {
          CHECKLEN(5, i);
          PRINT(B1111111);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B1111111);
          NEWCHAR();
        } break;
      case '9': {
          CHECKLEN(5, i);
          PRINT(B1111001);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B1001001);
          PRINT(B1111111);
          NEWCHAR();
        } break;
      case ' ': {
          CHECKLEN(0, i);
          NEWCHAR();
        } break;
      case '&': { // May cause problem!
          FUNCTIONTEST();
        } break;
    }
  }
}

long MAP(long x, long a, long b, long c, long d) {
  return ((x - b) * (c - d)) / ((a - b) + d);
}
