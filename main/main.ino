#include <RtcDS1302.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// LCD.
LiquidCrystal_I2C lcd(0x20, 16, 2);

// Rtc module ds1302
ThreeWire myWire(11, 10, 12); // DAT, CLK, RST
RtcDS1302<ThreeWire> Rtc(myWire);

// KEYPAD
const byte ROWS = 4;
const byte COLS = 4;

// declaration of functions
bool es_recreo_o_cambio_de_hora(int hora, int minuto, int segundo, bool *timbre_sonando);
void printDateTime(RtcDateTime now, bool *timbre_sonando);
void add_key(String *input, char *key, char *msj);

char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup()
{
  Rtc.Begin();

  lcd.init();
  lcd.backlight();

  pinMode(A3, OUTPUT);
}

bool es_recreo_o_cambio_de_hora(int hora, int minuto, int segundo, bool *timbre_sonando)
{
  static const int timbres[26][2] = {
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
      {19, 15},
  };
  for (short i = 0; i < 26; i++)
  {
    if (timbres[i][0] == hora and timbres[i][1] == minuto and segundo > 0 and segundo <= 10)
    {
      *timbre_sonando = true;
      return true;
    }
  }
  *timbre_sonando = false;
  return false;
};

void printDateTime(RtcDateTime now, bool *timbre_sonando)
{
  static int secondActual = 0;
  static int newSecond = secondActual;
  secondActual = now.Second();
  if (newSecond != secondActual && !(*timbre_sonando))
  {
    lcd.clear();
  };
  newSecond = secondActual;
  lcd.setCursor(0, 0);
  if (!(*timbre_sonando))
  {
    lcd.setCursor(0, 0);
    lcd.print("DATE: ");
    lcd.print(now.Year());
    lcd.print(":");
    lcd.print(now.Month());
    lcd.print(":");
    lcd.print(now.Day());
    lcd.setCursor(0, 1);
    lcd.print("TIME: ");
    lcd.print(now.Hour());
    lcd.print(":");
    lcd.print(now.Minute());
    lcd.print(":");
    lcd.print(now.Second());
  }
  else
  {
    if (now.Second() == 1)
      lcd.clear();
    lcd.print("Timbre sonando");
  }
};

void add_key(String *input, char *key, char *msj)
{
  if ((*key) >= '0' && (*key) <= '9')
  {
    *input += (*key);
    lcd.clear();
    lcd.print(msj);
    lcd.print((*input));
  }
}

void loop()
{
  static int currentStep = 0; // 0 -> inputYear, 1 -> inputMonth, 2 -> inputDay , etc.
  static bool config_mode = false;
  static bool timbre_sonando = false;
  static bool *timbre_sonando_ptr = &timbre_sonando;
  static int compileYear, compileMonth, compileDay, compileHour, compileMinute, compileSecond;
  static String inputYear, inputMonth, inputDay, inputHour, inputMinute, inputSecond;
  char key = keypad.getKey();
  if (key != NO_KEY || config_mode)
  {
    if (key == '#')
    {
      lcd.setCursor(0, 0);
      config_mode = true;
      lcd.clear();
      lcd.print("Configuracion de");
      lcd.setCursor(0, 1);
      lcd.print("fecha y hora");
    }
    else if (key == 'D')
    {
      config_mode = false;
      currentStep = 0;
      inputYear = inputMonth = inputDay = inputHour = inputMinute = inputSecond = "";
    }
    else if (config_mode)
    {
      switch (currentStep)
      {
      case 0:
        add_key(&inputYear, &key, "Year: ");
        if (inputYear.length() >= 4)
        {
          currentStep++;
          delay(500);
          lcd.clear();
          lcd.print("Month: ");
        }
        break;
      case 1:
        add_key(&inputMonth, &key, "Month: ");
        if (inputMonth.length() >= 2)
        {
          currentStep++;
          delay(500);
          lcd.clear();
          lcd.print("Day: ");
        }
        break;
      case 2:
        add_key(&inputDay, &key, "Day: ");
        if (inputDay.length() >= 2)
        {
          currentStep++;
          delay(500);
          lcd.clear();
          lcd.print("Hour: ");
        }
        break;
      case 3:
        add_key(&inputHour, &key, "Hour: ");
        if (inputHour.length() >= 2)
        {
          currentStep++;
          delay(500);
          lcd.clear();
          lcd.print("Minute: ");
        }
        break;
      case 4:
        add_key(&inputMinute, &key, "Minute: ");

        if (inputMinute.length() >= 2)
        {
          currentStep++;
          delay(500);
          lcd.clear();
          lcd.print("Second: ");
        }
        break;
      case 5:

        add_key(&inputSecond, &key, "Second: ");
        if (inputSecond.length() >= 2)
        {
          int year = inputYear.toInt();
          int month = inputMonth.toInt();
          int day = inputDay.toInt();
          int hour = inputHour.toInt();
          int minute = inputMinute.toInt();
          int second = inputSecond.toInt();

          RtcDateTime compiled = RtcDateTime(year, month, day, hour, minute, second);
          Rtc.SetDateTime(compiled);
          config_mode = false;
          currentStep = 0;
          inputYear = inputMonth = inputDay = inputHour = inputMinute = inputSecond = "";
        }
        break;
      }
    }
    return;
  }

  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now, timbre_sonando_ptr);
  const char *dayOfWeekStr[] = {"Domingo", "Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sabado"};
  const char *dayOfWeek = dayOfWeekStr[now.DayOfWeek()];
  if (strcmp(dayOfWeek, "Sabado") != 0 && strcmp(dayOfWeek, "Domingo") != 0)
  {
    if (es_recreo_o_cambio_de_hora(now.Hour(), now.Minute(), now.Second(), timbre_sonando_ptr))
    {
      digitalWrite(A3, HIGH);
    }
    else
    {
      digitalWrite(A3, LOW);
    }
  };
}
