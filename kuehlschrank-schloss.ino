#include <Adafruit_Fingerprint.h>

// On Leonardo/Micro or others with hardware serial, use those! #0 is green wire, #1 is white
// uncomment this line:
// #define mySerial Serial1

// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// comment these two lines if using hardware serial
SoftwareSerial mySerial(2, 3);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

//pin definition
#define ADDFINGER 7
#define DELFINGER 8
#define DELALL    9
#define COIN_PIN  10
#define TIMEOUT   5000 //in ms

int16_t id;
unsigned long sysTime;

uint8_t readnumber(void);
uint8_t deleteFingerprint(uint8_t);
uint8_t get_ID_count(void);
int getFingerprintIDez(void);
uint8_t getFingerprintEnroll(void);

void setup()
{
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);

  for (uint8_t i = 7; i <= 10; i++) {
    pinMode(i, INPUT_PULLUP);
  }


  uint8_t flag = 0;

  //initialisiere bis gefunden
  while (!flag) {

    // set the data rate for the sensor serial port
    finger.begin(57600);

    if (finger.verifyPassword()) {
      Serial.println("Found fingerprint sensor!");
      flag = 1;
    } else {
      Serial.println("Did not find fingerprint sensor :(\nRetrying...");
      delay(2000);
    }

  }

  finger.getTemplateCount();
  Serial.print("Sensor contains ");
  Serial.print(finger.templateCount);
  Serial.println(" templates");
  Serial.println("Waiting for valid finger...");


  

  uint8_t addFingerMode, delFingerMode, delAllMode;

}

uint8_t get_ID_count() {
  finger.getTemplateCount();
  return finger.templateCount;
}



//spannungsteiler für münzerkennung (links r1, rechts r2), anzahl pulse (1 2€, 2 1€, 3 50ctm 4 20ct, 5 10ct)


uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop()                     // run over and over again
{
  uint8_t test = !digitalRead(COIN_PIN);

  if(test)
    Serial.println("EINGANG");
  /*
  //pins einlesen
  uint8_t delFingerMode = !digitalRead(DELFINGER);
  uint8_t addFingerMode = !digitalRead(ADDFINGER);
  uint8_t delAllMode = !digitalRead(DELALL);
  

  if (addFingerMode) {
    Serial.println("Ready to enroll a fingerprint!");

    // muss geändert werden, damit existierende id nicht ueberschrieben wird------------------
    //evtl mit "finger.loadModel(ID)" -> "finger.getModel()" ???
    id = get_ID_count() + 1;


    Serial.print("Enrolling ID #");
    Serial.println(id);

    while (!getFingerprintEnroll());

  }
  else if (delFingerMode) {
    sysTime = millis();
    Serial.println("Halte den zu loeschenden Finger an den Sensor!");
    id = -1;

    // warte bis gueltige ID, oder wartezeit vorbei
    while (id == -1 && !(millis() - sysTime > TIMEOUT)) {
      id = getFingerprintIDez();
      delay(50);
    }
    if (id != -1) {
      Serial.print("Deleting ID #");
      Serial.println(id);

      deleteFingerprint(id);
    }
    else {
      Serial.println("Kein Finger gegeben!");
    }
  }
  //alle finger loeschen
  //else if(delAllMode)
  //  delAll();
  else {
    getFingerprintIDez();
    delay(50);            //don't need to run this at full speed.
  }
  */
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  return finger.fingerID;
}

uint8_t getFingerprintEnroll() {

  int p = -1;


  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);

  sysTime = millis();
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (millis() - sysTime > TIMEOUT) {
      p = -1;
      break;
    }
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  if (p != -1) {
    p = finger.image2Tz(1);
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
      case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
      case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
      case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
      case -1:
        Serial.println("Kein Finger gegeben!");
        break;
      default:
        Serial.println("Unknown error");
        return p;
    }

    Serial.println("Remove finger");
    delay(2000);
    p = 0;
    sysTime = millis();
    while (p != FINGERPRINT_NOFINGER && !(millis() - sysTime > TIMEOUT)) {
      p = finger.getImage();
    }
    Serial.print("ID ");
    Serial.println(id);
    p = -1;
    Serial.println("Place same finger again");
    sysTime = millis();
    while (p != FINGERPRINT_OK && !(millis() - sysTime > TIMEOUT)) {
      p = finger.getImage();
      switch (p) {
        case FINGERPRINT_OK:
          Serial.println("Image taken");
          break;
        case FINGERPRINT_NOFINGER:
          Serial.print(".");
          break;
        case FINGERPRINT_PACKETRECIEVEERR:
          Serial.println("Communication error");
          break;
        case FINGERPRINT_IMAGEFAIL:
          Serial.println("Imaging error");
          break;
        default:
          Serial.println("Unknown error");
          break;
      }

    }

    // OK success!

    p = finger.image2Tz(2);
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
      case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
      case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
      case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
      default:
        Serial.println("Unknown error");
        return p;
    }

    // OK converted!
    Serial.print("Creating model for #");
    Serial.println(id);

    p = finger.createModel();
    if (p == FINGERPRINT_OK) {
      Serial.println("Prints matched!");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      Serial.println("Communication error");
      return p;
    } else if (p == FINGERPRINT_ENROLLMISMATCH) {
      Serial.println("Fingerprints did not match");
      return p;
    } else {
      Serial.println("Unknown error");
      return p;
    }

    Serial.print("ID ");
    Serial.println(id);
    p = finger.storeModel(id);
    if (p == FINGERPRINT_OK) {
      Serial.println("Stored!");
      return 1;
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      Serial.println("Communication error");
      return p;
    } else if (p == FINGERPRINT_BADLOCATION) {
      Serial.println("Could not store in that location");
      return p;
    } else if (p == FINGERPRINT_FLASHERR) {
      Serial.println("Error writing to flash");
      return p;
    } else {
      Serial.println("Unknown error");
      return p;
    }
  }
  else {
    Serial.println("Zeit abgelaufen!");
  }
}



uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;

  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.print("Unknown error: 0x");
    Serial.println(p, HEX);
    return p;
  }
}
