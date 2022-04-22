#ifndef _sound_h
#define _sound_h

//#define SOUND_DEBUG

#define SOUND_SAMPLERATE 44100
#define SOUND_BUFFER_SIZE 1024

/* 
    OC = Output channel 1-2
    SC = Sound channel 1-4
*/

typedef union {
    struct {
        uint8_t length_data         : 6;
        uint8_t wave_pattern_duty   : 2;
    } fields;

    uint8_t value;
} sound_sc_sound_length_reg_t;

typedef union {
    struct {
        uint8_t envelope            : 3;
        uint8_t envelope_direction  : 1;
        uint8_t envelope_volume     : 4;
    } fields;

    uint8_t value;
} sound_sc_volume_envelope_reg_t;

typedef union {
    struct {
        uint8_t frequency_high      : 3;
        uint8_t reserved            : 3;
        uint8_t stop_output         : 1;
        uint8_t trigger             : 1;
    } fields;

    uint8_t value;
} sound_sc_frequency_high_reg_t;

typedef union {
    struct {
        uint8_t oc1_volume : 3;
        uint8_t vin_to_oc1 : 1;
        uint8_t oc2_volume : 3;
        uint8_t vin_to_oc2 : 1;
    } fields;

    uint8_t value;
} sound_oc_control_reg_t;

typedef union {
    struct {
        /* Output channel 1 */
        uint8_t sc1_to_oc1 : 1;
        uint8_t sc2_to_oc1 : 1;
        uint8_t sc3_to_oc1 : 1;
        uint8_t sc4_to_oc1 : 1;

        /* Output channel 2 */
        uint8_t sc1_to_oc2 : 1;
        uint8_t sc2_to_oc2 : 1;
        uint8_t sc3_to_oc2 : 1;
        uint8_t sc4_to_oc2 : 1;
    } fields;

    uint8_t value;
} sound_oc_select_reg_t;

typedef struct sound_sc1_t {
    /* Regs */
    sound_sc_sound_length_reg_t sound_length_reg;
    sound_sc_volume_envelope_reg_t volume_envelope_reg;
    sound_sc_frequency_high_reg_t frequency_high_reg;

    bool enabled;
    uint32_t cycles;
    uint32_t frequency;
    uint8_t duty_cycle;
    uint8_t fs_cycle;

    /* Audio functions */
    uint8_t envelope_timer;
    uint8_t length_timer;
} sound_sc1_t;

typedef struct sound_controller_t {
    sound_sc1_t sc1;

    /* Buffers */
    float buffer[SOUND_BUFFER_SIZE];
    uint32_t buffer_position;

    uint32_t cycles;
    bool playing;
    sound_oc_control_reg_t output_channel_control_reg;
    sound_oc_select_reg_t output_channel_select_reg;
    bool enabled;
} sound_controller_t;

extern sound_controller_t sound_controller;

void sound_init();
void sound_step(uint32_t cycles);

void sound_wb(uint8_t addr, uint8_t data);
uint8_t sound_rb(uint8_t addr);

#endif