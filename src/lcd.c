#include "emulator.h"

lcd_t lcd;

#ifdef LCD_DEBUG
#define DEBUG_LCD(...) printf("[lcd] "); printf(__VA_ARGS__)
#endif

uint8_t default_palette[4][3] = {
    {0xFF, 0xFF, 0xFF}, // White
    {0x96, 0x96, 0x96}, // Light Grey
    {0x3B, 0x3B, 0x3B}, // Dark Grey
    {0x00, 0x00, 0x00}  // Black
};

/*
uint8_t default_palette[4][3] = {
    {0x9B, 0xBC, 0x0F}, // White
    {0x8B, 0xAC, 0x0F}, // Light Grey
    {0x30, 0x62, 0x30}, // Dark Grey
    {0x0F, 0x38, 0x0F}  // Black
};
*/

char *color_names[4] = {
    "White",
    "Light Gray",
    "Dark Gray",
    "Black"
};

void lcd_init()
{
    lcd.cycles = 0;
    lcd.regs.status.fields.mode = 1;
}

void lcd_wb(uint8_t addr, uint8_t data)
{
    switch(addr) {
        // Control
        case 0x40:
            lcd.regs.control.value =  data;

            #ifdef LCD_DEBUG
            DEBUG_LCD("-> Control: %x\n", data);
            #endif

            break;
        
        // Status
        case 0x41:
            lcd.regs.status.value = data;

            #ifdef LCD_DEBUG
            DEBUG_LCD("-> Status: %x\n", data);
            #endif

            break;

        // SCY
        case 0x42:
            lcd.regs.scy = data;

            #ifdef LCD_DEBUG
            DEBUG_LCD("-> SXY: %x\n", data);
            #endif

            break;

        // SCX
        case 0x43:
            lcd.regs.scx = data;

            #ifdef LCD_DEBUG
            DEBUG_LCD("-> SCX: %x\n", data);
            #endif

            break;

        // LY
        case 0x44:
            lcd.regs.ly = data;

            #ifdef LCD_DEBUG
            DEBUG_LCD("-> LY: %x\n", data);
            #endif

            break;

        // LY Compare
        case 0x45:
            lcd.regs.lyc = data;

            #ifdef LCD_DEBUG
            DEBUG_LCD("-> LYC: %x\n", data);
            #endif

            break;

        // DMA
        case 0x46:

            #ifdef LCD_DEBUG
            DEBUG_LCD("-> DMA: %x\n", data);
            DEBUG_LCD("DMA transfer\n Source: %02X00\n Destination: FE00\n", data);
            #endif

            if (data > 0xDF) {
                return;
            }

            uint16_t source = (data << 8);
            uint16_t destination = 0xFE00;

            // Transfer
            for (int i=0; i < 160; i++) {
                mmu_wb(destination + i, mmu_rb(source + i));
            }

            break;

        // BGP
        case 0x47:
            lcd.regs.bgp = data;

            #ifdef LCD_DEBUG
            DEBUG_LCD("-> BGP: %x\n", data);

            DEBUG_LCD("Palette:\n");

            uint8_t color0 = lcd.regs.bgp & 3;
            uint8_t color1 = (lcd.regs.bgp >> 3) & 3;
            uint8_t color2 = (lcd.regs.bgp >> 5) & 3;
            uint8_t color3 = (lcd.regs.bgp >> 7) & 3;
            
            DEBUG_LCD(" - 0 %s\n", color_names[color0]);
            DEBUG_LCD(" - 1 %s\n", color_names[color1]);
            DEBUG_LCD(" - 2 %s\n", color_names[color2]);
            DEBUG_LCD(" - 3 %s\n", color_names[color3]);

            #endif
    }
}

uint8_t lcd_rb(uint8_t addr)
{
    uint8_t value = 0;

    switch(addr) {
        // Control
        case 0x40:
            value = lcd.regs.control.value;
            break;

        // Status
        case 0x41:
            value = lcd.regs.status.value;
            break;   

        // SCY
        case 0x42:
            value = lcd.regs.scy;
            break;

        // SCX
        case 0x43:
            value = lcd.regs.scx;
            break;

        // LY
        case 0x44:
            value = lcd.regs.ly;
            break;

        // LY Compare
        case 0x45:
            value = lcd.regs.lyc;
            break;  

        // BGP
        case 0x47:
            value = lcd.regs.bgp;
            break;
    }

    return value;
}

