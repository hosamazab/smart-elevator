#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Ticker.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "PubSubClient.h" //// MQTT

HTTPClient http;
WiFiClient wifiClient;
PubSubClient client(wifiClient);
const int buildID = 8;
const char* serverName = "http://154.238.228.9:5109/api/IOT/Create";
const char* ssid = "mohamed madkour";
const char* password = "mghm@1993";

// //*
bool cst=false;

WebServer server;
WebSocketsServer webSocket = WebSocketsServer(81);
Ticker timer;
int leds5[5] = {26, 27, 14, 15, 2};
int segpinin[] = {36, 39, 34, 35, 32, 33, 25};
const int output21 = 19;
const int output22 = 16;
const int output23 = 17;
const int output24 = 18;
const int output25 = 21;
const int output26 = 23;
const int output27 = 22;
const int output13 = 13;
byte segcodein[10][7] = {
  {1, 1, 1, 1, 1, 1, 0},  // display 0
  {0, 1, 1, 0, 0, 0, 0},  // display 1
  {1, 1, 0, 1, 1, 0, 1},  // display 2
  {1, 1, 1, 1, 0, 0, 1},  // display 3
  {0, 1, 1, 0, 0, 1, 1},  // display 4
  {1, 0, 1, 1, 0, 1, 1},  // display 5
  {1, 0, 1, 1, 1, 1, 1},  // display 6
  {1, 1, 1, 0, 0, 0, 0},  // display 7
  {1, 1, 1, 1, 1, 1, 1},  // display 8
  {1, 1, 1, 1, 0, 1, 1}   // display 9
};
byte segcodeerror[25][7] = { //array of number 0-9 in order from a of g/
  //a  b  c  d  e  f  g
  { 1, 1, 0, 0, 0, 1, 0},  //display 0/
  { 0, 0, 0, 0, 0, 0, 1},  //display 1/
  { 1, 0, 0, 0, 0, 0, 0},  //display 2/
  { 0, 1, 0, 0, 0, 1, 1},  //display 3/
  { 0, 0, 1, 0, 1, 0, 1},  //display 4/
  { 0, 1, 0, 0, 0, 0, 0},  //display 5/
  { 0, 0, 1, 0, 0, 0, 0},  //display 6/
  { 0, 0, 0, 1, 0, 0, 0},  //display 7/
  { 0, 0, 1, 1, 1, 0, 1},  //display 8/
  { 0, 0, 0, 0, 1, 0, 0},  //display 9/
  { 1, 0, 0, 0, 1, 0, 1},  //display 10/
  { 0, 0, 1, 0, 0, 1, 1},  //display 11/
  { 0, 1, 0, 0, 0, 1, 0},  //display 12/
  { 1, 0, 1, 0, 1, 0, 1},  //display 13/
  { 1, 0, 0, 0, 0, 0, 1},  //display 14/
  { 0, 0, 0, 1, 0, 0, 1},  //display 15/
  { 0, 0, 1, 0, 0, 1, 0},  //display 16/
  { 0, 1, 0, 0, 1, 0, 0},  //display 17/
  { 1, 0, 0, 1, 0, 0, 0},  //display 18/
  { 1, 1, 0, 0, 1, 1, 0},  //display 19/
  { 1, 1, 1, 0, 1, 1, 0},  //display 20/
  { 1, 0, 0, 0, 1, 1, 1},  //display 21/
  { 0, 1, 1, 0, 1, 1, 0},  //display 22/
  { 0, 0, 0, 0, 1, 1, 1},  //display 23/
  { 1, 1, 1, 0, 0, 1, 0},  //display 24/
};

int counter = 0;
bool read_data = false;
int lastnumber;
int x2;
unsigned long previousActionMillis = 0;
const unsigned long actionInterval = 15000;
char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>WebSocket Client</title>
</head>

<body>
  <h4>WebSocket Client served from the ELVE 5 Board!</h4>
  <p>FLOOR: <span id="lastnumber">0</span></p>
  <p>STATUSE: </p>
  <p>5 IED: <span id="data5">0</span></p>
  <p>CODE ERROR: <span id="code">0</span></p>
  <button onclick="sendToServer('led1')">CALL 1</button>
  <button onclick="sendToServer('led2')">CALL 2</button>
  <button onclick="sendToServer('led3')">CALL 3</button>
  <button onclick="sendToServer('led4')">CALL 4</button>
  <button onclick="sendToServer('led5')">CALL 5</button>
  <button onclick="sendToServer('led6')">CALL 6</button>
  <button onclick="sendToServer('led7')">CALL 7</button>
  <button onclick="sendToServer('cst')">CST</button>
  <div id="response"></div>

  <script>
    var Socket;

    function init() {
      Socket = new WebSocket('ws://' + window.location.hostname + ':81/');

      Socket.onmessage = function (event) {
        var data = JSON.parse(event.data);
        console.log(data);
        document.getElementById("lastnumber").innerText = data["lastnumber"];  // Update the displayed number
        document.getElementById("data5").innerText = data["data"];
        document.getElementById("code").innerText = data["code"];
       
        
        document.getElementById("response").innerText = "Response: " + data["status"]; // Display the response
      }
    }

    function sendToServer(message) {
      if (Socket.readyState === WebSocket.OPEN) {
        Socket.send(message);
      }
    }

    window.onload = function () {
      init();
    };
  </script>
