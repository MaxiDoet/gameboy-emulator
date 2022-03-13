#ifndef _lcd_h
#define _lcd_h

#define LCD_WIDTH 160
#define LCD_HEIGHT 144

#include <SDL2/SDL.h>

#define LCD_DEBUG

typedef struct lcd_regs_t {
    uint8_t control;
    uint8_t status;
    uint8_t scy;
    uint8_t scx;
    uint8_t ly;
    uint8_t lyc;
    uint8_t bgp;
} lcd_regs_t;

typedef struct lcd_t {
    uint8_t pixels[LCD_WIDTH * LCD_HEIGHT * 4];
    lcd_regs_t regs;
    uint32_t cycles;
    uint8_t palette[3][4];
} lcd_t;

#define LCD_CONTROL_BGWINDOW_ENABLE (1 << 0)
#define LCD_CONTROL_OBJ_ENABLE (1 << 1)
#define LCD_CONTROL_LCD_ENABLE (1 << 7)

#define TILES_PER_SCANLINE 32
#define BYTES_PER_TILE 16

void lcd_init();
void lcd_step(uint32_t cycles);
void lcd_render();

void lcd_wb(uint8_t addr, uint8_t data);
uint8_t lcd_rb(uint8_t addr);

extern lcd_t lcd;

#endif