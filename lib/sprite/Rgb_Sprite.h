#include "Arduino_DataBus.h"
#include "display/Arduino_RGB_Display.h"
#include "display/Arduino_DSI_Display.h"
#include "display/Arduino_CO5300.h"
#include "screen_type.h"
#if !defined(LITTLE_FOOT_PRINT)

#ifndef _RGB_SPRITE_H_
#define _RGB_SPRITE_H_

#define FP_SCALE 10

#define TFT_TRANSPARENT 0x0120 // This is actually a dark green

#include "Arduino_GFX.h"

class Rgb_Sprite : public Arduino_GFX{
public:

#if defined(SCREEN_RGB)
  Rgb_Sprite(Arduino_RGB_Display *output);
#elif defined(SCREEN_QSPI)
  Rgb_Sprite(Arduino_CO5300 *output);
#else 
  Rgb_Sprite(Arduino_DSI_Display *output);
#endif

  ~Rgb_Sprite();

  bool begin(int32_t speed = GFX_NOT_DEFINED) override;
  void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) override;
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
  void writeFastVLineCore(int16_t x, int16_t y, int16_t h, uint16_t color);
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
  void writeFastHLineCore(int16_t x, int16_t y, int16_t w, uint16_t color);
  void writeFillRectPreclipped(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;
  void drawIndexedBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint16_t *color_index, int16_t w, int16_t h, int16_t x_skip = 0) override;
  void drawIndexedBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint16_t *color_index, uint8_t chroma_key, int16_t w, int16_t h, int16_t x_skip = 0) override;
  void draw16bitRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) override;
  void draw16bitRGBBitmapWithTranColor(int16_t x, int16_t y, uint16_t *bitmap, uint16_t transparent_color, int16_t w, int16_t h) override;
  void draw16bitBeRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) override;
  void pushSprite(int16_t x, int16_t y);
  void flush(void) override;

  uint16_t *getFramebuffer();

  void drawWideLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t wide, uint16_t color);//只是画横线
  void drawString(const String& string, int32_t x, int32_t y, uint8_t font);

  bool createSprite(int16_t w, int16_t h);
  void setScrollRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color = BLACK);
  void scroll(int16_t dx, int16_t dy = 0);
  bool pushToSprite(Rgb_Sprite *dspr, int16_t x, int16_t y);
  bool pushToSprite(Rgb_Sprite *dspr, int16_t x, int16_t y, uint16_t transparent_color);
  // bool pushRotated(int16_t angle, uint32_t transp = 0x00FFFFFF);
  bool pushRotated(Rgb_Sprite *spr, int16_t angle, uint32_t transp = 0x00FFFFFF);

  //旋转部分的代码
  void setPivot(int16_t x, int16_t y);
  void setWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
  void setViewport(int32_t x, int32_t y, int32_t w, int32_t h, bool vpDatum = true);
  bool getRotatedBounds(int16_t angle, int16_t *min_x, int16_t *min_y,
                                     int16_t *max_x, int16_t *max_y);
  void getRotatedBounds(int16_t angle, int16_t w, int16_t h, int16_t xp, int16_t yp,
                        int16_t *min_x, int16_t *min_y, int16_t *max_x, int16_t *max_y);
  bool getRotatedBounds(Rgb_Sprite *spr, int16_t angle, int16_t *min_x, int16_t *min_y,
                                             int16_t *max_x, int16_t *max_y);

protected:
  uint16_t *_framebuffer = nullptr;


#if defined(SCREEN_RGB)
  Arduino_RGB_Display *_output = nullptr;
#elif defined(SCREEN_QSPI)
  Arduino_CO5300 *_output = nullptr;
#else 
  Arduino_DSI_Display *_output = nullptr;
#endif

  int16_t MAX_X, MAX_Y;

  // for flushQuad() only
  uint16_t *_rowBuf = nullptr;


  //增加的滚动的逻辑参数
  uint8_t  _bpp = 16;     // bits per pixel (1, 4, 8 or 16)
  int32_t  _sx, _sy; // x,y for scroll zone
  uint32_t _sw, _sh; // w,h for scroll zone
  uint32_t _scolor;  // gap fill colour for scroll zone

  //旋转的参数
  int32_t  _xs, _ys, _xe, _ye, _xptr, _yptr; // for setWindow
  int16_t  _xPivot;   // TFT x pivot point coordinate for rotated Sprites
  int16_t  _yPivot;   // TFT x pivot point coordinate for rotated Sprites
  int32_t  _sinra;   // Sine of rotation angle in fixed point
  int32_t  _cosra;   // Cosine of rotation angle in fixed point

  int32_t  _vpX, _vpY, _vpW, _vpH;    // Note: x start, y start, x end + 1, y end + 1
  int32_t  _xDatum;
  int32_t  _yDatum;
  int32_t  _xWidth;
  int32_t  _yHeight;
  bool     _vpDatum;
  bool     _vpOoB;

  int32_t  _iwidth, _iheight; // Sprite memory image bit width and height (swapped during rotations)
  int32_t  _dwidth, _dheight; // Real sprite width and height (for <8bpp Sprites)
  int32_t  _bitwidth;         // Sprite image bit width for drawPixel (for <8bpp Sprites, not swapped)


    uint8_t  textfont,  // Current selected font number
           textsize,  // Current font size multiplier
           textdatum, // Text reference datum
           rotation;  // Display rotation (0-3)
private:
};

#endif // _ARDUINO_CANVAS_H_

#endif // !defined(LITTLE_FOOT_PRINT)
