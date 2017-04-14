# mbed library for the EA DOG LCDs from Electronic Assembly

This library is for the LCDs from Electronic Assembly
http://www.lcd-module.de/produkte/dog.html <br>
The library support following types:<br>
EA DOGL128<br>
EA DOGM128<br>
EA DOGM132<br>
It uses better algorithms (Bresenham) for the graphics than the other libraries.<br>

## Installation

1. "Download":https://github.com/sstaub/EADOG/archive/master.zip the Master branch from GitHub.
2. Unzip and modify the folder name to "EADOG"
3. Move the modified folder on your Library folder (On your `Libraries` folder inside Sketchbooks or Arduino software).

## Example

Here is an example for an EA DOGM132 display with a STM32 Nucleo STM32F401RE board.

```
// display test for EA DOGM132x-5 with STM32F401RE Nucleo
// sstaub(c)2017

#include "mbed.h"
#include "EADOG.h"
#include "Small_7.h"
#include "Fonts/ArialR12x14.h"

/* DOGM132x-5 Wide Range
Pins for STM32F401RE Nucleo

DOG Pin   Purpose   ST Pin
40        CS        PB_6(D10)
39        Reset     PC_7(D9)
38        A0        PA_9(D8)
37        SCK       PA_5(D13, SCK)
36        MOSI      PA_7(D11, MOSI)
35        VDD       3.3V
34        VDD2      3.3V
33        VSS       GND
32        VOUT      1uF -> GND
31        CAP3P     1uF -> CAP1N
30        CAP1N     1uF -> CAP1P
29        CAP1P
28        CAP2P     1uF -> CAP2N
27        CAP2N
26        VSS       GND
25        V4        1uF -> GND
24        V3        1uF -> GND
23        V2        1uF -> GND
22        V1        1uF -> GND
21        V0        1uF -> GND

*/

Serial debug(USBTX, USBRX);
Timer t;

EADOG lcd(PA_7, PA_5, PC_7, PA_9, PB_6, DOGM132); // MOSI, SCK, Reset, A0, CS

int main() {
  t.start();
  debug.printf("create logo\n");

  lcd.update(MANUAL);
  lcd.rectangle(0, 0, 131, 31, 1);
  lcd.fillrect(2, 2, 4, 4, 1);
  lcd.fillrect(2, 27, 4, 29, 1);
  lcd.fillrect(61, 2, 63, 4, 1);
  lcd.fillrect(61, 27, 63, 29, 1);
  lcd.line(65, 0, 65, 31, 1);
  lcd.circle(33, 15, 10, 1);
  lcd.circle(33, 15, 6, 1);
  lcd.fillcircle(33, 15, 3, 1);
  lcd.update();
  lcd.update(AUTO);

  lcd.locate(70, 1);
  lcd.font((unsigned char*)ArialR12x14);
  lcd.printf("mbed");
  lcd.locate(70, 13);
  lcd.font((unsigned char*)Small_7);
  lcd.printf("EA");
  lcd.locate(70, 22);
  lcd.printf("DOGM132x-5");


  wait(1);
  lcd.display(CONTRAST, 16);
  debug.printf("contrast low\n");
  wait(1);
  lcd.display(CONTRAST, 40);
  debug.printf("contrast high\n");
  wait(1);
  lcd.display(CONTRAST);
  debug.printf("contrast default\n");
  wait(1);
  lcd.display(OFF);
  debug.printf("display off\n");
  wait(1);
  lcd.display(ON);
  debug.printf("display on\n");
  wait(1);
  lcd.display(SLEEP);
  debug.printf("display sleep\n");
  wait(1);
  lcd.display(ON);
  debug.printf("display wake up\n");


  while(1) {

    lcd.display(INVERT);
    lcd.display(BOTTOM);
    debug.printf("display normal\n");
    debug.printf("at time %f s\n", t.read());
    wait(2);

    lcd.display(DEFAULT);
    lcd.display(TOPVIEW);
    debug.printf("display rotate, invers\n");
    debug.printf("at time %f s\n", t.read());
    wait(2);
    }
  }

```

## Documentation

### Constructor

**EADOG(PinName mosi, PinName sck, PinName reset, PinName a0, PinName cs, uint8_t type)**<br>
create a EADOG object connected to SPI <br>
	
### Functions

