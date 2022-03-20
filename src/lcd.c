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
    lcd.pixels[y * LCD_WIDTH + x] = default_palette[color_index][0];
    lcd.pixels[y * LCD_WIDTH + x + 1] = default_palette[color_index][1];
    lcd.pixels[y * LCD_WIDTH + x + 2] = default_palette[color_index][2];
}

void draw_bg_line(uint8_t line)
{
    uint16_t bg_tile_map = lcd.regs.control.fields.bg_tile_map_area ? 0x9C00 : 0x9800;
    uint16_t bg_tile_data = lcd.regs.control.fields.bg_tile_data_area ? 0x8000 : 0x8800;

    uint8_t scrolled_y = (line + lcd.regs.scy);

    for (int x=0; x < LCD_WIDTH; x++) {
        uint8_t scrolled_x = lcd.regs.scx + x;

        uint16_t map_offset = (scrolled_y >> 3) << 5;
        int tile_index = mmu_rb(bg_tile_map + map_offset);

        uint8_t tile_x = scrolled_x & 7;
        uint8_t tile_y = scrolled_y & 7;
        uint16_t tile_offset = (tile_index * BYTES_PER_TILE) + (tile_y * 2);

        uint8_t bit_h = (mmu_rb(bg_tile_data + tile_offset) >> (7 - tile_x)) & 1;
        uint8_t bit_l = (mmu_rb(bg_tile_data + tile_offset + 1) >> (7 - tile_x)) & 1;

        uint8_t color_index = (bit_h << 1) | bit_l;
        draw_pixel(x, line, color_index);

        #ifdef LCD_DEBUG
        DEBUG_LCD("draw_bg_line() line: %d scrolled_x: %d scrolled_y: %d map_offset: %x tile_index: %d tile_addr: %04X map_addr: %04X tile_x: %d tile_y: %d color_index: %d\n", line, scrolled_x, scrolled_y, map_offset, tile_index, bg_tile_data + tile_offset, bg_tile_map + map_offset, tile_x, tile_y, color_index);
        #endif
    }
}

void lcd_test()
{
    // Copy some test data

    uint8_t data[16] = {
        0x00, 0x00, 0x3C, 0x3C, 0x5A, 0x5A, 0x66, 0x66,
        0x66, 0x66, 0x5A, 0x5A, 0x3C, 0x3C, 0x00, 0x00
    };

    memcpy(&mmu.vram[0x0820], &data, 16);

    //memset(&mmu.vram[0x1800], 0x00, 0x400);
}

void lyc_check()
{
    if (lcd.regs.ly == lcd.regs.lyc) {
        //printf("[lcd] lyc_check() ly: %d lyc: %d\n", lcd.regs.ly, lcd.regs.lyc);
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

        //lcd_test();

        // Mode 0
        if (lcd.regs.status.fields.mode == 0) {
            if (lcd.cycles >= 204) {
                lcd.cycles = 0;
                lcd.regs.ly++;
                lyc_check();

                if (lcd.regs.ly == 144) {
                    lcd.regs.status.fields.mode = 1;
                    lcd_render();
                    
                    cpu_request_interrupt(CPU_IF_VBLANK);
               } else {
                    lcd.regs.status.fields.mode = 2;
                }   
            }
        
        // Mode 1
        } else if (lcd.regs.status.fields.mode == 1) {
            if (lcd.cycles >= 456) {
                lcd.cycles = 0;

                if (lcd.regs.ly == 0) {
                    lcd.regs.status.fields.mode = 2;
                } else {
                    lcd.regs.ly++;
                    lyc_check();

                    if (lcd.regs.ly > 153) {
                        lcd.regs.ly = 0;
                        lcd.regs.status.fields.mode = 2;
                    }
                }
            }

        // Mode 2
        } else if (lcd.regs.status.fields.mode == 2) {
            if (lcd.cycles >= 80) {
                lcd.cycles = 0;

                lcd.regs.status.fields.mode = 3;
            }

        // Mode 3
        } else if (lcd.regs.status.fields.mode == 3) {
            if (lcd.cycles >= 172) {
                lcd.cycles = 0;

                lcd.regs.status.fields.mode = 0;

                draw_bg_line(lcd.regs.ly);
            }
        }

    } else {
        memset(lcd.pixels, 0x00, LCD_WIDTH * LCD_HEIGHT * 4);
        lyc_check();
    }
}

void lcd_render()
{
    for (int y=0; y < 144; y++) {
        for (int x=0; x < 160; x++) {
            SDL_SetRenderDrawColor(emu.renderer, 
                lcd.pixels[y * LCD_WIDTH + x],
                lcd.pixels[y * LCD_WIDTH + x + 1],
                lcd.pixels[y * LCD_WIDTH + x + 2],
                0xFF
            );
            SDL_RenderDrawPoint(emu.renderer, x, y);
        }
    }

    SDL_RenderPresent(emu.renderer);
}