#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Golazo";
const char* password = "nisiu1!!";

const int ledPin = 2;
const int pwmChannel = 0;

WebServer server(80);

void handleRoot() {
  String page = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <body>

  <h2>ESP32 Brightness Control</h2>

  <input type="range"
         min="0"
         max="255"
         value="0"
         oninput="updateBrightness(this.value)">

  <p id="value">0</p>

  <script>
    function updateBrightness(val) {
      document.getElementById("value").innerHTML = val;
      fetch("/set?brightness=" + val);
    }
  </script>

  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", page);
}

void handleBrightness() {

  if (server.hasArg("brightness")) {

    int brightness = server.arg("brightness").toInt();

    ledcWrite(pwmChannel, brightness);
  }

  server.send(200, "text/plain", "OK");
}

void setup() {

  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);

  ledcSetup(pwmChannel, 5000, 8);
  ledcAttachPin(ledPin, pwmChannel);

  WiFi.begin(ssid, password);

  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/set", handleBrightness);

  server.begin();
}

void loop() {
  server.handleClient();
}