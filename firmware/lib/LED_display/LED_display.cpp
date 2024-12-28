/*
 LED displays control module

*/
#include <Arduino.h>

#include "LED_display.h"

byte segment_digits[11] = 
{
  0b11000000, // 0
  0b11111001, // 1
  0b10100100, // 2
  0b10110000, // 3
  0b10011001, // 4
  0b10010010, // 5
  0b10000010, // 6
  0b11111000, // 7
  0b10000000, // 8
  0b10010000, // 9
  0b11111111  // all off
};

/* Define the pins connected to each segment  */
int segment_pins[7] = {A_SEG, B_SEG, C_SEG, D_SEG, E_SEG, F_SEG, G_SEG};

/* Define the pins connected to commons */
int com_pins[13] = {COM_D1, COM_D2, COM_M2, COM_Y3, COM_Y4, COM_HR1,
                    COM_HR2, COM_MIN1, COM_MIN2, COM_T2, COM_T1, LED_DAY, LED_C};

/* Pins to LEDs of Week day */
byte week_d[7] = {G_SEG, F_SEG, A_SEG, B_SEG, C_SEG, E_SEG, D_SEG};                    

byte display_state = 0;

byte segments = 0;
byte i = 0;

void segments_write(byte n)
{
  segments = segment_digits[n];
  for (i = 0; i < 7; i++) 
  { 
    digitalWrite(segment_pins[i], bitRead(segments, i)); 
  }
}

/* Function to read temperature from NTC thermistor and return an integer temperature value from 0 to 99°C */
int read_temperature() {
  int adc_value = analogRead(TEMP_PIN);  /* Read analog value from the thermistor */
  float voltage = adc_value * (VCC / ADC_MAX);  /* Convert ADC value to voltage */
  float resistance = REF_RESISTANCE * ((VCC / voltage) - 1.0);  /* Calculate the resistance of the thermistor */

  /* Calculate temperature using the Steinhart-Hart equation */
  float steinhart;
  steinhart = resistance / NOM_RESISTANCE;  /* (R/R25) */
  steinhart = log(steinhart);  /* ln(R/R25) */
  steinhart /= B_COEFFICIENT;  /* 1/B * ln(R/R25) */
  steinhart += 1.0 / (NOM_TEMPERATURE + 273.15);  /* + (1/T25) */
  steinhart = 1.0 / steinhart;  /* Invert the value */
  steinhart -= 273.15;  /* Convert from Kelvin to Celsius */

  /* Apply the offset for calibration */
  steinhart += TEMP_OFFSET;

  /* Convert to integer from 0 to 99 degrees Celsius */
  int temperature = int(steinhart);

  /* Ensure the temperature is within the 0-99 range */
  if (temperature < 0) {
    temperature = 0;
  } else if (temperature > 99) {
    temperature = 99;
  }

  return temperature;
}

/*
  Init display pins
*/
void display_init(void)
{
  for (i = 0; i < 7; i++)
  {
    pinMode(segment_pins[i], OUTPUT);
    digitalWrite(segment_pins[i], HIGH);
  }

  for (i = 0; i < 13; i++)
  {
    pinMode(com_pins[i], OUTPUT);
    digitalWrite(com_pins[i], LOW); 
  }
}

