#define SERIAL_FLASH true

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Callmebot_ESP8266.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>

const char* ssid = "Infinix";     // Your Wi-Fi network SSID
const char* password = "00000000";     // Wi-Fi network password

// Change your username here.
String username = "@Kaideens";

const int trigPin = D5;
const int echoPin = D6;
const int buzzerPin = D4;

const int led1 = D0;
const int led2 = D1;
const int led3 = D2;
long duration;
int distance;

int lowestDistanceAddress = 0;  // EEPROM address to store lowest distance
int lowestDistance = 100;   // Initialize with a large value

int buttonPressed = 0;

ESP8266WebServer server(80);  // Create a web server on port 80

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to Wi-Fi, IP address: ");
  Serial.println(WiFi.localIP());

  // Set up the web server
  server.on("/", HTTP_GET, handleRoot); // Serve the web page on the root path
  server.on("/getDistance", HTTP_GET, handleGetDistance); // Handle requests for the distance value
  server.on("/getLowestDistance", HTTP_GET, handleGetLowestDistance); // Handle requests for the lowest distance value
  server.on("/activateAlarm", HTTP_GET, handleActivateAlarm);
  server.begin();

  Callmebot.begin();
}

void loop() {
  server.handleClient();  // Handle web server requests
  
  if (buttonPressed) {
    activateAlarm();
    buttonPressed = 0; // Reset the flag
  }

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;


  Serial.println(distance);
  // Update the LED and buzzer based on the distance
  updateIndicators();

  if (distance < lowestDistance || lowestDistance == 0) {
    lowestDistance = distance;
  }
}

