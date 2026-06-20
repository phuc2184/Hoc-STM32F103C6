#ifndef __SSD1306_FONTS_H__
#define __SSD1306_FONTS_H__

#include <stdint.h>

typedef struct {
    const uint8_t FontWidth;    /* Chiều rộng của 1 ký tự */
    uint8_t FontHeight;         /* Chiều cao của 1 ký tự */
    const uint16_t *data;       /* Con trỏ trỏ tới mảng điểm ảnh */
} FontDef;

extern FontDef Font_7x10;

#endif // __SSD1306_FONTS_H__
