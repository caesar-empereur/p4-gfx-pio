
#if !defined(LITTLE_FOOT_PRINT)

#include "Rgb_Sprite.h"

Rgb_Sprite::Rgb_Sprite(Arduino_DSI_Display *output): Arduino_GFX(0,0){
    MAX_X = WIDTH - 1;
    MAX_Y = HEIGHT - 1;
    setRotation(0);
    _output = output;

    //滚动部分的参数
    _iwidth  = _dwidth  = _bitwidth = 0;
    _iheight = _dheight = 0;

    // Default scroll rectangle and gap fill colour
    _sx = 0;
    _sy = 0;
    _sw = 0;
    _sh = 0;
    // _scolor = TFT_TRANSPARENT;
}

bool Rgb_Sprite::begin(int32_t speed){
    return true;
}

void Rgb_Sprite::flush(){

}

Rgb_Sprite::~Rgb_Sprite(){
  if (_framebuffer)
  {
    free(_framebuffer);
  }
}



bool Rgb_Sprite::createSprite(int16_t w, int16_t h){

    if (!_framebuffer){
      size_t s = w * h * 2;

  #if defined(ESP32)
    if (psramFound()){
      _framebuffer = (uint16_t *)ps_malloc(s);
    }
  else{
      _framebuffer = (uint16_t *)malloc(s);
  }
    #else
        _framebuffer = (uint16_t *)malloc(s);
  #endif
      if (!_framebuffer){
        return false;
      }
    }

    WIDTH = w;
    HEIGHT = h;

    MAX_X = WIDTH - 1;
    MAX_Y = HEIGHT - 1;
    setRotation(0);

    //滚动部分的参数
    _iwidth  = _dwidth  = _bitwidth = w;
    _iheight = _dheight = h;

    // Default scroll rectangle and gap fill colour
    _sx = 0;
    _sy = 0;
    _sw = w;
    _sh = h;
    _scolor = TFT_TRANSPARENT;

    setViewport(0, 0, _dwidth, _dheight);
    setPivot(_iwidth/2, _iheight/2);

    return true;
}

void Rgb_Sprite::writePixelPreclipped(int16_t x, int16_t y, uint16_t color)
{

  uint16_t *fb = _framebuffer;
  switch (_rotation)
  {
  case 1:
    fb += (int32_t)x * _height;
    fb += _max_y - y;
    *fb = color;
    break;
  case 2:
    fb += (int32_t)(_max_y - y) * _width;
    fb += _max_x - x;
    *fb = color;
    break;
  case 3:
    fb += (int32_t)(_max_x - x) * _height;
    fb += y;
    *fb = color;
    break;
  default: // case 0:
    fb += (int32_t)y * _width;
    fb += x;
    *fb = color;
  }
}

void Rgb_Sprite::writeFastVLine(int16_t x, int16_t y,
                                    int16_t h, uint16_t color)
{
  switch (_rotation)
  {
  case 1:
    writeFastHLineCore(_height - y - h, x, h, color);
    break;
  case 2:
    writeFastVLineCore(_max_x - x, _height - y - h, h, color);
    break;
  case 3:
    writeFastHLineCore(y, _max_x - x, h, color);
    break;
  default: // case 0:
    writeFastVLineCore(x, y, h, color);
  }
}

void Rgb_Sprite::writeFastVLineCore(int16_t x, int16_t y,
                                        int16_t h, uint16_t color)
{
  // log_i("writeFastVLineCore(x: %d, y: %d, h: %d)", x, y, h);
  if (_ordered_in_range(x, 0, MAX_X) && h)
  { // X on screen, nonzero height
    if (h < 0)
    {             // If negative height...
      y += h + 1; //   Move Y to top edge
      h = -h;     //   Use positive height
    }
    if (y <= MAX_Y)
    { // Not off bottom
      int16_t y2 = y + h - 1;
      if (y2 >= 0)
      { // Not off top
        // Line partly or fully overlaps screen
        if (y < 0)
        {
          y = 0;
          h = y2 + 1;
        } // Clip top
        if (y2 > MAX_Y)
        {
          h = MAX_Y - y + 1;
        } // Clip bottom

        uint16_t *fb = _framebuffer + ((int32_t)y * WIDTH) + x;
        while (h--)
        {
          *fb = color;
          fb += WIDTH;
        }
      }
    }
  }
}

