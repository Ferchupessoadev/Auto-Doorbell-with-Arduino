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

const int timbres[100][100] = {
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
}

const int feriadosArg[11][2] = {
  { 1, 1 },           // Año Nuevo
  { 24, 3 },          // Día Nacional de la Memoria por la Verdad y la Justicia
  { 2, 4 },           // Día del Veterano y de los Caídos en la Guerra de Malvinas
  { 1, 5 },           // Día del Trabajador
  { 25, 5 },          // Día de la Revolución de Mayo
  { 20, 6 },          // Día de la Bandera
  { 9, 7 },           // Día de la Independencia
  { 17, 9 },          // Paso a la Inmortalidad del Gral. José de San Martín
  { 12, 10 },         // Día del Respeto a la Diversidad Cultural (antes Día de la Raza)
  { 25, 12 }          // Navidad
};

int cantidadDeFeriadosArg = 10;

int pinBuzzer = 8;

void setup () 
{
    Serial.begin(57600);
    // iniciamos el reloj.
    Rtc.Begin();
    // buzzer pin
    pinMode(pinBuzzer, OUTPUT);
//  Obtener la fecha y hora actual del sistema
//  const char compileDate[] = __DATE__;
//  const char compileTime[] = __TIME__;

//  Extraer los valores de año, mes, día, hora, minuto y segundo de la fecha de compilación
//  int compileYear, compileMonth, compileDay, compileHour, compileMinute, compileSecond;
//  char compileMonthStr[4];
//  sscanf(compileDate, "%s %d %d", compileMonthStr, &compileDay, &compileYear);
//  sscanf(compileTime, "%d:%d:%d", &compileHour, &compileMinute, &compileSecond);

//  Mapear el nombre del mes a su número correspondiente porque el Reloj solo admite los nombres que estan en el array de abajo.
//  const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
//  for (int i = 0; i < 12; ++i) {
//     if (strcmp(compileMonthStr, months[i]) == 0) {
//         compileMonth = i + 1;
//         break;
//     }
//  }
//
//  Configurar la fecha y hora en momento de compilación.
//  RtcDateTime manualDateTime(compileYear, compileMonth, compileDay, compileHour, compileMinute, compileSecond);
//  Rtc.SetDateTime(manualDateTime);
}

bool esFeriado(int dia, int mes) {
  for(long i = 0; i < cantidadDeFeriadosArg;i++) {
    if (dia == feriadosArg[i][0] and dia == feriadosArg[i][1]) {
      return true;  
    }
  }
  return false;
}

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
     if (strcmp(dayOfWeek, "Sabado") != 0 and strcmp(dayOfWeek, "Domingo") != 0) {
        const bool hoyEsFeriado = esFeriado(now.Day(), now.Month());
        if(now.Hour() == 16 and now.Minute() == 42) {
          digitalWrite(pinBuzzer, HIGH);
        };
     };
     delay(1000); // one second
}
