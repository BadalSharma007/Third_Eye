#define CAMERA_MODEL_AI_THINKER
#include "esp_camera.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "camera_pins.h"  // Ensure this file contains the correct pin definitions for your camera model

const char *ssid = "Abhishek's F14";  // Replace with your WiFi SSID
const char *password = "nba210404";   // Replace with your WiFi password

// Motor Driver Pins
#define ENA 12  // Enable A
#define IN1 13  // Left Motor Forward
#define IN2 15  // Left Motor Backward
#define IN3 14  // Right Motor Forward
#define IN4 2   // Right Motor Backward
#define ENB 12  // Enable B (same as ENA)

AsyncWebServer server(80);

// Function to initialize the camera
void initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Configure frame size and quality based on PSRAM availability
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;  // 1600x1200
    config.jpeg_quality = 10;            // Lower number means higher quality
    config.fb_count = 2;                 // Use 2 frame buffers
  } else {
    config.frame_size = FRAMESIZE_SVGA;  // 800x600
    config.jpeg_quality = 12;            // Lower quality
    config.fb_count = 1;                 // Use 1 frame buffer
  }

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera initialization failed with error 0x%x\n", err);
    return;
  }

  Serial.println("Camera initialized successfully.");
}

void setup() {
  Serial.begin(38400);  // Set baud rate for Serial Monitor

  // Connect to WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  // Print the camera link
  Serial.print("Camera Link: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/camera");

  initCamera();  // Initialize the camera

  // Setup Motor Pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Enable the motor drivers
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  // API Endpoint for Motor Control
  server.on("/move", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("dir")) {
      String direction = request->getParam("dir")->value();
      Serial.println("Received Command: " + direction);

      // Motor control logic
      if (direction == "forward") {
        digitalWrite(IN1, HIGH);  // Left Motor Forward
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);  // Right Motor Forward
        digitalWrite(IN4, LOW);
        Serial.println("Moving forward");
        delay(1000);  // Move forward for 1 second
        digitalWrite(IN1, LOW);   // Stop Left Motor
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);   // Stop Right Motor
        digitalWrite(IN4, LOW);
        Serial.println("Stopped moving forward");

      } else if (direction == "backward") {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);  // Left Motor Backward
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);  // Right Motor Backward
        Serial.println("Moving backward");
        delay(1000);  // Move backward for 1 second
        digitalWrite(IN1, LOW);   // Stop Left Motor
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);   // Stop Right Motor
        digitalWrite(IN4, LOW);
        Serial.println("Stopped moving backward");

      } else if (direction == "left") {
        digitalWrite(IN1, HIGH);  // Left Motor Forward
        digitalWrite(IN2, LOW);   // Ensure Left Motor is moving forward
        digitalWrite(IN3, LOW);   // Stop Right Motor
        digitalWrite(IN4, LOW);   // Ensure Right Motor is stopped
        Serial.println("Turning left");
        delay(500);  // Turn left for half a second
        digitalWrite(IN1, LOW);   // Stop Left Motor
        digitalWrite(IN2, LOW);
        Serial.println("Stopped turning left");

      } else if (direction == "right") {
        digitalWrite(IN1, LOW);   // Stop Left Motor
        digitalWrite(IN2, LOW);   // Ensure Left Motor is stopped
        digitalWrite(IN3, HIGH);  // Right Motor Forward
        digitalWrite(IN4, LOW);   // Ensure Right Motor is moving forward
        Serial.println("Turning right");
        delay(500);  // Turn right for half a second
        digitalWrite(IN3, LOW);   // Stop Right Motor
        digitalWrite(IN4, LOW);
        Serial.println("Stopped turning right");

      } else if (direction == "stop") {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        Serial.println("Stopping");
      } else {
        request->send(400, "text/plain", "Invalid direction");
        return;
      }

      // Debugging: Print the state of the motor control pins
      Serial.print("Motor States: IN1: ");
      Serial.print(digitalRead(IN1));
      Serial.print(" IN2: ");
      Serial.print(digitalRead(IN2));
      Serial.print(" IN3: ");
      Serial.print(digitalRead(IN3));
      Serial.print(" IN4: ");
      Serial.println(digitalRead(IN4));

      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Direction parameter missing");
    }
  });

  // API Endpoint for Camera Capture
  server.on("/camera", HTTP_GET, [](AsyncWebServerRequest *request) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      request->send(500, "text/plain", "Camera capture failed");
      return;
    }

    // Send the image as a JPEG
    request->send_P(200, "image/jpeg", fb->buf, fb->len);
    esp_camera_fb_return(fb);
  });

  server.begin();  // Start the server
  Serial.println("Server started.");
}