void Rgb_Sprite::writeFastHLine(int16_t x, int16_t y,int16_t w, uint16_t color)
{
  // log_i("writeFastHLine(x: %d, y: %d, w: %d)", x, y, w);
  switch (_rotation)
  {
  case 1:
    writeFastVLineCore(_max_y - y, x, w, color);
    break;
  case 2:
    writeFastHLineCore(_width - x - w, _max_y - y, w, color);
    break;
  case 3:
    writeFastVLineCore(y, _width - x - w, w, color);
    break;
  default: // case 0:
    writeFastHLineCore(x, y, w, color);
  }
}

void Rgb_Sprite::writeFastHLineCore(int16_t x, int16_t y,
                                        int16_t w, uint16_t color)
{
  // log_i("writeFastHLineCore(x: %d, y: %d, w: %d)", x, y, w);
  if (_ordered_in_range(y, 0, MAX_Y) && w)
  { // Y on screen, nonzero width
    if (w < 0)
    {             // If negative width...
      x += w + 1; //   Move X to left edge
      w = -w;     //   Use positive width
    }
    if (x <= MAX_X)
    { // Not off right
      int16_t x2 = x + w - 1;
      if (x2 >= 0)
      { // Not off left
        // Line partly or fully overlaps screen
        if (x < 0)
        {
          x = 0;
          w = x2 + 1;
        } // Clip left
        if (x2 > MAX_X)
        {
          w = MAX_X - x + 1;
        } // Clip right

        uint16_t *fb = _framebuffer + ((int32_t)y * WIDTH) + x;
        while (w--)
        {
          *(fb++) = color;
        }
      }
    }
  }
}

void Rgb_Sprite::writeFillRectPreclipped(int16_t x, int16_t y,
                                             int16_t w, int16_t h, uint16_t color)
{
  // log_i("writeFillRectPreclipped(x: %d, y: %d, w: %d, h: %d)", x, y, w, h);
  if (_rotation > 0)
  {
    int16_t t = x;
    switch (_rotation)
    {
    case 1:
      x = WIDTH - y - h;
      y = t;
      t = w;
      w = h;
      h = t;
      break;
    case 2:
      x = WIDTH - x - w;
      y = HEIGHT - y - h;
      break;
    case 3:
      x = y;
      y = HEIGHT - t - w;
      t = w;
      w = h;
      h = t;
      break;
    }
  }
  // log_i("adjusted writeFillRectPreclipped(x: %d, y: %d, w: %d, h: %d)", x, y, w, h);
  uint16_t *row = _framebuffer;
  row += y * WIDTH;
  row += x;
  for (int j = 0; j < h; j++)
  {
    for (int i = 0; i < w; i++)
    {
      row[i] = color;
    }
    row += WIDTH;
  }
}

void Rgb_Sprite::drawIndexedBitmap(
    int16_t x, int16_t y,
    uint8_t *bitmap, uint16_t *color_index, int16_t w, int16_t h, int16_t x_skip)
{
  if (
      ((x + w - 1) < 0) || // Outside left
      ((y + h - 1) < 0) || // Outside top
      (x > _max_x) ||      // Outside right
      (y > _max_y)         // Outside bottom
  )
  {
    return;
  }
  else
  {
    if ((y + h - 1) > _max_y)
    {
      h -= (y + h - 1) - _max_y;
    }
    if (y < 0)
    {
      bitmap -= y * (w + x_skip);
      h += y;
      y = 0;
    }
    if ((x + w - 1) > _max_x)
    {
      x_skip += (x + w - 1) - _max_x;
      w -= (x + w - 1) - _max_x;
    }
    if (x < 0)
    {
      bitmap -= x;
      x_skip -= x;
      w += x;
      x = 0;
    }
    uint16_t *row = _framebuffer;
    row += y * _width;
    row += x;
    int16_t i;
    int16_t wi;
    while (h--)
    {
      i = 0;
      wi = w;
      while (wi >= 4)
      {
        uint32_t b32 = *((uint32_t *)bitmap);
        row[i++] = color_index[(b32 & 0xff)];
        row[i++] = color_index[(b32 & 0xff00) >> 8];
        row[i++] = color_index[(b32 & 0xff0000) >> 16];
        row[i++] = color_index[(b32 & 0xff000000) >> 24];
        wi -= 4;
        bitmap += 4;
      }
      while (i < w)
      {
        row[i++] = color_index[*bitmap++];
      }
      bitmap += x_skip;
      row += _width;
    }
  }
}

