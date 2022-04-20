#include "emulator.h"

sound_controller_t sound_controller;

#ifdef SOUND_DEBUG
#define DEBUG_SOUND(...) printf("[sound] "); printf(__VA_ARGS__)
#endif

void sound_init()
{

}

void sound_wb(uint8_t addr, uint8_t data) {
    switch(addr) {
        // NR12
        case 0x12:
            sound_controller.sc1_volume_envelope_reg.value = data;

            #ifdef SOUND_DEBUG
            DEBUG_SOUND("NR12 <- %02X | Sweep amount: %d | Envelope Direction: %s | Envelope Volume: %d%%\n", 
                        data, 
                        sound_controller.sc1_volume_envelope_reg.fields.sweep_amount, 
                        sound_controller.sc1_volume_envelope_reg.fields.envelope_direction ? "Increase" : "Decrease",
                        sound_controller.sc1_volume_envelope_reg.fields.envelope_volume / 16 * 100
                        );
            #endif

            break;

        // NR13
        case 0x13:
            sound_controller.sc1_frequency |= data;
            
            #ifdef SOUND_DEBUG
            DEBUG_SOUND("NR13 <- %02X | Channel 1 Frequency: %dhz\n", data, 131072 / (2048 - sound_controller.sc1_frequency));
            #endif

            break;
    
        // NR14
        case 0x14:
            sound_controller.sc1_frequency_high_reg.value = data;
            sound_controller.sc1_frequency |= (sound_controller.sc1_frequency_high_reg.fields.frequency_high << 8);
            
            #ifdef SOUND_DEBUG
            DEBUG_SOUND("NR13 <- %02X | Channel 1 Frequency: %dhz\n", data, 131072 / (2048 - sound_controller.sc1_frequency));
            #endif

            break;

        // NR50
        case 0x24:
            sound_controller.output_channel_control_reg.value = data;

            #ifdef SOUND_DEBUG
            DEBUG_SOUND("NR50 <- %02X | Output Channel 1 Volume: %d%% | Output Vin to Output Channel 1: %s | Output Channel 2 Volume: %d%% | Output Vin to Output Channel 2: %s\n",
                        data,
                        sound_controller.output_channel_control_reg.fields.oc1_volume / 7 * 100,
                        sound_controller.output_channel_control_reg.fields.vin_to_oc1 ? "Enabled" : "Disabled",
                        sound_controller.output_channel_control_reg.fields.oc2_volume / 7 * 100,
                        sound_controller.output_channel_control_reg.fields.vin_to_oc2 ? "Enabled" : "Disabled"
                        );
            #endif

            break;
    }
}

uint8_t sound_rb(uint8_t addr) {

}

void sound_tick(uint32_t cycles)
{
    uint32_t sc1_freq = 131072 / (2048 - sound_controller.sc1_frequency);
}