</body>

</html>

)=====";

void setup() {
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP Address (AP): ");
  Serial.println(WiFi.localIP());
  pinMode(output22, OUTPUT); //set up LED
  pinMode(output23, OUTPUT); //set up LED
  pinMode(output24, OUTPUT); //set up LED
  pinMode(output21, OUTPUT); //set up LED
  pinMode(output25, OUTPUT); //set up LED
  pinMode(output27, OUTPUT); //set up LED  
  pinMode(output26, OUTPUT); //set up LED
  pinMode(output13, OUTPUT); //set up LED
  http.begin(serverName);

  for (int a = 0; a < 7; a++) {
    pinMode(segpinin[a], INPUT);
  } 
  
  for (int a = 0; a < 5; a++) // for loop for setting the pins as input*/
  {
    pinMode(leds5[a], INPUT);
  }

  server.on("/", [](){
    server.send_P(200, "text/html", webpage);  
  });

  server.begin();
  webSocket.begin();
  timer.attach(1, updateCounter);
  webSocket.onEvent(webSocketEvent);
} 
void sendHttpPostRequest(JsonDocument& jsonDoc) {
  // Serialize JSON document to a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Initialize HTTPClient object
  HTTPClient http;

  // Set target URL for the HTTP POST request
  http.begin(wifiClient, serverName);

  // Set headers for the HTTP request
  http.addHeader("Content-Type", "application/json");

  // Send HTTP POST request with JSON payload
  int httpResponseCode = http.POST(jsonString);

  // Check HTTP response code
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    // Print response payload (if any)
    String response = http.getString();
    Serial.println("Response payload: ");
    Serial.println(response);
  } else {
    Serial.print("Error during HTTP POST request. HTTP Error code: ");
    Serial.println(httpResponseCode);
  }

  // End HTTP session
  http.end();


}
unsigned long previousMillis = 0;
const long interval = 20000;
byte val[10][7] = {0};
int readDigit(int b) //Function to initialize each segment/
{

  bool x = false;
  for (int a = 0; a < 7; a++)
  {
    val[0][a] = digitalRead(segpinin[a]); /* instructing the respective segments for the numbers from 0 to 9 */
  }
  // Serial.print(val[0][0]);
  for (int i = 0; i < 7; i++) {
    if (val[0][i] == segcodein[b][i]) {

      x = true;
    }
    else {
      x = false;
      break;
    }
  }
  if (x == true) {

    return b;
  }
  else return 26;
} 
int readDigiterror(int b) //Function to initialize each segment/
{

  bool x = false;
  for (int a = 0; a < 7; a++)
  {
    val[0][a] = digitalRead(segpinin[a]); /* instructing the respective segments for the numbers from 0 to 9 */
  }
  // Serial.print(val[0][0]);
  for (int i = 0; i < 7; i++) {
    if (val[0][i] == segcodeerror[b][i]) {

      x = true;
    }
    else {
      x = false;
      break;
    }
  }
  if (x == true) {

    return b;
  }
  else return 26;
} 
String dataprevious;

String data;
String data5;
int x1 = 26;
void readpins() {
  int pins5[5];
  
  int x2;
  int b;
 
  for (int a = 0; a < 5; a++)
  {
    pins5[a] = digitalRead(leds5[a]);
  }
  for (b = 0; b < 25; b++)/* generating numbers from 0 to 24 */
  {

    x2 = readDigiterror(b);
    if (x2 == b) {
      break;
    }

  }
  
  data5 = String(pins5[0]) + String(pins5[1]) + String(pins5[2]) +
                 String(pins5[3]) + String(pins5[4]);
  StaticJsonDocument<200> doc1; // Adjust the size as needed based on your JSON data

  // Populate the JSON document
  doc1["iotId"] = String(buildID);
  doc1["stateLids"] = data5;
  doc1["stateCode"] = String(x1);

  if (data5 != "11111" || x1 != x2) {
     data = "iotId=" + String(buildID) + "&stateLids=" +
                  data5 + "&stateCode=" + (x1);
    // Serial.println(data);
    if (data != dataprevious) {
      Serial.println(data);
      read_data = true;
      sendHttpPostRequest(doc1);
      dataprevious = data;
    }
  }
  if (data5=="11111"&& x1==x2){
    StaticJsonDocument<200> doc2; // Adjust the size as needed based on your JSON data

  // Populate the JSON document
  doc2["iotId"] = String(buildID);
  doc2["stateLids"] = "11111";
  doc2["stateCode"] = String(26);
   data= "iotId=" + String(buildID) + "&stateLids=" +
                  "11111" + "&stateCode=" + (26);
  if(data!=dataprevious ){
    Serial.println(data);
    read_data = true;
      sendHttpPostRequest(doc2);
      dataprevious = data;}
  
  }
}
void updateCounter() {
  x2 = 26;
  for (int b = 0; b < 10; b++) {
    x2 = readDigit(b);
    readpins();
    if (x2 != 26) {
      break;
    }
  }

  if (lastnumber != x2) {
    lastnumber = x2;
    read_data = true;
  }
}
  void sendtoall(){

String json = "{\"lastnumber\":";
    json += String(lastnumber);
    json += ",\"data\":\"" + String(data5) + "\"";
    json += ",\"code\":";
    json += String(x1);
    json += ",\"status\":\"ok\"";
    json += "}";
     Serial.println(json);
    webSocket.broadcastTXT(json.c_str(), json.length());
    read_data = false;

 } 
