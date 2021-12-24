#include "LedControl.h" // need the library
#include <LiquidCrystal.h>
#include <EEPROM.h>
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
LedControl lc = LedControl(dinPin, clockPin, loadPin, 2); //DIN, CLK, LOAD,
byte matrixBrightness = 2;
int xNext;
int yNext;
int SW_state = 0;
const int minThreshold = 200;
const int maxThreshold = 600;
float moveInterval = 400;
unsigned long long lastMoved = 0;
byte lastSw = 1;
String nume;
byte directie = 0; //0 stop, 1 sus, 2 jos, 3 dreapta, 4 stanga
byte command = 0;
int xValue;
int yValue;
int score = 0;
int linie = 0;
byte column = 0;
String rowOne, rowTwo;
bool optionsOn = false;
bool highOn = false;
bool contrastOn = false;
bool newScoreOn = false;
bool highscoreResetOn = false;
bool gameOverOn = false;
byte optiune = 0;
int loc;
bool lcdChanged = false;
String alphaSmall = "abcdefghijklmnopqrstuvwxyz1234567890";
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
byte snakeLenght = 1;
bool menu = true;
bool foodupdate2;
bool foodupdate1;
bool gameOver = true;
float foodTimer;
float speedModifier;
void startGame() {
  randomSeed(analogRead(5));
  snakeBody[0][0] = 4; //x snake head
  snakeBody[1][0] = 4; //y snake head
  matrix[snakeBody[0][0]][snakeBody[1][0]] = 1;
  snakeLenght = 1;
  foody = 10;
  foodx = 10;
  score = 1;
  generateFood();
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("SNAKE");
  lcd.setCursor(3, 1);
  lcd.print("Score : ");
  lcd.print(score);
  menu = false;
  gameOver = false;
  directie = 1; //0 stop, 1 sus, 2 jos, 3 dreapta, 4 stanga
  command = 1;
  speedModifier = 1;
  moveInterval = 400;
  speedModifier = millis();
}

