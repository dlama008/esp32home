#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <time.h> 
#include <WebSocketsServer.h>
#define DECODE_NEC          
#include <IRremote.hpp>     

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
WebSocketsServer webSocket = WebSocketsServer(81); 
WiFiServer server(80);

// --- MUUTTUJAT ---
float currentT = 0, currentH = 0, mikkeliTemp = -999, mikkeliWind = 0, currentElecPrice = -1;
bool elecDataLoaded = false;
String lastElecPayload = "";
unsigned long lastMeasureTime = 0;
float margin = 0.0;

// NÄKYMÄT (0=Tupa, 1=Mikkeli, 2=Krypto, 3=Sähkö)
int currentView = 0;
const int maxViews = 3; 
int cryptoIndex = 0;
const int maxCryptos = 5;
bool isScreenSaverOn = false;

// API & AIKA
String weatherURL = "http://api.open-meteo.com/v1/forecast?latitude=61.6886&longitude=27.2723&current_weather=true&windspeed_unit=ms";
String elecURL = "https://api.porssisahko.net/v2/latest-prices.json";
const char* ntpServer = "pool.ntp.org";
const char* timeZone = "EET-2EEST,M3.5.0/3,M10.5.0/4"; 

#include "dashboard.h"
#include "functions.h"

// Cryptojen MÄÄRITTELY
CryptoCoin coins[5] = {
  {"ripple", "XRP", 0, 0},
  {"stellar", "XLM", 0, 0},
  {"ethereum", "ETH", 0, 0},
  {"xdc-network", "XDC", 0, 0},
  {"cardano", "ADA", 0, 0}
};

// --- IR KOODIT ---
#define IR_CH_UP     0xEB14FF00
#define IR_CH_DOWN   0xED12FF00
#define IR_POWER     0xFF00FF00
#define IR_OK        0xE817FF00
#define IR_LEFT      0xE916FF00
#define IR_RIGHT     0xE718FF00

#define DHTPIN 4           
#define IR_RECEIVE_PIN 27  

DHT dht(DHTPIN, DHT11);

const unsigned char PROGMEM droplet_icon[] = {
  0x01, 0x00, 0x01, 0x00, 0x03, 0x80, 0x03, 0x80, 0x07, 0xc0, 0x0f, 0xe0, 
  0x1f, 0xf0, 0x1f, 0xf0, 0x3f, 0xf8, 0x3f, 0xf8, 0x3f, 0xf8, 0x3f, 0xf8, 
  0x1f, 0xf0, 0x1f, 0xf0, 0x0f, 0xe0, 0x03, 0x80
};

// --- PIIRTOFUNKTIOT ---

void drawHeader(const char* title) {
    display.fillRect(0, 0, 128, 12, BLACK); 
    display.setTextSize(1); display.setTextColor(WHITE);
    display.setCursor(0, 0); display.print(title);
    
    struct tm timeinfo;
    if(getLocalTime(&timeinfo)){
      display.setCursor(95, 0); 
      if(timeinfo.tm_hour < 10) display.print("0"); display.print(timeinfo.tm_hour); display.print(":");
      if(timeinfo.tm_min < 10) display.print("0"); display.print(timeinfo.tm_min);
    }
    display.drawLine(0, 12, 128, 12, WHITE);
}

// --- UUSI TYYLIKÄS LATAUSRUUTU ---
void drawLoadingScreen() {
    display.clearDisplay();
    
    // Piirretään kehykset
    display.drawRect(0, 0, 128, 64, WHITE);
    
    // Yläotsikko
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(44, 4);
    display.print("SYSTEM");
    
    // Keskipalkki (Valkoinen tausta)
    display.fillRect(0, 20, 128, 24, WHITE);
    
    // Teksti mustalla värillä keskelle (Käänteiset värit)
    display.setTextColor(BLACK);
    display.setTextSize(2);
    display.setCursor(6, 25); // Asemointi keskelle
    display.print("HAETAAN...");
    
    // Palautetaan väri valkoiseksi ja piirretään pieni "latauspalkki" alas
    display.setTextColor(WHITE);
    display.drawRect(32, 52, 64, 4, WHITE);
    display.fillRect(32, 52, 32, 4, WHITE); // Puoliksi täynnä oleva palkki

    display.display();
}

