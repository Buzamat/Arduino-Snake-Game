#include "LedControl.h" // need the library
#include <LiquidCrystal.h>
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;
const int xPin = A0;
const int yPin = A1;
const int SW = 7;
const int buzzer = 13;
const int RS = 9;
const int enable = 8;
const int backlight = 6;
const int intensity = 129;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD,
byte matrixBrightness = 2;
int xNext;
int yNext;
int SW_state = 0;
const int minThreshold = 200;
const int maxThreshold = 600;
const int moveInterval = 300;
unsigned long long lastMoved = 0;
const byte matrixSize = 8;
bool matrixChanged = true;
byte matrix[matrixSize][matrixSize] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};



byte snakeBody[2][65];
byte foodx = 10;
byte foody = 10;
byte directie = 1; //0 stop, 1 sus, 2 jos, 3 dreapta, 4 stanga
byte command = 1;
byte snakeLenght = 1;
bool gameOver = false;
bool menu = true;
int xValue;
int yValue;
byte score = 0;
bool foodupdate2;
bool foodupdate1;





byte matrixByte[matrixSize] = {
  B00000000,
  B01000100,
  B00101000,
  B00010000,
  B00010000,
  B00010000,
  B00000000,
  B00000000
};



void setup() {
  randomSeed(analogRead(5));
  analogWrite(backlight, intensity);
  Serial.begin(9600);
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen

  snakeBody[0][0] = 4; //x snake head
  snakeBody[1][0] = 4; //y snake head
  matrix[snakeBody[0][0]][snakeBody[1][0]] = 1;

  lcd.begin(16, 2);
  // Print a message to the LCD.
  generateFood();
  pinMode(SW, INPUT_PULLUP);

        lcd.setCursor(6, 0);
      lcd.print("SNAKE");
            lcd.setCursor(0, 1);
      lcd.print(" Start New Game ");

}
void loop() {

  if (menu == true) {
    if (gameOver) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Last score : ");
      lcd.print(score);
      lcd.setCursor(0, 1);
      lcd.print(" Start New Game ");
      gameOver = false;
    }

    SW_state = digitalRead(SW);
   // xValue = analogRead(xPin);
   // yValue = analogRead(yPin);
    
   
    if (SW_state == 0) {
      lcd.clear();
      menu = false;
      score = 1;
      snakeLenght = 1;
      lcd.setCursor(6, 0);
      lcd.print("SNAKE");
      lcd.setCursor(3, 1);
      lcd.print("Score : ");
      lcd.print(score);
      SW_state=1;
    }

  }



  if (menu == false) {
    if (gameOver) { //game over animation
        tone(buzzer, 200, 1000);
      delay(1000);
      lc.clearDisplay(0);// clear screen
              tone(buzzer, 1100, 50);
      delay(500);
              tone(buzzer, 100, 50);
      updateMatrix();
              tone(buzzer, 1100, 50);
      delay(500);
              tone(buzzer, 100, 50);
      lc.clearDisplay(0);// clear screen
              tone(buzzer, 1100, 50);
      delay(500);
              tone(buzzer, 100, 50);
      updateMatrix();
              tone(buzzer, 1100, 50);
      delay(500);
              tone(buzzer, 100, 50);
      lc.clearDisplay(0);// clear screen
              tone(buzzer, 1100, 50);
      delay(500);
              tone(buzzer, 100, 50);
      updateMatrix();
      delay(1500);
      Serial.println("menu restarted");
      menu = true;
      for (int i = snakeLenght - 1; i >= 0; i--) {
        matrix[snakeBody[0][i]][snakeBody[1][i]] = 0;
        delay(moveInterval / 2);
                      tone(buzzer, 2000, 50);
        updateMatrix();
      }

    }

    else {


      updatePositions();

      if (millis() - lastMoved > moveInterval) {
        // game logic
        directie = command;
        lastMoved = millis();
        checkNext();

      }
      foodBlink();

      if (matrixChanged == true) {
        // matrix display logic
        updateMatrix();
        matrixChanged = false;
      }




    }
  }
}



