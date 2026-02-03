#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <time.h>

struct CryptoCoin { String id; String symbol; float price; float change; };

// Extern-viittaukset
extern CryptoCoin coins[5];
extern float currentT, currentH, mikkeliTemp, mikkeliWind, currentElecPrice, margin;
extern bool elecDataLoaded;
extern String lastElecPayload, weatherURL, elecURL;
extern int currentView, cryptoIndex;
extern bool isScreenSaverOn;

// --- SÄHKÖHINNAN ÄLYKÄS HAKU ---
void parseElectricityPrice() {
    if (lastElecPayload == "") return;
    DynamicJsonDocument doc(24576);
    DeserializationError error = deserializeJson(doc, lastElecPayload);
    if (error) return;

    JsonArray prices = doc["prices"];
    time_t nowRaw = time(NULL);
    struct tm * utc = gmtime(&nowRaw);

    if (utc->tm_year < 100) return;

    for (JsonObject p : prices) {
        const char* s = p["startDate"]; 
        int y, m, d, h;
        sscanf(s, "%d-%d-%dT%d", &y, &m, &d, &h);

        if (y == (utc->tm_year + 1900) && 
            m == (utc->tm_mon + 1) && 
            d == utc->tm_mday && 
            h == utc->tm_hour) {
            
            currentElecPrice = (float)p["price"] + margin;
            elecDataLoaded = true;
            break;
        }
    }
}

void fetchWeather() {
    HTTPClient http; http.begin(weatherURL);
    if (http.GET() == 200) {
      DynamicJsonDocument d(2048); deserializeJson(d, http.getString());
      mikkeliTemp = d["current_weather"]["temperature"];
      mikkeliWind = d["current_weather"]["windspeed"];
    } http.end();
}

// --- UUSI KRYPTOHAKU (CryptoCompare) ---
void fetchCrypto() {
    WiFiClientSecure s; s.setInsecure(); HTTPClient h;
    
    // CryptoComparen API: Haetaan XRP, XLM, ETH, XDC, ADA euroissa
    // Huom: CryptoCompare käyttää symboleita (ticker), ei pitkiä nimiä
    String url = "https://min-api.cryptocompare.com/data/pricemultifull?fsyms=XRP,XLM,ETH,XDC,ADA&tsyms=EUR";
    
    Serial.println("[API] Haetaan kryptoja (CryptoCompare)...");
    
    h.begin(s, url);
    int httpCode = h.GET();
    
    if (httpCode == 200) {
      String payload = h.getString();
      DynamicJsonDocument d(24000); // Varataan reilusti muistia
      DeserializationError error = deserializeJson(d, payload);
      
      if (error) {
          Serial.print("[API] JSON Virhe: "); Serial.println(error.c_str());
          return;
      }

      // CryptoCompare palauttaa datan muodossa: RAW -> SYMBOLI -> VALUUTTA -> DATAT
      JsonObject raw = d["RAW"];
      
      // Päivitetään kolikot suoraan symboleilla
      // 0: Ripple (XRP)
      if (raw.containsKey("XRP")) {
          coins[0].price = raw["XRP"]["EUR"]["PRICE"];
          coins[0].change = raw["XRP"]["EUR"]["CHANGEPCT24HOUR"];
      }
      // 1: Stellar (XLM)
      if (raw.containsKey("XLM")) {
          coins[1].price = raw["XLM"]["EUR"]["PRICE"];
          coins[1].change = raw["XLM"]["EUR"]["CHANGEPCT24HOUR"];
      }
      // 2: Ethereum (ETH)
      if (raw.containsKey("ETH")) {
          coins[2].price = raw["ETH"]["EUR"]["PRICE"];
          coins[2].change = raw["ETH"]["EUR"]["CHANGEPCT24HOUR"];
      }
      // 3: XDC Network (XDC)
      if (raw.containsKey("XDC")) {
          coins[3].price = raw["XDC"]["EUR"]["PRICE"];
          coins[3].change = raw["XDC"]["EUR"]["CHANGEPCT24HOUR"];
          Serial.printf("[API] XDC löytyi! Hinta: %.4f\n", coins[3].price);
      } else {
          Serial.println("[API] XDC puuttuu vastauksesta.");
      }
      // 4: Cardano (ADA)
      if (raw.containsKey("ADA")) {
          coins[4].price = raw["ADA"]["EUR"]["PRICE"];
          coins[4].change = raw["ADA"]["EUR"]["CHANGEPCT24HOUR"];
      }

      Serial.println("[API] Kryptot päivitetty onnistuneesti.");
    } else {
      Serial.print("[API] Virhe haussa: "); Serial.println(httpCode);
    }
    h.end();
}

void fetchElectricityRaw() {
    WiFiClientSecure s; s.setInsecure(); HTTPClient h; h.begin(s, elecURL);
    if (h.GET() == 200) { lastElecPayload = h.getString(); parseElectricityPrice(); }
    h.end();
}

void fetchAllData() { fetchWeather(); fetchCrypto(); fetchElectricityRaw(); }

#endif