void draw_pixel(uint8_t x, uint8_t y, uint8_t color_index)
{
    lcd.pixels[y * LCD_WIDTH + x][0] = default_palette[color_index][0];
    lcd.pixels[y * LCD_WIDTH + x][1] = default_palette[color_index][1];
    lcd.pixels[y * LCD_WIDTH + x][2] = default_palette[color_index][2];
}

void draw_bg_line()
{
    if (!lcd.regs.control.fields.bg_window_enable) {
        memset(lcd.pixels, 0xFF, LCD_WIDTH * LCD_HEIGHT * 3);
        return;
    }

    uint16_t bg_tile_map_area = lcd.regs.control.fields.bg_tile_map_area ? 0x9C00 : 0x9800;
    uint16_t bg_tile_data_area = lcd.regs.control.fields.bg_tile_data_area ? 0x8000 : 0x8800;

    uint8_t scrolled_line = (lcd.regs.ly + lcd.regs.scy);
    uint16_t scrolled_line_map_offset = (scrolled_line / 8) * TILES_PER_SCANLINE;

    for (int x=0; x < LCD_WIDTH; x++) {
        uint8_t scrolled_x = lcd.regs.scx + x;

        uint8_t tile_x = scrolled_x >> 3;
        uint8_t tile_offset_x = scrolled_x & 7;
        uint8_t tile_offset_y = scrolled_line & 7;

        uint16_t map_offset = scrolled_line_map_offset + tile_x;
        uint8_t tile_index = mmu_rb(bg_tile_map_area + map_offset);

        uint16_t tile_offset;

        if (lcd.regs.control.fields.bg_tile_data_area) {
            tile_offset = (tile_index * BYTES_PER_TILE) + (tile_offset_y * 2);
        } else {
            tile_offset = (((int8_t) tile_index + 128) * BYTES_PER_TILE) + (tile_offset_y * 2);
        }

        uint8_t bit_h = (mmu_rb(bg_tile_data_area + tile_offset) >> (7 - tile_offset_x)) & 1;
        uint8_t bit_l = (mmu_rb(bg_tile_data_area + tile_offset + 1) >> (7 - tile_offset_x)) & 1;

        uint8_t color_index = (bit_h << 1) | bit_l;
        draw_pixel(x, lcd.regs.ly, color_index);

        #ifdef LCD_DEBUG
        DEBUG_LCD("draw_bg_line() line: %d scrolled_x: %d scrolled_line: %d map_offset: %x tile_index: %d tile_addr: %04X map_addr: %04X tile_x: %d tile_offset_x: %d tile_offset_y: %d color_index: %d\n", lcd.regs.ly, scrolled_x, scrolled_line, map_offset, tile_index, bg_tile_data_area + tile_offset, bg_tile_map_area + map_offset, tile_x, tile_offset_x, tile_offset_y, color_index);
        #endif
    }
}

void draw_window_line()
{
    if (!(lcd.regs.control.fields.window_enable && lcd.regs.control.fields.bg_window_enable)) {
        return;
    }

    uint16_t window_tile_data_area = lcd.regs.control.fields.bg_tile_data_area ? 0x8000 : 0x8800;
    uint16_t window_tile_map_area = lcd.regs.control.fields.window_tile_map_area ? 0x9C00 : 0x9800;

    uint8_t scrolled_line = lcd.regs.ly - lcd.regs.wy; 
    uint16_t scrolled_line_map_offset = (scrolled_line / 8) * TILES_PER_SCANLINE;

    for (int x=0; x < LCD_WIDTH; x++) {
        uint8_t scrolled_x = x + lcd.regs.wy - 7;

        uint8_t tile_x = scrolled_x >> 3;
        uint8_t tile_offset_x = scrolled_x & 7;
        uint8_t tile_offset_y = scrolled_line & 7;

        uint16_t map_offset = scrolled_line_map_offset + tile_x;
        uint8_t tile_index = mmu_rb(window_tile_map_area + map_offset);

        uint16_t tile_offset;

        if (lcd.regs.control.fields.bg_tile_data_area) {
            tile_offset = (tile_index * BYTES_PER_TILE) + (tile_offset_y * 2);
        } else {
            tile_offset = (((int8_t) tile_index + 128) * BYTES_PER_TILE) + (tile_offset_y * 2);
        }

        uint8_t bit_h = (mmu_rb(window_tile_data_area + tile_offset) >> (7 - tile_offset_x)) & 1;
        uint8_t bit_l = (mmu_rb(window_tile_data_area + tile_offset + 1) >> (7 - tile_offset_x)) & 1;

        uint8_t color_index = (bit_h << 1) | bit_l;
        draw_pixel(x, lcd.regs.ly, color_index);

        #ifdef LCD_DEBUG
        DEBUG_LCD("draw_window_line() line: %d scrolled_x: %d scrolled_line: %d map_offset: %x tile_index: %d tile_addr: %04X map_addr: %04X tile_x: %d tile_offset_x: %d tile_offset_y: %d color_index: %d\n", lcd.regs.ly, scrolled_x, scrolled_line, map_offset, tile_index, window_tile_data_area + tile_offset, window_tile_map_area + map_offset, tile_x, tile_offset_x, tile_offset_y, color_index);
        #endif
    }
}