**void bitmap(Bitmap bm, int x, int y)**<br>
print bitmap to buffer <br>
>**Parameters**<br>
>bm -> Bitmap in flash<br> 
>x -> x start<br> 
>y -> y start<br>

**void character(uint8_t x, uint8_t y, uint8_t  c)**<br>
draw a character on given position out of the active font to the LCD <br>

**void circle(int x, int y, int r, uint8_t  colour)**<br>
draw a circle<br> 
>**Parameters**<br>
>x0,y0 -> center<br> 
>r -> radius<br>
>colour -> 1 set pixel, 0 erase pixel <br>

**void cls()**<br>
clears the screen

**void display(uint8_t display)**<br>
display functions<br>
>**Parameter display**<br>
>ON switch display on, or wake up from sleep<br>
>OFF set display off<br>
>SLEEP set display off and to sleep mode<br>
>BOTTOM (default) set display orientation 0°<br>
>TOPVIEW set display oriention to 180°<br> 
>INVERT invert the pixels<br>
>DEFAULT normal pixel display<br>
>CONTRAST set display contrast to default<br>

**void fillcircle(int x, int y, int r, uint8_t colour)**<br>
draw a filled circle<br>
>**Parameters**<br>
>x0,y0 -> center<br> 
>r -> radius<br> 
>color -> 1 set pixel, 0 erase pixel<br>

**void fillrect(int x0, int y0, int x1, int y1, uint8_t colour)**<br>
draw a filled rect<br>
>**Parameters**<br>
>x0,y0 -> top left corner<br>
>x1,y1 -> down right corner<br>
>color -> 1 set pixel, 0 erase pixel<br>

**void fillrrect(int x0, int y0, int x1, int y1, int rnd, uint8_t colour)**<br>
draw a filled rounded rect<br>
>x0,y0 -> top left corner<br>
>x1,y1 -> down right corner<br> 
>rnd -> radius of the rounding<br>
>color -> 1 set pixel, 0 erase pixel<br>

**void font(uint8_t * f)**<br>
select the font to use<br>
>**Parameter**<br>
>f -> pointer to font array<br>
font array can created with GLCD Font Creator from http://www.mikroe.com you have to add 4 parameter at the beginning of the font array to use:
the number of byte / char
the vertial size in pixel
the horizontal size in pixel
the number of byte per vertical line you also have to change the array to char[]

```
lcd.font((unsigned char*)Small_7);
```

**void line(int x0, int y0, int x1, int y1, uint8_t colour)**<br>
draw a 1 pixel line<br>
>**Parameters**<br>
>x0,y0 -> start point<br>
>x1,y1 -> end point<br>
>color -> 1 set pixel, 0 erase pixel<br>

**void locate(uint8_t x, uint8_t y)**<br>
set top left position of char/printf<br>
>**Parameters**<br>
>x -> x-position<br> 
>y -> y-position<br>

**void pixel(int x, int y, uint8_t colour)**<br>
draw a pixel in buffer at x, y black or white<br>
>**Parameters**<br>
>x -> horizontal position<br>
>y -> vertical position<br>
>colour -> 1 set pixel, 0 erase pixel there is no update, it writes the pixel only in the buffer<br>

**void point(int x, int y, uint8_t colour)**<br>
draw a single point<br>
>**Parameters**<br>
>x -> horizontal position<br>
>y -> vertical position<br>
>colour -> 1 set pixel, 0 erase pixel<br>

**void rectangle(int x0, int y0, int x1, int y1, uint8_t colour)**<br>
draw a rect<br>
>**Parameters**<br>
>x0,y0 -> top left corner<br>
>x1,y1 -> down right corner<br>
>color -> 1 set pixel, 0 erase pixel<br>

**void roundrect(int x0, int y0, int x1, int y1, int rnd, uint8_t colour)**<br>
draw a rounded rect<br>
>**Parameters**<br>
>x0,y0 -> top left corner<br>
>x1,y1 -> down right corner<br>
>rnd -> radius of the rounding<br>
>color -> 1 set pixel, 0 erase pixel<br>

**void update()**<br>
**void update(uint8_t mode)**<br>
update to copy display buffer to lcd<br>
>**Parameter mode**<br>
>AUTO set update mode to auto, default<br>
>MANUELL the update function must manually set