void handleRoot() {
  // Serve the HTML page with JavaScript to update the distance
  String html = F("<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Water Level Detection</title>");

  // html += F("<style>* { margin: 0; padding: 0; text-decoration: none; list-style: none; box-sizing: border-box; font-family: Arial; } body { display: flex; min-height: 100vh; background: url('https://s.tmimgcdn.com/scr/328500/blue-wave-water-background-design-vector-v32_328560-original.jpg'); background-repeat: no-repeat; background-size: cover; } body .main-container { margin: auto; background: rgb(241, 250, 255); padding: 20px 30px; border-radius: 20px;");
  // html += F("box-shadow: 2px 3px 10px rgb(143, 216, 255); display: flex; flex-direction: column; gap: 15px; min-height: 90vh; } body .main-container .title { color: rgb(245, 245, 245); background: rgb(143, 216, 255); padding: inherit; border-radius: inherit; } body .main-container .info-container { height: 540px; width: 100%; display: flex; } body .main-container .info-container .level-container {");
  // html += F("height: 100%; width: 50%; position: relative; display: flex; justify-content: center; flex-direction: column; } body .main-container .info-container .level-container .progress-bar { width: 25px; position: absolute; transform: rotateX(180deg); height: 82%; bottom: 15px; margin-left: 20%; z-index: 99; } body .main-container .info-container .level-container .progress-bar .progress { background: rgb(11, 122, 202); border-radius: 5px; height: 100%; }");
  // html += F("body .main-container .info-container .level-container .progress-bar .circle { width: 35px; height: 35px; border: solid 8px rgb(11, 122, 202); background: #eb3f14; border-radius: 50%; position: absolute; transform: translateY(-8px) translateX(-5px); top: 100%; } body .main-container .info-container .level-container .progress-bar .circle span { position: absolute; height: 4px; width: 50px; bottom: 8px; background: #eb3f14; z-index: 99; }");
  // html += F("body .main-container .info-container .level-container .progress-bar .percent { transform: rotateX(180deg); font-weight: 500; font-size: 22px; margin: 28px -20px; pointer-events: none; color: rgb(48, 14, 212); text-align: center; } body .main-container .info-container .level-container .ruler { width: 75px; margin: 24% 20% 0 auto; height: 82%; display: flex; flex-direction: column; position: relative; body .main-container .info-container");
  // html += F(".level-container .ruler .big-centimeter { min-height: 25%; border-top: 1px solid #282828; border-left: 1px solid #282828; display: flex; flex-direction: column; } body .main-container .info-container .level-container .ruler .big-centimeter:last-child { border-bottom: 1px solid #282828; } body .main-container .info-container .level-container .ruler .small-centimeter { min-height: 17%; width: 30%; border-top: 1px solid #282828; }");
  // html += F("body .main-container .info-container .level-container .ruler .small-centimeter:first-child { border: none; } body .main-container .info-container .level-container .ruler .small-centimeter:nth-child(4) { width: 45%; } body .main-container .info-container .level-container .ruler .label { position: absolute; padding-left: 5px; right: -28px; top: -5px; bottom: -5px; background: rgb(241, 250, 255); } body .main-container .info-container .level-container .label p {");
  // html += F("min-height: 24%; font-size: 14px; } body .main-container .info-container .level-container .label p:last-child { min-height: auto; } body .main-container .info-container .notification { display: flex; width: 50%; flex-direction: column; margin-top: 40px; } body .main-container .info-container .notification .date { text-align: center; font-size: 20px; font-weight: 500; color: rgb(245, 245, 245); padding: 15px 10px; background: rgb(86, 196, 255);");
  // html += F("border-radius: 10px; } body .main-container .info-container .notification .warning { font-size: 20px; padding: 10px 15px; background: rgb(255, 82, 82); border-radius: 5px; color: #fff; font-weight: 500; margin: auto; } </style> </head>");

  html += F("<style>* { margin: 0; padding: 0; text-decoration: none; list-style: none; box-sizing: border-box; font-family: Arial; } body { display: flex; min-height: 100vh; background: url('https://s.tmimgcdn.com/scr/328500/blue-wave-water-background-design-vector-v32_328560-original.jpg'); background-repeat: no-repeat; background-size: cover; } body .main-container { margin: auto; background: rgb(241, 250, 255); padding: 20px 30px; border-radius: 20px; ");
  html += F("box-shadow: 2px 3px 10px rgb(143, 216, 255); display: flex; flex-direction: column; gap: 15px; min-height: 90vh; } body .main-container .title { color: rgb(245, 245, 245); background: rgb(143, 216, 255); padding: inherit; border-radius: inherit; } body .main-container .info-container { height: 540px; width: 100%; display: flex; } body .main-container .info-container .level-container { height: 100%; width: 50%; position: relative; display: flex; ");
  html += F("justify-content: center; flex-direction: column; } body .main-container .info-container .level-container .progress-bar { width: 25px; position: absolute; transform: rotateX(180deg); height: 82%; bottom: 15px; margin-left: 20%; z-index: 99; } body .main-container .info-container .level-container .progress-bar .progress { background: rgb(11, 122, 202); border-radius: 5px; height: 100%; } body .main-container .info-container .level-container ");
  html += F(".progress-bar .circle { width: 35px; height: 35px; border: solid 8px rgb(11, 122, 202); background: #eb3f14; border-radius: 50%; position: absolute; transform: translateY(-8px) translateX(-5px); top: 100%; } body .main-container .info-container .level-container .progress-bar .circle span { position: absolute; height: 4px; width: 50px; bottom: 8px; background: #eb3f14; z-index: 99; } body .main-container .info-container .level-container ");
  html += F(".progress-bar .percent { transform: rotateX(180deg); font-weight: 500; font-size: 22px; margin: 28px -20px; pointer-events: none; color: rgb(48, 14, 212); text-align: center; } body .main-container .info-container .level-container .ruler { width: 75px; margin: 24% 20% 0 auto; height: 82%; display: flex; flex-direction: column; position: relative; } body .main-container .info-container .level-container .ruler .big-centimeter { min-height: 25%; ");
  html += F("border-top: 1px solid #282828; border-left: 1px solid #282828; display: flex; flex-direction: column; } body .main-container .info-container .level-container .ruler .big-centimeter:last-child { border-bottom: 1px solid #282828; } body .main-container .info-container .level-container .ruler .small-centimeter { min-height: 17%; width: 30%; border-top: 1px solid #282828; } body .main-container .info-container .level-container .ruler ");
  html += F(".small-centimeter:first-child { border: none; } body .main-container .info-container .level-container .ruler .small-centimeter:nth-child(4) { width: 45%; } body .main-container .info-container .level-container .ruler .label { position: absolute; padding-left: 5px; right: -28px; top: -5px; bottom: -5px; background: rgb(241, 250, 255); } body .main-container .info-container .level-container .label p { min-height: 24%; font-size: 14px; } ");
  html += F("body .main-container .info-container .level-container .label p:last-child { min-height: auto; } body .main-container .info-container .notification { display: flex; width: 50%; flex-direction: column; margin-top: 40px; } body .main-container .info-container .notification .date { text-align: center; font-size: 20px; font-weight: 500; color: rgb(245, 245, 245); padding: 15px 10px; background: rgb(86, 196, 255); border-radius: 10px; } ");
  html += F("body .main-container .info-container .notification .warning { font-size: 20px; padding: 10px 15px; background: navy; border-radius: 5px; color: #fff; font-weight: 500; margin: auto; } </style> </head>");

  html += F("<body><div class='main-container'><h1 class='title'>Water Level Detection</h1><div class='info-container'><div class='level-container' data-percent='100'><div class='progress-bar'><div class='progress'></div><span class='circle'><span></span></span><p class='percent'>100%</p></div><div class='ruler'><div class='label'><p>46cm</p><p>23cm</p><p>15.3cm</p><p>11.5cm</p><p>0cm</p></div><div class='big-centimeter'><div class='small-centimeter'></div>");
  html += F("<div class='small-centimeter'></div><div class='small-centimeter'></div><div class='small-centimeter'></div><div class='small-centimeter'></div><div class='small-centimeter'></div></div><div class='big-centimeter'><div class='small-centimeter'></div><div class='small-centimeter'></div><div class='small-centimeter'></div><div class='small-centimeter'></div><div class='small-centimeter'></div><div class='small-centimeter'></div></div><div class='big-centimeter'>");
  html += F("<div class='small-centimeter'></div><div class='small-centimeter'></div><div class='small-centimeter'></div><div class='small-centimeter'></div><div class='small-centimeter'></div><div class='small-centimeter'></div></div><div class='big-centimeter'><div class='small-centimeter'></div><div class='small-centimeter'></div><div class='small-centimeter'></div><div class='small-centimeter'></div><div class='small-centimeter'></div><div class='small-centimeter'></div></div>");
  html += F("</div></div><div class='notification'><div class='date'><div class='ttime'></div><div class='tdate'></div></div><p class='warning'>Highest Distance: 0 cm</p><button onclick=\"activateAlarm()\" style=\"background-color: #ff4d4d; color: white; padding: 10px; border: none; cursor: pointer; border-radius: 5px; font-size: 16px; margin-bottom: 15px;\">Activate Alarm</button></div></div></div>");

  html += F("<script>");
  html += F("function formatTime(date) { const options = { hour: 'numeric', minute: 'numeric', hour12: true }; return date.toLocaleTimeString('en-US', options); } document.querySelector('.ttime').innerHTML = formatTime(new Date());");
  html += F("function formatDate(date) { const options = { weekday: 'long', day: 'numeric', month: 'short', year: 'numeric' }; return date.toLocaleDateString('en-US', options); } document.querySelector('.tdate').innerHTML = formatDate(new Date()); const percentage = document.querySelector('.level-container'); const circle = document.querySelector('.circle'); const progress = document.querySelector('.progress'); const percent = document.querySelector('.percent'); let progressed = 0; let color;");
  html += F("function startProgress(targetPercent) { function updateProgress() { const step = 5; if (progressed < targetPercent) { progressed = Math.min(progressed + step, targetPercent); } else if (progressed > targetPercent) { progressed = Math.max(progressed - step, targetPercent); } if (targetPercent >= 80) { color = 'red'; } else if (targetPercent >= 51 && targetPercent <= 79) { color = 'orange'; } else if (targetPercent < 51) { color = 'yellow'; } circle.style.top = progressed + '%'; circle.style.backgroundColor = color; progress.style.height = progressed + '%'; progress.style.backgroundColor = color; ");
  html += F("percent.innerHTML = progressed + '%'; if (progressed !== targetPercent) { requestAnimationFrame(updateProgress); } } updateProgress(); }");

  html += F("function updateWaterLevel(level) { level = Math.min(46, Math.max(0, level)); let calculatedPercentage = Math.max(0, Math.min(100, 100 - (level / 46) * 100)); document.querySelector('.level-container').setAttribute('data-percent', Math.round(calculatedPercentage)); color = null; startProgress(Math.round(calculatedPercentage)); }");

  html += F("function fetchDistance() { fetch('/getDistance') .then(response => response.text()) .then(data => { updateWaterLevel(data); }) .catch(error => console.error('Error:', error)); }");

  html += F("function updateHighestLevel(highest) { highest = 46 - Math.min(46, Math.max(0, highest)); document.querySelector('.warning').innerHTML = 'Highest Distance: ' + highest + ' cm'; }");
  
  html += F("function fetchLowestDistance() { fetch('/getLowestDistance') .then(response => response.text()) .then(data => { updateHighestLevel(data); }) .catch(error => console.error('Error:', error)); }");
  
  html += F("function activateAlarm() { fetch('/activateAlarm') .then(response => response.text()) .then(data => { updateWaterLevel(data); }) .catch(error => console.error('Error:', error)); }");

  html += F("setInterval(fetchLowestDistance, 800);");
  html += F("setInterval(fetchDistance, 800);"); // Update every second
  html += F("fetchDistance(); fetchLowestDistance();"); // Fetch on page load
  html += F("</script>");
  html += F("</body></html>");

  server.send(200, "text/html", html);
}