void draw_sprite(uint8_t num)
{
    uint16_t oam_offset = num * 4;
    lcd_oam_t* oam = (lcd_oam_t *) &mmu.oam[oam_offset];
    
    uint8_t tile_index = oam->tile_index;
    uint16_t tile_offset = tile_index * BYTES_PER_TILE;
    uint8_t tile_x = oam->x;
    uint8_t tile_y = oam->y;
    bool flip_x = oam->flags.fields.x_flip;
    bool flip_y = oam->flags.fields.y_flip;

    if (tile_x == 0 || tile_x >= 160) return;
    if (tile_y == 0 || tile_y >= 168) return;

    for (int y=0; y < 8; y++) {
        uint8_t tile_offset_y = y * 2;
        uint8_t screen_y = tile_y - 16 + y;

        for (int x=0; x < 8; x++) {
            uint8_t bit_h = (mmu_rb(0x8000 + tile_offset + tile_offset_y) >> (7 - x)) & 1;
            uint8_t bit_l = (mmu_rb(0x8000 + tile_offset + tile_offset_y + 1) >> (7 - x)) & 1;

            uint8_t screen_x = tile_x - 8 + x;

            uint8_t color_index = (bit_h << 1) | bit_l;
            draw_pixel(screen_x, screen_y, color_index);
        }
    }
}

void draw_sprites()
{
    if (!lcd.regs.control.fields.obj_enable) {
        return;
    }

    for (int i=0; i < 40; i++) {
        draw_sprite(i);
    }
}

void lyc_check()
{
    if (lcd.regs.ly == lcd.regs.lyc) {
        if (lcd.regs.status.fields.lyc_stat) {
            lcd.regs.status.fields.lyc = 1;
            cpu_request_interrupt(CPU_IF_LCD_STAT);
        } else {
            lcd.regs.status.fields.lyc = 0;
        }
    }
}

void lcd_step(uint32_t cycles)
{
    if (lcd.regs.control.fields.lcd_ppu_enable) {
        lcd.cycles += cycles;

        if (lcd.regs.status.fields.mode == LCD_MODE_HBLANK) {
            if (lcd.cycles >= 204) {
                lcd.cycles = 0;
                lcd.regs.ly++;

                if (lcd.regs.ly == 144) {
                    lcd.regs.status.fields.mode = LCD_MODE_VBLANK;                    
                    cpu_request_interrupt(CPU_IF_VBLANK);
               } else {
                    if (lcd.regs.status.fields.mode_0_stat) {
                        cpu_request_interrupt(CPU_IF_LCD_STAT);
                    }

                    lcd.regs.status.fields.mode = LCD_MODE_OAM;
                }   
            }
        
        } else if (lcd.regs.status.fields.mode == LCD_MODE_VBLANK) {
            if (lcd.cycles >= 456) {
                lcd.cycles = 0;
                lcd.regs.ly++;

                if (lcd.regs.ly == 154) {
                    draw_sprites();
                    lyc_check();
                    emulator_render();
                    lcd.regs.ly = 0;
                    lcd.regs.status.fields.mode = LCD_MODE_OAM;
                }
            }

        } else if (lcd.regs.status.fields.mode == LCD_MODE_OAM) {
            if (lcd.cycles >= 80) {
                lcd.cycles = 0;

                lcd.regs.status.fields.mode = LCD_MODE_VRAM;
            }

        } else if (lcd.regs.status.fields.mode == LCD_MODE_VRAM) {
            if (lcd.cycles >= 172) {
                lcd.cycles = 0;

                lcd.regs.status.fields.mode = LCD_MODE_HBLANK;

                draw_bg_line();
                draw_window_line();
            }
        }

    }
}