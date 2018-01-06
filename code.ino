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

unsigned int angle1 = 1; // Distance between rows
unsigned int angle2 = 2; // Distance between characters
unsigned int delay1 = 750; // Exposure time
unsigned int delay2 = 0; // Delay after one charater was printed
unsigned int delay3 = 10000; // Delay before the text is going to be written again (refresh glow)
unsigned int delay4 = 100; // Delay for function test (100 = standard)
unsigned int delay5 = 50; // Delay after one row of dots is printed (servo moving delay)
unsigned int delay6 = 30000; // Program waits ### milliseconds until printing the test string
String string2 = "JUGEND FORSCHT"; // test string

// Main code (do not change!)

void setup() {
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
  digitalWrite(green, HIGH);
  delay(3000);
  digitalWrite(green, LOW);
  delay(delay3);
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
  digitalWrite(9, HIGH);
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
    int invangle = map(i, 0, 180, 180, 0);
    servo.write(invangle);
    digitalWrite(red, HIGH);
    delay(0.25 * delay4);
    digitalWrite(red, LOW);
  }
  servo.write(180);
  delay(10 * delay4);
}

void SERIALWAIT() {
  bool wait = true;
  Serial.begin(9600);
  Serial.setTimeout(1000);
  digitalWrite(blue, HIGH);
  do {
    if (Serial.available() > 0) {
      string1 = Serial.readString();
      Serial.end();
      wait = !wait;
      digitalWrite(blue, LOW);
    }
    digitalWrite(waitpin, HIGH);
  } while (wait == true && millis() <= delay6);
  if (millis() >= delay6) {
    string1 = string2;
  } else {
    digitalWrite(waitpin, LOW);
  }
}

void READABILITY() {
  if (angle <= 45) {
    MOVE(66);
  } else if (angle <= 90) {
    MOVE(21);
  } else if (angle <= 135) {
    MOVE(-21);
  } else if (angle <= 180) {
    MOVE(-66);
  }
}

void PRINT(byte dots) {
  dots <<= 1;
  if (dots & B00000010) {
    PORTB |= B00000010;
  }
  PORTD = dots & B11111100;
  digitalWrite(blue, HIGH);
  delay(0.5 * delay1);
  digitalWrite(blue, LOW);
  delay(0.5 * delay1);
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
  int invangle = map(newangle, 0, 180, 180, 0);
  servo.write(invangle);
  delay(delay5);
  if (newangle == 180) {
    OVERFLOW();
    newangle = 0;
  }
  angle = newangle;
}

void SERVORESET(int del) {
  servo.write(180);
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
  stoploop = true;
}

void CHECKLEN(int pnumb) {
  int needangle = (pnumb * angle1) + angle2;
  int leftangle = angle + needangle;
  if (leftangle > 180) {
    OVERFLOW();
    CHKSTL();
  }
}

bool CHKSTL() {
  if (stoploop == true) {
    stoploop = false;
    SERVORESET(1500);
    return true;
  } else {
    return false;
  }
}

