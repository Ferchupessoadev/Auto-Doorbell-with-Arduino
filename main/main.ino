#include <RtcDS1302.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <avr/pgmspace.h>

// Configuración de Hardware
#define TIMEOUT_SECOND 5
#define RELAY_PIN A3
#define RELAY_ACTIVE HIGH // Cambiar a LOW si el módulo relé activa por nivel bajo

// LCD I2C (Dirección 0x20, 16 columnas, 2 filas)
LiquidCrystal_I2C lcd(0x20, 16, 2);
static bool lcd_state = true;

// Módulo RTC DS1302 (DAT, CLK, RST)
ThreeWire myWire(11, 10, 12);
RtcDS1302<ThreeWire> Rtc(myWire);

// Teclado Matricial 4x4
const byte ROWS = 4;
const byte COLS = 4;

/*
 * NOTA DE CONEXIÓN DE PINES:
 * Los pines 0 y 1 son los pines de recepción/transmisión Serial (RX/TX).
 * Si requiere usar depuración por Puerto Serie, reconecte el teclado a otros pines
 * (por ejemplo, rowPins: 2, 3, 4, 5 y colPins: 6, 7, 8, 9).
 */
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte rowPins[ROWS] = {0, 1, 2, 3};
byte colPins[COLS] = {4, 5, 6, 7};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Estructura para almacenar horarios en Flash (PROGMEM) y liberar RAM
struct TimbreTime
{
  uint8_t hour;
  uint8_t minute;
};

#define TIMBRES_COUNT 26
const TimbreTime timbres[TIMBRES_COUNT] PROGMEM = {
    {7, 10}, {7, 15}, {8, 0}, {8, 40}, {8, 50}, {9, 30}, {10, 10}, {10, 20}, {11, 0}, {11, 40}, {11, 45}, {12, 25}, {13, 5}, {13, 25}, {13, 30}, {14, 10}, {14, 50}, {15, 0}, {15, 40}, {16, 20}, {16, 30}, {17, 10}, {17, 50}, {17, 55}, {18, 35}, {19, 15}};

// Estado de configuración por teclado
struct ConfigState
{
  bool active = false;
  uint8_t step = 0; // 0:Año, 1:Mes, 2:Día, 3:Hora, 4:Min, 5:Seg
  char buffer[5] = {0};
  uint8_t bufLen = 0;

  uint16_t year = 0;
  uint8_t month = 0;
  uint8_t day = 0;
  uint8_t hour = 0;
  uint8_t minute = 0;
  uint8_t second = 0;
};

static ConfigState configState;

// Declaración de funciones
bool es_recreo_o_cambio_de_hora(uint8_t hora, uint8_t minuto, uint8_t segundo, bool *timbre_sonando);
void printDateTime(const RtcDateTime &now, bool timbre_sonando);
void renderConfigScreen();
void processConfigKey(char key);

void setup()
{
  Rtc.Begin();

  lcd.init();
  lcd.backlight();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, !RELAY_ACTIVE); // Apagado por defecto
}

bool es_recreo_o_cambio_de_hora(uint8_t hora, uint8_t minuto, uint8_t segundo, bool *timbre_sonando)
{
  // El timbre debe sonar durante los primeros TIMEOUT_SECOND segundos (0 a TIMEOUT_SECOND-1)
  if (segundo < TIMEOUT_SECOND)
  {
    for (uint8_t i = 0; i < TIMBRES_COUNT; i++)
    {
      uint8_t h = pgm_read_byte(&timbres[i].hour);
      uint8_t m = pgm_read_byte(&timbres[i].minute);
      if (h == hora && m == minuto)
      {
        *timbre_sonando = true;
        return true;
      }
    }
  }
  *timbre_sonando = false;
  return false;
}

void printDateTime(const RtcDateTime &now, bool timbre_sonando)
{
  char line0[17];
  char line1[17];

  if (timbre_sonando)
  {
    snprintf(line0, sizeof(line0), " TIMBRE SONANDO ");
    snprintf(line1, sizeof(line1), "  *** RING ***  ");
  }
  else
  {
    snprintf(line0, sizeof(line0), "DATE: %04u/%02u/%02u", now.Year(), now.Month(), now.Day());
    snprintf(line1, sizeof(line1), "TIME: %02u:%02u:%02u", now.Hour(), now.Minute(), now.Second());
  }

  lcd.setCursor(0, 0);
  lcd.print(line0);
  lcd.setCursor(0, 1);
  lcd.print(line1);
}

