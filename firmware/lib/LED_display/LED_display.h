#ifndef __LED_display_H__
#define __LED_display_H__

/* LEDs and displays marks */
#define DISPLAY_DAY1    0
#define DISPLAY_DAY2    1
#define DISPLAY_MONTH1  2
#define DISPLAY_MONTH2  3
#define DISPLAY_YEAR1_2 4
#define DISPLAY_YEAR3   5
#define DISPLAY_YEAR4   6
#define DISPLAY_HOUR1   7
#define DISPLAY_HOUR2   8
#define DISPLAY_MIN1    9
#define DISPLAY_MIN2    10
#define DISPLAY_TEMP1   11
#define DISPLAY_TEMP2   12
#define DISPLAY_WEEKD   13
#define DISPLAY_LEDS    14

#define TEMP_MEAS       15

/* Ports defines */
#define TEMP_PIN  D26 /* Temperature pin */

/* Inputs */
#define A_SEG     D8
#define B_SEG     D9
#define C_SEG     D22
#define D_SEG     D27
#define E_SEG     D28
#define F_SEG     D7
#define G_SEG     D6

/* Outputs */
#define COM_D1    D15
#define COM_D2    D16
#define COM_M2    D17
#define COM_Y3    D18
#define COM_Y4    D19
#define COM_HR1   D14
#define COM_HR2   D13
#define COM_MIN1  D12
#define COM_MIN2  D11
#define COM_T2    D21
#define COM_T1    D5
#define LED_DAY   D20
#define LED_C     D10

/* Temperature meas constants */
#define REF_RESISTANCE  10000.0f  /* Reference resistor value in ohms (10k) */
#define NOM_RESISTANCE  10000.0f  /* NTC thermistor nominal resistance at 25°C (in ohms) */
#define NOM_TEMPERATURE 25.0f     /* NTC thermistor nominal temperature (in °C) */
#define B_COEFFICIENT   3950.0f   /* B coefficient of the thermistor */
#define ADC_MAX         4095.0f   /* Maximum ADC value (12-bit ADC) */
#define VCC             3.3f      /* Reference voltage */
#define TEMP_OFFSET     -5.0f     /* Temperature offset for calibration (-3 degrees to correct the reading) */

/* Struct to hold all the information to display */
struct info_struct {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second; 
  int day_of_week; /* 0 = Sunday, 1 = Monday, ..., 6 = Saturday */
  int temp; /* temperature */
};

int read_temperature();
void display_init(void);
void display_ctrl(info_struct infos);

#endif /* __LED_display_H__ */