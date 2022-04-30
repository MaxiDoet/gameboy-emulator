#include "emulator.h"

sound_controller_t sound_controller;

#ifdef SOUND_DEBUG
#define DEBUG_SOUND(...) printf("[sound] "); printf(__VA_ARGS__)
#endif

void sound_power_on();
void sc1_trigger();
void sc2_trigger();

const uint8_t wave_patterns[4][8] = {
    { 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 1, 1 },
    { 0, 1, 1, 1, 1, 1, 1, 0 },
};

void sound_init()
{
    sound_controller.buffer_position = 0;
}

void sound_wb(uint8_t addr, uint8_t data) {
    switch(addr) {
        // NR10
        case 0x10:
            sound_controller.sc1.sweep_reg.value = data;

            #ifdef SOUND_DEBUG
            DEBUG_SOUND("NR10 <- %02X | Sweep shift: %d | Sweep direction: %s | Sweep time: %d\n", 
                        data, 
                        sound_controller.sc1.sweep_reg.fields.sweep_shift, 
                        sound_controller.sc1.sweep_reg.fields.sweep_direction ? "Increase" : "Decrease",
                        sound_controller.sc1.sweep_reg.fields.sweep_time
                        );
            #endif

            break;

        // NR11
        case 0x11:
            sound_controller.sc1.sound_length_reg.value = data;

            #ifdef SOUND_DEBUG
            DEBUG_SOUND("NR11 <- %02X | Sound length data: %d | Wave Pattern duty: %d\n", 
                        data, 
                        sound_controller.sc1.sound_length_reg.fields.length_data, 
                        sound_controller.sc1.sound_length_reg.fields.wave_pattern_duty
                        );
            #endif

            sound_controller.sc1.length_timer = 64 - sound_controller.sc1.sound_length_reg.fields.length_data;

            break;

        // NR12
        case 0x12:
            sound_controller.sc1.volume_envelope_reg.value = data;

            #ifdef SOUND_DEBUG
            DEBUG_SOUND("NR12 <- %02X | Sweep amount: %d | Envelope Direction: %s | Envelope Volume: %d%%\n", 
                        data, 
                        sound_controller.sc1.volume_envelope_reg.fields.envelope_period, 
                        sound_controller.sc1.volume_envelope_reg.fields.envelope_direction ? "Increase" : "Decrease",
                        sound_controller.sc1.volume_envelope_reg.fields.initial_volume / 15 * 100
                        );
            #endif

            sound_controller.sc1.envelope_timer = sound_controller.sc1.volume_envelope_reg.fields.envelope_period;

            // Check DAC status
            if (sound_controller.sc1.volume_envelope_reg.fields.initial_volume) {
                sound_controller.sc1.dac_enabled = true;
            } else {
                sound_controller.sc1.dac_enabled = false;
            }

            break;

        // NR13
        case 0x13:
            sound_controller.sc1.frequency |= data;
            
            #ifdef SOUND_DEBUG
            DEBUG_SOUND("NR13 <- %02X | Channel 1 Frequency: %dhz\n", data, 131072 / (2048 - sound_controller.sc1.frequency));
            #endif

            break;
    
        // NR14
        case 0x14:
            sound_controller.sc1.frequency_high_reg.value = data;
            sound_controller.sc1.frequency |= (sound_controller.sc1.frequency_high_reg.fields.frequency_high << 8);
            
            #ifdef SOUND_DEBUG
            DEBUG_SOUND("NR14 <- %02X | Channel 1 Frequency: %dhz | Stop output if length expires: %s | Initial: %s\n", 
                        data, 
                        131072 / (2048 - sound_controller.sc1.frequency),
                        sound_controller.sc1.frequency_high_reg.fields.length_enabled ? "Enabled" : "Disabled",
                        sound_controller.sc1.frequency_high_reg.fields.trigger ? "Yes" : "No"
                        );
            #endif

            if (sound_controller.sc1.frequency_high_reg.fields.trigger) {
                sc1_trigger();
            }

            break;

        // NR21
        case 0x16:
            sound_controller.sc2.sound_length_reg.value = data;

            #ifdef SOUND_DEBUG
            DEBUG_SOUND("NR21 <- %02X | Sound length data: %d | Wave Pattern duty: %d\n", 
                        data, 
                        sound_controller.sc2.sound_length_reg.fields.length_data, 
                        sound_controller.sc2.sound_length_reg.fields.wave_pattern_duty
                        );
            #endif

            sound_controller.sc2.length_timer = 64 - sound_controller.sc2.sound_length_reg.fields.length_data;

            break;

        // NR22
        case 0x17:
            sound_controller.sc2.volume_envelope_reg.value = data;

            #ifdef SOUND_DEBUG
            DEBUG_SOUND("NR22 <- %02X | Sweep amount: %d | Envelope Direction: %s | Envelope Volume: %d%%\n", 
                        data, 
                        sound_controller.sc2.volume_envelope_reg.fields.envelope_period, 
                        sound_controller.sc2.volume_envelope_reg.fields.envelope_direction ? "Increase" : "Decrease",
                        sound_controller.sc2.volume_envelope_reg.fields.initial_volume / 15 * 100
                        );
            #endif

            sound_controller.sc2.envelope_timer = sound_controller.sc2.volume_envelope_reg.fields.envelope_period;

            // Check DAC status
            if (sound_controller.sc2.volume_envelope_reg.fields.initial_volume) {
                sound_controller.sc2.dac_enabled = true;
            } else {
                sound_controller.sc2.dac_enabled = false;
            }

            break;

        // NR23
        case 0x18:
            sound_controller.sc2.frequency |= data;
            
            #ifdef SOUND_DEBUG
            DEBUG_SOUND("NR23 <- %02X | Channel 2 Frequency: %dhz\n", data, 131072 / (2048 - sound_controller.sc2.frequency));
            #endif

            break;
    
        // NR24
        case 0x19:
            sound_controller.sc2.frequency_high_reg.value = data;
            sound_controller.sc2.frequency |= (sound_controller.sc2.frequency_high_reg.fields.frequency_high << 8);
            
            #ifdef SOUND_DEBUG
            DEBUG_SOUND("NR24 <- %02X | Channel 2 Frequency: %dhz | Stop output if length expires: %s | Initial: %s\n", 
                        data, 
                        131072 / (2048 - sound_controller.sc2.frequency),
                        sound_controller.sc2.frequency_high_reg.fields.length_enabled ? "Enabled" : "Disabled",
                        sound_controller.sc2.frequency_high_reg.fields.trigger ? "Yes" : "No"
                        );
            #endif

            if (sound_controller.sc2.frequency_high_reg.fields.trigger) {
                sc2_trigger();
            }

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
        
        // NR51
        case 0x25:
            sound_controller.output_channel_select_reg.value = data;

            break;

        // NR52
        case 0x26:
            if (data & (1 << 7)) {
                sound_controller.enabled = true;
                sound_power_on();
            } else {
                sound_controller.enabled = false;
            }

            #ifdef SOUND_DEBUG
            DEBUG_SOUND("NR52 <- %02X | %s\n",
                        data,
                        sound_controller.enabled ? "Enabled sound" : "Disabled sound"
                        );
            #endif

            break;
    }
}