void Rgb_Sprite::drawIndexedBitmap(
    int16_t x, int16_t y,
    uint8_t *bitmap, uint16_t *color_index, uint8_t chroma_key, int16_t w, int16_t h, int16_t x_skip)
{
  if (
      ((x + w - 1) < 0) || // Outside left
      ((y + h - 1) < 0) || // Outside top
      (x > _max_x) ||      // Outside right
      (y > _max_y)         // Outside bottom
  )
  {
    return;
  }
  else
  {
    if ((y + h - 1) > _max_y)
    {
      h -= (y + h - 1) - _max_y;
    }
    if (y < 0)
    {
      bitmap -= y * (w + x_skip);
      h += y;
      y = 0;
    }
    if ((x + w - 1) > _max_x)
    {
      x_skip += (x + w - 1) - _max_x;
      w -= (x + w - 1) - _max_x;
    }
    if (x < 0)
    {
      bitmap -= x;
      x_skip -= x;
      w += x;
      x = 0;
    }
    uint16_t *row = _framebuffer;
    row += y * _width;
    row += x;
    int16_t i;
    int16_t wi;
    uint8_t color_key;
    while (h--)
    {
      i = 0;
      wi = w;
      while (wi >= 4)
      {
        uint32_t b32 = *((uint32_t *)bitmap);
        color_key = (b32 & 0xff);
        if (color_key != chroma_key)
        {
          row[i] = color_index[color_key];
        }
        ++i;
        color_key = (b32 & 0xff00) >> 8;
        if (color_key != chroma_key)
        {
          row[i] = color_index[color_key];
        }
        ++i;
        color_key = (b32 & 0xff0000) >> 16;
        if (color_key != chroma_key)
        {
          row[i] = color_index[color_key];
        }
        ++i;
        color_key = (b32 & 0xff000000) >> 24;
        if (color_key != chroma_key)
        {
          row[i] = color_index[color_key];
        }
        ++i;
        wi -= 4;
        bitmap += 4;
      }
      while (i < w)
      {
        color_key = *bitmap++;
        if (color_key != chroma_key)
        {
          row[i] = color_index[color_key];
        }
        ++i;
      }
      bitmap += x_skip;
      row += _width;
    }
  }
}

void Rgb_Sprite::draw16bitRGBBitmap(int16_t x, int16_t y,
                                        uint16_t *bitmap, int16_t w, int16_t h)
{
  switch (_rotation)
  {
  case 1:
    gfx_draw_bitmap_to_framebuffer_rotate_1(bitmap, w, h, _framebuffer, x, y, _width, _height);
    break;
  case 2:
    gfx_draw_bitmap_to_framebuffer_rotate_2(bitmap, w, h, _framebuffer, x, y, _width, _height);
    break;
  case 3:
    gfx_draw_bitmap_to_framebuffer_rotate_3(bitmap, w, h, _framebuffer, x, y, _width, _height);
    break;
  default: // case 0:
    gfx_draw_bitmap_to_framebuffer(bitmap, w, h, _framebuffer, x, y, _width, _height);
  }
}

