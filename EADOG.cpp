/* mbed library for the mbed Lab Board  128*32 pixel LCD
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

#include "EADOG.h"
#include "mbed.h"
#include "stdio.h"
#include "Small_7.h"

EADOG::EADOG(PinName mosi, PinName sck, PinName reset, PinName a0, PinName cs, uint8_t type) : _spi(mosi, NC, sck), _reset(reset), _a0(a0), _cs(cs), _type(type), graphic_buffer() {
  topview = false;

  if (_type == DOGM132) {
    width = 132;
    height = 32;
    graphic_buffer_size = 528;
    graphic_buffer = new uint8_t [graphic_buffer_size];
    }

  if (_type == DOGM128 || _type == DOGL128) {
    width = 128;
    height = 64;
    graphic_buffer_size = 1024;
    graphic_buffer = new uint8_t [graphic_buffer_size];
    }
    init();
  }

static void inline swap(int &a, int &b) {
  int c = a;
  a = b;
  b = c;
  }

void EADOG::display(uint8_t display) {
  if (display == ON) { // display on
    write_command(0xA4);
    write_command(0xAF);
    }
  if (display == OFF) { // display off
    write_command(0xAE);
    }

  if (display == SLEEP) {// display sleep
    write_command(0xA5);
    write_command(0xAE);
    }
  if(display == INVERT) { // invert display
    write_command(0xA7);
    }
  if(display == DEFAULT) { // set to normal display
    write_command(0xA6);
    }
  if (display == TOPVIEW) { // reverse orientation
    topview = true;
    write_command(0xA0); // ADC normal
    write_command(0xC8); // reversed com31-com0
    update(); // update necessary
      }
  if (display == BOTTOM) { // normal orientation
    topview = false;
    write_command(0xA1); // ADC reverse
    write_command(0xC0); // normal com0-com31
    update(); // update necessary
    }
  if (display == CONTRAST) {
    if (_type == DOGM132) {
      write_command(0x81); //  set contrast to default for dogm 128
      write_command(0x1F);
      }
    if (_type == DOGM128) {
      write_command(0x81); //  set contrast to default for dogm132
      write_command(0x16);
      }
    if (_type == DOGL128) {
      write_command(0x81); //  set contrast to default for dogl132
      write_command(0x10);
      }
    }
  }

void EADOG::display(uint8_t display, uint8_t value) {
  if (display == CONTRAST) {
    if (value < 64) {
      write_command(0x81); //  set contrast
      write_command(value & 0x3F);
      }
    }
  }

// write command to lcd controller
void EADOG::write_command(uint8_t command) {
  _a0 = 0;
  _cs = 0;
  _spi.write(command);
  _cs = 1;
  }

// write data to lcd controller
void EADOG::write_data(uint8_t data) {
  _a0 = 1;
  _cs = 0;
  _spi.write(data);
  _cs = 1;
  }

// reset and init the lcd controller
void EADOG::init() {
  _spi.format(8, 3);                 // 8 bit spi mode 3
  _spi.frequency(20000000);          // 19,2 Mhz SPI clock

  //DigitalOut _reset(A1);
  _a0 = 0;
  _cs = 1;
  _reset = 0;                        // display reset
  wait_us(50);
  _reset = 1;                       // end reset
  wait_ms(5);

  // Start Initial Sequence
  write_command(0x40); // display start line 0
  write_command(0xA1); // ADC reverse
  write_command(0xC0); // normal com0-com31
  write_command(0xA6); // display normal
  write_command(0xA2); // set bias 1/9 (duty 1/33)
  write_command(0x2F); // booster, regulator and follower on
  write_command(0xF8); // set internal booster to 3x/4x
  write_command(0x00);
  if (_type == DOGM132) {
    write_command(0x23); //  set contrast
    write_command(0x81);
    write_command(0x1F);
    }
  if (_type == DOGM128) {
    write_command(0x27); //  set contrast
    write_command(0x81);
    write_command(0x16);
    }
  if (_type == DOGL128) {
    write_command(0x27); //  set contrast
    write_command(0x81);
    write_command(0x10);
    }
  write_command(0xAC); // no indicator
  write_command(0x00);
  write_command(0xAF); // display on

  // clear and update LCD
  cls();
  auto_update = 1;              // switch on auto update
  locate(0, 0);
  font((unsigned char*)Small_7);  // standard font
  }

// update lcd
void EADOG::update() {
  for (int i = 0; i < 4; i++) {
    write_page(i);
    }

  if (_type == DOGM128 || _type == DOGL128) {
    for (int i = 4; i < 8; i++) {
      write_page(i);
      }
    }

  _cs = 0;
  }

void EADOG::write_page(int page) {
    write_command(topview ? 0x04 : 0x00);      // set column low nibble 0
    write_command(0x10);      // set column hi  nibble 0
    write_command(0xB0 + page);      // set page address  7
    _a0 = 1;

    for (int i = width * page; i < width * (page+1); i++) {
      write_data(graphic_buffer[i]);
      }
  }

void EADOG::update(uint8_t mode) {
  if (mode == MANUAL) auto_update = 0;
  if (mode == AUTO) auto_update = 1;
  }

// clear screen
void EADOG::cls() {
  memset(graphic_buffer, 0x00, graphic_buffer_size);  // clear display graphic_buffer
  update();
  }

// set one pixel in graphic_buffer
void EADOG::pixel(int x, int y, uint8_t color) {
  if (x > width - 1 || y > height - 1 || x < 0 || y < 0) return;
  if (color == 0) graphic_buffer[x + ((y / 8) * width)] &= ~(1 << (y % 8)); // erase pixel
  else graphic_buffer[x + ((y / 8) * width)] |= (1 << (y % 8));   // set pixel
  }

void EADOG::point(int x, int y, uint8_t colour) {
  pixel(x, y, colour);
  if (auto_update) update();
  }

// This function uses Bresenham's algorithm to draw a straight line.
void EADOG::line(int x0, int y0, int x1, int y1, uint8_t colour) {
  int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2; /* error value e_xy */

  while(1) {
    pixel(x0, y0, 1);
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 > dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
    if (e2 < dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
    }
  if (auto_update) update();
  }

