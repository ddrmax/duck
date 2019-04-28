#ifdef MD

void setup()
{
  Serial.begin(115200);

  iAm = "mama-duck";
  empty.whoAmI = "mama-duck";
  setupDuck();

  setupDisplay();
  setupLoRa();
  setupPortal();
  setupLED();
  setupButton();

  Serial.println("Mama Online");
  u8x8.drawString(0, 1, "Mama Online");
}

void loop()
{
  sensorVal = digitalRead(25);

  if(turnOn == false && sensorVal == LOW) {
    turnOn = true;
    setColor(13,5,0);
    delay(1000);
  } else if(turnOn == true && sensorVal == LOW) {
    turnOn = false;
    setColor(0,0,0);
    delay(1000);
  }
  
  // ⚠️ Parses Civilian Requests into Data Structure
  readData();
  if (offline.fromCiv == 1 && offline.phone != NULL && offline.phone != "")
  {
    Serial.println("Start Send");
    sendPayload(offline);
    Serial.print("Sending Wifi Data from Mama\n");
    offline = empty;
    offline.fromCiv = 0;
  }

  receive(LoRa.parsePacket());
  //  strstr(offline.path.toCharArray, empty.duckID) != NULL
  if (offline.fromCiv == 0 && offline.phone != NULL && offline.phone != "" && offline.path.indexOf(empty.duckID) < 0) {
    if(turnOn) {
      setColor(0,13,0);
    }
    offline.path = offline.path + "," + empty.duckID;
    sendPayload(offline);
    Serial.print("I'm here");
    delay(random(0,2500));
    sendPayload(offline);
    offline = empty;
  }

  // Sends Duck Stat every 30 minutes
  sendDuckStat(offline);
}

#endif