void Rgb_Sprite::draw16bitRGBBitmapWithTranColor(
    int16_t x, int16_t y,
    uint16_t *bitmap, uint16_t transparent_color, int16_t w, int16_t h)
{
  if (
      ((x + w - 1) < 0) || // Outside left
      ((y + h - 1) < 0) || // Outside top
      (x > _max_x) ||      // Outside right
      (y > _max_y)         // Outside bottom
  )
  {
    return;
  }
  else
  {
    int16_t x_skip = 0;
    if ((y + h - 1) > _max_y)
    {
      h -= (y + h - 1) - _max_y;
    }
    if (y < 0)
    {
      bitmap -= y * w;
      h += y;
      y = 0;
    }
    if ((x + w - 1) > _max_x)
    {
      x_skip = (x + w - 1) - _max_x;
      w -= x_skip;
    }
    if (x < 0)
    {
      bitmap -= x;
      x_skip -= x;
      w += x;
      x = 0;
    }
    uint16_t *row = _framebuffer;
    row += y * _width;
    row += x;
    int16_t i;
    int16_t wi;
    uint16_t p;
    while (h--)
    {
      i = 0;
      wi = w;
      while (wi >= 4)
      {
        uint32_t b32 = *((uint32_t *)bitmap);
        p = (b32 & 0xffff);
        if (p != transparent_color)
        {
          row[i] = p;
        }
        ++i;
        p = (b32 & 0xffff0000) >> 16;
        if (p != transparent_color)
        {
          row[i] = p;
        }
        ++i;
        wi -= 2;
        bitmap += 2;
      }
      while (i < w)
      {
        p = *bitmap++;
        if (p != transparent_color)
        {
          row[i] = p;
        }
        ++i;
      }
      bitmap += x_skip;
      row += _width;
    }
  }
}

void Rgb_Sprite::draw16bitBeRGBBitmap(int16_t x, int16_t y,
                                          uint16_t *bitmap, int16_t w, int16_t h)
{
  if (
      ((x + w - 1) < 0) || // Outside left
      ((y + h - 1) < 0) || // Outside top
      (x > _max_x) ||      // Outside right
      (y > _max_y)         // Outside bottom
  )
  {
    return;
  }
  else
  {
    int16_t x_skip = 0;
    if ((y + h - 1) > _max_y)
    {
      h -= (y + h - 1) - _max_y;
    }
    if (y < 0)
    {
      bitmap -= y * w;
      h += y;
      y = 0;
    }
    if ((x + w - 1) > _max_x)
    {
      x_skip = (x + w - 1) - _max_x;
      w -= x_skip;
    }
    if (x < 0)
    {
      bitmap -= x;
      x_skip -= x;
      w += x;
      x = 0;
    }
    uint16_t *row = _framebuffer;
    row += y * _width;
    row += x;
    uint16_t color;
    for (int j = 0; j < h; j++)
    {
      for (int i = 0; i < w; i++)
      {
        color = *bitmap++;
        MSB_16_SET(row[i], color);
      }
      bitmap += x_skip;
      row += _width;
    }
  }
}

void Rgb_Sprite::pushSprite(int16_t x, int16_t y){
    if (_output){
        _output->draw16bitRGBBitmap(x, y, _framebuffer, WIDTH, HEIGHT);
    }
}


uint16_t *Rgb_Sprite::getFramebuffer(){
  return _framebuffer;
}

void Rgb_Sprite::setScrollRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color){
    if ((x >= _iwidth) || (y >= _iheight)) return;

    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }

    if ((x + w) > _iwidth ) w = _iwidth  - x;
    if ((y + h) > _iheight) h = _iheight - y;

    if ( w < 1 || h < 1) return;

    _sx = x;
    _sy = y;
    _sw = w;
    _sh = h;

    _scolor = color;
}