void drawLocalScreen(int x, int y) {
  drawHeader("OMA HUONE");
  display.setTextSize(2); display.setCursor(x + 10, y + 25); display.print(currentT, 1); display.print(" C");
  display.drawBitmap(x + 10, y + 45, droplet_icon, 16, 16, WHITE);
  display.setTextSize(1); display.setCursor(x + 35, y + 49); display.print("Kosteus: ");
  display.setTextSize(2); display.setCursor(x + 85, y + 47); display.print((int)currentH); display.print("%");
}

void drawWebScreen(int x, int y) {
  drawHeader("MIKKELI");
  display.setTextSize(2); display.setCursor(x + 10, y + 25); display.print(mikkeliTemp, 1); display.print(" C");
  display.setTextSize(1); display.setCursor(x + 10, y + 50); display.print("Tuuli: "); display.print(mikkeliWind, 1); display.print(" m/s");
}

void drawElecScreen(int x, int y) {
  drawHeader("PORSSISAHKO");
  if (!elecDataLoaded) { display.setCursor(x + 10, y + 30); display.print("Haetaan hintaa..."); } 
  else {
    display.setTextSize(2); display.setCursor(x + 10, y + 25); display.print(currentElecPrice, 2); display.setTextSize(1); display.print(" snt");
    display.setCursor(x + 10, y + 50); display.print("Hinta nyt");
  }
}

void drawCryptoDots(int activeIndex, int yOffset) {
    int dotStartX = 64 - (maxCryptos * 4); 
    for(int i=0; i<maxCryptos; i++) {
       if(i == activeIndex) display.fillCircle(dotStartX + (i*8), yOffset + 60, 2, WHITE); 
       else display.drawCircle(dotStartX + (i*8), yOffset + 60, 2, WHITE); 
    }
}

void drawCryptoContent(int index, int offsetX, int offsetY) {
    display.setTextWrap(false); 
    CryptoCoin c = coins[index];
    display.setTextSize(1); display.setCursor(offsetX + 0, offsetY + 16); display.print(c.symbol); display.print(" / EUR");
    display.setTextSize(2); display.setCursor(offsetX + 0, offsetY + 30);
    if (c.price >= 100) display.print(c.price, 1); else if (c.price >= 1) display.print(c.price, 3); else display.print(c.price, 5); 
    display.setTextSize(1); display.setCursor(offsetX + 0, offsetY + 50); display.print("7d: "); if (c.change >= 0) display.print("+"); display.print(c.change, 1); display.print("%");
    display.setTextWrap(true);
}

void drawView(int viewIndex, int x, int y) {
    if (viewIndex == 0) drawLocalScreen(x, y);
    else if (viewIndex == 1) drawWebScreen(x, y);
    else if (viewIndex == 2) { drawHeader("KRYPTOT"); drawCryptoContent(cryptoIndex, x, y); drawCryptoDots(cryptoIndex, y); }
    else if (viewIndex == 3) drawElecScreen(x, y);
}

// SIIRTYMÄT
void slidePageTransitionFix(int direction) {
    int steps = 6; int stepSize = SCREEN_HEIGHT / steps;
    int oldView = (direction == 1) ? currentView - 1 : currentView + 1;
    if (oldView < 0) oldView = maxViews; if (oldView > maxViews) oldView = 0;
    for (int i = 0; i <= steps; i++) {
        display.clearDisplay();
        int offset = i * stepSize; 
        if (direction == 1) { drawView(oldView, 0, 0 - offset); drawView(currentView, 0, SCREEN_HEIGHT - offset); } 
        else { drawView(oldView, 0, 0 + offset); drawView(currentView, 0, -SCREEN_HEIGHT + offset); }
        display.display();
    }
}

void slideCryptoTransition(int direction) {
    int steps = 8; int stepSize = SCREEN_WIDTH / steps;
    int oldIdx = (direction == 1) ? cryptoIndex - 1 : cryptoIndex + 1;
    if (oldIdx < 0) oldIdx = maxCryptos - 1; if (oldIdx >= maxCryptos) oldIdx = 0;
    for (int i = 0; i <= steps; i++) {
        display.clearDisplay(); drawHeader("KRYPTOT"); drawCryptoDots(cryptoIndex, 0); 
        int offset = i * stepSize * -direction; 
        drawCryptoContent(oldIdx, offset, 0); drawCryptoContent(cryptoIndex, offset + (SCREEN_WIDTH * direction), 0);
        display.display();
    }
}