void setup() {
  randomSeed(analogRead(5));
  analogWrite(backlight, EEPROM.read(100) * 25); //lcd contrast
  Serial.begin(9600);
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen

  lc.shutdown(1, false); // turn off power saving, enables display
  lc.setIntensity(1, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(1);// clear screen
  lcd.begin(16, 2);
  // Print a message to the LCD.
  pinMode(SW, INPUT_PULLUP);
  rowOne = String("  New Game");
  rowTwo = String("  Settings");
  lcd.setCursor(0, 0);
  lcd.print(rowOne);
  lcd.setCursor(0, 1);
  lcd.print(rowTwo);

}

void blinkLcd(byte c, byte r) {
  if (millis() % 1000 >= 499) {
    lcd.setCursor(c, r);
    lcd.print(" ");
  }
}
void   contrastSet() {
  if (linie == 1) {
    if (command == 1) {
      command = 5;
      if (optiune < 12) { //dreapta
        lcdChanged = true;
        optiune++;
        analogWrite(backlight, optiune * 25);

      }
    }

    if (command == 2) { //stanga
      command = 5;
      if (optiune > 0) {
        lcdChanged = true;
        optiune--;
        analogWrite(backlight, optiune * 25);
      }
    }
    if (command == 3) { //stanga
      command = 5;
      linie = 0;
      lcdChanged = true;
    }
  }

  else {
    if (command == 4) { //stanga
      command = 5;
      linie = 1;
      lcdChanged = true;
    }
  }
  if ( SW_state == 0) {
    if (lastSw == 1) {
      lastSw = 0; //sus
      if (linie == 0) { //save
        EEPROM.update(0, optiune);
        contrastOn = false;
        linie = 0;
        optiune = 0;
        rowOne = String("  Settings:");
        rowTwo = String("  Set Contrast");
      }
    }

  }
  if ( SW_state == 1) {
    lastSw = 1;
  }

  if (lcdChanged) {
    lcdChanged = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(rowOne);
    lcd.setCursor(1, 1);
    rowTwo = ' ';
    for (int i = 0; i < optiune; i++) {
      rowTwo = rowTwo + '#';
    }
    lcd.print(rowTwo);
    lcd.setCursor(0, linie);
    if (linie == 0) {
      lcd.print("<");
    }
    else {
      lcd.print(">");
      lcd.setCursor(15, linie);
      lcd.print("<");
    }

  }
  //blinkLcd(0,linie);
}
void   highscoreReset() {
  if (command == 1) {
    command = 5;
    if (optiune == 0) { //dreapta
      lcdChanged = true;
      optiune = 1;
      rowTwo = String("  YES  <CANCEL>");
    }
  }

  if (command == 2) { //stanga
    command = 5;
    if (optiune == 1) {
      lcdChanged = true;
      optiune = 0;
      rowTwo = String(" <YES>  CANCEL");
    }
  }

  if ( SW_state == 0) {
    if (lastSw == 1) {
      lastSw = 0; //sus
      if (optiune == 0) { //yes
        highscoreResetOn = false;
        linie = 0;
        optiune = 0;
        rowOne = String("  Settings:");
        rowTwo = String("  Set Contrast");
        Serial.println("Reset successfull");
        for (int i = 0; i < 3; i++) {
          writeScoreEEPROM(i, 0);
          writeStringEEPROM(i, "None");
        }


      }
      if (optiune == 1) {
        //no
        highscoreResetOn = false;
        linie = 0;
        optiune = 0;
        rowOne = String("  Settings:");
        rowTwo = String("  Set Contrast");
        Serial.println("Reset canceled");
      }
    }
  }
  if ( SW_state == 1) {
    lastSw = 1;
  }
  if (lcdChanged) {
    lcdChanged = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(rowOne);
    lcd.setCursor(0, 1);
    lcd.print(rowTwo);
  }
}
void setHighscore() {
  if (linie == 1) {
    if (command == 1) {//dreapta
      command = 5;
      if (column < 15) {
        lcdChanged = true;
        column++;
        optiune++;
      }
      else if (optiune < alphaSmall.length() - 1) {
        optiune++;
        rowTwo = alphaSmall.substring(optiune - 15, optiune);
        lcdChanged = true;
      }
    }

    if (command == 2) { //stanga
      command = 5;
      if (column > 0) {
        lcdChanged = true;
        column--;
        optiune--;
      }
      else if (optiune > 0) {
        optiune--;
        rowTwo = alphaSmall.substring(optiune, optiune + 16);
        lcdChanged = true;
      }
    }

    if (command == 3) { //sus
      command = 5;
      linie = 0;
      lcdChanged = true;
    }
  }
  else {
    if (command == 4) { //jos
      command = 5;
      linie = 1;
      lcdChanged = true;
    }

    if (command == 2) { //stanga
      command = 5;
      if (nume.length() > 0) {
        nume = nume.substring(0, nume.length() - 1);
        lcdChanged = true;
      }

    }
  }
  if ( SW_state == 0) {
    if (lastSw == 1) {
      lastSw = 0; //sus
      if (linie == 0) { //save
        if (nume.length() > 0) {
          writeStringEEPROM(loc, nume);
          newScoreOn = false;
          linie = 0;
          optiune = 0;
          column = 0;
          nume = "";
          rowOne = String("  New Game");
          rowTwo = String("  Settings");
        }
      }
      else {
        if (nume.length() <= 12) {
          nume = nume + alphaSmall[optiune];
          lcdChanged = true;
        }
      }
    }

  }
  if ( SW_state == 1) {
    lastSw = 1;
  }

  if (lcdChanged) {
    rowOne = nume;
    score = 16 - nume.length();
    lcdChanged = false;
    lcd.clear();
    lcd.setCursor(score / 2, 0);
    lcd.print(rowOne);
    lcd.setCursor(0, 1);
    lcd.print(rowTwo);
    lcd.setCursor(0, 0);
    lcd.print("<");
  }

  if (linie == 1) {
    if (millis() % 1000 >= 500) {
      lcd.setCursor(column, 1);
      lcd.print(alphaSmall[optiune]);
    }
    else {
      lcd.setCursor(column, 1);
      lcd.print("_");
    }
  }
  else {
    if (millis() % 1000 >= 500) {
      lcd.setCursor(0, 0);
      lcd.print("<");
    }
    else {
      lcd.setCursor(0, 0);
      lcd.print(" ");
    }
  }
}

void endScore() {
  if (optiune == 0) {
    lcd.clear();
    rowTwo = "   Score: " + String(score);
    lcd.setCursor(0, 1);
    lcd.print(rowTwo);

    for (int i = 2; i >= 0; i--) {
      if (score > readScoreEEPROM(i)) {
        loc = i;
      }
    }
    if (loc == 5) {
      rowOne = "   GAME OVER!";
      lcd.setCursor(0, 0);
      lcd.print(rowOne);
    }
    else {
      rowOne = " >>HIGHSCORE!<<";
      lcd.setCursor(0, 0);
      lcd.print(rowOne);
      for (int i = 2; i > loc; i--) {

        int placeholderInt = readScoreEEPROM(i - 1);
        writeScoreEEPROM(i, placeholderInt);
        String placeholderString = readStringEEPROM(i - 1);
        writeStringEEPROM(i, placeholderString);
      }
      writeStringEEPROM(loc, "Placeholder");
      writeScoreEEPROM(loc, score);
    }
    optiune = 1;
    score = 0;
  }

  if ( SW_state == 0) {//back
    if (lastSw == 1) {


      if (loc == 5) {
        lastSw = 0;
        gameOverOn = false;
        optiune = 0;
        linie = 0;
        rowOne = String("  New Game");
        rowTwo = String("  Settings");
      }
      else {
        lastSw = 0;
        gameOverOn = false;
        newScoreOn = true;
        optiune = 0;
        linie = 0;
        rowOne = String("     Set name   ");
        rowTwo = alphaSmall.substring(0, 16);
        lcdChanged = true;
      }
    }
  }

  if ( SW_state == 1) {
    lastSw = 1;
  }
}

void optionsMenu() {
  if (contrastOn) {
    contrastSet();
  }
  else if (highscoreResetOn) {
    highscoreReset();
  }

  else {
    if (command == 4) {
      command = 5;
      if (linie < 1) { //jos


        linie = linie + 1; optiune = optiune + 1;
      }
      else if (optiune < 2) {

        optiune = optiune + 1;
        rowOne = String("  Set Contrast");
        rowTwo = String("  Reset Scores");
      }
    }
    if (command == 3) { //sus

      command = 5;
      if (linie > 0) {

        linie = linie - 1; optiune = optiune - 1;

      }
      else if (optiune > 0) {
        optiune = optiune - 1;
        rowOne = String("  Settings:");
        rowTwo = String("  Set Contrast");
      }

    }

    if ( SW_state == 0) {
      if (lastSw == 1) {
        lastSw = 0; //sus
        if (optiune == 0) { //back menu
          rowOne = String("  New Game");
          rowTwo = String("  Settings");
          optionsOn = false;
          linie = 0;
          optiune = 0;
        }
        if (optiune == 1) {
          rowOne = String("  Set Contrast");
          contrastOn = true;
          optiune = EEPROM.read(100);
          lcdChanged = true;
          linie = 1;

        }
        if (optiune == 2) {
          rowOne = String("  Reset Scores?");
          rowTwo = String("  YES  <CANCEL>");
          highscoreResetOn = true;
          optiune = 1;
          lcdChanged = true;
        }
      }
    }

    if ( SW_state == 1) {
      lastSw = 1;
    }
    if (millis() - lastMoved > moveInterval) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(rowOne);
      lcd.setCursor(0, 1);
      lcd.print(rowTwo);
      lcd.setCursor(0, linie);
      if (optiune >= 1)lcd.print(">");
      else lcd.print("<");
      lastMoved = millis();
      blinkLcd(0, linie);
    }
  }
}

