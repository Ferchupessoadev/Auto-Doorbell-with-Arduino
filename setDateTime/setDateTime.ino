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

void setup () 
{
    Serial.begin(57600);
    // iniciamos el reloj.
    Rtc.Begin();

    // Obtener la fecha y hora actual del sistema
    const char compileDate[] = __DATE__;
    const char compileTime[] = __TIME__;

    // Extraer los valores de año, mes, día, hora, minuto y segundo de la fecha de compilación
    int compileYear, compileMonth, compileDay, compileHour, compileMinute, compileSecond;
    char compileMonthStr[4];
    sscanf(compileDate, "%s %d %d", compileMonthStr, &compileDay, &compileYear);
    sscanf(compileTime, "%d:%d:%d", &compileHour, &compileMinute, &compileSecond);

    // Mapear el nombre del mes a su número correspondiente porque el Reloj solo admite los nombres que estan en el array de abajo.
    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for (int i = 0; i < 12; ++i) {
      if (strcmp(compileMonthStr, months[i]) == 0) {
          compileMonth = i + 1;
          break;
      }
    }
  
    // Configurar la fecha y hora en momento de compilación.
    RtcDateTime manualDateTime(compileYear, compileMonth, compileDay, compileHour, compileMinute, compileSecond);
    Rtc.SetDateTime(manualDateTime);
}
void loop() {
  

}