// void handleRoot() {
//   // Serve the HTML page with JavaScript to update the distance
//   String html;
//   if (SPIFFS.begin()) {
//     // Open the HTML file
//     File file = SPIFFS.open("/pages/index.html", "r");
//     if (file) {
//       // Read the content of the file
//       html = file.readString();
//       file.close();
//     } else {
//       html = "Failed to open HTML file.";
//     }
//   } else {
//     html = "Failed to mount file system.";
//   }

//   server.send(200, "text/html", html);
// }

void handleGetDistance() {
  // Respond to requests for the distance value
  server.send(200, "text/plain", String(distance));
}

void handleGetLowestDistance() {
  server.send(200, "text/plain", String(lowestDistance));
}

void handleActivateAlarm() {
  buttonPressed = 1;
  distance = 0; // Set distance to 0
  server.send(200, "text/plain", String(distance));
}

void activateAlarm() {
  // Your existing code for setting LEDs, buzzer, etc.

  digitalWrite(led1, HIGH);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);

  tone(buzzerPin, 2000);

  // Pause the loop for 15 seconds
  delay(15000);

  // Reset indicators after 30 seconds
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  noTone(buzzerPin);
}

int buzzerStartTime = 0;
int buzzerDelay = 5000;  // Adjust seconds in milliseconds for red alarm