void highScores() {
  if (command == 1) {//dreapta
    if (optiune < 2) {
      optiune++;
      command = 5;
      lcdChanged = true;
    }
  }

  if (command == 2) { //stanga
    if (optiune > 0) {
      optiune--;
      command = 5;
      lcdChanged = true;
    }
  }

  if ( SW_state == 0) {//back
    if (lastSw == 1) {
      lastSw = 0;
      highOn = false;
      optiune = 0;
      linie = 0;
      rowOne = String("  New Game");
      rowTwo = String("  Settings");
    }
  }
  if ( SW_state == 1) {
    lastSw = 1;
  }

  if (lcdChanged) {
    lcdChanged = false;
    lcd.clear();

    rowOne = "< Score#";
    rowOne = rowOne + String(optiune + 1) + ":";
    rowOne = rowOne + readScoreEEPROM(optiune);
    lcd.setCursor(0, 0);
    lcd.print(rowOne);
    rowTwo = readStringEEPROM(optiune);
    linie = 16 - rowTwo.length();
    linie = linie / 2;
    lcd.setCursor(linie, 1);
    lcd.print(rowTwo);
    if (optiune == 0) {
      lcd.setCursor(0, 0);
      lcd.print(" ");
    }

    if (optiune != 2) {
      lcd.setCursor(15, 0);
      lcd.print(">");
    }
  }
}
void mainMenu() {
  if (optionsOn) {
    optionsMenu();
  }
  else if (highOn) {
    highScores();
  }

  else if (gameOverOn) {
    endScore();
  }

  else if (newScoreOn) {
    setHighscore();
  }

  else {
    if (command == 4) {
      command = 5;
      if (linie < 1) { //jos


        linie = linie + 1; optiune = optiune + 1;
      }
      else if (optiune < 2) {

        optiune = optiune + 1;
        rowOne = String("  Settings");
        rowTwo = String("  Highscore");
      }
    }
    if (command == 3) { //sus

      command = 5;
      if (linie > 0) {

        linie = linie - 1; optiune = optiune - 1;

      }
      else if (optiune > 0) {
        optiune = optiune - 1;
        rowOne = String("  New Game");
        rowTwo = String("  Settings");
      }

    }

    if ( SW_state == 0) {
      if (lastSw == 1) {
        lastSw = 0; //sus
        if (optiune == 0) {
          linie = 0;
          optiune = 0;
          loc = 5;
          startGame();
          return;

        }
        if (optiune == 1) {
          rowOne = String("  Settings : ");
          rowTwo = String("  Contrast");
          optionsOn = true;
          linie = 0;
          optiune = 0;
        }
        if (optiune == 2) {
          lcdChanged = true;
          highOn = true;
          optiune = 0;
        }
      }
    }
    if ( SW_state == 1) {
      lastSw = 1;
    }

    if (millis() - lastMoved > moveInterval) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(rowOne);
      lcd.setCursor(0, 1);
      lcd.print(rowTwo);
      lcd.setCursor(0, linie);
      lcd.print(">");
      lastMoved = millis();
      blinkLcd(0, linie);
    }
  }
}