void renderConfigScreen()
{
  char line0[17];
  char line1[17];

  const char *prompts[] = {
      "Anno (2000-2099)",
      "Mes (01-12)",
      "Dia (01-31)",
      "Hora (00-23)",
      "Minuto (00-59)",
      "Segundo (00-59)"};

  snprintf(line0, sizeof(line0), "%-16s", prompts[configState.step]);
  snprintf(line1, sizeof(line1), "Val: %-11s", configState.buffer);

  lcd.setCursor(0, 0);
  lcd.print(line0);
  lcd.setCursor(0, 1);
  lcd.print(line1);
}

void processConfigKey(char key)
{
  if (key == 'D')
  {
    // Cancelar modo configuración
    configState.active = false;
    configState.step = 0;
    configState.bufLen = 0;
    configState.buffer[0] = '\0';
    lcd.clear();
    return;
  }

  if (key == '*')
  {
    // Borrar último dígito (Backspace)
    if (configState.bufLen > 0)
    {
      configState.bufLen--;
      configState.buffer[configState.bufLen] = '\0';
      renderConfigScreen();
    }
    return;
  }

  if (key >= '0' && key <= '9')
  {
    uint8_t maxLen = (configState.step == 0) ? 4 : 2;
    if (configState.bufLen < maxLen)
    {
      configState.buffer[configState.bufLen++] = key;
      configState.buffer[configState.bufLen] = '\0';
      renderConfigScreen();

      // Si completó la cantidad de dígitos esperada
      if (configState.bufLen == maxLen)
      {
        int val = atoi(configState.buffer);
        bool valid = false;

        switch (configState.step)
        {
        case 0: // Año
          if (val >= 2000 && val <= 2099)
          {
            configState.year = val;
            valid = true;
          }
          break;
        case 1: // Mes
          if (val >= 1 && val <= 12)
          {
            configState.month = val;
            valid = true;
          }
          break;
        case 2: // Día
          if (val >= 1 && val <= 31)
          {
            configState.day = val;
            valid = true;
          }
          break;
        case 3: // Hora
          if (val >= 0 && val <= 23)
          {
            configState.hour = val;
            valid = true;
          }
          break;
        case 4: // Minuto
          if (val >= 0 && val <= 59)
          {
            configState.minute = val;
            valid = true;
          }
          break;
        case 5: // Segundo
          if (val >= 0 && val <= 59)
          {
            configState.second = val;
            valid = true;
          }
          break;
        }

        if (valid)
        {
          configState.step++;
          configState.bufLen = 0;
          configState.buffer[0] = '\0';

          if (configState.step > 5)
          {
            // Guardar en el RTC DS1302
            RtcDateTime dt(configState.year, configState.month, configState.day,
                           configState.hour, configState.minute, configState.second);
            Rtc.SetDateTime(dt);

            configState.active = false;
            configState.step = 0;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(" Guardado OK! ");
            delay(1000);
            lcd.clear();
          }
          else
          {
            renderConfigScreen();
          }
        }
        else
        {
          // Entrada inválida: limpiar buffer para reintentar
          lcd.setCursor(0, 1);
          lcd.print("Err! Reintente  ");
          delay(800);
          configState.bufLen = 0;
          configState.buffer[0] = '\0';
          renderConfigScreen();
        }
      }
    }
  }
}

void loop()
{
  char key = keypad.getKey();

  // Alternar retroiluminación LCD con la tecla 'A' en cualquier momento
  if (key == 'A')
  {
    lcd_state = !lcd_state;
    if (lcd_state)
      lcd.backlight();
    else
      lcd.noBacklight();
  }

  if (configState.active)
  {
    if (key != NO_KEY)
    {
      processConfigKey(key);
    }
    return;
  }

  // Presionar '#' activa el modo de configuración
  if (key == '#')
  {
    configState.active = true;
    configState.step = 0;
    configState.bufLen = 0;
    configState.buffer[0] = '\0';
    lcd.clear();
    renderConfigScreen();
    return;
  }

  // Funcionamiento Normal
  static bool timbre_sonando = false;
  RtcDateTime now = Rtc.GetDateTime();

  printDateTime(now, timbre_sonando);

  // Verificar si es fin de semana (0: Domingo, 6: Sábado)
  uint8_t dow = now.DayOfWeek();
  if (dow != 0 && dow != 6)
  {
    if (es_recreo_o_cambio_de_hora(now.Hour(), now.Minute(), now.Second(), &timbre_sonando))
    {
      digitalWrite(RELAY_PIN, RELAY_ACTIVE);
    }
    else
    {
      digitalWrite(RELAY_PIN, !RELAY_ACTIVE);
    }
  }
  else
  {
    digitalWrite(RELAY_PIN, !RELAY_ACTIVE);
    timbre_sonando = false;
  }
}