uint8_t sound_rb(uint8_t addr) {
    uint8_t result = 0;

    switch(addr) {
        // NR10
        case 0x10:
            result = sound_controller.sc1.sweep_reg.value;

            break;

        // NR11
        case 0x11:
            result = sound_controller.sc1.sound_length_reg.value & 0xC0;

            break;

        // NR12
        case 0x12:
            result = sound_controller.sc1.volume_envelope_reg.value;

            break;

        // NR14
        case 0x14:
            result |= (sound_controller.sc1.frequency_high_reg.fields.length_enabled) ? (1 << 6) : 0;

            break;

        // NR22
        case 0x17:
            result = sound_controller.sc2.sound_length_reg.value & 0xC0;

            break;

        // NR23
        case 0x18:
            result = sound_controller.sc2.volume_envelope_reg.value;

            break;

        // NR24
        case 0x19:
            result |= (sound_controller.sc2.frequency_high_reg.fields.length_enabled) ? (1 << 6) : 0;

            break;

        // NR50
        case 0x24:
            result = sound_controller.output_channel_control_reg.value;
            
            break;
        
        // NR51
        case 0x25:
            result = sound_controller.output_channel_select_reg.value;

            break;

        // NR52
        case 0x26:
            result |= (sound_controller.enabled) ? (1 << 7) : 0;
            result |= (sound_controller.sc1.enabled) ? (1 << 0) : 0;
            result |= (sound_controller.sc2.enabled) ? (1 << 1) : 0;

            break;
    }
}

void sound_power_on()
{
    // Reset all registers
    sound_controller.sc1.sweep_reg.value = 0x00;
    sound_controller.sc1.sound_length_reg.value = 0x00;
    sound_controller.sc1.volume_envelope_reg.value = 0x00;
    sound_controller.sc1.frequency_high_reg.value = 0x00;

    sound_controller.sc2.sound_length_reg.value = 0x00;
    sound_controller.sc2.volume_envelope_reg.value = 0x00;
    sound_controller.sc2.frequency_high_reg.value = 0x00;

    sound_controller.sc1.duty_cycle = 0;
    sound_controller.sc2.duty_cycle = 0;

    sound_controller.fs_cycle = 0;
}

