#include "emulator.h"

sound_controller_t sound_controller;

#ifdef SOUND_DEBUG
#define DEBUG_SOUND(...) printf("[sound] "); printf(__VA_ARGS__)
#endif

const uint8_t wave_patterns[4][8] = {
    { 0, 0, 0, 0, 0, 0, 0, 1 },
    { 0, 0, 0, 0, 0, 0, 1, 1 },
    { 0, 0, 0, 0, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 0, 0 },
};

void sound_init()
{
    sound_controller.buffer_position = 0;
}

void sound_wb(uint8_t addr, uint8_t data) {
    switch(addr) {
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
                        sound_controller.sc1.volume_envelope_reg.fields.envelope, 
                        sound_controller.sc1.volume_envelope_reg.fields.envelope_direction ? "Increase" : "Decrease",
                        sound_controller.sc1.volume_envelope_reg.fields.envelope_volume / 15 * 100
                        );
            #endif

            sound_controller.sc1.envelope_timer = sound_controller.sc1.volume_envelope_reg.fields.envelope;

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
                        sound_controller.sc1.frequency_high_reg.fields.stop_output ? "Enabled" : "Disabled",
                        sound_controller.sc1.frequency_high_reg.fields.trigger ? "Yes" : "No"
                        );
            #endif

            if (sound_controller.sc1.frequency_high_reg.fields.trigger) {
                sound_controller.sc1.enabled = true;
                sound_controller.sc1.envelope_timer = sound_controller.sc1.volume_envelope_reg.fields.envelope;
                sound_controller.sc1.length_timer = 64 - sound_controller.sc1.sound_length_reg.fields.length_data;
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
            sound_controller.enabled = (data & (1 << 7));

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
            result |= (sound_controller.sc1.frequency_high_reg.fields.stop_output) ? (1 << 6) : 0;

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

            break;
    }
}

//void sound_callback(void *userdata, uint8_t *stream, int len)
void sound_callback()
{
    SDL_QueueAudio(emulator.audiodev_id, (uint8_t *) sound_controller.buffer, sizeof(float) * SOUND_BUFFER_SIZE);

    while (SDL_GetQueuedAudioSize(emulator.audiodev_id) > sizeof(float) * SOUND_BUFFER_SIZE) { }
}

void sc1_envelope_step()
{
    uint8_t current_volume = sound_controller.sc1.volume_envelope_reg.fields.envelope_volume;
    bool increment = (sound_controller.sc1.volume_envelope_reg.fields.envelope_direction == 1);

    if (sound_controller.sc1.volume_envelope_reg.fields.envelope == 0) {
        return;
    }

    if (sound_controller.sc1.envelope_timer > 0) {
        sound_controller.sc1.envelope_timer -= 1;
        return;
    }
    
    sound_controller.sc1.envelope_timer = sound_controller.sc1.volume_envelope_reg.fields.envelope;

    if ((increment && (current_volume < 0x0F)) || (!increment && (current_volume > 0x00))) {
        if (increment) {
            current_volume += 1;
        } else {
            current_volume -= 1;
        }
            
        sound_controller.sc1.volume_envelope_reg.fields.envelope_volume = current_volume;
    }
}

void sc1_length_step()
{
    if (!sound_controller.sc1.frequency_high_reg.fields.stop_output) {
        return;
    }

    sound_controller.sc1.length_timer--;

    if (sound_controller.sc1.length_timer == 0) {
        sound_controller.sc1.enabled = false;
    }
}

float sc1_step()
{
    if (!sound_controller.sc1.enabled) {
        return 0.0f;
    }

    sound_controller.sc1.cycles++;
    if (sound_controller.sc1.cycles >= (2048 - sound_controller.sc1.frequency) * 4) {
        sound_controller.sc1.cycles = 0;
        sound_controller.sc1.duty_cycle = (sound_controller.sc1.duty_cycle + 1) % 8;
    }

    float amplitude = (float) wave_patterns[sound_controller.sc1.sound_length_reg.fields.wave_pattern_duty][sound_controller.sc1.duty_cycle];
    
    if ((sound_controller.cycles % 8192) == 0) {
        sound_controller.sc1.fs_cycle = (sound_controller.sc1.fs_cycle + 1) % 8;

        switch(sound_controller.sc1.fs_cycle) {
            case 0:
            case 2:
            case 4:
            case 6:
                sc1_length_step();
                break;
            
            case 7:
                sc1_envelope_step();
                break;
        }
    }

    amplitude *= (float) sound_controller.sc1.volume_envelope_reg.fields.envelope_volume;

    return (amplitude / 7.5f) - 1.0f;
}

void sound_step(uint32_t cycles)
{
    if (!sound_controller.enabled) {
        return;
    }

    for (int i=0; i < (int) cycles; i++) {
        float sc1 = sc1_step();

        sound_controller.cycles++;

        if ((sound_controller.cycles % (4194304 / SOUND_SAMPLERATE)) == 0) {
            sound_controller.cycles = 0;
            
            float left = 1.0f;
            float right = 1.0f;

            // Mixing
            if (sound_controller.output_channel_select_reg.fields.sc1_to_oc1) left *= sc1;
            if (sound_controller.output_channel_select_reg.fields.sc1_to_oc2) right *= sc1;

            sound_controller.buffer[sound_controller.buffer_position] = ((float) sound_controller.output_channel_control_reg.fields.oc1_volume / 7.0) * left;
            sound_controller.buffer[sound_controller.buffer_position + 1] = ((float) sound_controller.output_channel_control_reg.fields.oc2_volume / 7.0) * right;
            sound_controller.buffer_position += 2;

            if (sound_controller.buffer_position >= SOUND_BUFFER_SIZE) {
                sound_controller.buffer_position = 0;

                sound_callback();
            }
        }
    }
}