void loop() {
  // Main loop does nothing, all handled by server
  delay(10);
}

// #define CAMERA_MODEL_AI_THINKER
// #include "esp_camera.h"
// #include <WiFi.h>
// #include <ESPAsyncWebServer.h>
// #include "camera_pins.h"  // Ensure this file contains the correct pin definitions for your camera model

// const char *ssid = "Abhishek's F14";  // Replace with your WiFi SSID
// const char *password = "nba210404";   // Replace with your WiFi password

// // Motor Driver Pins
// #define ENA 12  // Enable A
// #define IN1 13  // Left Motor Forward
// #define IN2 15  // Left Motor Backward
// #define IN3 14  // Right Motor Forward
// #define IN4 2   // Right Motor Backward
// #define ENB 12  // Enable B (same as ENA)

// AsyncWebServer server(80);

// // Function to initialize the camera
// void initCamera() {
//   camera_config_t config;
//   config.ledc_channel = LEDC_CHANNEL_0;
//   config.ledc_timer = LEDC_TIMER_0;
//   config.pin_d0 = Y2_GPIO_NUM;
//   config.pin_d1 = Y3_GPIO_NUM;
//   config.pin_d2 = Y4_GPIO_NUM;
//   config.pin_d3 = Y5_GPIO_NUM;
//   config.pin_d4 = Y6_GPIO_NUM;
//   config.pin_d5 = Y7_GPIO_NUM;
//   config.pin_d6 = Y8_GPIO_NUM;
//   config.pin_d7 = Y9_GPIO_NUM;
//   config.pin_xclk = XCLK_GPIO_NUM;
//   config.pin_pclk = PCLK_GPIO_NUM;
//   config.pin_vsync = VSYNC_GPIO_NUM;
//   config.pin_href = HREF_GPIO_NUM;
//   config.pin_sscb_sda = SIOD_GPIO_NUM;
//   config.pin_sscb_scl = SIOC_GPIO_NUM;
//   config.pin_pwdn = PWDN_GPIO_NUM;
//   config.pin_reset = RESET_GPIO_NUM;
//   config.xclk_freq_hz = 20000000;
//   config.pixel_format = PIXFORMAT_JPEG;

//   // Configure frame size and quality based on PSRAM availability
//   if (psramFound()) {
//     config.frame_size = FRAMESIZE_UXGA;  // 1600x1200
//     config.jpeg_quality = 10;            // Lower number means higher quality
//     config.fb_count = 2;                 // Use 2 frame buffers
//   } else {
//     config.frame_size = FRAMESIZE_SVGA;  // 800x600
//     config.jpeg_quality = 12;            // Lower quality
//     config.fb_count = 1;                 // Use 1 frame buffer
//   }

//   // Initialize the camera
//   esp_err_t err = esp_camera_init(&config);
//   if (err != ESP_OK) {
//     Serial.printf("Camera initialization failed with error 0x%x\n", err);
//     return;
//   }

//   Serial.println("Camera initialized successfully.");
// }

// void setup() {
//   Serial.begin(38400);  // Set baud rate for Serial Monitor

//   // Connect to WiFi
//   Serial.print("Connecting to WiFi");
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nWiFi Connected!");

