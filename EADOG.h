/* mbed library for the EA DOGM128 (128x64), DOGM132 (132x32), DOGL128 (128x64) pixel LCD
 * use ST7565R controller
 * Copyright (c) 2016 Stefan Staub
 * Released under the MIT License: http://mbed.org/license/mit
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef EADOG_H
#define EADOG_H

#include "mbed.h"

/** display type
  *
  * @param DOGM128 128x64 2.3''
  * @param DOGM132 132x32 2.1''
  * @param DOGL128 128x64 2.8''
  *
  */
enum {DOGM128, DOGM132, DOGL128};

/** update modes
  *
  * @param AUTO default
  * @param MANUELL
  *
  */
enum {MANUAL, AUTO};

/** display settings
  *
  * @param ON display on, default
  * @param OFF display off
  * @param SLEEP sets the display to sleep mode
  * @param DEFAULT default
  * @param INVERT inverts display
  * @param BOTTOM default 0°
  * @param TOPVIEW rotate the screento 180°
  *
  */
enum {ON, OFF, SLEEP, DEFAULT, INVERT, BOTTOM, TOPVIEW, CONTRAST};

/** bitmap
  *
  */
struct Bitmap {
  int xSize;
  int ySize;
  int byte_in_Line;
  char *data;
  };

/** Library for the EADOG LCD displays from electronic assembly
 *
 * Example for EA DOGM132x-5 with STM32F401RE Nucleo
 * @code
 * // display test for EA DOGM132x-5 with STM32F401RE Nucleo
 * // sstaub(c)2016
 *
 * #include "mbed.h"
 * #include "EADOG.h"
 * #include "Small_7.h"
 * #include "Fonts/ArialR12x14.h"
 *
 * // DOGM132x-5 Wide Range
 * // Pins for STM32F401RE Nucleo
 *
 * // DOG Pin   Purpose   ST Pin
 * // 40        CS        PB_6(D10)
 * // 39        Reset     PC_7(D9)
 * // 38        A0        PA_9(D8)
 * // 37        SCK       PA_5(D13, SCK)
 * // 36        MOSI      PA_7(D11, MOSI)
 * // 35        VDD       3.3V
 * // 34        VDD2      3.3V
 * // 33        VSS       GND
 * // 32        VOUT      1uF -> GND
 * // 31        CAP3P     1uF -> CAP1N
 * // 30        CAP1N     1uF -> CAP1P
 * // 29        CAP1P
 * // 28        CAP2P     1uF -> CAP2N
 * // 27        CAP2N
 * // 26        VSS       GND
 * // 25        V4        1uF -> GND
 * // 24        V3        1uF -> GND
 * // 23        V2        1uF -> GND
 * // 22        V1        1uF -> GND
 * // 21        V0        1uF -> GND
 * 
 *
 * EADOG lcd(PA_7, PA_5, PC_7, PA_9, PB_6, DOGM132); // MOSI, SCK, Reset, A0, CS
 * 
 * int main() {
 *   // create logo
 *   lcd.update(MANUAL);
 *   lcd.rectangle(0, 0, 131, 31, 1);
 *   lcd.fillrect(2, 2, 4, 4, 1);
 *   lcd.fillrect(2, 27, 4, 29, 1);
 *   lcd.fillrect(61, 2, 63, 4, 1);
 *   lcd.fillrect(61, 27, 63, 29, 1);
 *   lcd.line(65, 0, 65, 31, 1);
 *   lcd.circle(33, 15, 10, 1);
 *   lcd.circle(33, 15, 6, 1);
 *   lcd.fillcircle(33, 15, 3, 1);
 *   lcd.update();
 *   lcd.update(AUTO);
 *   // create description text
 *   lcd.locate(70, 1);
 *   lcd.font((unsigned char*)ArialR12x14);
 *   lcd.printf("mbed");
 *   lcd.locate(70, 13);
 *   lcd.font((unsigned char*)Small_7);
 *   lcd.printf("EA");
 *   lcd.locate(70, 22);
 *   lcd.printf("DOGM132x-5");
 *
 *   wait(1);
 *   lcd.display(CONTRAST, 16); // contrast low
 *   wait(1);
 *   lcd.display(CONTRAST, 40); // contrast high
 *   wait(1);
 *   lcd.display(CONTRAST);     // contrast default
 *   wait(1);
 *   lcd.display(OFF);          // display off
 *   wait(1);
 *   lcd.display(ON);           // display on
 *   wait(1);
 *   lcd.display(SLEEP);        // display sleep
 *   wait(1);
 *   lcd.display(ON);           // display wake up
 *
 *   while(1) {                 // main loop
 *     lcd.display(INVERT);     // display inverted
 *     lcd.display(BOTTOM);     // display normal orientation
 *     wait(2);
 *     lcd.display(DEFAULT);    // display 
 *     lcd.display(TOPVIEW);    // display rotated
 *     wait(2);    
 *     }
 *   }
 * @endcode
 */
class EADOG : public Stream  {

public:

  /** create a EADOG object connected to SPI
    *
    * @param mosi pinname
    * @param sck pinname
    * @param reset pinname
    * @param a0 pinname
    * @param cs pinname
    * @param type DOGM128, DOGM132, DOGL128
    *
    */
  EADOG(PinName mosi, PinName sck, PinName reset, PinName a0, PinName cs, uint8_t type);

  /** draw a pixel in buffer at x, y black or white
    *
    * @param x horizontal position
    * @param y vertical position
    * @param colour 1 set pixel, 0 erase pixel
    * there is no update, it writes the pixel only in the buffer
    */
  void pixel(int x, int y, uint8_t colour);

