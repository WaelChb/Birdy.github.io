#include <Tomoto_HM330X.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>
Servo servoMotor; 
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "AKPO";
const char* password = "partageadel";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;





#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Tomoto_HM330X sensor;
// Tomoto_HM330X capteur(Wire1); // pour utiliser le fil d'alternative

void printValue(const char* label, int value) {
  Serial.print(label);
  Serial.print(": ");
  Serial.println(value);
}

void setup() {
  servoMotor.attach(D8);
  Serial.begin(9600);
   if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  delay(100);

display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  // Wire.begin(25, 21); // to specify the I2C pins

  if (!sensor.begin()) {
    Serial.println("Failed to initialize HM330X");
    while (1)
      ;
  }

  Serial.println("HM330X initialized");
  Serial.println();

// Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}




void loop() {

  if (!sensor.readSensor()) {
    Serial.println("Failed to read HM330X");
  } else {
    printValue("Sensor number", sensor.getSensorNumber());

  //   Serial.println("Concentration basée sur la norme de particules standard CF=1 (ug/m^3) --");
  //   printValue("PM1.0", sensor.std.getPM1());
  //   printValue("PM2.5", sensor.std.getPM2_5());
  //   printValue("PM10", sensor.std.getPM10());

  //   Serial.println("Concentration basée sur l'environnement atmosphérique (ug/m^3) --");
  //   printValue("PM1.0", sensor.atm.getPM1());
  //   printValue("PM2.5", sensor.atm.getPM2_5());
  //   printValue("PM10", sensor.atm.getPM10());

  // // Peut-être pris en charge ou non, en fonction du modèle du capteur
  //   Serial.println("Nombre de particules avec un diamètre de (/0.1L) --");
  //   printValue(">=0.3um", sensor.count.get0_3());
  //   printValue(">=0.5um", sensor.count.get0_5());
  //   printValue(">=1.0um", sensor.count.get1());
  //   printValue(">=2.5um", sensor.count.get2_5());
  //   printValue(">=5.0um", sensor.count.get5());
  //   printValue(">=10um", sensor.count.get10());



    if (sensor.std.getPM2_5() <= 12.0){
  display.clearDisplay();

  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Excellente qualité de l'air."));
  
 servoMotor.write(0);
  display.display();      // Show initial text
  delay(100);
      delay(1000);
      printValue("PM2.5", sensor.std.getPM2_5());

    }

    
    if (sensor.std.getPM2_5() > 12.0 && sensor.std.getPM2_5() <= 35.0){
       display.clearDisplay();

  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Moyenne qualité de l'air "));


  servoMotor.write(45);

  display.display();      // Show initial text
  delay(100);
      //display.println("Bonne qualité de l'air");
      delay(1000);
      printValue("PM2.5", sensor.std.getPM2_5());

    }
    
    if (sensor.std.getPM2_5() > 35 ){
      
       display.clearDisplay();

  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F(" Trés Mauvaise qualité de l'air"));
   servoMotor.write(180);

  display.display();      // Show initial text
  delay(100);
      //display.println("Bonne qualité de l'air");
      delay(1000);
      printValue("PM2.5", sensor.std.getPM2_5());

    }
    WiFiClient client = server.available();
  if (client) {

    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // ... (Votre code existant)

            // Ajout de la valeur PM2.5 dans la réponse HTML
            client.print("<p>PM2.5: ");
            client.print(sensor.std.getPM2_5());
            client.println(" ug/m³</p>");

            // ... (Votre code existant)

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
    





    Serial.println();
  }

  delay(5000);




 



}
