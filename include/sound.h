#ifndef _sound_h
#define _sound_h

//#define SOUND_DEBUG

/* 
    OC = Output channel 1-2
    SC = Sound channel 1-4
*/

/*
typedef union {
    struct {
        uint8_t sweep_amount    : 3;
        uint8_t sweep_direction : 1;
        uint8_t sweep_time      : 2;
    } fields;

    uint8_t value;
} lcd_ch1_sweep_reg_t;
*/

typedef union {
    struct {
        uint8_t sweep_amount        : 3;
        uint8_t envelope_direction  : 1;
        uint8_t envelope_volume     : 4;
    } fields;

    uint8_t value;
} sound_sc_volume_envelope_reg_t;

typedef union {
    struct {
        uint8_t frequency_high      : 3;
        uint8_t reserved            : 3;
        uint8_t counter_selection   : 1;
        uint8_t initial             : 1;
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
        uint8_t c1_to_so2 : 1;
        uint8_t c2_to_so2 : 1;
        uint8_t c3_to_so2 : 1;
        uint8_t c4_to_so2 : 1;
    } fields;

    uint8_t value;
} sound_oc_select_reg_t;

typedef struct sound_controller_t {
    /* Sound channel 1*/
    sound_sc_volume_envelope_reg_t sc1_volume_envelope_reg;
    uint16_t sc1_frequency;
    sound_sc_frequency_high_reg_t sc1_frequency_high_reg;    

    sound_oc_control_reg_t output_channel_control_reg;
    sound_oc_select_reg_t output_channel_select_reg;
    bool enabled;
} sound_controller_t;

extern sound_controller_t sound_controller;

void sound_init();
void sound_tick(uint32_t cycles);

void sound_wb(uint8_t addr, uint8_t data);
uint8_t sound_rb(uint8_t addr);

#endif