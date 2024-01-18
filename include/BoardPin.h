#ifndef _BoardPin_
#define _BoardPin_

#define I2C_CL      22  // GPIO 22
#define I2C_DA      21  // GPIO 21

#define RECV_PIN    26  // GPIO 26      // Infrared receiving pin

#ifndef LED_BUILTIN
#define LED_BUILTIN 27  // GPIO 27
#endif

#define LED_RED     25  // GPIO 25
#define LED_GREEN   33  // GPIO 33
#define LED_BLUE    32  // GPIO 32

#define VSPI_CS      5  // GPIO 5
#define VSPI_CLK    18  // GPIO 18
#define VSPI_MISO   19  // GPIO 19
#define VSPI_MOSI   23  // GPIO 23

#define KEY_0       16  // GPIO 16 R
#define KEY_1       17  // GPIO 17 B
#define KEY_2        4  // GPIO  4 G
#define INPUT_PIN   36  // GPIO 36 Spare input pin

#define WS2812_PIN  19  // GPIO 19
#define POT0 A6         // ADC1-6 = GPIO 34
#define POT1 A7         // ADC1-7 = GPIO 35

#endif // _BoardPin_


// On I2C bus is 0.96/1.30" OLED display 
// BMP180, Air pressure and tempertur sensor
// GY521, MPU-6050 3-axis gyroscope and acceleration_