void EADOG::rectangle(int x0, int y0, int x1, int y1, uint8_t colour) {
  uint8_t upd = auto_update;
  auto_update = 0;
  line(x0, y0, x1, y0, colour);
  line(x0, y1, x1, y1, colour);
  line(x0, y0, x0, y1, colour);
  line(x1, y0, x1, y1, colour);
  auto_update = upd;
  if (auto_update) update();
  }

void EADOG::fillrect(int x0, int y0, int x1, int y1, uint8_t colour) {
  if (x0 > x1) swap(x0, x1);
  if (y0 > y1) swap(y0, y1);
  for (int i = x0; i <= x1; i++) {
    for (int j = y0; j <= y1; j++) {
      pixel(i, j, colour);
      }
    }
  if (auto_update) update();
  }

void EADOG::roundrect(int x0, int y0, int x1, int y1, int rnd, uint8_t colour) {
  if (x0 > x1) swap(x0, x1);
  if (y0 > y1) swap(y0, y1);
  uint8_t upd = auto_update;
  auto_update = 0;
  int r = rnd;
  int x = -r, y = 0, err = 2 - 2 * r;
  line(x0 + rnd, y0, x1 - rnd, y0, colour);
  line(x0 + rnd, y1, x1 - rnd, y1, colour);
  line(x0, y0 + rnd, x0, y1 - rnd, colour);
  line(x1, y0 + rnd, x1, y1 - rnd, colour);
  do {
    pixel(x1 - rnd + y, y0 + x + rnd, 1); // 1 I. quadrant
    pixel(x1 - rnd - x, y1 + y - rnd, 1); // 2 IV. quadrant
    pixel(x0 + rnd - y, y1 - rnd - x, 1); // 3 III. quadrant
    pixel(x0 + rnd + x, y0 + rnd - y, 1); // 4 II. quadrant
    r = err;
    if (r <= y) err += ++y * 2 + 1;
    if (r > x || err > y) err += ++x * 2 + 1;
    } while (x < 0);
  auto_update = upd;
  if (auto_update) update();
  }

