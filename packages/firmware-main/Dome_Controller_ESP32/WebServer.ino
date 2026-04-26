// WebServer.ino - ASCOM Alpaca API and Web Interface Routes

void setupWebServer() {
  // ==================== ASCOM Alpaca Management API ====================
  
  // Discovery endpoint
  server.on("/management/apiversions", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    JsonArray versions = doc.createNestedArray("Value");
    versions.add(1);
    
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/management/v1/description", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<512> doc;
    JsonObject value = doc.createNestedObject("Value");
    value["ServerName"] = DEVICE_NAME;
    value["Manufacturer"] = "Sternwarte Hüfingen";
    value["ManufacturerVersion"] = DEVICE_DRIVER_VERSION;
    value["Location"] = "Hüfingen, Germany";
    
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/management/v1/configureddevices", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<512> doc;
    JsonArray devices = doc.createNestedArray("Value");
    JsonObject device = devices.createNestedObject();
    device["DeviceName"] = DEVICE_NAME;
    device["DeviceType"] = "Dome";
    device["DeviceNumber"] = DEVICE_NUMBER;
    device["UniqueID"] = WiFi.macAddress();
    
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  // ==================== ASCOM Alpaca Dome API ====================
  
  // Common properties
  server.on("/api/v1/dome/0/connected", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = isConnected;
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/connected", HTTP_PUT, [](AsyncWebServerRequest *request) {
    if (request->hasParam("Connected", true)) {
      isConnected = request->getParam("Connected", true)->value() == "true";
      if (DEBUG_ALPACA) Serial.printf("ALPACA: Connected = %s\n", isConnected ? "true" : "false");
    }
    
    StaticJsonDocument<200> doc;
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/description", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = DEVICE_DESCRIPTION;
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/driverinfo", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = DEVICE_DRIVER_INFO;
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/driverversion", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = DEVICE_DRIVER_VERSION;
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/name", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = DEVICE_NAME;
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  // Dome-specific properties
  server.on("/api/v1/dome/0/altitude", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = 0.0;  // Fixed altitude for rotating dome
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/athome", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = (abs(azimuthDeg) < 5.0);  // Home at 0°
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/atpark", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = false;  // No parking position
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/azimuth", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = azimuthDeg;
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/canfindhome", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = useAbsolutePosition;  // Can find home with reed contacts
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/canpark", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = false;
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/cansetaltitude", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = false;
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/cansetazimuth", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = true;
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/cansetpark", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = false;
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/cansetshutter", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = false;  // Shutter not implemented in this version
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/canslave", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = false;
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/cansyncazimuth", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = true;
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/slewing", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["Value"] = isSlewing;
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  // Dome methods
  server.on("/api/v1/dome/0/abortslew", HTTP_PUT, [](AsyncWebServerRequest *request) {
    setMotor(MOTOR_STOP);
    isSlewing = false;
    if (DEBUG_ALPACA) Serial.println("ALPACA: Slew aborted");
    
    StaticJsonDocument<200> doc;
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/slewtoazimuth", HTTP_PUT, [](AsyncWebServerRequest *request) {
    if (request->hasParam("Azimuth", true)) {
      targetAzimuthDeg = request->getParam("Azimuth", true)->value().toFloat();
      
      // Normalize target
      while (targetAzimuthDeg < 0) targetAzimuthDeg += 360.0;
      while (targetAzimuthDeg >= 360.0) targetAzimuthDeg -= 360.0;
      
      // Calculate shortest path
      float delta = targetAzimuthDeg - azimuthDeg;
      if (delta > 180.0) delta -= 360.0;
      if (delta < -180.0) delta += 360.0;
      
      // Start slewing
      if (abs(delta) > 2.0) {  // 2° deadband
        if (delta > 0) {
          setMotor(MOTOR_CW);
        } else {
          setMotor(MOTOR_CCW);
        }
        isSlewing = true;
        
        if (DEBUG_ALPACA) Serial.printf("ALPACA: Slew to %.1f° (delta: %.1f°)\n", targetAzimuthDeg, delta);
      } else {
        setMotor(MOTOR_STOP);
        isSlewing = false;
      }
    }
    
    StaticJsonDocument<200> doc;
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  server.on("/api/v1/dome/0/synctoazimuth", HTTP_PUT, [](AsyncWebServerRequest *request) {
    if (request->hasParam("Azimuth", true)) {
      float syncAzimuth = request->getParam("Azimuth", true)->value().toFloat();
      int32_t syncPos = azimuthToEncoder(syncAzimuth);
      encoder.setCount(syncPos);
      encoderPos = syncPos;
      azimuthDeg = syncAzimuth;
      
      if (DEBUG_ALPACA) Serial.printf("ALPACA: Synced to %.1f° (pos: %d)\n", syncAzimuth, syncPos);
    }
    
    StaticJsonDocument<200> doc;
    request->send(200, "application/json", createAlpacaResponse(doc, 
      request->hasParam("ClientTransactionID") ? request->getParam("ClientTransactionID")->value().toInt() : 0));
  });
  
  // ==================== Web Interface Pages ====================
  
  // Page 1: Azimuth Display
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", getMainPage());
  });
  
  // Page 2: Debug Info
  server.on("/debug", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", getDebugPage());
  });
  
  // Status API for web pages
  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<512> doc;
    doc["azimuth"] = statusData.azimuth;
    doc["targetAzimuth"] = statusData.targetAzimuth;
    doc["slewing"] = statusData.slewing;
    doc["connected"] = statusData.connected;
    doc["motorState"] = statusData.motorState;
    doc["encoderValue"] = statusData.encoderValue;
    doc["reedContact1"] = statusData.reedContact1;
    doc["reedContact2"] = statusData.reedContact2;
    doc["uptime"] = statusData.uptime;
    doc["wifiRSSI"] = statusData.wifiRSSI;
    doc["syncCount"] = statusData.syncCount;
    
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });
  
  // CSS
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/css", getCSS());
  });
  
  // JavaScript
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/javascript", getJavaScript());
  });
  
  // Start server
  server.begin();
  Serial.println("Web server started");
}
