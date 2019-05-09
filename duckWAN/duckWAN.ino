// DuckWAN
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

// OG
#include <WiFi.h>
#include <U8x8lib.h>

#include <DNSServer.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "indexBeta.h"

/***************************************************
  un/comment lines to compile Ducklink/Mama/Papa
***************************************************/

// Recommendation First compile Mama board, then reverse and compile Papa board
//#define DL
//const char *AP = " 🆘 DUCK EMERGENCY PORTAL";

//#define MD
//const char *AP = " 🆘 MAMA EMERGENCY PORTAL";

#define PD
const char *AP = " 🆘 PAPA EMERGENCY PORTAL";

#define THIRTYMIN (1000UL * 60 * 30);
unsigned long rolltime = millis() + THIRTYMIN;

IPAddress apIP(192, 168, 1, 1);
WebServer webServer(80);

DNSServer dnsServer;
const byte DNS_PORT = 53;

/**
   Hotspot/Access Point (🐥 DuckLink 🆘 )
   Local DNS (duck.local)
*/
//const char *AP   = " 🆘 EMERGENCY PORTAL";


const char *DNS  = "duck";

String portal = MAIN_page;
String id = "";
String iAm = "Civ";
String runTime;

/**
   Tracer for debugging purposes
   Toggle (trace = 1) to print offlineements in Serial
   Toggle (trace = 0) to turn off offlineements
*/
int trace         = 0;

byte msgCount     = 0;             // count of outgoing messages
int interval      = 2000;          // interval between sends
long lastSendTime = 0;             // time of last packet send


// Structure with message data
typedef struct
{
  // Duck
  String whoAmI;   // offline Classifier (Default is Civ for Civilian)
  String duckID;   // Duck (ESP32) Mac Address
  String whereAmI; // Geo-coordinates (Default in 0,0 - Null Island) - Under construction - Omer's App
  String runTime;  // How long have I been on?

  // Check to see if message is from Civilian or Duck
  int fromCiv = 0;
  String messageId;
  // Civilian
  String fname;
  String street;
  String phone;
  String occupants;
  String danger;
  String vacant;
  String firstaid;
  String water;
  String food;
  String msg;
  String path;
} Data;

Data offline;
Data empty;

// the OLED used
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

void setupDisplay()
{
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
}

/**
   showReceivedstat
   Displays Received stat on OLED and Serial Monitor
*/
void showReceivedData()
{
  /**
     The total time it took for PAPA to create a packet,
     send it to MAMA. MAMA parsing victim requests, and
     send it back to PAPA.
  */
  String waiting = String(millis() - lastSendTime);

  Serial.println("Class: "        +  offline.whoAmI     );
  Serial.println("ID : "          +  offline.duckID    );
  Serial.println("Location: "     +  offline.whereAmI     );
  Serial.println("On for: "       +  offline.runTime + " milliseconds\n" );
  Serial.println("Message ID: "   +  offline.messageId );

  Serial.println("Name: "         +  offline.fname     );
  Serial.println("Street: "       +  offline.street    );
  Serial.println("Phone: "        +  offline.phone     );
  Serial.println("Occupants: "    +  offline.occupants );
  Serial.println("Dangers: "      +  offline.danger    );
  Serial.println("Vacant: "       +  offline.vacant    );
  Serial.println("First Aid: "    +  offline.firstaid  );
  Serial.println("Water: "        +  offline.water     );
  Serial.println("Food: "         +  offline.food      );
  Serial.println("Mess: "         +  offline.msg       );
  Serial.println("Time: "         +  waiting + " milliseconds\n");

  Serial.println("Path: "         +  offline.path      );

  Serial.print("FromCiv: ");
  Serial.println(offline.fromCiv    );
}