void sc1_trigger()
{
    sound_controller.sc1.enabled = true;

    if (sound_controller.sc1.length_timer == 0) {
        sound_controller.sc1.length_timer = 64;
    }

    sound_controller.sc1.timer = (uint16_t) ((2048 - sound_controller.sc1.frequency) * 4);

    sound_controller.sc1.current_volume = sound_controller.sc1.volume_envelope_reg.fields.initial_volume;
    sound_controller.sc1.envelope_timer = sound_controller.sc1.volume_envelope_reg.fields.envelope_period;
    
    sound_controller.sc1.sweep_enabled = true;
}

void sc1_envelope_step()
{
    bool increment = (sound_controller.sc1.volume_envelope_reg.fields.envelope_direction == 1);

    if (sound_controller.sc1.volume_envelope_reg.fields.envelope_period == 0) {
        return;
    }

    if (sound_controller.sc1.envelope_timer > 0) {
        sound_controller.sc1.envelope_timer -= 1;
    }
    
    if (sound_controller.sc1.envelope_timer == 0) {
        sound_controller.sc1.envelope_timer = sound_controller.sc1.volume_envelope_reg.fields.envelope_period;

        if ((increment && (sound_controller.sc1.current_volume < 0x0F)) || (!increment && (sound_controller.sc1.current_volume > 0x00))) {
            if (increment) {
                sound_controller.sc1.current_volume += 1;
            } else {
                sound_controller.sc1.current_volume -= 1;
            }
        }
    }
}

void sc1_length_step()
{
    if (!sound_controller.sc1.frequency_high_reg.fields.length_enabled) {
        return;
    }

    sound_controller.sc1.length_timer--;

    if (sound_controller.sc1.length_timer == 0) {
        sound_controller.sc1.enabled = false;
    }
}

uint16_t sc1_sweep_calculate_frequency()
{
    uint16_t sweep_frequency = sound_controller.sc1.sweep_frequency >> sound_controller.sc1.sweep_reg.fields.sweep_shift;

    if (sound_controller.sc1.sweep_reg.fields.sweep_direction) {
        sweep_frequency = sound_controller.sc1.sweep_frequency - sweep_frequency;
    } else {
        sweep_frequency = sound_controller.sc1.sweep_frequency + sweep_frequency;
    }

    if (sweep_frequency > 2047 || sound_controller.sc1.sweep_reg.fields.sweep_shift == 0) {
        sound_controller.sc1.sweep_enabled = false;
        sound_controller.sc1.enabled = false;
    }

    return sweep_frequency;
}

void sc1_sweep_step()
{
    if (!sound_controller.sc1.sweep_enabled) {
        return;
    }

    sound_controller.sc1.sweep_timer--;

    if (sound_controller.sc1.sweep_timer == 0) {
        if (sound_controller.sc1.sweep_reg.fields.sweep_time == 0) {
            sound_controller.sc1.sweep_timer = 8;
        } else {
            sound_controller.sc1.sweep_timer = sound_controller.sc1.sweep_reg.fields.sweep_time;
        }

        if (sound_controller.sc1.sweep_reg.fields.sweep_time > 0) {
            uint16_t sweep_frequency = sc1_sweep_calculate_frequency();

            sound_controller.sc1.sweep_frequency = sweep_frequency;
            sound_controller.sc1.frequency = sweep_frequency;

            sc1_sweep_calculate_frequency();
        }
    }
}

float sc1_step()
{
    sound_controller.sc1.timer--;
    if (sound_controller.sc1.timer == 0) {
        sound_controller.sc1.timer = (uint16_t) ((2048 - sound_controller.sc1.frequency) * 4);
        sound_controller.sc1.duty_cycle = (sound_controller.sc1.duty_cycle + 1) % 8;
    }

    if (!sound_controller.sc1.enabled || !sound_controller.sc1.dac_enabled) {
        return 0.0f;
    }

    float amplitude = (float) wave_patterns[sound_controller.sc1.sound_length_reg.fields.wave_pattern_duty][sound_controller.sc1.duty_cycle];

    amplitude *= (float) sound_controller.sc1.current_volume;

    return amplitude;
}

void sc2_trigger()
{
    sound_controller.sc2.enabled = true;

    if (sound_controller.sc2.length_timer == 0) {
        sound_controller.sc2.length_timer = 64;
    }

    sound_controller.sc2.timer = (uint16_t) ((2048 - sound_controller.sc2.frequency) * 4);

    sound_controller.sc2.current_volume = sound_controller.sc2.volume_envelope_reg.fields.initial_volume;
    sound_controller.sc2.envelope_timer = sound_controller.sc2.volume_envelope_reg.fields.envelope_period;
}

