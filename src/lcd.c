#include "emulator.h"

lcd_t lcd;

#ifdef LCD_DEBUG
#define DEBUG_LCD(...) printf("[lcd] "); printf(__VA_ARGS__)
#endif

const char *color_names[4] = {
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

        // LY (Read only)
        case 0x44:
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
            if (data > 0xDF) {
                return;
            }

            uint16_t source = data * 0x100;
            uint16_t destination = 0xFE00;

            // Transfer
            for (int i=0; i < 0x9F; i++) {
                mmu_wb(destination + i, mmu_rb(source + i));
            }

            cpu.cycles += 160;

            #ifdef LCD_DEBUG
            DEBUG_LCD("-> DMA: %x\n", data);
            DEBUG_LCD("DMA transfer\n Source: %04X\n Destination: FE00\n", source);
            #endif

            break;

        // BGP
        case 0x47:
            lcd.regs.bgp = data;

            lcd.palette[0] = lcd.regs.bgp & 3;
            lcd.palette[1] = (lcd.regs.bgp >> 2) & 3;
            lcd.palette[2] = (lcd.regs.bgp >> 4) & 3;
            lcd.palette[3] = (lcd.regs.bgp >> 6) & 3;

            #ifdef LCD_DEBUG
            DEBUG_LCD("-> BGP: %x\n", data);

            DEBUG_LCD("Palette:\n");
            DEBUG_LCD(" - 0 %s\n", color_names[lcd.palette[0]]);
            DEBUG_LCD(" - 1 %s\n", color_names[lcd.palette[1]]);
            DEBUG_LCD(" - 2 %s\n", color_names[lcd.palette[2]]);
            DEBUG_LCD(" - 3 %s\n", color_names[lcd.palette[3]]);

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

        // BGP (Write only)
        case 0x47:
            value = 0;
            break;
    }

    return value;
}

static inline void set_pixel(uint8_t x, uint8_t y, uint8_t color_index)
{
    lcd.color_buffer[y * LCD_WIDTH + x] = lcd.palette[color_index];
}

void draw_bg_line()
{
    if (!lcd.regs.control.fields.bg_window_enable) {
        memset(lcd.color_buffer, 0, LCD_WIDTH * LCD_HEIGHT);
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

        uint8_t bit_h = (mmu_rb(bg_tile_data_area + tile_offset + 1) >> (7 - tile_offset_x)) & 1;
        uint8_t bit_l = (mmu_rb(bg_tile_data_area + tile_offset) >> (7 - tile_offset_x)) & 1;

        uint8_t color_index = (bit_h << 1) | bit_l;
        set_pixel(x, lcd.regs.ly, color_index);
    }
}

void draw_window_line()
{
    if (!(lcd.regs.control.fields.window_enable && lcd.regs.control.fields.bg_window_enable)) {
        return;
    }

    uint16_t window_tile_data_area = lcd.regs.control.fields.bg_tile_data_area ? 0x8000 : 0x8800;
    uint16_t window_tile_map_area = lcd.regs.control.fields.window_tile_map_area ? 0x9C00 : 0x9800;

    if (lcd.regs.wx > 166) return;
    if (lcd.regs.wy > 143) return;

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

        uint8_t bit_h = (mmu_rb(window_tile_data_area + tile_offset + 1) >> (7 - tile_offset_x)) & 1;
        uint8_t bit_l = (mmu_rb(window_tile_data_area + tile_offset) >> (7 - tile_offset_x)) & 1;

        uint8_t color_index = (bit_h << 1) | bit_l;
        set_pixel(x, lcd.regs.ly, color_index);
    }
}

