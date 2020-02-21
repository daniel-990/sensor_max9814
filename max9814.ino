#include <ESP8266WiFi.h>

#include <SPI.h>
#include <Wire.h>

//IP or name of address root: ie: google.com
//NOT google.com/nothing/after/the/dotcom.html
const char* hostGet = "danielarangovillegas.ml";

const char* ssid = "nombre de la red";
const char* password = "contraseña de la red";

//sensor de audio
const int sampleWindow = 250; // Sample window width in mS
unsigned int sample;

int WiFiCon() {
    // Check if we have a WiFi connection, if we don't, connect.
    int xCnt = 0;

    if (WiFi.status() != WL_CONNECTED) {

        Serial.println();
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);

        WiFi.mode(WIFI_STA);

        WiFi.begin(ssid, password);

        while (WiFi.status() != WL_CONNECTED && xCnt < 50) {
            delay(500);
            Serial.print(".");
            xCnt++;
        }

        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFiCon=0");
            return 0; //never connected
        } else {
            Serial.println("WiFiCon=1");
            Serial.println("");
            Serial.println("WiFi connected");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());
            return 1; //1 is initial connection
        }

    } else {
        Serial.println("WiFiCon=2");
        return 2; //2 is already connected

    }
}


void setup() {
    Serial.begin(115200);
    WiFiCon();
}

void loop() {

    if (WiFiCon() > 0) {

        unsigned long startMillis = millis(); // Start of sample window
        unsigned int peakToPeak = 0; // peak-to-peak level
        unsigned int signalMax = 0;
        unsigned int signalMin = 1024;

        // collect data for 250 mS
        while (millis() - startMillis < sampleWindow) {
            sample = analogRead(0);
            if (sample < 1024) { // toss out spurious readings
                if (sample > signalMax) {
                    signalMax = sample; // save just the max levels
                } else if (sample < signalMin) {
                    signalMin = sample; // save just the min levels
                }
            }
        }

        peakToPeak = signalMax - signalMin; // max - min = peak-peak amplitude
        double volts = (peakToPeak * 10.0) / 1024; // convert to volts
        double dBW = 85;
        Serial.println(volts);

        if (volts < 4.2) {
            dBW = 20.0 * log10(volts * 200) + 20;
            Serial.print(String(volts) + ", sonido en: , ");
            Serial.println(String(dBW) + "dB");


            WiFiClient clientGet;
            const int httpGetPort = 80;

            //the path and file to send the data to:
            String urlGet = "/data.php";


            // We now create and add parameters:
            String src = String(dBW) + "dB";

            urlGet += "?datos=" + src;

            Serial.print(">>> Connecting to host: ");
            Serial.println(hostGet);

            if (!clientGet.connect(hostGet, httpGetPort)) {
                Serial.print("Connection failed: ");
                Serial.print(hostGet);
            } else {
                clientGet.println("GET " + urlGet + " HTTP/1.1");
                clientGet.print("Host: ");
                clientGet.println(hostGet);
                clientGet.println("User-Agent: ESP8266/1.0");
                clientGet.println("Connection: close\r\n\r\n");

                unsigned long timeoutP = millis();
                while (clientGet.available() == 0) {

                    if (millis() - timeoutP > 10000) {
                        Serial.print(">>> Client Timeout: ");
                        Serial.println(hostGet);
                        clientGet.stop();
                        return;
                    }
                }

                //just checks the 1st line of the server response. Could be expanded if needed.
                while (clientGet.available()) {
                    String retLine = clientGet.readStringUntil('\r');
                    Serial.println(retLine);
                    break;
                }

            } //end client connection if else

            Serial.print(">>> Closing host: ");
            Serial.println(hostGet);

            clientGet.stop();


        }
        delay(30000);


    }

}