void loop() {

  if (menu == true) {
    SW_state = digitalRead(SW);
    updatePositions();
    mainMenu();
  }

  else {

    if (gameOver) { //game over animation
      menu = true;
      gameOverOn = true;
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
      for (int i = snakeLenght - 1; i >= 0; i--) {
        matrix[snakeBody[0][i]][snakeBody[1][i]] = 0;
        delay(moveInterval / 2);
        tone(buzzer, 2000, 50);
        updateMatrix();
      }

    }

    else {

      updateGamePositions();
      if (millis() - foodTimer >= 5000) {
        generateFood();
      }
      speedModifier = millis();
      moveInterval = 300 - (speedModifier / 1000);

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


    score = score + 1;
    score = score + (millis() - foodTimer) / 100;
    lcd.setCursor(3, 1);
    lcd.print("Score : ");
    lcd.print(score);

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
  foodTimer = millis();
}


void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      if (matrix[row][col] != 2) {
        lc.setLed(0, row, col, matrix[row][col]);
      }
      if (matrix[row][col] == 2) {
        if (row != foodx || col != foody) {
          matrix[row][col] = 0;
        }
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
  if (command == 0) {
    if (xValue < minThreshold) {
      command = 2;
    }
    if (xValue > maxThreshold) {
      command = 1;
    }
    if (yValue > maxThreshold) {
      command = 4;
    }
    if (yValue < minThreshold) {
      command = 3;
    }
  }
  if (yValue > minThreshold && yValue < maxThreshold & xValue > minThreshold && xValue < maxThreshold) {
    command = 0;
  }
}


void updateGamePositions() {
  xValue = analogRead(xPin);
  yValue = analogRead(yPin);
  if (xValue < minThreshold) {
    if (directie != 2) {
      command = 1;
      Serial.println("stanga");
    }
  }


  if (xValue > maxThreshold) {
    if (directie != 1) {
      command = 2;
      Serial.println("dreapta");
    }
  }

  if (yValue > maxThreshold) {
    if (directie != 4) {
      command = 3;
      Serial.println("jos");
    }
  }
  if (yValue < minThreshold) {
    if (directie != 3) {
      command = 4;
      Serial.println("sus");
    }
  }
}

////EEPROM STUFF:

/*
  100 - contrast

  16 - score/255
  17 - score%255
  18-35 - name#1

  36 - score/255
  37 - score%255
  38-55 - name#2

  56 - score/255
  57 - score%255
  58-75 - name#3
*/

void writeStringEEPROM(int addrOffset, const String &strToWrite)
{
  addrOffset = 18 + addrOffset * 20;
  byte len = strToWrite.length();
  EEPROM.update(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.update(addrOffset + 1 + i, strToWrite[i]);
  }
}

String readStringEEPROM(int addrOffset)
{
  addrOffset = 18 + addrOffset * 20;
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0';
  return String(data);
}


void writeScoreEEPROM(int addrOffset, int scoreToWrite)
{
  EEPROM.update(16 + addrOffset * 20, scoreToWrite / 256);
  EEPROM.update(17 + addrOffset * 20, scoreToWrite % 256);
}

int readScoreEEPROM(int addrOffset)
{
  int data;
  data = EEPROM.read(16 + addrOffset * 20) * 256;
  data = data + EEPROM.read(17 + addrOffset * 20);
  return data;
}
