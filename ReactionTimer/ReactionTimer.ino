#include <Wire.h>

#include <LiquidCrystal_I2C.h>

// Project sponsor: Jeff Mayhill
// Mind Junction
// Email: mayhill@slingshot.co.nz
// Creator: Cedar Lake Instruments LLC
// Date: March, 2017
//
// Description:
// Times user reaction
// 1 Start button input
// 2 LED on after random time
// 3 Start timer
// 4 Capture user input button or timeout
// 5 Display reaction time
// 6 (optional) buzz tone depending on success or failure


//
// I/O Connections:
// Arduino pin   - I/O description 
// 2 - START INPUT
// 3 - REACT INPUT
// 5 - BUZZER 
// 6 - TESTING LED
// 7 - DONE LED

// Arduino pin definitions
//#define I2C_ADDR 0x3F // Define I2C Address where the PCF8574A is on my devices
#define I2C_ADDR 0x27  // Define I2C Address where the PCF8574A is
#define START_PB 2
#define REACT_PB 3
#define BUZZER 5
#define TESTING_LED 6
#define DONE_LED 7


// ******************************** ADJUSTMENTS ********************************
#define TIMEOUT 5
#define GAME_DONE_DELAY 5
#define EXCELLENT_TIME 500
#define GOOD_TIME 2000
// Random start delay in milliseconds

#define RANDOMIZER 1000 
// Global variables
LiquidCrystal_I2C _lcd(I2C_ADDR,16,2);
// Runs once
void setup() 
{
    pinMode(START_PB, INPUT_PULLUP);
    pinMode(REACT_PB, INPUT_PULLUP);
    pinMode(TESTING_LED, OUTPUT);
    pinMode(DONE_LED, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    
    digitalWrite(BUZZER, LOW);
    digitalWrite(DONE_LED, LOW);
    digitalWrite(TESTING_LED, LOW);
    
    setupLcd();
    _lcd.setCursor(0,0);
    _lcd.print("      Ready     ");
}

// Runs continuously
void loop() 
{
    static int state = 0;
    static unsigned long randomDelay = 0;
    static unsigned long startTime = 0;
    unsigned long range=0;
    
    switch(state)
    {
        case 0:
            // Wait to start
            digitalWrite(TESTING_LED, LOW);
            digitalWrite(DONE_LED, LOW);
            
            if (digitalRead(START_PB) == LOW)
            {
                randomDelay = random(500, RANDOMIZER) + millis();
                state = 1;
            }
            if (checkEarlyStart())
            {
                state = 9;
            }
            break;
        case 1:
            // Random time to start timing
            if (millis() < randomDelay)
            {
                if (checkEarlyStart())
                {
                    state = 9;
                }
            }
            else
            {
                _lcd.setCursor(0,0);
                _lcd.print("  Timing...");
                digitalWrite(TESTING_LED, HIGH);
                startTime = millis();
                state = 2;
            }
            break;
        case 2:
            // Timing 
            printTimingInfo(startTime);
            
            if(digitalRead(REACT_PB) == LOW)
            {
                digitalWrite(DONE_LED, HIGH);
                _lcd.setCursor(0,0);
                range = (unsigned long)(millis()-startTime);
                if (range < EXCELLENT_TIME)
                {
                    _lcd.print("  Very Good     ");
                }
                else if (range < GOOD_TIME)
                {
                    _lcd.print("  Good Time     ");
                }
                else 
                {
                    _lcd.print("  Not Bad       ");
                }
                state = 3;
            }
            // Check for timeout
            else 
            {
                if (millis() > (unsigned long)(startTime + TIMEOUT * 1000))
                {
                    _lcd.setCursor(0,0);
                    _lcd.print("Sorry, too long");
                    state = 3;
                }
            }
            break;     
       case 3:
           // Game over
           delay(GAME_DONE_DELAY*1000);
           _lcd.clear();
           _lcd.setCursor(0,0);
           _lcd.print("      Ready     ");
           state = 0;
           break;           
       case 9:
            // Error state
            state = 3;            
            break;       
    }
}

bool checkEarlyStart()
{
    if (digitalRead(REACT_PB) == LOW)
    {
        _lcd.setCursor(0,0);
        _lcd.print(" DON'T PUSH YET");
        for (int i=0; i < 3; i++)
        {
            digitalWrite(BUZZER, HIGH);
            delay(100);
            digitalWrite(BUZZER, LOW);
            delay(250);
        }
        
        return true;
    }
    else
    {
        return false;
    }
}

// Initialize the LCD
void setupLcd()
{
    // LCD setup   
    _lcd.init();
    delay(100);
    // Switch on the backlight
    _lcd.backlight();
}

void printTimingInfo(long startTime)
{
    unsigned long timeNow = millis();
    int sec = ((unsigned long)(timeNow-startTime))/1000;
    int ms = ((unsigned long)(timeNow-startTime))%1000;
    _lcd.setCursor(6,1); 
    _lcd.print(sec); _lcd.print("."); _lcd.print(ms); _lcd.print("   ");
}