void draw_sprites()
{
    if (!lcd.regs.control.fields.obj_enable) {
        return;
    }

    uint8_t sprite_size = lcd.regs.control.fields.obj_size ? 2 : 1;

    for (int i=0; i < 40; i++) {
        uint16_t oam_offset = i * 4;
        lcd_oam_t* oam_entry = (lcd_oam_t *) &mmu.oam[oam_offset];
    
        uint8_t tile_index = lcd.regs.control.fields.obj_size ? oam_entry->tile_index & 0xFE : oam_entry->tile_index;
        uint16_t tile_offset = tile_index * BYTES_PER_TILE * sprite_size;
        uint8_t tile_x = oam_entry->x - 8;
        uint8_t tile_y = oam_entry->y - 16;
        bool flip_x = oam_entry->flags.fields.x_flip;
        bool flip_y = oam_entry->flags.fields.y_flip;

        if (tile_x == 0 || tile_x >= 160) continue;
        if (tile_y == 0 || tile_y >= 168) continue;

        for (int y=0; y < TILE_HEIGHT * sprite_size; y++) {
            uint8_t tile_offset_y = y * 2;
            uint8_t screen_y = flip_y ? (tile_y + 8 - y) : (tile_y + y);

            for (int x=0; x < 8; x++) {
                uint8_t bit_h = (mmu_rb(0x8000 + tile_offset + tile_offset_y + 1) >> (7 - x)) & 1;
                uint8_t bit_l = (mmu_rb(0x8000 + tile_offset + tile_offset_y) >> (7 - x)) & 1;

                uint8_t screen_x = flip_x ? (tile_x + 8 - x) : (tile_x + x);

                uint8_t color_index = (bit_h << 1) | bit_l;

                if (color_index == 0) {
                    continue;
                }

                if (oam_entry->flags.fields.bg_window_over_obj && (lcd.color_buffer[screen_y * LCD_WIDTH + screen_x] != 0)) {
                    continue;
                }

                set_pixel(screen_x, screen_y, color_index);
            }
        }
    }
}

void lcd_step(uint32_t cycles)
{
    if (lcd.regs.control.fields.lcd_ppu_enable) {
        lcd.cycles += cycles;

        if (lcd.regs.status.fields.mode == LCD_MODE_HBLANK) {
            if (lcd.cycles >= 204) {
                lcd.cycles -= 204;

                draw_bg_line();
                draw_window_line();

                lcd.regs.ly++;

                if (lcd.regs.ly == 143) {
                    lcd.regs.status.fields.mode = LCD_MODE_VBLANK;                    
                    cpu_request_interrupt(CPU_IF_VBLANK);
               } else {
                    if (lcd.regs.status.fields.mode_2_stat) {
                        cpu_request_interrupt(CPU_IF_LCD_STAT);
                    }

                    lcd.regs.status.fields.mode = LCD_MODE_OAM;
                }
            }
        
        } else if (lcd.regs.status.fields.mode == LCD_MODE_VBLANK) {
            if (lcd.cycles >= 456) {
                lcd.cycles -= 456;

                lcd.regs.ly++;

                if (lcd.regs.ly == 153) {
                    draw_sprites();
                    render();
                    
                    lcd.regs.ly = 0;
                    lcd.regs.status.fields.mode = LCD_MODE_OAM;
                }
            }

        } else if (lcd.regs.status.fields.mode == LCD_MODE_OAM) {
            if (lcd.cycles >= 80) {
                lcd.cycles -= 80;

                lcd.regs.status.fields.mode = LCD_MODE_VRAM;
            }

        } else if (lcd.regs.status.fields.mode == LCD_MODE_VRAM) {
            if (lcd.cycles >= 172) {
                lcd.cycles -= 172;

                if (lcd.regs.status.fields.mode_0_stat) {
                    cpu_request_interrupt(CPU_IF_LCD_STAT);
                }

                if (lcd.regs.ly == lcd.regs.lyc) {
                    lcd.regs.status.fields.lyc = 1;

                    if (lcd.regs.status.fields.lyc_stat) {
                        cpu_request_interrupt(CPU_IF_LCD_STAT);
                    }
                } else {
                    lcd.regs.status.fields.lyc = 0;
                }

                lcd.regs.status.fields.mode = LCD_MODE_HBLANK;
            }
        }
    }
}