void CHECKCHAR() {
  for (int i = 0; i <= string1.length(); i++) {
    if (CHKSTL() == true) {
      i = 1;
    }
    char1 = string1.charAt(i);
    if (char1 == 'a' || char1 == 'A') {
      CHECKLEN(5);
      PRINT(B0111111);
      PRINT(B1000100);
      PRINT(B1000100);
      PRINT(B1000100);
      PRINT(B0111111);
      NEWCHAR();
    } else if (char1 == 'b' || char1 == 'B') {
      CHECKLEN(5);
      PRINT(B1111111);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B0110110);
      NEWCHAR();
    } else if (char1 == 'c' || char1 == 'C') {
      CHECKLEN(5);
      PRINT(B0111110);
      PRINT(B1000001);
      PRINT(B1000001);
      PRINT(B1000001);
      PRINT(B0100010);
      NEWCHAR();
    } else if (char1 == 'd' || char1 == 'D') {
      CHECKLEN(5);
      PRINT(B1111111);
      PRINT(B1000001);
      PRINT(B1000001);
      PRINT(B1000001);
      PRINT(B0111110);
      NEWCHAR();
    } else if (char1 == 'e' || char1 == 'E') {
      CHECKLEN(5);
      PRINT(B1111111);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B1000001);
      NEWCHAR();
    } else if (char1 == 'f' || char1 == 'F') {
      CHECKLEN(5);
      PRINT(B1111111);
      PRINT(B1001000);
      PRINT(B1001000);
      PRINT(B1001000);
      PRINT(B1000000);
      NEWCHAR();
    } else if (char1 == 'g' || char1 == 'G') {
      CHECKLEN(5);
      PRINT(B0111110);
      PRINT(B1000001);
      PRINT(B1000101);
      PRINT(B1000101);
      PRINT(B0100110);
      NEWCHAR();
    } else if (char1 == 'h' || char1 == 'H') {
      CHECKLEN(5);
      PRINT(B1111111);
      PRINT(B0001000);
      PRINT(B0001000);
      PRINT(B0001000);
      PRINT(B1111111);
      NEWCHAR();
    } else if (char1 == 'i' || char1 == 'I') {
      CHECKLEN(3);
      PRINT(B1000001);
      PRINT(B1111111);
      PRINT(B1000001);
      NEWCHAR();
    } else if (char1 == 'j' || char1 == 'J') {
      CHECKLEN(5);
      PRINT(B0000110);
      PRINT(B0000001);
      PRINT(B0000001);
      PRINT(B0000001);
      PRINT(B1111110);
      NEWCHAR();
    } else if (char1 == 'k' || char1 == 'K') {
      CHECKLEN(5);
      PRINT(B1111111);
      PRINT(B0001000);
      PRINT(B0010100);
      PRINT(B0100010);
      PRINT(B1000001);
      NEWCHAR();
    } else if (char1 == 'l' || char1 == 'L') {
      CHECKLEN(5);
      PRINT(B1111111);
      PRINT(B0000001);
      PRINT(B0000001);
      PRINT(B0000001);
      PRINT(B0000001);
      NEWCHAR();
    } else if (char1 == 'm' || char1 == 'M') {
      CHECKLEN(5);
      PRINT(B1111111);
      PRINT(B0100000);
      PRINT(B0010000);
      PRINT(B0100000);
      PRINT(B1111111);
      NEWCHAR();
    } else if (char1 == 'n' || char1 == 'N') {
      CHECKLEN(5);
      PRINT(B1111111);
      PRINT(B0010000);
      PRINT(B0001000);
      PRINT(B0000100);
      PRINT(B1111111);
      NEWCHAR();
    } else if (char1 == 'o' || char1 == 'O') {
      CHECKLEN(5);
      PRINT(B0111110);
      PRINT(B1000001);
      PRINT(B1000001);
      PRINT(B1000001);
      PRINT(B0111110);
      NEWCHAR();
    } else if (char1 == 'p' || char1 == 'P') {
      CHECKLEN(5);
      PRINT(B1111111);
      PRINT(B1000100);
      PRINT(B1000100);
      PRINT(B1000100);
      PRINT(B0111000);
      NEWCHAR();
    } else if (char1 == 'q' || char1 == 'Q') {
      CHECKLEN(5);
      PRINT(B0111110);
      PRINT(B1000001);
      PRINT(B1000101);
      PRINT(B1000011);
      PRINT(B0111111);
      NEWCHAR();
    } else if (char1 == 'r' || char1 == 'R') {
      CHECKLEN(5);
      PRINT(B1111111);
      PRINT(B1000100);
      PRINT(B1000100);
      PRINT(B1000100);
      PRINT(B0111011);
      NEWCHAR();
    } else if (char1 == 's' || char1 == 'S') {
      CHECKLEN(5);
      PRINT(B0110010);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B0100110);
      NEWCHAR();
    } else if (char1 == 't' || char1 == 'T') {
      CHECKLEN(5);
      PRINT(B1000000);
      PRINT(B1000000);
      PRINT(B1111111);
      PRINT(B1000000);
      PRINT(B1000000);
      NEWCHAR();
    } else if (char1 == 'u' || char1 == 'U') {
      CHECKLEN(5);
      PRINT(B1111110);
      PRINT(B0000001);
      PRINT(B0000001);
      PRINT(B0000001);
      PRINT(B1111110);
      NEWCHAR();
    } else if (char1 == 'v' || char1 == 'V') {
      CHECKLEN(5);
      PRINT(B1111100);
      PRINT(B0000010);
      PRINT(B0000001);
      PRINT(B0000010);
      PRINT(B1111100);
      NEWCHAR();
    } else if (char1 == 'w' || char1 == 'W') {
      CHECKLEN(5);
      PRINT(B1111111);
      PRINT(B0000010);
      PRINT(B0000100);
      PRINT(B0000010);
      PRINT(B1111111);
      NEWCHAR();
    } else if (char1 == 'x' || char1 == 'X') {
      CHECKLEN(5);
      PRINT(B1100011);
      PRINT(B0010100);
      PRINT(B0001000);
      PRINT(B0010100);
      PRINT(B1100011);
      NEWCHAR();
    } else if (char1 == 'y' || char1 == 'Y') {
      CHECKLEN(5);
      PRINT(B1110000);
      PRINT(B0001000);
      PRINT(B0000111);
      PRINT(B0001000);
      PRINT(B1110000);
      NEWCHAR();
    } else if (char1 == 'z' || char1 == 'Z') {
      CHECKLEN(5);
      PRINT(B1000011);
      PRINT(B1000101);
      PRINT(B1001001);
      PRINT(B1010001);
      PRINT(B1100001);
      NEWCHAR();
    } else if (char1 == '?') {
      CHECKLEN(5);
      PRINT(B0100000);
      PRINT(B1000000);
      PRINT(B1000101);
      PRINT(B1001000);
      PRINT(B0110000);
      NEWCHAR();
    } else if (char1 == '!') {
      CHECKLEN(1);
      PRINT(B1111101);
      NEWCHAR();
    } else if (char1 == '.') {
      CHECKLEN(1);
      PRINT(B0000001);
      NEWCHAR();
    } else if (char1 == ':') {
      CHECKLEN(1);
      PRINT(B0100010);
      NEWCHAR();
    } else if (char1 == '(') {
      CHECKLEN(2);
      PRINT(B0111110);
      PRINT(B1000001);
      NEWCHAR();
    } else if (char1 == ')') {
      CHECKLEN(2);
      PRINT(B1000001);
      PRINT(B0111110);
      NEWCHAR();
    } else if (char1 == '%') {
      CHECKLEN(5);
      PRINT(B1100010);
      PRINT(B1100100);
      PRINT(B0001000);
      PRINT(B0010011);
      PRINT(B0100011);
      NEWCHAR();
    } else if (char1 == '$') {
      CHECKLEN(5);
      PRINT(B0111010);
      PRINT(B0101010);
      PRINT(B1101011);
      PRINT(B0101010);
      PRINT(B0101110);
      NEWCHAR();
    } else if (char1 == '+') {
      CHECKLEN(5);
      PRINT(B0001000);
      PRINT(B0001000);
      PRINT(B0111110);
      PRINT(B0001000);
      PRINT(B0001000);
      NEWCHAR();
    } else if (char1 == '-') {
      CHECKLEN(5);
      PRINT(B0001000);
      PRINT(B0001000);
      PRINT(B0001000);
      PRINT(B0001000);
      PRINT(B0001000);
      NEWCHAR();
    } else if (char1 == '=') {
      CHECKLEN(5);
      PRINT(B0010100);
      PRINT(B0010100);
      PRINT(B0010100);
      PRINT(B0010100);
      PRINT(B0010100);
      NEWCHAR();
    } else if (char1 == '0') {
      CHECKLEN(5);
      PRINT(B1111111);
      PRINT(B1000001);
      PRINT(B1000001);
      PRINT(B1000001);
      PRINT(B1111111);
      NEWCHAR();
    } else if (char1 == '1') {
      CHECKLEN(3);
      PRINT(B0100000);
      PRINT(B1111111);
      PRINT(B1000000);
      NEWCHAR();
    } else if (char1 == '2') {
      CHECKLEN(5);
      PRINT(B1001111);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B1111001);
      NEWCHAR();
    } else if (char1 == '3') {
      CHECKLEN(5);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B1111111);
      NEWCHAR();
    } else if (char1 == '4') {
      CHECKLEN(5);
      PRINT(B1111000);
      PRINT(B0001000);
      PRINT(B0001000);
      PRINT(B0001000);
      PRINT(B1111111);
      NEWCHAR();
    } else if (char1 == '5') {
      CHECKLEN(5);
      PRINT(B1111001);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B1001111);
      NEWCHAR();
    } else if (char1 == '6') {
      CHECKLEN(5);
      PRINT(B1111111);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B1001111);
      NEWCHAR();
    } else if (char1 == '7') {
      CHECKLEN(5);
      PRINT(B1000000);
      PRINT(B1000000);
      PRINT(B1000111);
      PRINT(B1001000);
      PRINT(B1110000);
      NEWCHAR();
    } else if (char1 == '8') {
      CHECKLEN(5);
      PRINT(B1111111);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B1111111);
      NEWCHAR();
    } else if (char1 == '9') {
      CHECKLEN(5);
      PRINT(B1111001);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B1001001);
      PRINT(B1111111);
      NEWCHAR();
    } else if (char1 == ' ') {
      CHECKLEN(0);
      NEWCHAR();
    } else if (char1 == '&') {
      FUNCTIONTEST();
    }
  }
}