void Rgb_Sprite::scroll(int16_t dx, int16_t dy){
    if (abs(dx) >= _sw || abs(dy) >= _sh){
        fillRect (_sx, _sy, _sw, _sh, _scolor);
        return;
    }

    // Fetch the scroll area width and height set by setScrollRect()
    uint32_t w  = _sw - abs(dx); // line width to copy
    uint32_t h  = _sh - abs(dy); // lines to copy
    int32_t iw  = _iwidth;       // rounded up width of sprite

    // Fetch the x,y origin set by setScrollRect()
    uint32_t tx = _sx; // to x
    uint32_t fx = _sx; // from x
    uint32_t ty = _sy; // to y
    uint32_t fy = _sy; // from y

    // Adjust for x delta
    if (dx <= 0) fx -= dx;
    else tx += dx;

    // Adjust for y delta
    if (dy <= 0) fy -= dy;
    else{ // Scrolling down so start copy from bottom
        ty = ty + _sh - 1; // "To" pointer
        iw = -iw;          // Pointer moves backwards
        fy = ty - dy;      // "From" pointer
    }

    // Calculate "from y" and "to y" pointers in RAM
    uint32_t fyp = fx + fy * _iwidth;
    uint32_t typ = tx + ty * _iwidth;

    // Now move the pixels in RAM
    while (h--){ // move pixel lines (to, from, byte count)
        memmove( _framebuffer + typ, _framebuffer + fyp, w<<1);
        typ += iw;
        fyp += iw;
    }

    // flush();

    // Fill the gap left by the scrolling
    if (dx > 0) fillRect(_sx, _sy, dx, _sh, _scolor);
    if (dx < 0) fillRect(_sx + _sw + dx, _sy, -dx, _sh, _scolor);
    if (dy > 0) fillRect(_sx, _sy, _sw, dy, _scolor);
    if (dy < 0) fillRect(_sx, _sy + _sh + dy, _sw, -dy, _scolor);
}


bool Rgb_Sprite::pushToSprite(Rgb_Sprite *dspr, int16_t x, int16_t y){
    dspr->draw16bitRGBBitmap(x, y, _framebuffer, _iwidth, _iheight);
    return true;
}

bool Rgb_Sprite::pushToSprite(Rgb_Sprite *dspr, int16_t x, int16_t y, uint16_t transp){

    uint16_t sline_buffer[_iwidth];

    //这一行注释放开，会导致滚动的时候留下黑色尾巴，最后前景全部变黑
    // transp = transp>>8 | transp<<8;

    // Scan destination bounding box and fetch transformed pixels from source Sprite
    for (int32_t ys = 0; ys < _iheight; ys++) {
        int32_t ox = x;
        uint32_t pixel_count = 0;

        for (int32_t xs = 0; xs < _iwidth; xs++) {
            uint16_t rp = 0;
            rp = _framebuffer[xs + ys * _iwidth];

            if (transp == rp) {
              if (pixel_count) {
                dspr->draw16bitRGBBitmap(ox, y, sline_buffer,  pixel_count, 1);
                ox += pixel_count;
                pixel_count = 0;
              }
              ox++;
            } else {
              sline_buffer[pixel_count++] = rp;
            }
        }
        if (pixel_count) dspr->draw16bitRGBBitmap(ox, y, sline_buffer, pixel_count, 1);
        y++;
    }
    return true;
}

void Rgb_Sprite::setPivot(int16_t x, int16_t y){
    _xPivot = x;
    _yPivot = y;
}