  /** draw a single point
    *
    * @param x horizontal position
    * @param y vertical position
    * @param colour 1 set pixel, 0 erase pixel
    *
    */
  void point(int x, int y, uint8_t colour);

  /** draw a 1 pixel line
    *
    * @param x0,y0 start point
    * @param x1,y1 end point
    * @param color 1 set pixel, 0 erase pixel
    *
    */
  void line(int x0, int y0, int x1, int y1, uint8_t colour);

  /** draw a rect
    *
    * @param x0,y0 top left corner
    * @param x1,y1 down right corner
    * @param color 1 set pixel, 0 erase pixel
    *
    */
  void rectangle(int x0, int y0, int x1, int y1, uint8_t colour);

  /** draw a filled rect
    *
    * @param x0,y0 top left corner
    * @param x1,y1 down right corner
    * @param color 1 set pixel, 0 erase pixel
    *
    */
  void fillrect(int x0, int y0, int x1, int y1, uint8_t colour);

  /** draw a rounded rect
    *
    * @param x0,y0 top left corner
    * @param x1,y1 down right corner
    * @param rnd radius of the rounding
    * @param color 1 set pixel, 0 erase pixel
    *
    */
  void roundrect(int x0, int y0, int x1, int y1, int rnd, uint8_t colour);

  /** draw a filled rounded rect
    *
    * @param x0,y0 top left corner
    * @param x1,y1 down right corner
    * @param rnd radius of the rounding
    * @param color 1 set pixel, 0 erase pixel
    *
    */
  void fillrrect(int x0, int y0, int x1, int y1, int rnd, uint8_t colour);


  /** draw a circle
    *
    * @param x0,y0 center
    * @param r radius
    * @param colour 1 set pixel, 0 erase pixel
    *
    */
  void circle(int x, int y, int r, uint8_t colour);

  /** draw a filled circle
    *
    * @param x0,y0 center
    * @param r radius
    * @param color 1 set pixel, 0 erase pixel
    *
    * use circle with different radius,
    * can miss some pixel
    *
    */
  void fillcircle(int x, int y, int r, uint8_t colour);

  /** update copy display buffer to lcd
    *
    * @param AUTO set update mode to auto, default
    * @param MANUELL the update function must manually set
    * update() copy display buffer to lcd
    */
  void update();
  void update(uint8_t mode);

  /** display functions
    *
    * @param display ON switch display on, or wake up from sleep
    * @param display OFF set display off
    * @param display SLEEP set display off and to sleep mode
    * @param display BOTTOM (default) set display orientation 0°
    * @param display TOPVIEW draw display oriention to 180°
    * @param display INVERT invert the pixels
    * @param display DEFAULT normal pixel display
    * @param display CONTRAST set display contrast to default
    *
    */
  void display(uint8_t display);

  /** display functions
    *
    * @param display CONTRAST set display contrast with value,
    * @param value sets display contrast 0 - 63, default 31
    *
    */
  void display(uint8_t display, uint8_t value);

  /** clear the screen
    *
    */
  void cls();

  /** draw a character on given position out of the active font to the LCD
    *
    * @param x x-position of char (top left)
    * @param y y-position
    * @param c char to print
    *
    */
  void character(uint8_t x, uint8_t y, uint8_t c);

  /** set top left position of char/printf
    *
    * @param x x-position
    * @param y y-position
    *
    */
  void locate(uint8_t x, uint8_t y);

  /** select the font to use
    *
    * @param f pointer to font array
    *
    *   font array can created with GLCD Font Creator from http://www.mikroe.com
    *   you have to add 4 parameter at the beginning of the font array to use:
    *   - the number of byte / char
    *   - the vertial size in pixel
    *   - the horizontal size in pixel
    *   - the number of byte per vertical line
    *   you also have to change the array to char[]
    * @code
    * lcd.font((unsigned char*)Small_7);
    * @endcode
    */
  void font(uint8_t *f);

  /** calculate pixel width of a string with current font
    *
    * @param text string
    */
  unsigned int textwidth(const char* text);

  /** print bitmap to buffer
    *
    * @param bm Bitmap in flash
    * @param x  x start
    * @param y  y start
    *
    */
  void bitmap(Bitmap bm, int x, int y);

  // declarations
  SPI _spi;
  DigitalOut _reset;
  DigitalOut _a0;
  DigitalOut _cs;

protected:

  /** stream class, put a char on the screen
    *
    * @param value char to print
    * @returns printed char
    *
    */
  virtual int _putc(int value);

  /** stream class, dummy
    *
    */
  virtual int _getc();

  /** init the DOGM132 LCD controller
    *
    */
  void init(); //Init the ST7565R LCD controller

  /** write data to the LCD controller
    *
    * @param dat data written to LCD controller
    *
    */
  void write_data(uint8_t data); // Write data to the LCD controller

  /** Write a command the LCD controller
    *
    * @param cmd command to be written
    *
    */
  void write_command(uint8_t command); // Write a command the LCD controller

  /** Write a page of data to the LCD controller
   *
   * @param page page number from 0-7
   *
   */
  void write_page(int page);

  // Variables
  uint8_t *font_buffer;
  uint8_t char_x;
  uint8_t char_y;
  uint8_t auto_update;
  uint8_t width;
  uint8_t height;
  uint8_t _type;
  uint8_t *graphic_buffer;
  uint32_t graphic_buffer_size;
  bool topview;

  };

#endif
