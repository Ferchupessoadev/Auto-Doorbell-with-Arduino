#include <RtcDS1302.h> 
#include <Wire.h> 
#include <IRremote.h>
#include <LiquidCrystal_I2C.h>

// config of lcd.
LiquidCrystal_I2C lcd(0x27, 16, 2);

// config of Rtc module ds1302
ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
// CONEXIONES DS1302:
// DS1302 CLK/SCLK --> 5
// DS1302 DAT/IO --> 4
// DS1302 RST/CE --> 2
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND

// pines
const int pinBuzzer = 8;
// variables
int timbre_sonando = false;
// botonos del control del kit de arduino.
#define boton_0 0xE916FF00
#define boton_1 0xF30CFF00
#define boton_2 0xE718FF00
#define boton_3 0xA15EFF00
#define boton_4 0xF708FF00
#define boton_5 0xE31CFF00
#define boton_6 0xA55AFF00
#define boton_7 0xBD42FF00
#define boton_8 0xAD52FF00
#define boton_9 0xB54AFF00
#define boton_more 0xEA15FF00
#define boton_less 0xF807FF00

const int timbres[26][2] = {
   // mañana.
// hora, minutos
   {7, 10},
   {7, 15},
   {8, 0}, 
   {8, 40},
   {8, 50},
   {9, 30},
   {10, 10},
   {10, 20},
   {11, 0},
   {11, 40},
   {11, 45},
   {12, 25},
   {13, 5},
   // tarde.
   {13, 25},
   {13, 30},
   {14, 10},
   {14, 50},
   {15, 0},
   {15, 40},
   {16, 20},
   {16, 30},
   {17, 10},
   {17, 50},
   {17, 55},
   {18, 35},
   {19, 25},
};

void setup () 
{
    Serial.begin(57600);
    // iniciamos el reloj.
    Rtc.Begin();

    // iniciamos el lcd
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("hello world");
    
    // buzzer pin
    pinMode(pinBuzzer, OUTPUT);
    
    IrReceiver.begin(9, DISABLE_LED_FEEDBACK);
}

bool es_recreo_o_cambio_de_hora(int hora, int minuto, int segundo) {
    for(long i = 0; i < 26; i++) {
        if(timbres[i][0] == hora and timbres[i][1] == minuto and segundo > 0 and segundo <= 10) {
            return true;
        }
    }
    return false;
};


int secondActual = 0;
int newSecond = secondActual;
char* printDateTime(RtcDateTime now) {
    Serial.print("Date: ");
    const char* dayOfWeekStr[] = {"Domingo", "Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado"};
    const char* dayOfWeek = dayOfWeekStr[now.DayOfWeek()];
    Serial.print(dayOfWeek);
    Serial.print(" ");
    Serial.print(now.Day());
    Serial.print("/");
    Serial.print(now.Month());
    Serial.print("/");
    Serial.print(now.Year());
    Serial.println();
    Serial.print("Time: ");
    Serial.print(now.Hour());
    Serial.print(":");
    Serial.print(now.Minute());
    Serial.print(":");
    Serial.print(now.Second());
    Serial.println();
    secondActual = now.Second();
    if(newSecond != secondActual and !timbre_sonando) {
       lcd.clear();  
    };
    newSecond = secondActual;
    lcd.setCursor(0,0);
    if(!timbre_sonando) {
      lcd.print(now.Hour());
      lcd.print(":");
      lcd.print(now.Minute());
      lcd.print(":");
      lcd.print(now.Second());  
    } else {
      lcd.print("Timbre sonando");
    }

    return dayOfWeek;
};
char opcion;
void loop ()
{
     while(IrReceiver.decode() == 0) {
         RtcDateTime now = Rtc.GetDateTime();
         const char* dayOfWeek = printDateTime(now);
         if (strcmp(dayOfWeek, "Sabado") != 0 and strcmp(dayOfWeek, "Domingo") != 0) {
            if(Serial.available()) {
              opcion = Serial.read();
            }
            if (opcion == 'p') {
                digitalWrite(pinBuzzer, HIGH);  
                timbre_sonando = true;
            } else if(es_recreo_o_cambio_de_hora(now.Hour(), now.Minute(), now.Second())) {
                digitalWrite(pinBuzzer, HIGH);
                timbre_sonando = true;
            } else if (timbre_sonando == true) {
                digitalWrite(pinBuzzer, LOW);  
                timbre_sonando = false;
                lcd.clear();  
            }
        }; 
     };
     switch (IrReceiver.decodedIRData.decodedRawData) {
        case boton_0:opcion = 'p'; break;
        case boton_1: opcion = 'n';break;
     };
    IrReceiver.resume();
    delay(10);
}