void senttoone (uint8_t num,String response ){
String json = "{\"lastnumber\":";
    json += String(lastnumber);
    json += ",\"data\":\"" + String(data5) + "\"";
    json += ",\"code\":";
    json += String(x1); 
    json += ",\"status\":\"" + response + "\"";
    json += "}";
     Serial.println(json);
     webSocket.sendTXT(num, json);
     read_data = false;

 }
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
       sendtoall();
      Serial.printf("Client connected from %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
      break;
    }
    case WStype_TEXT: {
      Serial.printf("payload [%u]: %s\n", num, payload);
      // Handle incoming messages here
      if (strcmp((char*)payload, "led1") == 0) {
        // Handle turning on LED 1
        digitalWrite(output26, HIGH);
        delay(1000);
        digitalWrite(output26, LOW);
        // Send "ok" response for LED 1
        senttoone ( num,"ok" ) ;
        
      } else if (strcmp((char*)payload, "led2") == 0) {
        // Handle turning on LED 2
        digitalWrite(output27, HIGH);
        delay(1000);
        digitalWrite(output27, LOW);
        // Send "ok" response for LED 2
         senttoone ( num,"ok" ) ;

      } else if (strcmp((char*)payload, "led3") == 0) {
        // Handle turning on LED 3
        digitalWrite(output25, HIGH);
        delay(1000);
        digitalWrite(output25, LOW);
        // Send "ok" response for LED 3
        senttoone ( num,"ok" ) ;

      } else if (strcmp((char*)payload, "led4") == 0) {
        // Handle turning on LED 4
        digitalWrite(output21, HIGH);
        delay(1000);
        digitalWrite(output21, LOW);
        // Send "ok" response for LED 4
       senttoone ( num,"ok" ) ;

      } else if (strcmp((char*)payload, "led5") == 0) {
        // Handle turning on LED 5
        digitalWrite(output24, HIGH);
        delay(1000);
        digitalWrite(output24, LOW);
        // Send "ok" response for LED 5
         senttoone ( num,"ok" ) ;

      } else if (strcmp((char*)payload, "led6") == 0) {
        // Handle turning on LED 6
        digitalWrite(output23, HIGH);
        delay(1000);
        digitalWrite(output23, LOW);
        // Send "ok" response for LED 6
         senttoone ( num,"ok" ) ;

      } else if (strcmp((char*)payload, "led7") == 0) {
        // Handle turning on LED 7
        digitalWrite(output22, HIGH);
        delay(1000);
        digitalWrite(output22, LOW);
        // Send "ok" response for LED 7
        senttoone ( num,"ok" ) ;
       } else if (strcmp((char*)payload, "cst") == 0) {
        // Handle turning on LED 7
         digitalWrite(output13, HIGH);
          cst=true; 
          
         previousActionMillis=millis();
        // Send "ok" response for LED 7
        senttoone ( num,"ok" ) ;
            

      } else {
        // Handle unsupported message
        Serial.println("Unsupported message received");
        // Send "failure" response for unsupported message
         senttoone ( num,"failure" ) ;

      }
      break;
    }
  }
}

String lastip ;
void pubicip(){ 

if (WiFi.status() == WL_CONNECTED) {
  HTTPClient http;

  // Send GET request to retrieve public IP
  http.begin("http://api.ipify.org");
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    if (lastip != payload) {
      Serial.print("Public IP: ");
      Serial.println(payload);
      lastip = payload;
    } 
  } else {
    
  }

  http.end();
}
}

void loop() {
  webSocket.loop();
  server.handleClient();  
  pubicip();
  if (read_data) {
    sendtoall();
     
  }
  if (cst==true){ unsigned long currentMillis = millis();
Serial.println(currentMillis);
  // Check if it's time to perform an action based on the interval
  if (currentMillis - previousActionMillis >= actionInterval) {
    // Update the last action time
    previousActionMillis = currentMillis;
    digitalWrite(output13, LOW);
          cst=false; }
  }
  readpins();
}