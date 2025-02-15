#include <Arduino.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>

/* Definições da Rede */
const char* SSID = "";      // Nome da rede WiFi
const char* PASSWORD = "";  // Senha da rede WiFi

WiFiServer server(80);

/* Definições do LCD */
#define LCD_ADDR    0x27
#define LCD_COLUMNS 16
#define LCD_ROWS    2

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLUMNS, LCD_ROWS);  // Endereço I2C do LCD 16x2

/* Definições de I/O */
#define SENSOR_FINISH_PIN    27
#define SENSOR_START_PIN     26
#define BOTAO_RESET_PIN      15
#define LED_STATUS           2

/* Variáveis auxiliares */
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
    lcd.print(elapsedTime);
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

void setup() 
{
    Serial.begin(115200);
    pinMode(LED_STATUS, OUTPUT);
    pinMode(BOTAO_RESET_PIN, INPUT);
    pinMode(SENSOR_START_PIN, INPUT);
    pinMode(SENSOR_FINISH_PIN, INPUT);
    
    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED) 
    {
        digitalWrite(LED_STATUS, HIGH);
        delay(500);
        Serial.print(".");
        digitalWrite(LED_STATUS, LOW);
        delay(250);
    }
 
    Serial.println("");
    Serial.println("WiFi conectada.");
    Serial.println("Endereço de IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_STATUS, HIGH);
 
    server.begin();

    lcd.init();
    lcd.backlight();    // Ativa a luz de fundo
    lcd.setCursor(0,0);
    lcd.print("Cronometro:");
    lcd.setCursor(0,1);
    lcd.print("Pressione START");
}

void loop() 
{
    if (isRunning) 
    {
        elapsedTime = millis() - startTime;
        updateDisplay();
    }

    WiFiClient client = server.available();

    if (client) 
    {
        String currentLine = "";
        while (client.connected()) 
        {
            if (client.available()) 
            {
                char c = client.read();
                if (c == '\n') 
                {
                    if (currentLine.length() == 0) 
                    {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();

                        client.println("<!DOCTYPE html><html><head><title>Cronômetro</title></head><body>");
                        client.println("<script>");
                        client.println("function refreshPage() {");
                        client.println("  location.reload(true);");
                        client.println("}");
                        client.println("setInterval(refreshPage, 1000);");  // Refresh every 1 second
                        client.println("</script></head><body>");
                        client.println("<h1>Cronometro</h1>");
                        client.println("<p>Tempo: " + String(elapsedTime) + " ms</p>");
                        client.println("</body></html>");
                        break;
                    } 
                    else 
                    {
                        currentLine = "";
                    }
                } 
                else if (c != '\r') 
                {
                    currentLine += c;
                }
            }
        }
        client.stop();
    }

    // Sensor para iniciar o cronômetro
    if (digitalRead(SENSOR_START_PIN) == LOW) 
    {
        startTimer();   
    }

    // Sensor para pausar o cronômetro
    if (digitalRead(SENSOR_FINISH_PIN) == LOW) 
    {
        pauseTimer();
    }

    // Botão para reiniciar o cronômetro
    if (digitalRead(BOTAO_RESET_PIN) == HIGH) 
    {
        resetTimer();
        delay(500);  // Debounce
    }
}