bool isLevelThreeSent = false;
bool isLevelTwoSent = false;

void updateIndicators() {
  // Update the LED and buzzer based on the distance

  // Update the LED based on the distance
  if (distance <= 9) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);

    //Serial.println(Callmebot.debug());

    // Check if the buzzer delay has elapsed
    if (millis() - buzzerStartTime >= buzzerDelay && buzzerStartTime != 0) {
      tone(buzzerPin, 2000);
      // Reset the start time to avoid repeated buzzing
      buzzerStartTime = 0;
    }
    // Check if the buzzer has not been started yet
    else if (buzzerStartTime == 0) {
      // Set the start time
      buzzerStartTime = millis();
    }

    if(!isLevelThreeSent) {
      Serial.println("sending 0");
      Callmebot.telegramMessage(username, "HIGH ALERT --- LEVEL 3");
      Serial.println("sending 1");
      isLevelThreeSent = true;
    }

    
  } else if (distance <= 23) {
    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, LOW);

    // Sound pattern for distance 35
    for (int i = 0; i < 2; ++i) {
      tone(buzzerPin, 1000);
      delay(30);
      noTone(buzzerPin);
      delay(50);
    }
    
    // Reset the buzzer timer when distance is greater than 14
    if (buzzerStartTime != 0) {
      buzzerStartTime = 0;
    }

    if(!isLevelTwoSent) {
      Serial.println("sending 0");
      Callmebot.telegramMessage(username, "HIGH ALERT --- LEVEL 2");
      Serial.println("sending 1");
      isLevelTwoSent = true;
    }
  
  } else if (distance <= 46 ) {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, HIGH);

    // Sound pattern for distance 70
    for (int i = 0; i < 2; ++i) {
      tone(buzzerPin, 0);
      delay(500);
      noTone(buzzerPin);
      delay(500);
    }
  } else {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    noTone(buzzerPin);
  }
  
  delay(1);
}