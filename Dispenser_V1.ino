#include <LiquidCrystal_I2C.h>
#define LED_BACKLIGHT 7
#define LED_KUNING 8
#define LED_HIJAU 9
#define RELAY 10
#define BUZZER 6
#define SENSOR_FLOAT 4
#define SENSOR_WATER_FLOW 2
#define SENSOR_MAGNET 5

LiquidCrystal_I2C lcd(0x27, 16, 2);

byte block[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

byte sensorInterrupt = 0;
byte sensorPin       = SENSOR_WATER_FLOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long oldTime;

unsigned long time;

void setup() {
  Serial.begin(9600);
  pinMode(SENSOR_MAGNET, INPUT_PULLUP);
  pinMode(SENSOR_FLOAT, INPUT_PULLUP);
  pinMode(LED_BACKLIGHT, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_KUNING, OUTPUT);
  pinMode(LED_HIJAU, OUTPUT);
  pinMode(RELAY, OUTPUT);

  //  LCD Setup
  lcd.begin();
  lcd.backlight();
  lcdPrint("Smart Dispenser", "    V.4.0.1");

  //  Lampu Nyala
  for (int i = 0 ; i < 3 ; i++) {
    lampuSetup();
    delay(500);
  }
  delay(1000);
  lcd.clear();
  ////  Memulai sistem
  initializationSystem();
  digitalWrite(LED_HIJAU, 1);

  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);
  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  lcdPrint("Dispenser Siap", "Silahkan Minum");
  delay(3000);
  lcd.clear();
}

void loop() {
  
  if ((millis() - oldTime) > 1000)
  {
    detachInterrupt(0);
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;
    unsigned int frac;
    pulseCount = 0;
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }
  
  if (flowMilliLitres > 0) {
    digitalWrite(LED_KUNING, 1);
    lcd.clear();
    lcdPrint("Tunggu Sebentar", "Sedang Bekerja");
    lcd.clear();
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Anda Telah Minum ");
    lcd.setCursor(0, 1);
    lcd.print(totalMilliLitres);
    lcd.print(" ml");
    digitalWrite(BUZZER, 0);
    digitalWrite(LED_KUNING, 0);

    int floatState = digitalRead(SENSOR_FLOAT);
    Serial.print("float: ");
    Serial.println(floatState);

    int sensorValue = digitalRead(SENSOR_MAGNET);
    Serial.print("magnet: ");
    Serial.println(sensorValue);
    
//  Pompa Air 
    if (floatState == 0 && sensorValue == 1){
    time++ ;
    lcd.clear();
    Serial.println(time);
      delay(500);
      digitalWrite(LED_BACKLIGHT, HIGH);
      if(time > 30){
        digitalWrite(RELAY, LOW);
        lcd.setCursor(0, 0);
        lcd.print("Air Galon Habis!");
        delay(500);
        digitalWrite(BUZZER, HIGH);
        digitalWrite(LED_KUNING, HIGH);
        delay (250);
        digitalWrite(BUZZER, LOW);
        digitalWrite(LED_KUNING, LOW);
        delay (3000);
          }else{
            digitalWrite(RELAY, HIGH);
        }
    }else if (floatState == 0 && sensorValue == 0){
    time++ ;
    lcd.clear();
    Serial.println(time);
      delay(500);
      digitalWrite(LED_BACKLIGHT, HIGH);
      if(time > 30){
        digitalWrite(RELAY, LOW);
        lcd.setCursor(0, 0);
        lcd.print("Air Galon Habis!");
        delay(500);
        digitalWrite(BUZZER, HIGH);
        digitalWrite(LED_KUNING, HIGH);
        delay (250);
        digitalWrite(BUZZER, LOW);
        digitalWrite(LED_KUNING, LOW);
        delay (3000);
          }else{
            digitalWrite(RELAY, HIGH);
        }
    }else if(floatState == 1 && sensorValue ==0){
      time = 0;
      digitalWrite(RELAY, LOW);
      digitalWrite(LED_BACKLIGHT, LOW);
    }
    else{
      time = 0;
      digitalWrite(RELAY, LOW);
      digitalWrite(LED_BACKLIGHT, LOW);
    }
    
//  Pintu Galon
  }

}

// VOID SYSTEM
void pulseCounter()
{
  pulseCount++;
}

void lcdPrint(char *text1, char *text2)
{
  lcd.setCursor(0, 0);
  lcd.print(text1);
  lcd.setCursor(0, 1);
  lcd.print(text2);
}

void lampuSetup() {
  digitalWrite(LED_BACKLIGHT, 1);
  digitalWrite(LED_KUNING, 1);
  digitalWrite(LED_HIJAU, 1);
  digitalWrite(BUZZER, 1);
  delay(500);
  digitalWrite(LED_BACKLIGHT, 0);
  digitalWrite(LED_KUNING, 0);
  digitalWrite(LED_HIJAU, 0);
  digitalWrite(BUZZER, 0);
  delay(500);
}

void initializationSystem()
{
  lcd.setCursor(0, 0);
  lcd.print("Memulai Sistem");
  lcd.createChar(0, block);
  lcd.home();
  lcd.setCursor(0, 1);
  for (int j = 0; j < 16; j++)
  {
    lcd.write(j);
    delay(250);
  }
  lcd.clear();
}