void refreshDisplay() {
    if(isScreenSaverOn) return;
    display.clearDisplay(); drawView(currentView, 0, 0); display.display();
}

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  
  Wire.begin(21, 22);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { Serial.println("OLED virhe!"); for(;;); }
  
  dht.begin();
  display.clearDisplay(); display.setTextColor(WHITE); display.setTextSize(1);
  display.setCursor(0, 20); display.print("Yhdistaa WiFi..."); display.display();
  
  WiFi.begin("YOUR_SSID", "YOUR_PASSWORD");
  while (WiFi.status() != WL_CONNECTED) delay(500);
  
  display.clearDisplay(); display.setCursor(0, 20); display.print("Haetaan dataa..."); display.display();
  
  configTime(0, 0, ntpServer); setenv("TZ", timeZone, 1); tzset();
  server.begin(); webSocket.begin(); 
  
  fetchAllData();
  refreshDisplay();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_TEXT) {
        String msg = (char*)payload;
        if (msg == "refresh") {
            Serial.println("[Web] Komento: REFRESH (Päivitä tiedot)");
            drawLoadingScreen(); // Näytä latausruutu myös web-komennoilla
            fetchAllData();
            refreshDisplay();
        }
    }
}

void loop() {
  static bool wsInit = false; if (!wsInit) { webSocket.onEvent(webSocketEvent); wsInit = true; }
  webSocket.loop();
  
  WiFiClient client = server.available();
  if (client) {
    String line = client.readStringUntil('\r');
    if (line.indexOf("GET / ") >= 0) {
      client.println("HTTP/1.1 200 OK\nContent-type:text/html\nConnection: close\n");
      client.print(index_html);
    }
    client.stop();
  }

  if (IrReceiver.decode()) {
      unsigned long code = IrReceiver.decodedIRData.decodedRawData;
      if (code != 0) {
          bool action = false;
          
          if (code == IR_CH_UP) {
              currentView++; if (currentView > maxViews) currentView = 0; 
              slidePageTransitionFix(1); action = true;
          }
          else if (code == IR_CH_DOWN) {
              currentView--; if (currentView < 0) currentView = maxViews; 
              slidePageTransitionFix(-1); action = true;
          }
          else if (code == IR_RIGHT && currentView == 2) {
              cryptoIndex++; if (cryptoIndex >= maxCryptos) cryptoIndex = 0; 
              slideCryptoTransition(1); action = true;
          }
          else if (code == IR_LEFT && currentView == 2) {
              cryptoIndex--; if (cryptoIndex < 0) cryptoIndex = maxCryptos - 1; 
              slideCryptoTransition(-1); action = true;
          }
          else if (code == IR_OK) { 
              drawLoadingScreen();
              fetchAllData(); 
              action = true; 
          }
          else if (code == IR_POWER) {
              isScreenSaverOn = !isScreenSaverOn; 
              if(isScreenSaverOn) display.ssd1306_command(SSD1306_DISPLAYOFF); 
              else display.ssd1306_command(SSD1306_DISPLAYON); 
              action = true;
          }
          
          if (action && code == IR_OK) refreshDisplay();
      }
      IrReceiver.resume(); 
  }

  if (millis() - lastMeasureTime > 5000) {
    lastMeasureTime = millis();
    float t = dht.readTemperature(); float h = dht.readHumidity();
    if (!isnan(h) && !isnan(t)) { currentT = t; currentH = h; }
    
    // --- RAKENNETAAN JSON, JOSSA ON NYT MYÖS KRYPTOT ---
    String json = "{";
    json += "\"t\":\"" + String(currentT, 1) + "\",";
    json += "\"h\":\"" + String((int)currentH) + "\",";
    json += "\"mt\":\"" + String(mikkeliTemp, 1) + "\",";
    json += "\"ep\":\"" + String(currentElecPrice, 2) + "\",";
    
    // Lisätään kryptot JSON-taulukkona "c"
    json += "\"c\":[";
    for(int i=0; i<5; i++) {
       json += "{";
       json += "\"s\":\"" + coins[i].symbol + "\","; // Symboli
       json += "\"p\":" + String(coins[i].price, (coins[i].price < 10 ? 4 : 2)) + ","; // Hinta (tarkempi jos halpa)
       json += "\"ch\":" + String(coins[i].change, 1); // Muutos %
       json += "}";
       if(i < 4) json += ",";
    }
    json += "]";
    json += "}";

    webSocket.broadcastTXT(json);
    if (!isScreenSaverOn) refreshDisplay();
  }
}