void sc2_envelope_step()
{
    bool increment = (sound_controller.sc2.volume_envelope_reg.fields.envelope_direction == 1);

    if (sound_controller.sc2.volume_envelope_reg.fields.envelope_period == 0) {
        return;
    }

    if (sound_controller.sc2.envelope_timer > 0) {
        sound_controller.sc2.envelope_timer -= 1;
    }
    
    if (sound_controller.sc2.envelope_timer == 0) {
        sound_controller.sc2.envelope_timer = sound_controller.sc2.volume_envelope_reg.fields.envelope_period;

        if ((increment && (sound_controller.sc2.current_volume < 0x0F)) || (!increment && (sound_controller.sc2.current_volume > 0x00))) {
            if (increment) {
                sound_controller.sc2.current_volume += 1;
            } else {
                sound_controller.sc2.current_volume -= 1;
            }
        }
    }
}

void sc2_length_step()
{
    if (!sound_controller.sc2.frequency_high_reg.fields.length_enabled) {
        return;
    }

    sound_controller.sc2.length_timer--;

    if (sound_controller.sc2.length_timer == 0) {
        sound_controller.sc2.enabled = false;
    }
}

float sc2_step()
{
    sound_controller.sc2.timer--;
    if (sound_controller.sc2.timer == 0) {
        sound_controller.sc2.timer = (uint16_t) ((2048 - sound_controller.sc2.frequency) * 4);
        sound_controller.sc2.duty_cycle = (sound_controller.sc2.duty_cycle + 1) % 8;
    }

    if (!sound_controller.sc2.enabled || !sound_controller.sc2.dac_enabled) {
        return 0.0f;
    }

    float amplitude = (float) wave_patterns[sound_controller.sc2.sound_length_reg.fields.wave_pattern_duty][sound_controller.sc2.duty_cycle];

    amplitude *= (float) sound_controller.sc2.current_volume;

    return amplitude;
}

void sound_step(uint32_t cycles)
{
    if (!sound_controller.enabled) {
        return;
    }

    for (uint32_t i=0; i < cycles; i++) {
        sound_controller.cycles++;

        float sc1 = sc1_step();
        float sc2 = sc2_step();

        // Frame Sequencer
        if ((sound_controller.cycles % 8192) == 0) {
            switch(sound_controller.fs_cycle) {
                case 0:
                    sc1_length_step();
                    sc2_length_step();
                    break;

                case 2:
                    sc1_length_step();
                    sc2_length_step();

                    sc1_sweep_step();
                    break;

                case 4:
                    sc1_length_step();
                    sc2_length_step();
                    break;

                case 6:
                    sc1_length_step();
                    sc2_length_step();

                    sc1_sweep_step();
                    break;

                case 7:
                    sc1_envelope_step();
                    sc2_envelope_step();
                    break;
            }

            sound_controller.fs_cycle = (sound_controller.fs_cycle + 1) % 8;
        }

        if ((sound_controller.cycles % (4194304 / SOUND_SAMPLERATE)) == 0) {
            float left = 1.0f;
            float right = 1.0f;

            if (sound_controller.output_channel_select_reg.fields.sc1_to_oc1) right += sc1;
            if (sound_controller.output_channel_select_reg.fields.sc1_to_oc2) left += sc1;
            if (sound_controller.output_channel_select_reg.fields.sc2_to_oc1) right += sc2;
            if (sound_controller.output_channel_select_reg.fields.sc2_to_oc2) left += sc2;

            sound_controller.buffer[sound_controller.buffer_position] = ((float) (sound_controller.output_channel_control_reg.fields.oc2_volume + 1) / 7.0) * left;
            sound_controller.buffer[sound_controller.buffer_position + 1] = ((float) (sound_controller.output_channel_control_reg.fields.oc1_volume + 1) / 7.0) * right;
            sound_controller.buffer_position += 2;

            if (sound_controller.buffer_position >= SOUND_BUFFER_SIZE) {
                sound_controller.buffer_position = 0;

                SDL_QueueAudio(emulator.audiodev_id, sound_controller.buffer, sizeof(float) * SOUND_BUFFER_SIZE);
                memset(sound_controller.buffer, 0x00, sizeof(float) * SOUND_BUFFER_SIZE);

                while (SDL_GetQueuedAudioSize(emulator.audiodev_id) > sizeof(float) * SOUND_BUFFER_SIZE) { }
            }
        }
    }
}