void setupPortal()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  WiFi.softAP(AP);
  Serial.println("Created Hotspot");

  dnsServer.start(DNS_PORT, "*", apIP);

  webServer.onNotFound([]()
  {
    webServer.send(200, "text/html", portal);
  });

  webServer.on("/id", []() {
    webServer.send(200, "text/html", offline.duckID + "," + offline.whoAmI);
  });

  webServer.on("/mac", []() {
    String    page = "<h1>Duck Mac Address</h1><h3>Data:</h3> <h4>" + offline.duckID + "</h4>";
    webServer.send(200, "text/html", page);
  });

  webServer.begin();

  if (!MDNS.begin(DNS))
  {
    Serial.println("Error setting up MDNS responder!");
  }
  else
  {
    Serial.println("Created local DNS");
    MDNS.addService("http", "tcp", 80);
  }
}

void readData()
{
  //  Data offlineA = offline;

  //Serial.println("Tracer -- ID: " + id + " Webserver: " + webServer.arg(0));

  // Handles Captive Portal Requests
  dnsServer.processNextRequest();
  webServer.handleClient();

  String webId = webServer.arg(0);

  if (id != webId)
  {
    u8x8.clear();
    u8x8.drawString(0, 4, "New Response");

    //    for (int i = 0; i < webServer.args(); i++)
    //    {
    //      Serial.println(webServer.argName(i) + ": " + webServer.arg(i));
    //    }

    offline.fromCiv    = 1;
    offline.messageId  = webServer.arg(0);
    offline.fname      = webServer.arg(1);
    offline.street     = webServer.arg(2);
    offline.phone      = webServer.arg(3);
    offline.occupants  = webServer.arg(4);
    offline.danger     = webServer.arg(5);
    offline.vacant     = webServer.arg(6);
    offline.firstaid   = webServer.arg(7);
    offline.water      = webServer.arg(8);
    offline.food       = webServer.arg(9);
    offline.msg        = "";
    offline.path       = empty.duckID;

    u8x8.setCursor(0, 16);
    u8x8.print("Name: " + offline.fname);

    Serial.println("Before____ID: " + id + " Webserver: " + webServer.arg(0));

    id = webId;

    Serial.println("After_____ID: " + id + " Webserver: " + webServer.arg(0));
    showReceivedData();

  }
  //  return offlineA;
}

String duckID()
{
  char id1[15];
  char id2[15];

  uint64_t chipid = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
  uint16_t chip = (uint16_t)(chipid >> 32);

  snprintf(id1, 15, "%04X", chip);
  snprintf(id2, 15, "%08X", (uint32_t)chipid);

  String ID1 = id1;
  String ID2 = id2;

  return ID1 + ID2;
}

void setupDuck()
{
  offline.whoAmI   = iAm;
  empty.whoAmI     = offline.whoAmI;
  offline.duckID   = duckID();
  empty.duckID     = offline.duckID;
  offline.whereAmI = "0,0"; // Until further dev, default is null island
  empty.whereAmI   = offline.whereAmI;
  offline.runTime  = millis();
  empty.runTime    = millis();

  // Test - Print to serial
  Serial.println("\nClass: "        +  offline.whoAmI     );
  Serial.println("ID : "            +  offline.duckID    );
  Serial.println("Location: "       +  offline.whereAmI     );
  Serial.println("On for: "         +  offline.runTime + " milliseconds\n\n" );
}


static const PROGMEM u1_t NWKSKEY[16] ={ 0xCB, 0x46, 0x63, 0xCD, 0xDD, 0x22, 0x8B, 0x16, 0x1E, 0xD3, 0x6A, 0x1E, 0xB9, 0x98, 0x2C, 0x76 };
static const PROGMEM u1_t APPSKEY[16] ={ 0xBE, 0x06, 0x39, 0x85, 0x83, 0xC3, 0xB8, 0x46, 0xCF, 0x91, 0xB0, 0x30, 0x33, 0x64, 0x49, 0x37 };
static const u4_t DEVADDR = 0x26011BC6; // <-- Change this address for every node!

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in config.h, otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

