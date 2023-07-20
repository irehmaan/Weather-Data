#include <Arduino.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <DHT_U.h>
#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>

#endif

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Your Wi-Fi Name/SSID"
#define WIFI_PASSWORD "Wi-Fi Password"
// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "Your Firebase Web API Key"

/* 3. Define the RTDB URL */
#define DATABASE_URL "Your Firebase RTDB URL"  //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "Registered Email"
#define USER_PASSWORD "Password"

// Define Firebase Data object
FirebaseData fbdo;
FirebaseData stream;

FirebaseAuth auth;
FirebaseConfig config;

#define DHTPin 19
#define DHTTYPE DHT11

DHT dht(DHTPin, DHTTYPE);
Adafruit_BMP085 bmp;

String parentPath = "/WeatherData";
unsigned long sendDataPrevMillis = 0;
String uid;

String childPath[2] = {"/node1", "/node2"};

float h;
float t;
float Pressure;
int Altitude;
int Real_Altitude;
float Pressure_at_sealevel;




void streamCallback(MultiPathStreamData stream)
{
  size_t numChild = sizeof(childPath) / sizeof(childPath[0]);

  for (size_t i = 0; i < numChild; i++)
  {
    if (stream.get(childPath[i]))
    {
      Serial.printf("path: %s, event: %s, type: %s, value: %s%s", stream.dataPath.c_str(), stream.eventType.c_str(), stream.type.c_str(), stream.value.c_str(), i < numChild - 1 ? "\n" : "");
    }
  }

  Serial.println();

  // This is the size of stream payload received (current and max value)
  // Max payload size is the payload size under the stream path since the stream connected
  // and read once and will not update until stream reconnection takes place.
  // This max value will be zero as no payload received in case of ESP8266 which
  // BearSSL reserved Rx buffer size is less than the actual stream payload.
  Serial.printf("Received stream payload size: %d (Max. %d)\n\n", stream.payloadLength(), stream.maxPayloadLength());
}

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
    Serial.println("stream timed out, resuming...\n");

  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}

void setup()
{
  dht.begin();
   if(!bmp.begin()) {
    Serial.println("Not a valid BMP 180 Sensor, check wiring !");
  }
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  // Or use legacy authenticate method
  // config.database_url = DATABASE_URL;
  // config.signer.tokens.legacy_token = "<database secret>";

  // To connect without auth in Test Mode, see Authentications/TestMode/TestMode.ino

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

  // Recommend for ESP8266 stream, adjust the buffer size to match your stream data size
#if defined(ESP8266)
  stream.setBSSLBufferSize(2048 /* Rx in bytes, 512 - 16384 */, 512 /* Tx in bytes, 512 - 16384 */);
#endif

  // The data under the node being stream (parent path) should keep small
  // Large stream payload leads to the parsing error due to memory allocation.

  // The MultiPathStream works as normal stream with the payload parsing function.

  if (!Firebase.beginMultiPathStream(stream, parentPath))
    Serial.printf("sream begin error, %s\n\n", stream.errorReason().c_str());

  Firebase.setMultiPathStreamCallback(stream, streamCallback, streamTimeoutCallback);

  /** Timeout options, below is default config.

  //WiFi reconnect timeout (interval) in ms (10 sec - 5 min) when WiFi disconnected.
  config.timeout.wifiReconnect = 10 * 1000;

  //Socket begin connection timeout (ESP32) or data transfer timeout (ESP8266) in ms (1 sec - 1 min).
  config.timeout.socketConnection = 30 * 1000;

  //ESP32 SSL handshake in ms (1 sec - 2 min). This option doesn't allow in ESP8266 core library.
  config.timeout.sslHandshake = 2 * 60 * 1000;

  //Server response read timeout in ms (1 sec - 1 min).
  config.timeout.serverResponse = 10 * 1000;

  //RTDB Stream keep-alive timeout in ms (20 sec - 2 min) when no server's keep-alive event data received.
  config.timeout.rtdbKeepAlive = 45 * 1000;

  //RTDB Stream reconnect timeout (interval) in ms (1 sec - 1 min) when RTDB Stream closed and want to resume.
  config.timeout.rtdbStreamReconnect = 1 * 1000;

  //RTDB Stream error notification timeout (interval) in ms (3 sec - 30 sec). It determines how often the readStream
  //will return false (error) when it called repeatedly in loop.
  config.timeout.rtdbStreamError = 3 * 1000;

  */
    // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);
 
}

void loop()
{
//   h = dht.readHumidity();
//  // Read temperature as Celsius (the default)
//   t = dht.readTemperature();
//    // Check if any reads failed and exit early (to try again).
//  if (isnan(h) || isnan(t)) {
//    Serial.println(F("Failed to read from DHT sensor!"));
//    return;
//  }
   
  // Firebase.ready() should be called repeatedly to handle authentication tasks.

  if (Firebase.ready() && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    // Firebase RTDB Path 
    String path = "/users/"+uid+"/WeatherData";
    Serial.print("\nSet json...");

    FirebaseJson json;

    for (size_t i = 0; i < 10; i++)
    {
      
//      json.set("DHT11/Temperature", t);
      json.set("BMP180/Humidity", h);
      json.set("BMP180/Pressure", Pressure);
      json.set("BMP180/Altitude",Altitude);
      json.set("BMP180/Temp",t);
      json.set("BMP180/Pressure At Sea Level", Pressure_at_sealevel);
      
      // The response is ignored in this async function, it may return true as long as the connection is established.
      // The purpose for this async function is to set, push and update data instantly.
      Firebase.setJSONAsync(fbdo, path.c_str(), json);
    
    }

    Serial.println("ok\n");
  }
  tempSensor();
  BMP();
}
void tempSensor() {

   h = dht.readHumidity();
  // Read temperature as Celsius (the default)
   t = dht.readTemperature();
    // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
//    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
 
  Serial.print(F("Humidity: "));
  Serial.print(h);
//  Serial.print(F("%  Temperature: "));
//  Serial.print(t);
  
}
void BMP() {
//  Serial.print("Pressure = ");
  Pressure = bmp.readPressure();
//  Serial.print(bmp.readPressure());
//  Serial.println(" Pa");
    
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
//  Serial.print("Altitude = ");
  Altitude = bmp.readAltitude();
//  Serial.print(bmp.readAltitude());
//  Serial.println(" meters");
//
//  Serial.print("Pressure at sealevel= ");
  Pressure_at_sealevel = bmp.readSealevelPressure();
//  Serial.print(bmp.readSealevelPressure());
//  Serial.println(" Pa");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
//  Serial.print("Real altitude = ");
  Real_Altitude = bmp.readAltitude(102000);
//  Serial.print(bmp.readAltitude(102000));
//  Serial.println(" meters");
 t = bmp.readTemperature();


}
