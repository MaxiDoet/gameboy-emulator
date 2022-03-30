#include "emulator.h"

lcd_t lcd;

#ifdef LCD_DEBUG
#define DEBUG_LCD(...) printf("[lcd] "); printf(__VA_ARGS__)
#endif

#define LCD_MODE_HBLANK 0
#define LCD_MODE_VBLANK 1
#define LCD_MODE_OAM    2
#define LCD_MODE_VRAM   3

uint8_t default_palette[4][3] = {
    {0xFF, 0xFF, 0xFF}, // White
    {0x96, 0x96, 0x96}, // Light Grey
    {0x3B, 0x3B, 0x3B}, // Dark Grey
    {0x00, 0x00, 0x00}  // Black
};

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

void draw_bg_line(uint8_t line)
{
    uint16_t bg_tile_map = lcd.regs.control.fields.bg_tile_map_area ? 0x9C00 : 0x9800;
    uint16_t bg_tile_data = lcd.regs.control.fields.bg_tile_data_area ? 0x8000 : 0x8800;

    uint8_t scrolled_line = (line + lcd.regs.scy);
    uint16_t scrolled_line_map_offset = (scrolled_line / 8) * TILES_PER_SCANLINE;

    for (int x=0; x < LCD_WIDTH; x++) {
        uint8_t scrolled_x = lcd.regs.scx + x;

        uint8_t tile_x = scrolled_x >> 3;
        uint8_t tile_offset_x = scrolled_x & 7;
        uint8_t tile_offset_y = scrolled_line & 7;

        uint16_t map_offset = scrolled_line_map_offset + tile_x;
        int tile_index = mmu_rb(bg_tile_map + map_offset);

        uint16_t tile_offset = (tile_index * BYTES_PER_TILE) + (tile_offset_y * 2);

        uint8_t bit_h = (mmu_rb(bg_tile_data + tile_offset) >> (7 - tile_offset_x)) & 1;
        uint8_t bit_l = (mmu_rb(bg_tile_data + tile_offset + 1) >> (7 - tile_offset_x)) & 1;

        uint8_t color_index = (bit_h << 1) | bit_l;
        draw_pixel(x, line, color_index);

        #ifdef LCD_DEBUG
        DEBUG_LCD("draw_bg_line() line: %d scrolled_x: %d scrolled_line: %d map_offset: %x tile_index: %d tile_addr: %04X map_addr: %04X tile_x: %d tile_offset_x: %d tile_offset_y: %d color_index: %d\n", line, scrolled_x, scrolled_line, map_offset, tile_index, bg_tile_data + tile_offset, bg_tile_map + map_offset, tile_x, tile_offset_x, tile_offset_y, color_index);
        #endif
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

                draw_bg_line(lcd.regs.ly);
            }
        }

    }
}