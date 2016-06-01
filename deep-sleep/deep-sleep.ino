// Include the ESP8266 WiFi library. (Works a lot like the
// Arduino WiFi library.)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiSSID[] = "the shire";
const char WiFiPSK[] = "parker7275dahlia";

// Time to sleep (in seconds):
const int sleepTimeS = 30;
const int EXT_LED = 4;


void setup() 
{
  initHardware();
  connectWiFi();
  digitalWrite(EXT_LED, HIGH);
  while (postToCjp() != 1)
  {
    delay(100);
  }
  digitalWrite(EXT_LED, LOW);
  // deepSleep time is defined in microseconds. Multiply
  // seconds by 1e6 
  ESP.deepSleep(sleepTimeS * 1000000);
}

void loop() 
{
}

void connectWiFi()
{
  byte ledStatus = LOW;

  // Set WiFi mode to station (as opposed to AP or AP_STA)
  WiFi.mode(WIFI_STA);
  // WiFI.begin([ssid], [passkey]) initiates a WiFI connection
  // to the stated [ssid], using the [passkey] as a WPA, WPA2,
  // or WEP passphrase.
  WiFi.begin(WiFiSSID, WiFiPSK);

  // Use the WiFi.status() function to check if the ESP8266
  // is connected to a WiFi network.
  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink the LED
    digitalWrite(EXT_LED, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;

    // Delays allow the ESP8266 to perform critical tasks
    // defined outside of the sketch. These tasks include
    // setting up, and maintaining, a WiFi connection.
    delay(100);
    // Potentially infinite loops are generally dangerous.
    // Add delays -- allowing the processor to perform other
    // tasks -- wherever possible.
  }
}

void initHardware()
{
  Serial.begin(9600);
  Serial.print("init hardware\n");
  pinMode(EXT_LED, OUTPUT);
}

int postToCjp()
{
  HTTPClient http;
  String postData;
  // LED turns on when we enter, it'll go off when we 
  // successfully post.
  digitalWrite(EXT_LED, HIGH);

  http.begin("http://cjparker.us/nug/api/rawDataXXX");
  http.addHeader("Content-Type", "application/json");

  int f = 0;
  int h = 0;
  int red = 0;
  int green = 0;
  int blue = 0;
  postData = "{\"tempF\" : ";
  postData.concat(f);
  postData.concat(",\"humidity\":");
  postData.concat(h);
  postData.concat(",\"RGB\": {");
  postData.concat("\"R\":");
  postData.concat(red);
  postData.concat(", \"G\":");
  postData.concat(green);
  postData.concat(",\"B\":");
  postData.concat(blue);
  postData.concat("}");
  postData.concat("}");

  Serial.print(postData);
  Serial.println();

  int httpCode = http.POST(postData);

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] POST... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();


  // Before we exit, turn the LED off.
  digitalWrite(EXT_LED, LOW);

  return 1; // Return success
}

