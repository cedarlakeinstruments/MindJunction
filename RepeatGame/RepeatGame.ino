#include <Wire.h>

#include <LiquidCrystal_I2C.h>

// Project sponsor: Jeff Mayhill
// Mind Junction
// Email: mayhill@slingshot.co.nz
// Creator: Cedar Lake Instruments LLC
// Date: March, 2017
//
// Description:
// User must repeat sequence displayed
// 1 Start button input
// 2 LEDs go on and off in a sequence
// 3 User-start LED on
// 4 Player has fixed time to push buttons to mimic the sequence
// 5 Show pass or fail


//
// I/O Connections:
// Arduino pin   - I/O description 
// 2 - START Game INPUT
// 3 - PB1 INPUT
// 4 - PB2 INPUT
// 5 - PB3 INPUT
// 6 - PB4 INPUT

// 7 - START LED
// 8 - LED1 OUTPUT
// 9 - LED2 OUTPUT
// 10 - LED3 OUTPUT
// 11 - LED4 OUTPUT


// Arduino pin definitions
//#define I2C_ADDR 0x3F // Define I2C Address where the PCF8574A is on my devices
#define I2C_ADDR 0x27  // Define I2C Address where the PCF8574A is
// Switch inputs
#define START_PB 2
#define PB1 3
#define PB2 4
#define PB3 5
#define PB4 6

// LED outputs
#define START_LED 7
#define LED1 8
#define LED2 9
#define LED3 10
#define LED4 11
#define BUZZER 13

// ******************************** ADJUSTMENTS ********************************
#define TIMEOUT 5
#define GAME_DONE_DELAY 5
#define MINIMUM_OFF_TIME 100
#define MAXIMUM_OFF_TIME 500
#define MINIMUM_ON_TIME 100
#define MAXIMUM_ON_TIME 2000
#define GAME_STEPS 4

// Random start delay in milliseconds
#define RANDOMIZER 1000 

// Global variables
LiquidCrystal_I2C _lcd(I2C_ADDR,16,2);
// Runs once
void setup() 
{
    pinMode(START_PB, INPUT_PULLUP);
    pinMode(PB1, INPUT_PULLUP);
    pinMode(PB2, INPUT_PULLUP);
    pinMode(PB3, INPUT_PULLUP);
    pinMode(PB4, INPUT_PULLUP);
    
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(START_LED, OUTPUT);
    initGame();
    setupLcd();
}


void initGame()
{
    digitalWrite(BUZZER, LOW);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(START_LED, LOW);
}

// Runs continuously
void loop() 
{
    static unsigned long randomDelay = 0;
    static unsigned long startTime = 0;
    static int lastInput = 0;
    int ledMap[] = {LED1, LED2, LED3, LED4};
    initGame();
    
    // Prompt user to start
    _lcd.setCursor(0,0);
    _lcd.print("      Ready     ");
    
    // wait for start push
    while( digitalRead(START_PB) == HIGH)
    {
        delay(100);
    }
    //delay(random(100, RANDOMIZER));
    
    int history[GAME_STEPS];
    // Started
    for (int i = 0; i < GAME_STEPS; i++)
    {
        // Turn on LEDs in random sequence for random times
        int maxLed = sizeof(ledMap)/sizeof(int);
        int led = random(0, maxLed);
        history[i] = led;
        
        // Turn ON randomly selected LED
        digitalWrite(ledMap[led], HIGH);
        delay(random(MINIMUM_ON_TIME, MAXIMUM_ON_TIME));
        digitalWrite(ledMap[led], LOW);
        delay(random(MINIMUM_OFF_TIME, MAXIMUM_OFF_TIME));
    }

    // Now we've played the sequence, wait for user to repeat it
    digitalWrite(START_LED, HIGH);
    
    bool done = false;
    int j = 0;
    while(!done)
    {
        delay(50);
        int currentInput = ScanInputs();
        if (currentInput != -1)
        {
            // Valid input
            if (currentInput != lastInput)
            {
                // Input change
                if (currentInput == history[j])
                {
                    if (j == GAME_STEPS-1)
                    {
                        done = true;
                        // Show end of game
                        _lcd.setCursor(0,0);
                        _lcd.print("   You Win !!!");
                        _lcd.setCursor(1,0);
                        _lcd.print("Congratulations");
                    }
                }
                else
                {
                    // Mismatch. Game over
                    _lcd.setCursor(0,0);
                    _lcd.print("Sorry, try again");
                    done = true;
                }
                j++;
            }
        }
        lastInput = currentInput;
    } // while (!done)
    delay(5000);
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

//
// Checks all input pushbuttons and returns
// -1 if no valid input
// 0 if all inputs inactive
// pin # of active input
int ScanInputs()
{
    int inputs[] = {PB1, PB2, PB3, PB4};
    int s = -1;
    for (int i=0; i < 4; i++)
    {
        if (digitalRead(inputs[i]) == LOW)
        {
            if (s == -1)
            {
                s = i;
            }
            else
            {
                // Already have an active input
                break;
            }
        }
    }
    return s;
}
