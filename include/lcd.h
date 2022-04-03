#ifndef _lcd_h
#define _lcd_h

#define LCD_WIDTH 160
#define LCD_HEIGHT 144
#define LCD_SCALE 1

#include <SDL2/SDL.h>

//#define LCD_DEBUG

typedef union {
    struct {
        uint8_t palette_number_cgb      : 3;
        uint8_t tile_vram_bank          : 1;
        uint8_t palette_number_non_cgb  : 1;
        uint8_t x_flip                  : 1;
        uint8_t y_flip                  : 1;
        uint8_t bg_window_over_obj      : 1;
    } fields;

    uint8_t value;
} lcd_oam_flags_t;

typedef struct lcd_oam_t {
    uint8_t y;
    uint8_t x;
    uint8_t tile_index;
    lcd_oam_flags_t flags;
} lcd_oam_t;

typedef union {
    struct {
        uint8_t bg_window_enable        : 1;
        uint8_t obj_enable              : 1;
        uint8_t obj_size                : 1;
        uint8_t bg_tile_map_area        : 1;
        uint8_t bg_tile_data_area       : 1;
        uint8_t window_enable           : 1;
        uint8_t window_tile_map_area    : 1;
        uint8_t lcd_ppu_enable          : 1;
    } fields;

    uint8_t value;  
} lcd_control_reg_t;

typedef union {
    struct {
        uint8_t mode            : 2;
        uint8_t lyc             : 1;
        uint8_t mode_0_stat     : 1;
        uint8_t mode_1_stat     : 1;
        uint8_t mode_2_stat     : 1;
        uint8_t lyc_stat        : 1;
    } fields;

    uint8_t value;
} lcd_status_reg_t;

typedef struct lcd_regs_t {
    lcd_control_reg_t control;
    lcd_status_reg_t status;
    
    /* Scrolling */
    uint8_t scy;
    uint8_t scx;
    uint8_t ly;
    uint8_t lyc;
    uint8_t wy;
    uint8_t wx;

    uint8_t bgp;
} lcd_regs_t;

typedef struct lcd_t {
    uint8_t pixels[LCD_WIDTH * LCD_HEIGHT][3];
    lcd_regs_t regs;
    uint32_t cycles;
    uint8_t palette[3][4];
} lcd_t;

#define LCD_MODE_HBLANK 0
#define LCD_MODE_VBLANK 1
#define LCD_MODE_OAM    2
#define LCD_MODE_VRAM   3

#define LCD_CONTROL_BGWINDOW_ENABLE (1 << 0)
#define LCD_CONTROL_OBJ_ENABLE (1 << 1)
#define LCD_CONTROL_LCD_ENABLE (1 << 7)

#define TILES_PER_SCANLINE 32
#define BYTES_PER_TILE 16

void lcd_init();
void lcd_step(uint32_t cycles);

void lcd_wb(uint8_t addr, uint8_t data);
uint8_t lcd_rb(uint8_t addr);

extern lcd_t lcd;

#endif