bool Rgb_Sprite::pushRotated(int16_t angle, uint32_t transp){
    // Bounding box parameters
    int16_t min_x;
    int16_t min_y;
    int16_t max_x;
    int16_t max_y;

    if (!getRotatedBounds(angle, &min_x, &min_y, &max_x, &max_y) ) return false;

    uint16_t sline_buffer[max_x - min_x + 1];

    int32_t xt = min_x - _xPivot;
    int32_t yt = min_y - _yPivot;
    uint32_t xe = _iwidth << FP_SCALE;
    uint32_t ye = _iheight << FP_SCALE;
    uint16_t tpcolor = (uint16_t)transp;
    if (transp != 0x00FFFFFF) {
        // tpcolor = tpcolor>>8 | tpcolor<<8;
    }

    // Scan destination bounding box and fetch transformed pixels from source Sprite
    for (int32_t y = min_y; y <= max_y; y++, yt++) {
      int32_t x = min_x;
      uint32_t xs = (_cosra * xt - (_sinra * yt - (_xPivot << FP_SCALE)) + (1 << (FP_SCALE - 1)));
      uint32_t ys = (_sinra * xt + (_cosra * yt + (_yPivot << FP_SCALE)) + (1 << (FP_SCALE - 1)));

      while ((xs >= xe || ys >= ye) && x < max_x) { x++; xs += _cosra; ys += _sinra; }
      if (x == max_x) continue;

      uint32_t pixel_count = 0;
      do {
        uint32_t rp;
        int32_t xp = xs >> FP_SCALE;
        int32_t yp = ys >> FP_SCALE;
        rp = _framebuffer[xp + yp * _iwidth];
        if (transp != 0x00FFFFFF && tpcolor == rp) {
          if (pixel_count) {
              // _output->getDataBus()->writePixels(sline_buffer, pixel_count);
              // spr->draw16bitRGBBitmap(x - pixel_count, y, sline_buffer, pixel_count, 1);
              pixel_count = 0;
          }
        }
        else {
          sline_buffer[pixel_count++] = rp;
        }
      } while (++x < max_x && (xs += _cosra) < xe && (ys += _sinra) < ye);
      if (pixel_count) {
          // _output->getDataBus()->writePixels(sline_buffer, pixel_count);
          // spr->draw16bitRGBBitmap(x - pixel_count, y, sline_buffer, pixel_count, 1);
      }
    }
    return true;
}

bool Rgb_Sprite::pushRotated(Rgb_Sprite *spr, int16_t angle, uint32_t transp){
    // Bounding box parameters
    int16_t min_x;
    int16_t min_y;
    int16_t max_x;
    int16_t max_y;

    if (!getRotatedBounds(spr, angle, &min_x, &min_y, &max_x, &max_y) ) return false;

    uint16_t sline_buffer[max_x - min_x + 1];

    int32_t xt = min_x - spr->_xPivot;
    int32_t yt = min_y - spr->_yPivot;
    uint32_t xe = _iwidth << FP_SCALE;
    uint32_t ye = _iheight << FP_SCALE;
    uint16_t tpcolor = (uint16_t)transp;
    if (transp != 0x00FFFFFF) {
        // tpcolor = tpcolor>>8 | tpcolor<<8;
    }

    // Scan destination bounding box and fetch transformed pixels from source Sprite
    for (int32_t y = min_y; y <= max_y; y++, yt++) {
      int32_t x = min_x;
      uint32_t xs = (_cosra * xt - (_sinra * yt - (_xPivot << FP_SCALE)) + (1 << (FP_SCALE - 1)));
      uint32_t ys = (_sinra * xt + (_cosra * yt + (_yPivot << FP_SCALE)) + (1 << (FP_SCALE - 1)));

      while ((xs >= xe || ys >= ye) && x < max_x) { x++; xs += _cosra; ys += _sinra; }
      if (x == max_x) continue;

      uint32_t pixel_count = 0;
      do {
        uint32_t rp;
        int32_t xp = xs >> FP_SCALE;
        int32_t yp = ys >> FP_SCALE;
        rp = _framebuffer[xp + yp * _iwidth];
        if (transp != 0x00FFFFFF && tpcolor == rp) {
          if (pixel_count) {
              spr->draw16bitRGBBitmap(x - pixel_count, y, sline_buffer, pixel_count, 1);
              pixel_count = 0;
          }
        }
        else {
          sline_buffer[pixel_count++] = rp;
        }
      } while (++x < max_x && (xs += _cosra) < xe && (ys += _sinra) < ye);
      if (pixel_count) {
          spr->draw16bitRGBBitmap(x - pixel_count, y, sline_buffer, pixel_count, 1);
      }
    }
    return true;
}