//   // Print the camera link
//   Serial.print("Camera Link: http://");
//   Serial.print(WiFi.localIP());
//   Serial.println("/camera");

//   initCamera();  // Initialize the camera

//   // Setup Motor Pins
//   pinMode(ENA, OUTPUT);
//   pinMode(IN1, OUTPUT);
//   pinMode(IN2, OUTPUT);
//   pinMode(IN3, OUTPUT);
//   pinMode(IN4, OUTPUT);
//   pinMode(ENB, OUTPUT);

//   // Enable the motor drivers
//   digitalWrite(ENA, HIGH);
//   digitalWrite(ENB, HIGH);

//   // API Endpoint for Motor Control
//   server.on("/move", HTTP_GET, [](AsyncWebServerRequest *request) {
//     if (request->hasParam("dir")) {
//       String direction = request->getParam("dir")->value();
//       Serial.println("Received Command: " + direction);

//       // Motor control logic
//       if (direction == "forward") {
//         digitalWrite(IN1, HIGH);  // Left Motor Forward
//         digitalWrite(IN2, LOW);
//         digitalWrite(IN3, HIGH);  // Right Motor Forward
//         digitalWrite(IN4, LOW);
//         Serial.println("Moving forward");

//       } else if (direction == "backward") {
//         digitalWrite(IN1, LOW);
//         digitalWrite(IN2, HIGH);  // Left Motor Backward
//         digitalWrite(IN3, LOW);
//         digitalWrite(IN4, HIGH);  // Right Motor Backward
//         Serial.println("Moving backward");

//       } else if (direction == "left") {

//         digitalWrite(IN1, HIGH);  // Left Motor Forward
//         digitalWrite(IN2, LOW);   // Ensure Left Motor is moving forward
//         digitalWrite(IN3, LOW);   // Stop Right Motor
//         digitalWrite(IN4, LOW);   // Ensure Right Motor is stopped
//         Serial.println("Turning left");

//       } else if (direction == "right") {
//         digitalWrite(IN1, LOW);   // Stop Left Motor
//         digitalWrite(IN2, LOW);   // Ensure Left Motor is stopped
//         digitalWrite(IN3, HIGH);  // Right Motor Forward
//         digitalWrite(IN4, LOW);   // Ensure Right Motor is moving forward
//         Serial.println("Turning right");

//       } else if (direction == "stop") {
//         digitalWrite(IN1, LOW);
//         digitalWrite(IN2, LOW);
//         digitalWrite(IN3, LOW);
//         digitalWrite(IN4, LOW);
//         Serial.println("Stopping");
//       } else {
//         request->send(400, "text/plain", "Invalid direction");
//         return;
//       }

//       // Debugging: Print the state of the motor control pins
//       Serial.print("Motor States: IN1: ");
//       Serial.print(digitalRead(IN1));
//       Serial.print(" IN2: ");
//       Serial.print(digitalRead(IN2));
//       Serial.print(" IN3: ");
//       Serial.print(digitalRead(IN3));
//       Serial.print(" IN4: ");
//       Serial.println(digitalRead(IN4));

//       request->send(200, "text/plain", "OK");
//     } else {
//       request->send(400, "text/plain", "Direction parameter missing");
//     }
//   });

//   // API Endpoint for Camera Capture
//   server.on("/camera", HTTP_GET, [](AsyncWebServerRequest *request) {
//     camera_fb_t *fb = esp_camera_fb_get();
//     if (!fb) {
//       Serial.println("Camera capture failed");
//       request->send(500, "text/plain", "Camera capture failed");
//       return;
//     }

//     // Send the image as a JPEG
//     request->send_P(200, "image/jpeg", fb->buf, fb->len);
//     esp_camera_fb_return(fb);
//   });

//   server.begin();  // Start the server
//   Serial.println("Server started.");
// }

// void loop() {
//   // Main loop does nothing, all handled by server
//   delay(10);
// }