void EADOG::fillrrect(int x0, int y0, int x1, int y1, int rnd, uint8_t colour) {
  if (x0 > x1) swap(x0, x1);
  if (y0 > y1) swap(y0, y1);
  uint8_t upd = auto_update;
  auto_update = 0;
  int r = rnd;
  int x = -r, y = 0, err = 2 - 2 * r;
  for (int i = x0; i <= x1; i++) {
    for (int j = y0+rnd; j <= y1-rnd; j++) {
      pixel(i, j, colour);
      }
    }
  do {
    line(x0 + rnd - y, y0 + rnd + x, x1 - rnd + y, y0 + rnd + x, 1);
    line(x0 + rnd + x, y1 - rnd + y, x1 - rnd - x, y1 - rnd + y, 1);
    r = err;
    if (r <= y) err += ++y * 2 + 1;
    if (r > x || err > y) err += ++x * 2 + 1;
    } while (x < 0);
  auto_update = upd;
  if (auto_update) update();
  }

void EADOG::circle(int x0, int y0, int r, uint8_t colour) {
  int x = -r, y = 0, err = 2 - 2 * r;
  do {
    pixel(x0 + y, y0 + x, 1); // 1 I. quadrant
    pixel(x0 - x, y0 + y, 1); // 2 IV. quadrant
    pixel(x0 - y, y0 - x, 1); // 3 III. quadrant
    pixel(x0 + x, y0 - y, 1); // 4 II. quadrant
    r = err;
    if (r <= y) err += ++y * 2 + 1;
    if (r > x || err > y) err += ++x * 2 + 1;
    } while (x < 0);
  if (auto_update) update();
  }

void EADOG::fillcircle(int x0, int y0, int r, uint8_t colour) {
  uint8_t upd;
  upd = auto_update;
  auto_update = 0;
  int x = -r, y = 0, err = 2 - 2 * r;
  do {
    line(x0 - y, y0 + x, x0 + y, y0 + x, 1);
    line(x0 + x, y0 + y, x0 - x, y0 + y, 1);
    r = err;
    if (r <= y) err += ++y * 2 + 1;
    if (r > x || err > y) err += ++x * 2 + 1;
    } while (x < 0);
  auto_update = upd;
  if (auto_update) update();
  }

void EADOG::locate(uint8_t x, uint8_t y) {
  char_x = x;
  char_y = y;
  }

int EADOG::_putc(int value) {
  if (value == '\n') {    // new line
    char_x = 0;
    char_y = char_y + font_buffer[2];
    if (char_y >= height - font_buffer[2]) {
      char_y = 0;
      }
    }
  else {
    character(char_x, char_y, value);
    if (auto_update) update();
    }
  return value;
  }

int EADOG::_getc() {
      return -1;
  }

void EADOG::character(uint8_t x, uint8_t y, uint8_t c) {
  unsigned int hor, vert, offset, bpl, b;
  uint8_t *sign;
  uint8_t z, w;

  if ((c < 31) || (c > 127)) return;   // test char range

  // read font parameter from start of array
  offset = font_buffer[0];                    // bytes / char
  hor = font_buffer[1];                       // get hor size of font
  vert = font_buffer[2];                      // get vert size of font
  bpl = font_buffer[3];                       // bytes per line

  if (char_x + hor > width) {
    char_x = 0;
    char_y = char_y + vert;
    if (char_y >= height - font_buffer[2]) {
      char_y = 0;
      }
    }

  sign = &font_buffer[((c - 32) * offset) + 4]; // start of char bitmap
  w = sign[0];                          // width of actual char
  // construct the char into the font_graphic_buffer
  for (unsigned int j = 0; j < vert; j++) {  //  vert line
    for (unsigned int i = 0; i < hor; i++) {   //  horz line
      z =  sign[bpl * i + ((j & 0xF8) >> 3) + 1];
      b = 1 << (j & 0x07);
      if (( z & b ) == 0x00) {
        pixel(x+i, y+j, 0);
        }
      else {
        pixel(x+i, y+j, 1);
        }
      }
    }
  char_x += w;
  }


void EADOG::font(uint8_t *f) {
  font_buffer = f;
  }

void EADOG::bitmap(Bitmap bm, int x, int y) {
  int b;
  char d;

  for (int v = 0; v < bm.ySize; v++) {   // lines
    for (int h = 0; h < bm.xSize; h++) { // pixel
      if (h + x >= width) break;
      if (v + y >= height) break;
      d = bm.data[bm.byte_in_Line * v + ((h & 0xF8) >> 3)];
      b = 0x80 >> (h & 0x07);
      if ((d & b) == 0) {
        pixel(x+h, y+v, 0);
        }
      else {
        pixel(x+h, y+v, 1);
        }
      }
    }
    if (auto_update) update();
  }