bool Rgb_Sprite::getRotatedBounds(Rgb_Sprite *spr, int16_t angle, int16_t *min_x, int16_t *min_y,
                                                                    int16_t *max_x, int16_t *max_y){
    // Get the bounding box of this rotated source Sprite relative to Sprite pivot
    getRotatedBounds(angle, _dwidth, _dheight, _xPivot, _yPivot, min_x, min_y, max_x, max_y);

    // Move bounding box so source Sprite pivot coincides with destination Sprite pivot
    *min_x += spr->_xPivot;
    *max_x += spr->_xPivot;
    *min_y += spr->_yPivot;
    *max_y += spr->_yPivot;

    // Test only to show bounding box
    // spr->fillSprite(TFT_BLACK);
    // spr->drawRect(min_x, min_y, max_x - min_x + 1, max_y - min_y + 1, TFT_GREEN);

    // Return if bounding box is completely outside of destination Sprite
    if (*min_x > spr->_dwidth) return true;
    if (*min_y > spr->_dheight) return true;
    if (*max_x < 0) return true;
    if (*max_y < 0) return true;

    // Clip bounding box to Sprite boundaries
    // Clipping to a viewport will be done by destination Sprite pushImage function
    if (*min_x < 0) min_x = 0;
    if (*min_y < 0) min_y = 0;
    if (*max_x > spr->_dwidth)  *max_x = spr->_dwidth;
    if (*max_y > spr->_dheight) *max_y = spr->_dheight;

    return true;
}

void Rgb_Sprite::getRotatedBounds(int16_t angle, int16_t w, int16_t h, int16_t xp, int16_t yp,
                                   int16_t *min_x, int16_t *min_y, int16_t *max_x, int16_t *max_y){
    // Trig values for the rotation
    float radAngle = -angle * 0.0174532925; // Convert degrees to radians
    float sina = sin(radAngle);
    float cosa = cos(radAngle);

    w -= xp; // w is now right edge coordinate relative to xp
    h -= yp; // h is now bottom edge coordinate relative to yp

    // Calculate new corner coordinates
    int16_t x0 = -xp * cosa - yp * sina;
    int16_t y0 =  xp * sina - yp * cosa;

    int16_t x1 =  w * cosa - yp * sina;
    int16_t y1 = -w * sina - yp * cosa;

    int16_t x2 =  h * sina + w * cosa;
    int16_t y2 =  h * cosa - w * sina;

    int16_t x3 =  h * sina - xp * cosa;
    int16_t y3 =  h * cosa + xp * sina;

    // Find bounding box extremes, enlarge box to accomodate rounding errors
    *min_x = x0-2;
    if (x1 < *min_x) *min_x = x1-2;
    if (x2 < *min_x) *min_x = x2-2;
    if (x3 < *min_x) *min_x = x3-2;

    *max_x = x0+2;
    if (x1 > *max_x) *max_x = x1+2;
    if (x2 > *max_x) *max_x = x2+2;
    if (x3 > *max_x) *max_x = x3+2;

    *min_y = y0-2;
    if (y1 < *min_y) *min_y = y1-2;
    if (y2 < *min_y) *min_y = y2-2;
    if (y3 < *min_y) *min_y = y3-2;

    *max_y = y0+2;
    if (y1 > *max_y) *max_y = y1+2;
    if (y2 > *max_y) *max_y = y2+2;
    if (y3 > *max_y) *max_y = y3+2;

    _sinra = round(sina * (1<<FP_SCALE));
    _cosra = round(cosa * (1<<FP_SCALE));
}