/* Show all infos in the LED displays */
void display_ctrl(info_struct infos)
{
  switch (display_state)
  {
    /* First digit of Day */
    case DISPLAY_DAY1:
      if(infos.day/10)  /* Display off in case of 0 */
      {
        segments_write(infos.day / 10); 
      }else
      {
        segments_write(10);
      }
      if(infos.month / 10)  /* First digit of month is shared here */
      {
        digitalWrite(F_SEG, LOW);
      }
      digitalWrite(COM_D1, HIGH);
      delay(1);
      segments_write(10);
      digitalWrite(COM_D1, LOW);
      display_state = DISPLAY_DAY2;
      break;

    /* Second digit of Day */
    case DISPLAY_DAY2:
      segments_write(infos.day % 10);
      digitalWrite(COM_D2, HIGH);
      delay(1);
      segments_write(10);
      digitalWrite(COM_D2, LOW);
      display_state = DISPLAY_MONTH1;
      break;

    /* First digit of Month */
    case DISPLAY_MONTH1:
      /* Show with DAY 1 display */
      display_state = DISPLAY_MONTH2;
      break;

    /* Second digit of Month */
    case DISPLAY_MONTH2:
      segments_write(infos.month % 10);
      digitalWrite(COM_M2, HIGH);
      delay(1);
      segments_write(10);
      digitalWrite(COM_M2, LOW);
      display_state = DISPLAY_YEAR1_2;
      break;

    /* First two digits of Year */
    case DISPLAY_YEAR1_2:
      /* First two digits of year are always on */
      display_state = DISPLAY_YEAR3;
      break;

    /* Third digit of Year */
    case DISPLAY_YEAR3:
      segments_write((infos.year / 10) % 10);
      digitalWrite(COM_Y3, HIGH);
      delay(1);
      segments_write(10);
      digitalWrite(COM_Y3, LOW);
      display_state = DISPLAY_YEAR4;
      break;

    /* Fourth digit of Year */
    case DISPLAY_YEAR4:
      segments_write(infos.year % 10);
      digitalWrite(COM_Y4, HIGH);
      delay(1);
      segments_write(10);
      digitalWrite(COM_Y4, LOW);
      display_state = DISPLAY_HOUR1;
      break;

    /* First digit of Hour */
    case DISPLAY_HOUR1:
      if(infos.hour / 10)
      {
        segments_write(infos.hour / 10);
      }else
      {
        segments_write(10);
      }
      digitalWrite(COM_HR1, HIGH);
      delay(1);
      segments_write(10);
      digitalWrite(COM_HR1, LOW);
      display_state = DISPLAY_HOUR2;
      break;

    /* Second digit of hour */
    case DISPLAY_HOUR2:
      segments_write(infos.hour % 10);
      digitalWrite(COM_HR2, HIGH);
      delay(1);
      segments_write(10);
      digitalWrite(COM_HR2, LOW);
      display_state = DISPLAY_MIN1;
      break;

    /* First digit of Minute */
    case DISPLAY_MIN1:
      segments_write(infos.minute / 10);
      digitalWrite(COM_MIN1, HIGH);
      delay(1);
      segments_write(10);
      digitalWrite(COM_MIN1, LOW);
      display_state = DISPLAY_MIN2;
      break;

    /* Second digit of Minute */
    case DISPLAY_MIN2:
      segments_write(infos.minute % 10);
      digitalWrite(COM_MIN2, HIGH);
      delay(1);
      segments_write(10);
      digitalWrite(COM_MIN2, LOW);
      display_state = DISPLAY_TEMP1;
      break;

    /* First digit of Temperature */
    case DISPLAY_TEMP1:
      segments_write(infos.temp / 10);
      digitalWrite(COM_T1, HIGH);
      delay(1);
      segments_write(10);
      digitalWrite(COM_T1, LOW);
      display_state = DISPLAY_TEMP2;
      break;

    /* Second digit of Temperature */
    case DISPLAY_TEMP2:
      segments_write(infos.temp % 10);
      digitalWrite(COM_T2, HIGH);
      delay(1);
      segments_write(10);
      digitalWrite(COM_T2, LOW);
      display_state = DISPLAY_TEMP2;
      display_state = DISPLAY_WEEKD;
      break;

    /* Weekday LED */
    case DISPLAY_WEEKD:
      digitalWrite(week_d[infos.day_of_week],LOW);
      digitalWrite(LED_DAY, HIGH);
      delay(1);
      digitalWrite(week_d[infos.day_of_week],HIGH);
      digitalWrite(LED_DAY, LOW);  
      display_state = DISPLAY_LEDS;
      break;

    /* Other LEDs */
    case DISPLAY_LEDS:
      digitalWrite(C_SEG, LOW);
      digitalWrite(D_SEG, LOW);
      digitalWrite(LED_C, HIGH);
      delay(1);
      digitalWrite(C_SEG, HIGH);
      digitalWrite(D_SEG, HIGH);
      digitalWrite(LED_C, LOW);  

      display_state = DISPLAY_DAY1;
      break;

    default:
      display_state = DISPLAY_DAY1;
      break;
  }
}