//static const u1_t PROGMEM APPEUI[8] = { 0107060504030201 };
//void os_getArtEui (u1_t* buf)
//{
//  memcpy_P(buf, APPEUI, 8);
//}
//static const u1_t PROGMEM DEVEUI[8] = { 0107060504030201 };
//void os_getDevEui (u1_t* buf) 
//{
//  memcpy_P(buf, DEVEUI, 8);
//}
//
//static const u1_t PROGMEM APPKEY[16] = { 01020304050607030210111213141516 };
//void os_getDevKey (u1_t* buf) 
//{
//  memcpy_P(buf, APPKEY, 16);
//}

static uint8_t mydata[] = "QuackityQuackQuack";
static osjob_t sendjob;

// Schedule TX every this many seconds.
const unsigned TX_INTERVAL = 60;

const lmic_pinmap lmic_pins = {
  .nss = 18,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 14,
  .dio = {26, 33, 32}
};

void onEvent (ev_t ev)
{
  Serial.print(os_getTime());
  Serial.print(": ");
  switch (ev)
  {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      break;

    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      break;

    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      break;

    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      break;

    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;

    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      {
        u4_t netid = 0;
        devaddr_t devaddr = 0;
        u1_t nwkKey[16];
        u1_t artKey[16];
        LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);

        Serial.print("netid: ");
        Serial.println(netid, DEC);

        Serial.print("devaddr: ");
        Serial.println(devaddr, HEX);

        Serial.print("artKey: ");
        for (int i = 0; i < sizeof(artKey); ++i)
        {
          Serial.print(artKey[i], HEX);
        }
        Serial.println("");

        Serial.print("nwkKey: ");
        for (int i = 0; i < sizeof(nwkKey); ++i)
        {
          Serial.print(nwkKey[i], HEX);
        }
        Serial.println("");
      }

      // Disable link check validation (automatically enabled
      // during join, but because slow data rates change max TX
      // size, we don't use it in this example.
      LMIC_setLinkCheckMode(0);
      break;

    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;

    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;

    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));

      if (LMIC.txrxFlags & TXRX_ACK)
        Serial.println(F("Received ack"));

      if (LMIC.dataLen)
      {
        Serial.print(F("Received "));
        Serial.print(LMIC.dataLen);
        Serial.println(F(" bytes of payload"));
      }

      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
      break;

    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      break;

    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;

    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      break;

    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      break;

    case EV_TXSTART:
      Serial.println(F("EV_TXSTART"));
      break;
    default:
      Serial.print(F("Unknown event: "));
      Serial.println((unsigned) ev);
      break;
  }
}

void do_send(osjob_t* j)
{
  if (LMIC.opmode & OP_TXRXPEND)
  {
    Serial.println(F("OP_TXRXPEND, not sending"));
  }
  else
  {
    LMIC_setTxData2(1, mydata, sizeof(mydata) - 1, 0);
    Serial.println(F("Packet queued"));
  }
}

void setup()
{
  Serial.begin(115200);

  iAm = "duckWAN";
  setupDuck();

  setupDisplay();
  setupWAN();
  setupPortal();

  Serial.println("Ducklink Online");
  u8x8.drawString(0, 1, "Ducklink Online");
}

void loop()
{
  // ⚠️ Parses Civilian Requests into Data Structure
  readData();
  
  if (offline.fromCiv == 1 && offline.fname != NULL && offline.fname != "")
  {
    Serial.println("Start send from DuckLink");

    // Need new sendPayload and new couple function
    // sendPayload(offline);
    // showReceivedData();

    loopWAN();
    offline = empty;
    setupDuck();
  }
  else
  {
    delay(500);
    Serial.print(".");
  }

  // ⚠️ Need New sendDuckStat
  // Sends Duck Stat every 30 minutes
  //  sendDuckStat(offline);
}

void setupWAN()
{
  Serial.begin(9600);
  Serial.println(F("Starting"));

  os_init();
  LMIC_reset();

  do_send(&sendjob);
}

void loopWAN()
{
  os_runloop_once();
}
