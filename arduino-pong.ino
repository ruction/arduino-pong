#include <Wire.h>                  // This library allows you to communicate with I2C
#include <Adafruit_GFX.h>          // Adafruit GFX graphics core library
#include <Adafruit_SSD1306.h>      // Driver library for 'monochrome' 128x64 and 128x32 OLEDs


// Define the PINS you're goint to use on your Arduino Nano
int controller1 = 2;               // ANALOG 2
int controller2 = 3;               // ANALOG 3
int ledPin = 4;                    // DIGITAL 4
int btnPin = 5;                    // DIGITAL 5

// Define variables
int buttonState = 0;               // HIGH = Pressed
int gameState = 0;                 // 0 = Home, 1 = Game, 2 = End

int controllerValue1 = 0;          // variable to store the value coming from the potentiometer
int controllerValue2 = 0;          // variable to store the value coming from the potentiometer

int paddlePositionPlayer1 = 0;
int paddlePositionPlayer2 = 0;  

int scorePlayer1 = 0;    
int scorePlayer2 = 0;

int ballX = 128/2;      
int ballY = 64/2;
int ballSpeedX = 2;
int ballSpeedY = 1;

#define OLED_RESET 4

Adafruit_SSD1306 display(OLED_RESET);
    
#if (SSD1306_LCDHEIGHT != 64)
  #error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif



void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(btnPin, INPUT);
  
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
  display.clearDisplay();

  digitalWrite(ledPin, HIGH);                 // Status led on...
}

void loop() {
 
  // Read controller value and calculate paddle position
  controllerValue1 = analogRead(controller1);
  controllerValue2 = analogRead(controller2);

  paddlePositionPlayer1 = controllerValue1 * (46.0 / 1023.0);
  paddlePositionPlayer2 = controllerValue2 * (46.0 / 1023.0);

  // Set button state
  buttonState = digitalRead(btnPin);

  if (buttonState == HIGH && gameState == 0) {
      gameState = 1;
      delay(100);
  } else if (buttonState == HIGH && (gameState == 1 || gameState == 2)) {
      gameState = 0;
      scorePlayer1 = 0;
      scorePlayer2 = 0;
      ballX = 128/2;
      ballY = 64/2;
      delay(100);
  }
  
  if (gameState == 0) {
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(40, 18);
    display.println("PONG");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(32, 38);
    display.println("press start");
    display.display();
    display.clearDisplay();      
  }

  if (gameState == 1) {
    drawField(scorePlayer1, scorePlayer2);
    
    collisionControl();
    drawBall();
    
    display.display();
    display.clearDisplay();
  }

  if (gameState == 2) {
    drawField(scorePlayer1, scorePlayer2);

    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    if (scorePlayer1 == 2) {
      display.setCursor(15, 30);
    } else if (scorePlayer2 == 2) {
      display.setCursor(77, 30);
    }
    display.println("winner!");
    display.display();
    display.clearDisplay();   
  }
}

void drawField(int score1, int score2) {
    display.fillRect(0, round(paddlePositionPlayer1), 2, 18, 1);
    display.fillRect(126, round(paddlePositionPlayer2), 2, 18, 1);
  
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(55, 0);
    display.print(score1);
    display.print(":");
    display.print(score2);

    display.fillRect(63, 12, 1, 5, 1);
    display.fillRect(63, 22, 1, 5, 1);
    display.fillRect(63, 32, 1, 5, 1);
    display.fillRect(63, 42, 1, 5, 1);
    display.fillRect(63, 52, 1, 5, 1);
    display.fillRect(63, 62, 1, 5, 1);
}

void collisionControl() {
  //bounce from top and bottom
  if (ballY >= 64 - 2 || ballY <= 0) {
    ballSpeedY *= -1;
  } 

  //score points if ball hits wall behind player
  if (ballX >= 128 - 2 || ballX <= 0) {
    if (ballSpeedX > 0) {
      scorePlayer1++;
      ballX = 128 / 4;
    }
    if (ballSpeedX < 0) {
      scorePlayer2++;
      ballX = 128 / 4 * 3;
    }  
    if (scorePlayer1 == 2 || scorePlayer2 == 2) {
      gameState = 2;
    }
  }

  //bounce from player1
  if (ballX >= 0 && ballX <= 2 && ballSpeedX < 0) {
    if (ballY > round(paddlePositionPlayer1) - 2 && ballY < round(paddlePositionPlayer1) + 18) {
      ballSpeedX *= -1;
    }
  }
  //bounce from player2
  if (ballX >= 128-2-2 && ballX <= 128-2 && ballSpeedX > 0) {
    if (ballY > round(paddlePositionPlayer2) - 2 && ballY < round(paddlePositionPlayer2) + 18) {
      ballSpeedX *= -1;
    }

  }
}

void drawBall() {

  display.fillRect(ballX, ballY, 2, 2, 1);
  
  ballX += ballSpeedX;
  ballY += ballSpeedY;
}

