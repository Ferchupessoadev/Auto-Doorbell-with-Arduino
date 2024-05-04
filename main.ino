// CONEXIONES DS1302:
// DS1302 CLK/SCLK --> 5
// DS1302 DAT/IO --> 4
// DS1302 RST/CE --> 2
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND

#include <RtcDS1302.h> 
#include <Wire.h> 

ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

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
   {19, 12},
};

const int pinBuzzer = 8;
int timbre_sonando = false;

void setup () 
{
    Serial.begin(57600);
    // iniciamos el reloj.
    Rtc.Begin();
    
    // buzzer pin
    pinMode(pinBuzzer, OUTPUT);
}

bool es_recreo_o_cambio_de_hora(int hora, int minuto, int segundo) {
    for(long i = 0; i < 26; i++) {
        if(timbres[i][0] == hora and timbres[i][1] == minuto and segundo > 0 and segundo <= 10) {
            return true;
        }
    }
    return false;
};



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
    return dayOfWeek;
};

void loop ()
{
     RtcDateTime now = Rtc.GetDateTime();
     const char* dayOfWeek = printDateTime(now);
     // if (strcmp(dayOfWeek, "Sabado") != 0 and strcmp(dayOfWeek, "Domingo") != 0) {
        if(es_recreo_o_cambio_de_hora(now.Hour(), now.Minute(), now.Second())) {
            digitalWrite(pinBuzzer, HIGH);
            timbre_sonando = true;
        } else if (timbre_sonando == true) {
                digitalWrite(pinBuzzer, LOW);  
                timbre_sonando = false;
        }
    // };
     delay(1000);
}
