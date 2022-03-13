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
    lcd.regs.status = 0x01;
}

void lcd_wb(uint8_t addr, uint8_t data)
{
    switch(addr) {
        // Control
        case 0x40:
            lcd.regs.control = data;

            #ifdef LCD_DEBUG
            DEBUG_LCD("-> Control: %x\n", data);
            #endif

            break;
        
        // Status
        case 0x41:
            lcd.regs.status = data;

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

            /*
            // Update palette
            lcd.palette[0][0] = default_palette[color0][0]; 
            lcd.palette[0][1] = default_palette[color0][1];
            lcd.palette[0][2] = default_palette[color0][2]; 

            lcd.palette[1][0] = default_palette[color1][0]; 
            lcd.palette[1][1] = default_palette[color1][1];
            lcd.palette[1][2] = default_palette[color1][2]; 

            lcd.palette[2][0] = default_palette[color2][0]; 
            lcd.palette[2][1] = default_palette[color2][1];
            lcd.palette[2][2] = default_palette[color2][2]; 

            lcd.palette[3][0] = default_palette[color3][0]; 
            lcd.palette[3][1] = default_palette[color3][1];
            lcd.palette[3][2] = default_palette[color3][2]; 
            */

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
            value = lcd.regs.control;
            break;

        // Status
        case 0x41:
            value = lcd.regs.status;
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

void next_scanline()
{
    printf("LY: %d\n", lcd.regs.ly);
    lcd.regs.ly++;
    if (lcd.regs.ly > 153) {
        lcd.regs.ly = 0;
    }
}

void draw_pixel(uint8_t x, uint8_t y, uint8_t color_index)
{
    lcd.pixels[y * LCD_WIDTH + x] = default_palette[color_index][0];
    lcd.pixels[y * LCD_WIDTH + x + 1] = default_palette[color_index][1];
    lcd.pixels[y * LCD_WIDTH + x + 2] = default_palette[color_index][2];
}

void draw_bg_line(uint8_t line)
{
    uint16_t bg_tile_map = (lcd.regs.control & (1 << 3)) ? 0x9C00 : 0x9800;
    uint16_t bg_tile_data = 0x8000;

    uint8_t scrolled_y = line + lcd.regs.scy;

    for (int i=0; i < LCD_WIDTH; i++) {
        uint8_t scrolled_x = i + lcd.regs.scx;

        uint8_t map_offset = (scrolled_y * TILES_PER_SCANLINE) + (i / 8);
        uint8_t tile_index = mmu_rb(bg_tile_map + map_offset);

        uint8_t tile_x = i % 8;
        uint8_t tile_y = scrolled_y % 8;
        uint16_t tile_addr = bg_tile_data + (tile_index * BYTES_PER_TILE) + (tile_y * 2);

        uint8_t bit_h = (mmu_rb(tile_addr) << (7 - tile_x)) & 1;
        uint8_t bit_l = (mmu_rb(tile_addr + 1) << (7 - tile_x)) & 1;

        uint8_t color_index = (bit_h << 1) | bit_l;
        draw_pixel(i, line, color_index);

        #ifdef LCD_DEBUG
        DEBUG_LCD("draw_bg_line() line: %d scrolled_x: %d scrolled_y: %d map_offset: %x tile_index: %d tile_addr: %04X tile_x: %d tile_y: %d\n", line, scrolled_x, scrolled_y, map_offset, tile_index, tile_addr, tile_x, tile_y);
        #endif
    }
}

void lcd_step(uint32_t cycles)
{
    if (lcd.regs.control & LCD_CONTROL_LCD_ENABLE) {
        lcd.cycles += cycles;

        // Mode 0
        if ((lcd.regs.status & 3) == 0) {
            if (lcd.cycles >= 204) {
                lcd.cycles = 0;
                next_scanline();

                if (lcd.regs.ly == 143) {
                    lcd.regs.status &= ~3;
                    lcd.regs.status |= 1;
                    lcd_render();
                    cpu_request_interrupt(CPU_IF_VBLANK);
               } else {
                    lcd.regs.status &= ~3;
                    lcd.regs.status |= 2;
                }   
            }
        
        // Mode 1
        } else if ((lcd.regs.status & 3) == 1) {
            if (lcd.cycles >= 456) {
                lcd.cycles = 0;
                
                if (lcd.regs.ly == 0) {
                    lcd.regs.status &= ~3;
                    lcd.regs.status |= 2;
                } else {
                    next_scanline();
                    
                    if (lcd.regs.ly == 0) {
                        lcd.regs.status &= ~3;
                        lcd.regs.status |= 2;
                    }
                }
            }

        // Mode 2
        } else if ((lcd.regs.status & 3) == 2) {
            if (lcd.cycles >= 80) {
                lcd.cycles = 0;

                lcd.regs.status &= ~3;
                lcd.regs.status |= 3;
            }

        // Mode 3
        } else if ((lcd.regs.status & 3) == 3) {
            if (lcd.cycles >= 172) {
                lcd.cycles = 0;

                lcd.regs.status &= ~3;

                draw_bg_line(lcd.regs.ly);
            }
        }

    } else {
        memset(lcd.pixels, 0x00, LCD_WIDTH * LCD_HEIGHT * 4);
    }
}

void lcd_render()
{
    DEBUG_LCD("Render\n");
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