void checkNext() {
  switch (directie) {
    case 1: //fata
      xNext = snakeBody[0][0] + 1;
      yNext = snakeBody[1][0];
      break;
    case 2: //spate
      xNext = snakeBody[0][0] - 1;
      yNext = snakeBody[1][0];
      break;
    case 3: //dreapta
      xNext = snakeBody[0][0];
      yNext = snakeBody[1][0] + 1;
      break;
    case 4: //stanga
      xNext = snakeBody[0][0];
      yNext = snakeBody[1][0] - 1;
      break;
    case 0: //pe loc
      // statements
      break;

  }
  if (xNext >= 8) {
    xNext = 0;
  }
  if (xNext < 0) {
    xNext = 7;
  }
  if (yNext >= 8) {
    yNext = 0;
  }
  if (yNext < 0) {
    yNext = 7;
  }

  if (matrix[xNext][yNext] == 1) { //daca a muscat propria coada
    gameOver = true;
    return;
  }

  if (xNext == foodx && yNext == foody) { //daca a gasit mancare

  tone(buzzer, 1200, 100);
    for (byte i = snakeLenght; i >= 1; i--) {
      snakeBody[0][i] = snakeBody[0][i - 1];
      snakeBody[1][i] = snakeBody[1][i - 1];
    }
    snakeLenght++;
    snakeBody[0][0] = xNext;
    snakeBody[1][0] = yNext;
    matrix[snakeBody[0][0]][snakeBody[1][0]] = 1;

    generateFood();
    matrixChanged = true;
    return;
  }

  if (matrix[xNext][yNext] == 0) { //daca e spatiu gol
    matrix[snakeBody[0][snakeLenght - 1]][snakeBody[1][snakeLenght - 1]] = 0;
    for (byte i = snakeLenght - 1; i >= 1; i--) {
      snakeBody[0][i] = snakeBody[0][i - 1];
      snakeBody[1][i] = snakeBody[1][i - 1];
    }
    snakeBody[0][0] = xNext;
    snakeBody[1][0] = yNext;
    matrix[snakeBody[0][0]][snakeBody[1][0]] = 1;
    matrixChanged = true;

  }
}

void generateFood() {//sa pun o limita aici eventual cand e maxed out sarpele
  foodx = random(8);
  foody = random(8);

  for (byte i = 0; i <= snakeLenght; i++)
  {
    if (snakeBody[0][i] == foodx && snakeBody[1][i] == foody) {
      generateFood();
    }
  }
  matrix[foodx][foody] = 2; //sa il fac 2 pentru food si eventual schimb matrix in byte, schimb afisare matrix pt 1 si 0 si 2
  matrixChanged = true;
  score = score + 1;
  lcd.setCursor(3, 1);
  lcd.print("Score : ");
  lcd.print(score);
}



void updateByteMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, matrixByte[row]);
  }
}



void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      if (matrix[row][col] != 2) {
        lc.setLed(0, row, col, matrix[row][col]);
      }
    }
  }
}
void foodBlink() {
  if (millis() % 500 >= 250) {
    lc.setLed(0, foodx, foody, 1);
    foodupdate1 = true;
    if (foodupdate2 != foodupdate1) {
      matrixChanged = true;
      foodupdate2 = true;
    }
  }
  else {
    lc.setLed(0, foodx, foody, 0);
    foodupdate1 = false;
    if (foodupdate2 != foodupdate1) {
      matrixChanged = true;
      foodupdate2 = false;
    }
  }
}
void updatePositions() {
  xValue = analogRead(xPin);
  yValue = analogRead(yPin);
  if (xValue < minThreshold) {
    if (directie != 1) {
      command = 2;
    }
  }


  if (xValue > maxThreshold) {
    if (directie != 2) {
      command = 1;
    }
  }

  if (yValue > maxThreshold) {
    if (directie != 3) {
      command = 4;
    }
  }
  if (yValue < minThreshold) {
    if (directie != 4) {
      command = 3;
    }
  }

}
