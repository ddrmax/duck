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

  Serial.println("Mama Online");
  u8x8.drawString(0, 1, "Mama Online");
}

void loop()
{
  // ⚠️ Parses Civilian Requests into Data Structure
//  readData();
//  if (offline.fromCiv == 1 && offline.phone != NULL && offline.phone != "")
//  {
//    Serial.println("Start Send");
//    sendPayload(offline);
//    Serial.print("Sending Wifi Data from Mama\n");
//    offline = empty;
//    offline.fromCiv = 0;
//  }
//
//  receive(LoRa.parsePacket());
//  //  strstr(offline.path.toCharArray, empty.duckID) != NULL
//  if (offline.fromCiv == 0 && offline.phone != NULL && offline.phone != "" && offline.path.indexOf(empty.duckID) < 0) {
//    offline.path = offline.path + "," + empty.duckID;
//    sendPayload(offline);
//    Serial.print("I'm here");
//    delay(random(0,2500));
//    sendPayload(offline);
//    offline = empty;
//  }
  offline.messageId = makeId();
  offline.msg = "We are Here!!";
  offline.path = offline.duckID;
  offline.occupants = "1";

  sendPayload(offline);
  delay(60000*30);
}

String makeId() {
  char items[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  char uuid[9];

  for(int i = 0; i <= 3; i++) {
    uuid[i] = items[random(0,35)];
  }
  uuid[9] = '\0';

  String str = String(uuid);
  
  return  str;
  
}

#endif