bool Rgb_Sprite::getRotatedBounds(int16_t angle, int16_t *min_x, int16_t *min_y,
                                     int16_t *max_x, int16_t *max_y){
    // Get the bounding box of this rotated source Sprite relative to Sprite pivot
    getRotatedBounds(angle, _dwidth, _dheight, _xPivot, _yPivot, min_x, min_y, max_x, max_y);

    // Move bounding box so source Sprite pivot coincides with TFT pivot
    *min_x += _xPivot;
    *max_x += _xPivot;
    *min_y += _yPivot;
    *max_y += _yPivot;

    // Return if bounding box is outside of TFT viewport
    if (*min_x > _vpW) return false;
    if (*min_y > _vpH) return false;
    if (*max_x < _vpX) return false;
    if (*max_y < _vpY) return false;

    // Clip bounding box to be within TFT viewport
    if (*min_x < _vpX) *min_x = _vpX;
    if (*min_y < _vpY) *min_y = _vpY;
    if (*max_x > _vpW) *max_x = _vpW;
    if (*max_y > _vpH) *max_y = _vpH;

    return true;
}



void Rgb_Sprite::setViewport(int32_t x, int32_t y, int32_t w, int32_t h, bool vpDatum){
    // Viewport metrics (not clipped)
    _xDatum  = x; // Datum x position in screen coordinates
    _yDatum  = y; // Datum y position in screen coordinates
    _xWidth  = w; // Viewport width
    _yHeight = h; // Viewport height

    // Full size default viewport
    _vpDatum = false; // Datum is at top left corner of screen (true = top left of viewport)
    _vpOoB   = false; // Out of Bounds flag (true is all of viewport is off screen)
    _vpX = 0;         // Viewport top left corner x coordinate
    _vpY = 0;         // Viewport top left corner y coordinate
    _vpW = width();   // Equivalent of TFT width  (Nb: viewport right edge coord + 1)
    _vpH = height();  // Equivalent of TFT height (Nb: viewport bottom edge coord + 1)

    // Clip viewport to screen area
    if (x<0) { w += x; x = 0; }
    if (y<0) { h += y; y = 0; }
    if ((x + w) > width() ) { w = width()  - x; }
    if ((y + h) > height() ) { h = height() - y; }

    // Check if viewport is entirely out of bounds
    if (w < 1 || h < 1){
        // Set default values and Out of Bounds flag in case of error
        _xDatum = 0;
        _yDatum = 0;
        _xWidth  = width();
        _yHeight = height();
        _vpOoB = true;      // Set Out of Bounds flag to inhibit all drawing
        return;
    }
    if (!vpDatum){
        _xDatum = 0; // Reset to top left of screen if not using a viewport datum
        _yDatum = 0;
        _xWidth  = width();
        _yHeight = height();
    }
    // Store the clipped screen viewport metrics and datum position
    _vpX = x;
    _vpY = y;
    _vpW = x + w;
    _vpH = y + h;
    _vpDatum = vpDatum;
}

void transpose(int32_t a, int32_t b){
    int32_t c = a;
    a = b;
    b = c;
}

void Rgb_Sprite::setWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1){
    if (x0 > x1) transpose(x0, x1);
    if (y0 > y1) transpose(y0, y1);
    
    int32_t w = width();
    int32_t h = height();

    if ((x0 >= w) || (x1 < 0) || (y0 >= h) || (y1 < 0))
    { // Point to that extra "off screen" pixel
      _xs = 0;
      _ys = _dheight;
      _xe = 0;
      _ye = _dheight;
    }
    else
    {
      if (x0 < 0) x0 = 0;
      if (x1 >= w) x1 = w - 1;
      if (y0 < 0) y0 = 0;
      if (y1 >= h) y1 = h - 1;

      _xs = x0;
      _ys = y0;
      _xe = x1;
      _ye = y1;
    }

    _xptr = _xs;
    _yptr = _ys;
}

void Rgb_Sprite::drawString(const String& string, int32_t poX, int32_t poY, uint8_t font){
    _output->setTextSize(1);
    _output->setCursor(poX, poY);
    _output->println(string);
}


void Rgb_Sprite::drawWideLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t wide, uint16_t color){
    for(int16_t i = 1; i<= wide; i++){
        // _output->drawLine(x0, y0+(i-1), x1, y1+(i-1), color);

        writeFastHLine(x0, y0+(i-1), x1-x0, color);
    }

}


#endif // !defined(LITTLE_FOOT_PRINT)
