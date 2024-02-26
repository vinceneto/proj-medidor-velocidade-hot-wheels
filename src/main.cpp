#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDR    0x27
#define LCD_COLUMNS 16
#define LCD_ROWS    2

#define SENSOR_FINISH_PIN    9
#define SENSOR_START_PIN     8
#define BOTAO_RESET_PIN      7

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLUMNS, LCD_ROWS);  // Endereço I2C do LCD 16x2

unsigned long startTime;
unsigned long elapsedTime = 0;
boolean isRunning = false;

void startTimer() 
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Cronometro:");
    startTime = millis() - elapsedTime;
    isRunning = true;
}

void pauseTimer()
{
  isRunning = false;
}

void updateDisplay() 
{
    lcd.setCursor(0,1);
    lcd.print("Tempo: ");
    lcd.print(elapsedTime);  // Converte millis para segundos
    lcd.print(" ms");
}

void resetTimer() 
{
    elapsedTime = 0;
    updateDisplay();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Cronometro:");
    lcd.setCursor(0,1);
    lcd.print("Pressione START");
}
void setup() {
    lcd.begin(16,2);   // Inicializa o LCD
    lcd.backlight();    // Ativa a luz de fundo
    lcd.setCursor(0,0);
    lcd.print("Cronometro:");
    lcd.setCursor(0,1);
    lcd.print("Pressione START");
}

void loop() {
    if (isRunning) {
        elapsedTime = millis() - startTime;
        updateDisplay();
    }

    // Botão para iniciar/pausar o cronômetro
    if (digitalRead(SENSOR_START_PIN) == LOW) {
        startTimer();   
    }

    if (digitalRead(SENSOR_FINISH_PIN) == LOW) {
        pauseTimer();
    }

    // Botão para reiniciar o cronômetro
    if (digitalRead(BOTAO_RESET_PIN) == HIGH) {
        resetTimer();
        delay(500);  // Debounce
    }
}
