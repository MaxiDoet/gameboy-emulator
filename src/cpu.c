#include "emulator.h"

cpu_t cpu;

#define FLAG_CARRY (1 << 4)
#define FLAG_HALFCARRY (1 << 5)
#define FLAG_SUBTRACTION (1 << 6)
#define FLAG_ZERO (1 << 7)

#define SET_FLAG(flag) cpu.regs.f |= flag
#define CLEAR_FLAG(flag) cpu.regs.f &= ~flag
#define CHECK_FLAG(flag) ((cpu.regs.f & flag) == flag)

void cpu_stack_push(uint16_t value)
{
    //cpu.regs.sp -= 2;
    //mmu_ww(cpu.regs.sp, value);

    cpu.regs.sp -= 1;
    uint8_t high = value >> 8;
    uint8_t low = value & 0xFF;
    mmu_wb(cpu.regs.sp, high);
    cpu.regs.sp -= 1;
    mmu_wb(cpu.regs.sp, low);
}

uint16_t cpu_stack_pop()
{
    //uint16_t value = mmu_rw(cpu.regs.sp);
    //cpu.regs.sp += 2;

    uint8_t low = mmu_rb(cpu.regs.sp);
    cpu.regs.sp += 1;
    uint8_t high = mmu_rb(cpu.regs.sp);
    cpu.regs.sp += 1;

    return (high << 8) | low;
}

/* CB */

uint8_t instruction_cb_rl(uint8_t value)
{
    uint8_t carry = CHECK_FLAG(FLAG_CARRY) ? 1 : 0;
    uint8_t result = (value << 1) | carry;

    if (result == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_CARRY);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    if ((value & (1 << 7))) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);
    
    return result;
}

uint8_t instruction_cb_rr(uint8_t value)
{
    uint8_t carry = CHECK_FLAG(FLAG_CARRY) ? 0x80 : 0x00;
    uint8_t result = (value >> 1) | carry;

    if (result == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_CARRY);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    if ((value & (1 << 0))) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);
    
    return result;
}

uint8_t instruction_cb_rlc(uint8_t value)
{
    uint8_t carry = value & (1 << 7);
    uint8_t result = (value << 1) | carry;

    if (result == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    if (carry) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    return result;
}

uint8_t instruction_cb_sra(uint8_t value) 
{
    uint8_t carry = (value & (1 << 0));
    uint8_t result = (value >> 1);

    if (result == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    if (carry) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    return result;
}

uint8_t instruction_cb_srl(uint8_t value)
{
    uint8_t carry = (value & (1 << 0));
    uint8_t result = (value >> 1) & 0x7F;

    if (result == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    if (carry) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    return result;
}

uint8_t instruction_cb_sla(uint8_t value)
{
    uint8_t carry = (value & (1 << 7));
    uint8_t result = value << 1;

    if (result == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    if (carry) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    return result;
}

void instruction_cb_sra_a()
{
    cpu.cycles += 8;

    cpu.regs.a = instruction_cb_sra(cpu.regs.a);
}

void instruction_cb_sra_b()
{
    cpu.cycles += 8;

    cpu.regs.b = instruction_cb_sra(cpu.regs.b);
}

void instruction_cb_sra_c()
{
    cpu.cycles += 8;

    cpu.regs.c = instruction_cb_sra(cpu.regs.c);
}

void instruction_cb_sra_d()
{
    cpu.cycles += 8;

    cpu.regs.d = instruction_cb_sra(cpu.regs.d);
}

void instruction_cb_sra_e()
{
    cpu.cycles += 8;

    cpu.regs.e = instruction_cb_sra(cpu.regs.e);
}

void instruction_cb_sra_h()
{
    cpu.cycles += 8;

    cpu.regs.h = instruction_cb_sra(cpu.regs.h);
}

void instruction_cb_sra_l()
{
    cpu.cycles += 8;

    cpu.regs.l = instruction_cb_sra(cpu.regs.l);
}

void instruction_cb_sra_hlp()
{
    cpu.cycles += 16;

    mmu_wb(cpu.regs.hl, instruction_cb_sra(mmu_rb(cpu.regs.hl)));
}

void instruction_cb_rlc_a()
{
    cpu.cycles += 8;

    cpu.regs.a = instruction_cb_rlc(cpu.regs.a);
}

void instruction_cb_rlc_b()
{
    cpu.cycles += 8;

    cpu.regs.b = instruction_cb_rlc(cpu.regs.b);
}

void instruction_cb_rlc_c()
{
    cpu.cycles += 8;

    cpu.regs.c = instruction_cb_rlc(cpu.regs.c);
}

void instruction_cb_rlc_d()
{
    cpu.cycles += 8;

    cpu.regs.d = instruction_cb_rlc(cpu.regs.d);
}

void instruction_cb_rlc_e()
{
    cpu.cycles += 8;

    cpu.regs.e = instruction_cb_rlc(cpu.regs.e);
}

void instruction_cb_rlc_h()
{
    cpu.cycles += 8;

    cpu.regs.h = instruction_cb_rlc(cpu.regs.h);
}

void instruction_cb_rlc_l()
{
    cpu.cycles += 8;

    cpu.regs.l = instruction_cb_rlc(cpu.regs.l);
}

void instruction_cb_rlc_hlp()
{
    cpu.cycles += 16;

    mmu_wb(cpu.regs.hl, instruction_cb_rlc(mmu_rb(cpu.regs.hl)));
}

void instruction_cb_rl_a()
{
    cpu.cycles += 8;

    cpu.regs.a = instruction_cb_rl(cpu.regs.a);
}

void instruction_cb_rl_b()
{
    cpu.cycles += 8;

    cpu.regs.b = instruction_cb_rl(cpu.regs.b);
}

void instruction_cb_rl_c()
{
    cpu.cycles += 8;

    cpu.regs.c = instruction_cb_rl(cpu.regs.c);
}

void instruction_cb_rl_d()
{
    cpu.cycles += 8;

    cpu.regs.d = instruction_cb_rl(cpu.regs.d);
}

void instruction_cb_rl_e()
{
    cpu.cycles += 8;

    cpu.regs.e = instruction_cb_rl(cpu.regs.e);
}

void instruction_cb_rl_h()
{
    cpu.cycles += 8;

    cpu.regs.h = instruction_cb_rl(cpu.regs.h);
}

void instruction_cb_rl_l()
{
    cpu.cycles += 8;

    cpu.regs.l = instruction_cb_rl(cpu.regs.l);
}

void instruction_cb_rl_hlp()
{
    cpu.cycles += 8;

    mmu_wb(cpu.regs.hl, instruction_cb_rl(mmu_rb(cpu.regs.hl)));
}

void instruction_cb_rr_a()
{
    cpu.cycles += 8;

    cpu.regs.a = instruction_cb_rr(cpu.regs.a);
}

void instruction_cb_rr_b()
{
    cpu.cycles += 8;

    cpu.regs.b = instruction_cb_rr(cpu.regs.b);
}

void instruction_cb_rr_c()
{
    cpu.cycles += 8;

    cpu.regs.c = instruction_cb_rr(cpu.regs.c);
}

void instruction_cb_rr_d()
{
    cpu.cycles += 8;

    cpu.regs.d = instruction_cb_rr(cpu.regs.d);
}

void instruction_cb_rr_e()
{
    cpu.cycles += 8;

    cpu.regs.e = instruction_cb_rr(cpu.regs.e);
}

void instruction_cb_rr_h()
{
    cpu.cycles += 8;

    cpu.regs.h = instruction_cb_rr(cpu.regs.h);
}

void instruction_cb_rr_l()
{
    cpu.cycles += 8;

    cpu.regs.l = instruction_cb_rr(cpu.regs.l);
}

void instruction_cb_rr_hlp()
{
    cpu.cycles += 8;

    mmu_wb(cpu.regs.hl, instruction_cb_rr(mmu_rb(cpu.regs.hl)));
}

void instruction_cb_swap_a()
{
    cpu.regs.a = ((cpu.regs.a & 0x0F) << 4) | ((cpu.regs.a >> 4) & 0x0F);

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 8;
}

void instruction_cb_swap_b()
{
    cpu.regs.b = ((cpu.regs.b & 0x0F) << 4) | ((cpu.regs.b >> 4) & 0x0F);

    if (cpu.regs.b == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 8;
}

void instruction_cb_swap_c()
{
    cpu.regs.c = ((cpu.regs.c & 0x0F) << 4) | ((cpu.regs.c >> 4) & 0x0F);

    if (cpu.regs.c == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 8;
}

void instruction_cb_swap_d()
{
    cpu.regs.d = ((cpu.regs.d & 0x0F) << 4) | ((cpu.regs.d >> 4) & 0x0F);

    if (cpu.regs.d == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 8;
}

void instruction_cb_swap_e()
{
    cpu.regs.e = ((cpu.regs.e & 0x0F) << 4) | ((cpu.regs.e >> 4) & 0x0F);

    if (cpu.regs.e == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 8;
}

void instruction_cb_swap_h()
{
    cpu.regs.h = ((cpu.regs.h & 0x0F) << 4) | ((cpu.regs.h >> 4) & 0x0F);

    if (cpu.regs.h == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 8;
}

void instruction_cb_swap_l()
{
    cpu.regs.l = ((cpu.regs.l & 0x0F) << 4) | ((cpu.regs.l >> 4) & 0x0F);

    if (cpu.regs.l == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 8;
}

void instruction_cb_swap_hlp()
{
    uint8_t value = mmu_rb(cpu.regs.hl);
    mmu_wb(cpu.regs.hl, ((value & 0x0F) << 4) | ((value>> 4) & 0x0F));

    if (mmu_rb(cpu.regs.hl) == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 8;
}

void instruction_cb_res_0_a()
{
    cpu.regs.a &= ~(1 << 0);

    cpu.cycles += 8;
}

void instruction_cb_res_1_a()
{
    cpu.regs.a &= ~(1 << 1);

    cpu.cycles += 8;
}

void instruction_cb_res_2_a()
{
    cpu.regs.a &= ~(1 << 2);

    cpu.cycles += 8;
}

void instruction_cb_res_3_a()
{
    cpu.regs.a &= ~(1 << 3);

    cpu.cycles += 8;
}

void instruction_cb_res_4_a()
{
    cpu.regs.a &= ~(1 << 4);

    cpu.cycles += 8;
}

void instruction_cb_res_5_a()
{
    cpu.regs.a &= ~(1 << 5);

    cpu.cycles += 8;
}

void instruction_cb_res_6_a()
{
    cpu.regs.a &= ~(1 << 6);

    cpu.cycles += 8;
}

void instruction_cb_res_7_a()
{
    cpu.regs.a &= ~(1 << 7);

    cpu.cycles += 8;
}

void instruction_cb_res_0_hlp()
{
    mmu_wb(cpu.regs.hl, mmu_rb(cpu.regs.hl) & ~(1 << 0));

    cpu.cycles += 16;
}

void instruction_cb_res_1_hlp()
{
    mmu_wb(cpu.regs.hl, mmu_rb(cpu.regs.hl) & ~(1 << 1));

    cpu.cycles += 16;
}

void instruction_cb_res_2_hlp()
{
    mmu_wb(cpu.regs.hl, mmu_rb(cpu.regs.hl) & ~(1 << 2));

    cpu.cycles += 16;
}

void instruction_cb_res_3_hlp()
{
    mmu_wb(cpu.regs.hl, mmu_rb(cpu.regs.hl) & ~(1 << 3));

    cpu.cycles += 16;
}

void instruction_cb_res_4_hlp()
{
    mmu_wb(cpu.regs.hl, mmu_rb(cpu.regs.hl) & ~(1 << 4));

    cpu.cycles += 16;
}

void instruction_cb_res_5_hlp()
{
    mmu_wb(cpu.regs.hl, mmu_rb(cpu.regs.hl) & ~(1 << 5));

    cpu.cycles += 16;
}

void instruction_cb_res_6_hlp()
{
    mmu_wb(cpu.regs.hl, mmu_rb(cpu.regs.hl) & ~(1 << 6));

    cpu.cycles += 16;
}

void instruction_cb_res_7_hlp()
{
    mmu_wb(cpu.regs.hl, mmu_rb(cpu.regs.hl) & ~(1 << 7));

    cpu.cycles += 16;
}

void instruction_cb_set_0_a()
{
    cpu.regs.a |= (1 << 0);

    cpu.cycles += 8;
}

void instruction_cb_set_1_a()
{
    cpu.regs.a |= (1 << 1);

    cpu.cycles += 8;
}

void instruction_cb_set_2_a()
{
    cpu.regs.a |= (1 << 2);

    cpu.cycles += 8;
}

void instruction_cb_set_3_a()
{
    cpu.regs.a |= (1 << 3);

    cpu.cycles += 8;
}

void instruction_cb_set_4_a()
{
    cpu.regs.a |= (1 << 4);

    cpu.cycles += 8;
}

void instruction_cb_set_5_a()
{
    cpu.regs.a |= (1 << 5);

    cpu.cycles += 8;
}

void instruction_cb_set_6_a()
{
    cpu.regs.a |= (1 << 6);

    cpu.cycles += 8;
}

void instruction_cb_set_7_a()
{
    cpu.regs.a |= (1 << 7);

    cpu.cycles += 8;
}

void instruction_cb_set_0_b()
{
    cpu.regs.b |= (1 << 0);

    cpu.cycles += 8;
}

void instruction_cb_set_1_b()
{
    cpu.regs.b |= (1 << 1);

    cpu.cycles += 8;
}

void instruction_cb_set_2_b()
{
    cpu.regs.b |= (1 << 2);

    cpu.cycles += 8;
}

void instruction_cb_set_3_b()
{
    cpu.regs.b |= (1 << 3);

    cpu.cycles += 8;
}

void instruction_cb_set_4_b()
{
    cpu.regs.b |= (1 << 4);

    cpu.cycles += 8;
}

void instruction_cb_set_5_b()
{
    cpu.regs.b |= (1 << 5);

    cpu.cycles += 8;
}

void instruction_cb_set_6_b()
{
    cpu.regs.b |= (1 << 6);

    cpu.cycles += 8;
}

void instruction_cb_set_7_b()
{
    cpu.regs.b |= (1 << 7);

    cpu.cycles += 8;
}

void instruction_cb_set_0_c()
{
    cpu.regs.c |= (1 << 0);

    cpu.cycles += 8;
}

void instruction_cb_set_1_c()
{
    cpu.regs.c |= (1 << 1);

    cpu.cycles += 8;
}

void instruction_cb_set_2_c()
{
    cpu.regs.c |= (1 << 2);

    cpu.cycles += 8;
}

void instruction_cb_set_3_c()
{
    cpu.regs.c |= (1 << 3);

    cpu.cycles += 8;
}

void instruction_cb_set_4_c()
{
    cpu.regs.c |= (1 << 4);

    cpu.cycles += 8;
}

void instruction_cb_set_5_c()
{
    cpu.regs.c |= (1 << 5);

    cpu.cycles += 8;
}

void instruction_cb_set_6_c()
{
    cpu.regs.c |= (1 << 6);

    cpu.cycles += 8;
}

void instruction_cb_set_7_c()
{
    cpu.regs.c |= (1 << 7);

    cpu.cycles += 8;
}

void instruction_cb_set_0_d()
{
    cpu.regs.d |= (1 << 0);

    cpu.cycles += 8;
}

void instruction_cb_set_1_d()
{
    cpu.regs.d |= (1 << 1);

    cpu.cycles += 8;
}

void instruction_cb_set_2_d()
{
    cpu.regs.d |= (1 << 2);

    cpu.cycles += 8;
}

void instruction_cb_set_3_d()
{
    cpu.regs.d |= (1 << 3);

    cpu.cycles += 8;
}

void instruction_cb_set_4_d()
{
    cpu.regs.d |= (1 << 4);

    cpu.cycles += 8;
}

void instruction_cb_set_5_d()
{
    cpu.regs.d |= (1 << 5);

    cpu.cycles += 8;
}

void instruction_cb_set_6_d()
{
    cpu.regs.d |= (1 << 6);

    cpu.cycles += 8;
}

void instruction_cb_set_7_d()
{
    cpu.regs.d |= (1 << 7);

    cpu.cycles += 8;
}

void instruction_cb_set_0_e()
{
    cpu.regs.e |= (1 << 0);

    cpu.cycles += 8;
}

void instruction_cb_set_1_e()
{
    cpu.regs.e |= (1 << 1);

    cpu.cycles += 8;
}

void instruction_cb_set_2_e()
{
    cpu.regs.e |= (1 << 2);

    cpu.cycles += 8;
}

void instruction_cb_set_3_e()
{
    cpu.regs.e |= (1 << 3);

    cpu.cycles += 8;
}

void instruction_cb_set_4_e()
{
    cpu.regs.e |= (1 << 4);

    cpu.cycles += 8;
}

void instruction_cb_set_5_e()
{
    cpu.regs.e |= (1 << 5);

    cpu.cycles += 8;
}

void instruction_cb_set_6_e()
{
    cpu.regs.e |= (1 << 6);

    cpu.cycles += 8;
}

void instruction_cb_set_7_e()
{
    cpu.regs.e |= (1 << 7);

    cpu.cycles += 8;
}

void instruction_cb_set_0_h()
{
    cpu.regs.h |= (1 << 0);

    cpu.cycles += 8;
}

void instruction_cb_set_1_h()
{
    cpu.regs.h |= (1 << 1);

    cpu.cycles += 8;
}

void instruction_cb_set_2_h()
{
    cpu.regs.h |= (1 << 2);

    cpu.cycles += 8;
}

void instruction_cb_set_3_h()
{
    cpu.regs.h |= (1 << 3);

    cpu.cycles += 8;
}

void instruction_cb_set_4_h()
{
    cpu.regs.h |= (1 << 4);

    cpu.cycles += 8;
}

void instruction_cb_set_5_h()
{
    cpu.regs.h |= (1 << 5);

    cpu.cycles += 8;
}

void instruction_cb_set_6_h()
{
    cpu.regs.h |= (1 << 6);

    cpu.cycles += 8;
}

void instruction_cb_set_7_h()
{
    cpu.regs.h |= (1 << 7);

    cpu.cycles += 8;
}

void instruction_cb_set_0_l()
{
    cpu.regs.l |= (1 << 0);

    cpu.cycles += 8;
}

void instruction_cb_set_1_l()
{
    cpu.regs.l |= (1 << 1);

    cpu.cycles += 8;
}

void instruction_cb_set_2_l()
{
    cpu.regs.l |= (1 << 2);

    cpu.cycles += 8;
}

void instruction_cb_set_3_l()
{
    cpu.regs.l |= (1 << 3);

    cpu.cycles += 8;
}

void instruction_cb_set_4_l()
{
    cpu.regs.l |= (1 << 4);

    cpu.cycles += 8;
}

void instruction_cb_set_5_l()
{
    cpu.regs.l |= (1 << 5);

    cpu.cycles += 8;
}

void instruction_cb_set_6_l()
{
    cpu.regs.l |= (1 << 6);

    cpu.cycles += 8;
}

void instruction_cb_set_7_l()
{
    cpu.regs.l |= (1 << 7);

    cpu.cycles += 8;
}

void instruction_cb_set_0_hlp()
{
    mmu_wb(cpu.regs.hl, mmu_rb(cpu.regs.hl) | (1 << 0));

    cpu.cycles += 16;
}

void instruction_cb_set_1_hlp()
{
    mmu_wb(cpu.regs.hl, mmu_rb(cpu.regs.hl) | (1 << 1));
    
    cpu.cycles += 16;
}

void instruction_cb_set_2_hlp()
{
    mmu_wb(cpu.regs.hl, mmu_rb(cpu.regs.hl) | (1 << 2));
    
    cpu.cycles += 16;
}

void instruction_cb_set_3_hlp()
{
    mmu_wb(cpu.regs.hl, mmu_rb(cpu.regs.hl) | (1 << 3));
    
    cpu.cycles += 16;
}

void instruction_cb_set_4_hlp()
{
    mmu_wb(cpu.regs.hl, mmu_rb(cpu.regs.hl) | (1 << 4));
    
    cpu.cycles += 16;
}

void instruction_cb_set_5_hlp()
{
    mmu_wb(cpu.regs.hl, mmu_rb(cpu.regs.hl) | (1 << 5));
    
    cpu.cycles += 16;
}

void instruction_cb_set_6_hlp()
{
    mmu_wb(cpu.regs.hl, mmu_rb(cpu.regs.hl) | (1 << 6));
    
    cpu.cycles += 16;
}

void instruction_cb_set_7_hlp()
{
    mmu_wb(cpu.regs.hl, mmu_rb(cpu.regs.hl) | (1 << 7));
    
    cpu.cycles += 16;
}

void instruction_cb_bit_0_a()
{
    uint8_t value = cpu.regs.a;

    if (value & (1 << 0)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_1_a()
{
    uint8_t value = cpu.regs.a;

    if (value & (1 << 1)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_2_a()
{
    uint8_t value = cpu.regs.a;

    if (value & (1 << 2)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_3_a()
{
    uint8_t value = cpu.regs.a;

    if (value & (1 << 3)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_4_a()
{
    uint8_t value = cpu.regs.a;

    if (value & (1 << 4)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_5_a()
{
    uint8_t value = cpu.regs.a;

    if (value & (1 << 5)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_6_a()
{
    uint8_t value = cpu.regs.a;

    if (value & (1 << 6)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_7_a()
{
    uint8_t value = cpu.regs.a;

    if (value & (1 << 7)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_0_b()
{
    uint8_t value = cpu.regs.b;

    if (value & (1 << 0)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_1_b()
{
    uint8_t value = cpu.regs.b;

    if (value & (1 << 1)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_2_b()
{
    uint8_t value = cpu.regs.b;

    if (value & (1 << 2)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_3_b()
{
    uint8_t value = cpu.regs.b;

    if (value & (1 << 3)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_4_b()
{
    uint8_t value = cpu.regs.b;

    if (value & (1 << 4)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_5_b()
{
    uint8_t value = cpu.regs.b;

    if (value & (1 << 5)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_6_b()
{
    uint8_t value = cpu.regs.b;

    if (value & (1 << 6)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_7_b()
{
    uint8_t value = cpu.regs.b;

    if (value & (1 << 7)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_0_c()
{
    uint8_t value = cpu.regs.c;

    if (value & (1 << 0)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_1_c()
{
    uint8_t value = cpu.regs.c;

    if (value & (1 << 1)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_2_c()
{
    uint8_t value = cpu.regs.c;

    if (value & (1 << 2)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_3_c()
{
    uint8_t value = cpu.regs.c;

    if (value & (1 << 3)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_4_c()
{
    uint8_t value = cpu.regs.c;

    if (value & (1 << 4)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_5_c()
{
    uint8_t value = cpu.regs.c;

    if (value & (1 << 5)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_6_c()
{
    uint8_t value = cpu.regs.c;

    if (value & (1 << 6)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_7_c()
{
    uint8_t value = cpu.regs.c;

    if (value & (1 << 7)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_0_d()
{
    uint8_t value = cpu.regs.d;

    if (value & (1 << 0)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_1_d()
{
    uint8_t value = cpu.regs.d;

    if (value & (1 << 1)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_2_d()
{
    uint8_t value = cpu.regs.d;

    if (value & (1 << 2)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_3_d()
{
    uint8_t value = cpu.regs.d;

    if (value & (1 << 3)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_4_d()
{
    uint8_t value = cpu.regs.d;

    if (value & (1 << 4)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_5_d()
{
    uint8_t value = cpu.regs.d;

    if (value & (1 << 5)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_6_d()
{
    uint8_t value = cpu.regs.d;

    if (value & (1 << 6)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_7_d()
{
    uint8_t value = cpu.regs.d;

    if (value & (1 << 7)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_0_e()
{
    uint8_t value = cpu.regs.e;

    if (value & (1 << 0)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_1_e()
{
    uint8_t value = cpu.regs.e;

    if (value & (1 << 1)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_2_e()
{
    uint8_t value = cpu.regs.e;

    if (value & (1 << 2)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_3_e()
{
    uint8_t value = cpu.regs.e;

    if (value & (1 << 3)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_4_e()
{
    uint8_t value = cpu.regs.e;

    if (value & (1 << 4)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_5_e()
{
    uint8_t value = cpu.regs.e;

    if (value & (1 << 5)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_6_e()
{
    uint8_t value = cpu.regs.e;

    if (value & (1 << 6)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_7_e()
{
    uint8_t value = cpu.regs.e;

    if (value & (1 << 7)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_0_h()
{
    uint8_t value = cpu.regs.h;

    if (value & (1 << 0)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_1_h()
{
    uint8_t value = cpu.regs.h;

    if (value & (1 << 1)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_2_h()
{
    uint8_t value = cpu.regs.h;

    if (value & (1 << 2)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_3_h()
{
    uint8_t value = cpu.regs.h;

    if (value & (1 << 3)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_4_h()
{
    uint8_t value = cpu.regs.h;

    if (value & (1 << 4)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_5_h()
{
    uint8_t value = cpu.regs.h;

    if (value & (1 << 5)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_6_h()
{
    uint8_t value = cpu.regs.h;

    if (value & (1 << 6)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_7_h()
{
    uint8_t value = cpu.regs.h;

    if (value & (1 << 7)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_0_l()
{
    uint8_t value = cpu.regs.l;

    if (value & (1 << 0)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_1_l()
{
    uint8_t value = cpu.regs.l;

    if (value & (1 << 1)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_2_l()
{
    uint8_t value = cpu.regs.l;

    if (value & (1 << 2)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_3_l()
{
    uint8_t value = cpu.regs.l;

    if (value & (1 << 3)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_4_l()
{
    uint8_t value = cpu.regs.l;

    if (value & (1 << 4)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_5_l()
{
    uint8_t value = cpu.regs.l;

    if (value & (1 << 5)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_6_l()
{
    uint8_t value = cpu.regs.l;

    if (value & (1 << 6)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_7_l()
{
    uint8_t value = cpu.regs.l;

    if (value & (1 << 7)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_0_hlp()
{
    uint8_t value = mmu_rb(cpu.regs.hl);

    if (value & (1 << 0)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_1_hlp()
{
    uint8_t value = mmu_rb(cpu.regs.hl);

    if (value & (1 << 1)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_2_hlp()
{
    uint8_t value = mmu_rb(cpu.regs.hl);

    if (value & (1 << 2)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_3_hlp()
{
    uint8_t value = mmu_rb(cpu.regs.hl);

    if (value & (1 << 3)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_4_hlp()
{
    uint8_t value = mmu_rb(cpu.regs.hl);

    if (value & (1 << 4)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_5_hlp()
{
    uint8_t value = mmu_rb(cpu.regs.hl);

    if (value & (1 << 5)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_6_hlp()
{
    uint8_t value = mmu_rb(cpu.regs.hl);

    if (value & (1 << 6)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_bit_7_hlp()
{
    uint8_t value = mmu_rb(cpu.regs.hl);

    if (value & (1 << 7)) {
        CLEAR_FLAG(FLAG_ZERO);
    } else {
        SET_FLAG(FLAG_ZERO);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 8;
}

void instruction_cb_srl_a()
{
    cpu.cycles += 8;

    cpu.regs.a = instruction_cb_srl(cpu.regs.a);
}

void instruction_cb_srl_b()
{
    cpu.cycles += 8;

    cpu.regs.b = instruction_cb_srl(cpu.regs.b);
}

void instruction_cb_srl_c()
{
    cpu.cycles += 8;

    cpu.regs.c = instruction_cb_srl(cpu.regs.c);
}

void instruction_cb_srl_d()
{
    cpu.cycles += 8;

    cpu.regs.d = instruction_cb_srl(cpu.regs.d);
}

void instruction_cb_srl_e()
{
    cpu.cycles += 8;

    cpu.regs.e = instruction_cb_srl(cpu.regs.e);
}

void instruction_cb_srl_h()
{
    cpu.cycles += 8;

    cpu.regs.h = instruction_cb_srl(cpu.regs.h);
}

void instruction_cb_srl_l()
{
    cpu.cycles += 8;

    cpu.regs.l = instruction_cb_srl(cpu.regs.l);
}

void instruction_cb_srl_hlp()
{
    cpu.cycles += 8;

    mmu_wb(cpu.regs.hl, instruction_cb_srl(mmu_rb(cpu.regs.hl)));
}

/* SLA */

void instruction_cb_sla_a()
{
    cpu.cycles += 8;

    cpu.regs.a = instruction_cb_sla(cpu.regs.a);
}

void instruction_cb_sla_b()
{
    cpu.cycles += 8;

    cpu.regs.b = instruction_cb_sla(cpu.regs.b);
}

void instruction_cb_sla_c()
{
    cpu.cycles += 8;

    cpu.regs.c = instruction_cb_sla(cpu.regs.c);
}

void instruction_cb_sla_d()
{
    cpu.cycles += 8;

    cpu.regs.d = instruction_cb_sla(cpu.regs.d);
}

void instruction_cb_sla_e()
{
    cpu.cycles += 8;

    cpu.regs.e = instruction_cb_sla(cpu.regs.e);
}

void instruction_cb_sla_h()
{
    cpu.cycles += 8;

    cpu.regs.h = instruction_cb_sla(cpu.regs.h);
}

void instruction_cb_sla_l()
{
    cpu.cycles += 8;

    cpu.regs.l = instruction_cb_sla(cpu.regs.l);
}

void instruction_cb_sla_hlp()
{
    cpu.cycles += 16;

    mmu_wb(cpu.regs.hl, instruction_cb_sla(mmu_rb(cpu.regs.hl)));
}

/* Instructions */

/* Control */

void instruction_ccf()
{
    if (cpu.regs.f & FLAG_CARRY) {
        CLEAR_FLAG(FLAG_CARRY);
    } else {
        SET_FLAG(FLAG_CARRY);
    }

    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 4;
}

void instruction_scf()
{
    SET_FLAG(FLAG_CARRY);
 
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 4;
}

void instruction_nop()
{
    cpu.cycles += 4;
}

void instruction_halt()
{
    cpu.halted = true;

    cpu.cycles += 4;
}

void instruction_stop()
{
    // TODO: Stop emulator
    cpu.halted = true;
    
    timer.div = 0;

    cpu.cycles += 4;
}

void instruction_di()
{
    cpu.ime = false;

    cpu.cycles += 4;
}

void instruction_ei()
{
    cpu.ime = true;

    cpu.cycles += 4;
}

/* Jump */

void instruction_jp_nn()
{
    cpu.regs.pc = mmu_rw(cpu.regs.pc);

    cpu.cycles += 16;
}

void instruction_jp_hl()
{
    cpu.regs.pc = cpu.regs.hl;

    cpu.cycles += 4;
}

void instruction_jp_nz_nn()
{
    if (!CHECK_FLAG(FLAG_ZERO)) {
        cpu.regs.pc = mmu_rw(cpu.regs.pc);
        cpu.cycles += 16;
    } else {
        cpu.cycles += 12;
        cpu.regs.pc += 2;
    }
}

void instruction_jp_z_nn()
{
    if (CHECK_FLAG(FLAG_ZERO)) {
        cpu.regs.pc = mmu_rw(cpu.regs.pc);
        cpu.cycles += 16;
    } else {
        cpu.cycles += 12;
        cpu.regs.pc += 2;
    }
}

void instruction_jp_nc_nn()
{
    if (!CHECK_FLAG(FLAG_CARRY)) {
        cpu.regs.pc = mmu_rw(cpu.regs.pc);
        cpu.cycles += 16;
    } else {
        cpu.cycles += 12;
        cpu.regs.pc += 2;
    }
}

void instruction_jp_c_nn()
{
    if (CHECK_FLAG(FLAG_CARRY)) {
        cpu.regs.pc = mmu_rw(cpu.regs.pc);
        cpu.cycles += 16;
    } else {
        cpu.cycles += 12;
        cpu.regs.pc += 2;
    }
}

void instruction_jr_dd()
{
    cpu.regs.pc += (int8_t) mmu_rb(cpu.regs.pc) + 1;

    cpu.cycles += 12;
}

void instruction_jr_nz_dd()
{
    if (!CHECK_FLAG(FLAG_ZERO)) {
        cpu.regs.pc += (int8_t) mmu_rb(cpu.regs.pc) + 1;
        cpu.cycles += 12;
    } else {
        cpu.regs.pc += 1;
        cpu.cycles += 8;
    }
}

void instruction_jr_z_dd()
{
    if (CHECK_FLAG(FLAG_ZERO)) {
        cpu.regs.pc += (int8_t) mmu_rb(cpu.regs.pc) + 1;
        cpu.cycles += 12;
    } else {
        cpu.regs.pc += 1;
        cpu.cycles += 8;
    }
}

void instruction_jr_nc_dd()
{
    if (!CHECK_FLAG(FLAG_CARRY)) {
        cpu.regs.pc += (int8_t) mmu_rb(cpu.regs.pc) + 1;
        cpu.cycles += 12;
    } else {
        cpu.regs.pc += 1;
        cpu.cycles += 8;
    }
}

void instruction_jr_c_dd()
{
    if (CHECK_FLAG(FLAG_CARRY)) {
        cpu.regs.pc += (int8_t) mmu_rb(cpu.regs.pc) + 1;
        cpu.cycles += 12;
    } else {
        cpu.regs.pc += 1;
        cpu.cycles += 8;
    }
}

void instruction_call_nn()
{
    cpu.regs.sp -= 2;
    mmu_ww(cpu.regs.sp, cpu.regs.pc + 2);
    cpu.regs.pc = mmu_rw(cpu.regs.pc);

    cpu.cycles += 24;
}

void instruction_call_nz_nn()
{
    if (!CHECK_FLAG(FLAG_ZERO)) {
        cpu.regs.sp -= 2;
        mmu_ww(cpu.regs.sp, cpu.regs.pc + 2);
        cpu.regs.pc = mmu_rw(cpu.regs.pc);
        cpu.cycles += 24;
    } else {
        cpu.regs.pc += 2;
        cpu.cycles += 12;
    }
}

void instruction_call_z_nn()
{
    if (CHECK_FLAG(FLAG_ZERO)) {
        cpu.regs.sp -= 2;
        mmu_ww(cpu.regs.sp, cpu.regs.pc + 2);
        cpu.regs.pc = mmu_rw(cpu.regs.pc);
        cpu.cycles += 24;
    } else {
        cpu.regs.pc += 2;
        cpu.cycles += 12;
    }
}

void instruction_call_nc_nn()
{
    if (!CHECK_FLAG(FLAG_CARRY)) {
        cpu.regs.sp -= 2;
        mmu_ww(cpu.regs.sp, cpu.regs.pc + 2);
        cpu.regs.pc = mmu_rw(cpu.regs.pc);
        cpu.cycles += 24;
    } else {
        cpu.regs.pc += 2;
        cpu.cycles += 12;
    }
}

void instruction_call_c_nn()
{
    if (CHECK_FLAG(FLAG_CARRY)) {
        cpu.regs.sp -= 2;
        mmu_ww(cpu.regs.sp, cpu.regs.pc + 2);
        cpu.regs.pc = mmu_rw(cpu.regs.pc);
        cpu.cycles += 24;
    } else {
        cpu.regs.pc += 2;
        cpu.cycles += 12;
    }
}

void instruction_ret()
{
    cpu.regs.pc = mmu_rw(cpu.regs.sp);
    cpu.regs.sp += 2;
    cpu.cycles += 16;
}

void instruction_ret_nz()
{
    if (!CHECK_FLAG(FLAG_ZERO)) {
        cpu.regs.pc = mmu_rw(cpu.regs.sp);
        cpu.regs.sp += 2;
        cpu.cycles += 20;
    } else {
        cpu.cycles += 8;
    }
}

void instruction_ret_z()
{
    if (CHECK_FLAG(FLAG_ZERO)) {
        cpu.regs.pc = mmu_rw(cpu.regs.sp);
        cpu.regs.sp += 2;
        cpu.cycles += 20;
    } else {
        cpu.cycles += 8;
    }
}

void instruction_ret_nc()
{
    if (!CHECK_FLAG(FLAG_CARRY)) {
        cpu.regs.pc = mmu_rw(cpu.regs.sp);
        cpu.regs.sp += 2;
        cpu.cycles += 20;
    } else {
        cpu.cycles += 8;
    }
}

void instruction_ret_c()
{
    if (CHECK_FLAG(FLAG_CARRY)) {
        cpu.regs.pc = mmu_rw(cpu.regs.sp);
        cpu.regs.sp += 2;
        cpu.cycles += 20;
    } else {
        cpu.cycles += 8;
    }
}

void instruction_reti()
{
    cpu.regs.pc = mmu_rw(cpu.regs.sp);
    cpu.regs.sp += 2;
    cpu.ime = true;

    cpu.cycles += 16;
}

void instruction_rst_00()
{
    cpu_stack_push(cpu.regs.pc);
    cpu.regs.pc = 0x0000;

    cpu.cycles += 16;
}

void instruction_rst_08()
{
    cpu_stack_push(cpu.regs.pc);
    cpu.regs.pc = 0x0008;

    cpu.cycles += 16;
}

void instruction_rst_10()
{
    cpu_stack_push(cpu.regs.pc);
    cpu.regs.pc = 0x0010;

    cpu.cycles += 16;
}

void instruction_rst_18()
{
    cpu_stack_push(cpu.regs.pc);
    cpu.regs.pc = 0x0018;

    cpu.cycles += 16;
}

void instruction_rst_20()
{
    cpu_stack_push(cpu.regs.pc);
    cpu.regs.pc = 0x0020;

    cpu.cycles += 16;
}

void instruction_rst_28()
{
    cpu_stack_push(cpu.regs.pc);
    cpu.regs.pc = 0x0028;

    cpu.cycles += 16;
}

void instruction_rst_30()
{
    cpu_stack_push(cpu.regs.pc);
    cpu.regs.pc = 0x0030;

    cpu.cycles += 16;
}

void instruction_rst_38()
{
    cpu_stack_push(cpu.regs.pc);
    cpu.regs.pc = 0x0038;

    cpu.cycles += 16;
}

/* 8-bit Load instructions */

void instruction_ld_a_a()
{
    cpu.regs.a = cpu.regs.a;

    cpu.cycles += 4;
}

void instruction_ld_a_b()
{
    cpu.regs.a = cpu.regs.b;

    cpu.cycles += 4;
}

void instruction_ld_a_c()
{
    cpu.regs.a = cpu.regs.c;

    cpu.cycles += 4;
}

void instruction_ld_a_d()
{
    cpu.regs.a = cpu.regs.d;

    cpu.cycles += 4;
}

void instruction_ld_a_e()
{
    cpu.regs.a = cpu.regs.e;

    cpu.cycles += 4;
}

void instruction_ld_a_h()
{
    cpu.regs.a = cpu.regs.h;

    cpu.cycles += 4;
}

void instruction_ld_a_l()
{
    cpu.regs.a = cpu.regs.l;

    cpu.cycles += 4;
}

void instruction_ld_b_a()
{
    cpu.regs.b = cpu.regs.a;

    cpu.cycles += 4;
}

void instruction_ld_b_b()
{
    cpu.regs.b = cpu.regs.b;

    cpu.cycles += 4;
}

void instruction_ld_b_c()
{
    cpu.regs.b = cpu.regs.c;

    cpu.cycles += 4;
}

void instruction_ld_b_d()
{
    cpu.regs.b = cpu.regs.d;

    cpu.cycles += 4;
}

void instruction_ld_b_e()
{
    cpu.regs.b = cpu.regs.e;

    cpu.cycles += 4;
}

void instruction_ld_b_h()
{
    cpu.regs.b = cpu.regs.h;

    cpu.cycles += 4;
}

void instruction_ld_b_l()
{
    cpu.regs.b = cpu.regs.l;

    cpu.cycles += 4;
}

void instruction_ld_c_a()
{
    cpu.regs.c = cpu.regs.a;

    cpu.cycles += 4;
}

void instruction_ld_c_b()
{
    cpu.regs.c = cpu.regs.b;

    cpu.cycles += 4;
}

void instruction_ld_c_c()
{
    cpu.regs.c = cpu.regs.c;

    cpu.cycles += 4;
}

void instruction_ld_c_d()
{
    cpu.regs.c = cpu.regs.d;

    cpu.cycles += 4;
}

void instruction_ld_c_e()
{
    cpu.regs.c = cpu.regs.e;

    cpu.cycles += 4;
}

void instruction_ld_c_h()
{
    cpu.regs.c = cpu.regs.h;

    cpu.cycles += 4;
}

void instruction_ld_c_l()
{
    cpu.regs.c = cpu.regs.l;

    cpu.cycles += 4;
}

void instruction_ld_d_a()
{
    cpu.regs.d = cpu.regs.a;

    cpu.cycles += 4;
}

void instruction_ld_d_b()
{
    cpu.regs.d = cpu.regs.b;

    cpu.cycles += 4;
}

void instruction_ld_d_c()
{
    cpu.regs.d = cpu.regs.c;

    cpu.cycles += 4;
}

void instruction_ld_d_d()
{
    cpu.regs.d = cpu.regs.d;

    cpu.cycles += 4;
}

void instruction_ld_d_e()
{
    cpu.regs.d = cpu.regs.e;

    cpu.cycles += 4;
}

void instruction_ld_d_h()
{
    cpu.regs.d = cpu.regs.h;

    cpu.cycles += 4;
}

void instruction_ld_d_l()
{
    cpu.regs.d = cpu.regs.l;

    cpu.cycles += 4;
}

void instruction_ld_e_a()
{
    cpu.regs.e = cpu.regs.a;

    cpu.cycles += 4;
}

void instruction_ld_e_b()
{
    cpu.regs.e = cpu.regs.b;

    cpu.cycles += 4;
}

void instruction_ld_e_c()
{
    cpu.regs.e = cpu.regs.c;

    cpu.cycles += 4;
}

void instruction_ld_e_d()
{
    cpu.regs.e = cpu.regs.d;

    cpu.cycles += 4;
}

void instruction_ld_e_e()
{
    cpu.regs.e = cpu.regs.e;

    cpu.cycles += 4;
}

void instruction_ld_e_h()
{
    cpu.regs.e = cpu.regs.h;

    cpu.cycles += 4;
}

void instruction_ld_e_l()
{
    cpu.regs.e = cpu.regs.l;

    cpu.cycles += 4;
}

void instruction_ld_h_a()
{
    cpu.regs.h = cpu.regs.a;

    cpu.cycles += 4;
}

void instruction_ld_h_b()
{
    cpu.regs.h = cpu.regs.b;

    cpu.cycles += 4;
}

void instruction_ld_h_c()
{
    cpu.regs.h = cpu.regs.c;

    cpu.cycles += 4;
}

void instruction_ld_h_d()
{
    cpu.regs.h = cpu.regs.d;

    cpu.cycles += 4;
}

void instruction_ld_h_e()
{
    cpu.regs.h = cpu.regs.e;

    cpu.cycles += 4;
}

void instruction_ld_h_h()
{
    cpu.regs.h = cpu.regs.h;

    cpu.cycles += 4;
}

void instruction_ld_h_l()
{
    cpu.regs.h = cpu.regs.l;

    cpu.cycles += 4;
}

void instruction_ld_l_a()
{
    cpu.regs.l = cpu.regs.a;

    cpu.cycles += 4;
}

void instruction_ld_l_b()
{
    cpu.regs.l = cpu.regs.b;

    cpu.cycles += 4;
}

void instruction_ld_l_c()
{
    cpu.regs.l = cpu.regs.c;

    cpu.cycles += 4;
}

void instruction_ld_l_d()
{
    cpu.regs.l = cpu.regs.d;

    cpu.cycles += 4;
}

void instruction_ld_l_e()
{
    cpu.regs.l = cpu.regs.e;

    cpu.cycles += 4;
}

void instruction_ld_l_h()
{
    cpu.regs.l = cpu.regs.h;

    cpu.cycles += 4;
}

void instruction_ld_l_l()
{
    cpu.regs.l = cpu.regs.l;

    cpu.cycles += 4;
}

void instruction_ld_a_n()
{
    cpu.regs.a = mmu_rb(cpu.regs.pc);

    cpu.regs.pc += 1;
    cpu.cycles += 8;
}

void instruction_ld_b_n()
{
    cpu.regs.b = mmu_rb(cpu.regs.pc);

    cpu.regs.pc += 1;
    cpu.cycles += 8;
}

void instruction_ld_c_n()
{
    cpu.regs.c = mmu_rb(cpu.regs.pc);

    cpu.regs.pc += 1;
    cpu.cycles += 8;
}

void instruction_ld_d_n()
{
    cpu.regs.d = mmu_rb(cpu.regs.pc);

    cpu.regs.pc += 1;
    cpu.cycles += 8;
}

void instruction_ld_e_n()
{
    cpu.regs.e = mmu_rb(cpu.regs.pc);

    cpu.regs.pc += 1;
    cpu.cycles += 8;
}

void instruction_ld_h_n()
{
    cpu.regs.h = mmu_rb(cpu.regs.pc);

    cpu.regs.pc += 1;
    cpu.cycles += 8;
}

void instruction_ld_l_n()
{
    cpu.regs.l = mmu_rb(cpu.regs.pc);

    cpu.regs.pc += 1;
    cpu.cycles += 8;
}

void instruction_ld_a_hlp()
{
    cpu.regs.a = mmu_rb(cpu.regs.hl);

    cpu.cycles += 8;
}

void instruction_ld_b_hlp()
{
    cpu.regs.b = mmu_rb(cpu.regs.hl);

    cpu.cycles += 8;
}

void instruction_ld_c_hlp()
{
    cpu.regs.c = mmu_rb(cpu.regs.hl);

    cpu.cycles += 8;
}

void instruction_ld_d_hlp()
{
    cpu.regs.d = mmu_rb(cpu.regs.hl);

    cpu.cycles += 8;
}

void instruction_ld_e_hlp()
{
    cpu.regs.e = mmu_rb(cpu.regs.hl);

    cpu.cycles += 8;
}

void instruction_ld_h_hlp()
{
    cpu.regs.h = mmu_rb(cpu.regs.hl);

    cpu.cycles += 8;
}

void instruction_ld_l_hlp()
{
    cpu.regs.l = mmu_rb(cpu.regs.hl);

    cpu.cycles += 8;
}

void instruction_ld_hlp_a()
{
    mmu_wb(cpu.regs.hl, cpu.regs.a);

    cpu.cycles += 8;
}

void instruction_ld_hlp_b()
{
    mmu_wb(cpu.regs.hl, cpu.regs.b);

    cpu.cycles += 8;
}

void instruction_ld_hlp_c()
{
    mmu_wb(cpu.regs.hl, cpu.regs.c);

    cpu.cycles += 8;
}

void instruction_ld_hlp_d()
{
    mmu_wb(cpu.regs.hl, cpu.regs.d);

    cpu.cycles += 8;
}

void instruction_ld_hlp_e()
{
    mmu_wb(cpu.regs.hl, cpu.regs.e);

    cpu.cycles += 8;
}

void instruction_ld_hlp_h()
{
    mmu_wb(cpu.regs.hl, cpu.regs.h);

    cpu.cycles += 8;
}

void instruction_ld_hlp_l()
{
    mmu_wb(cpu.regs.hl, cpu.regs.l);

    cpu.cycles += 8;
}

void instruction_ld_hlp_n()
{
    mmu_wb(cpu.regs.hl, mmu_rb(cpu.regs.pc));

    cpu.regs.pc += 1;
    cpu.cycles += 12;
}

void instruction_ld_a_bcp()
{
    cpu.regs.a = mmu_rb(cpu.regs.bc);

    cpu.cycles += 8;
}

void instruction_ld_a_dep()
{
    cpu.regs.a = mmu_rb(cpu.regs.de);

    cpu.cycles += 8;
}

void instruction_ld_a_nnp()
{
    cpu.regs.a = mmu_rb(mmu_rw(cpu.regs.pc));

    cpu.regs.pc += 2;
    cpu.cycles += 16;
}

void instruction_ld_bcp_a()
{
    mmu_wb(cpu.regs.bc, cpu.regs.a);

    cpu.cycles += 8;
}

void instruction_ld_dep_a()
{
    mmu_wb(cpu.regs.de, cpu.regs.a);

    cpu.cycles += 8;
}

void instruction_ld_nnp_a()
{
    mmu_wb(mmu_rw(cpu.regs.pc), cpu.regs.a);

    cpu.regs.pc += 2;
    cpu.cycles += 16;
}

void instruction_ld_a_io_n()
{
    cpu.regs.a = mmu_rb(0xFF00 + mmu_rb(cpu.regs.pc));

    cpu.regs.pc += 1;
    cpu.cycles += 12;
}

void instruction_ld_io_n_a()
{
    mmu_wb(0xFF00 + mmu_rb(cpu.regs.pc), cpu.regs.a);

    cpu.regs.pc += 1;
    cpu.cycles += 12;
}

void instruction_ld_a_io_c()
{
    cpu.regs.a = mmu_rb(0xFF00 + cpu.regs.c);

    cpu.cycles += 8;
}

void instruction_ld_io_c_a()
{
    mmu_wb(0xFF00 + cpu.regs.c, cpu.regs.a);

    cpu.cycles += 8;
}

void instruction_ldi_hlp_a()
{
    mmu_wb(cpu.regs.hl, cpu.regs.a);
    cpu.regs.hl++;

    cpu.cycles += 8;
}

void instruction_ldi_a_hlp()
{
    cpu.regs.a = mmu_rb(cpu.regs.hl);
    cpu.regs.hl++;

    cpu.cycles += 8;
}

void instruction_ldd_hlp_a()
{
    mmu_wb(cpu.regs.hl, cpu.regs.a);
    cpu.regs.hl--;

    cpu.cycles += 8;
}

void instruction_ldd_a_hlp()
{
    cpu.regs.a = mmu_rb(cpu.regs.hl);
    cpu.regs.hl--;

    cpu.cycles += 8;
}

void instruction_ld_bc_nn()
{
    cpu.regs.bc = mmu_rw(cpu.regs.pc);
    cpu.regs.pc += 2;

    cpu.cycles += 12;
}

void instruction_ld_de_nn()
{
    cpu.regs.de = mmu_rw(cpu.regs.pc);
    cpu.regs.pc += 2;

    cpu.cycles += 12;
}

void instruction_ld_hl_nn()
{
    cpu.regs.hl = mmu_rw(cpu.regs.pc);
    cpu.regs.pc += 2;

    cpu.cycles += 12;
}

void instruction_ld_sp_nn()
{
    cpu.regs.sp = mmu_rw(cpu.regs.pc);
    cpu.regs.pc += 2;

    cpu.cycles += 12;
}

void instruction_ld_nnp_sp()
{
    mmu_ww(mmu_rw(cpu.regs.pc), cpu.regs.sp);
    cpu.regs.pc += 2;

    cpu.cycles += 20;
}

void instruction_ld_sp_hl()
{
    cpu.regs.sp = cpu.regs.hl;

    cpu.cycles += 8;
}

void instruction_push_af()
{
    cpu_stack_push(cpu.regs.af);

    cpu.cycles += 16;
}

void instruction_push_bc()
{
    cpu_stack_push(cpu.regs.bc);

    cpu.cycles += 16;
}

void instruction_push_de()
{
    cpu_stack_push(cpu.regs.de);

    cpu.cycles += 16;
}

void instruction_push_hl()
{
    cpu_stack_push(cpu.regs.hl);

    cpu.cycles += 16;
}

void instruction_pop_af()
{
    cpu.regs.af = cpu_stack_pop() & 0xFFF0;

    cpu.cycles += 12;
}

void instruction_pop_bc()
{
    cpu.regs.bc = cpu_stack_pop();

    cpu.cycles += 12;
}

void instruction_pop_de()
{
    cpu.regs.de = cpu_stack_pop();

    cpu.cycles += 12;
}

void instruction_pop_hl()
{
    cpu.regs.hl = cpu_stack_pop();

    cpu.cycles += 12;
}

void instruction_add_a_a()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += cpu.regs.a;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (((cpu.regs.a & 0x0F) + (cpu.regs.a & 0x0F)) > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_add_a_b()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += cpu.regs.b;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_add_a_c()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += cpu.regs.c;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_add_a_d()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += cpu.regs.d;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_add_a_e()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += cpu.regs.e;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_add_a_h()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += cpu.regs.h;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_add_a_l()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += cpu.regs.l;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_add_a_n()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += mmu_rb(cpu.regs.pc);

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.regs.pc += 1;
    cpu.cycles += 8;
}

void instruction_add_a_hlp()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += mmu_rb(cpu.regs.hl);

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 8;
}

/*
void instruction_adc(uint8_t value)
{
    if (CHECK_FLAG(FLAG_CARRY)) value++;
    if (value == cpu.regs.a) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a ) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);
    cpu.regs.a += cpu.regs.a;
}
*/

void instruction_adc_a_a()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += cpu.regs.a;

    if (CHECK_FLAG(FLAG_CARRY)) cpu.regs.a++;
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_adc_a_b()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += cpu.regs.b;

    if (CHECK_FLAG(FLAG_CARRY)) cpu.regs.a++;
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_adc_a_c()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += cpu.regs.c;

    if (CHECK_FLAG(FLAG_CARRY)) cpu.regs.a++;
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_adc_a_d()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += cpu.regs.d;

    if (CHECK_FLAG(FLAG_CARRY)) cpu.regs.a++;
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_adc_a_e()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += cpu.regs.e;

    if (CHECK_FLAG(FLAG_CARRY)) cpu.regs.a++;
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_adc_a_h()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += cpu.regs.h;

    if (CHECK_FLAG(FLAG_CARRY)) cpu.regs.a++;
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_adc_a_l()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += cpu.regs.l;

    if (CHECK_FLAG(FLAG_CARRY)) cpu.regs.a++;
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_adc_a_n()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += mmu_rb(cpu.regs.pc);

    if (CHECK_FLAG(FLAG_CARRY)) cpu.regs.a++;
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.regs.pc += 1;
    cpu.cycles += 8;
}

void instruction_adc_a_hlp()
{
    uint8_t tmp = cpu.regs.a;
    cpu.regs.a += mmu_rb(cpu.regs.hl);

    if (CHECK_FLAG(FLAG_CARRY)) cpu.regs.a++;
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (cpu.regs.a > 0x0F) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < tmp) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 8;
}

void instruction_sub(uint8_t value)
{    
    SET_FLAG(FLAG_SUBTRACTION);
    if ((value & 0x0F) > (cpu.regs.a & 0x0F)) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (cpu.regs.a < value) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.regs.a -= value;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
}

void instruction_sub_a_a()
{
    instruction_sub(cpu.regs.a);

    cpu.cycles += 4;
}

void instruction_sub_a_b()
{
    instruction_sub(cpu.regs.b);

    cpu.cycles += 4;
}

void instruction_sub_a_c()
{
    instruction_sub(cpu.regs.c);

    cpu.cycles += 4;
}

void instruction_sub_a_d()
{
    instruction_sub(cpu.regs.d);

    cpu.cycles += 4;
}

void instruction_sub_a_e()
{
    instruction_sub(cpu.regs.e);

    cpu.cycles += 4;
}

void instruction_sub_a_h()
{
    instruction_sub(cpu.regs.h);

    cpu.cycles += 4;
}

void instruction_sub_a_l()
{
    instruction_sub(cpu.regs.l);

    cpu.cycles += 4;
}

void instruction_sub_a_n()
{
    instruction_sub(mmu_rb(cpu.regs.pc));

    cpu.regs.pc += 1;
    cpu.cycles += 8;
}

void instruction_sub_a_hlp()
{
    instruction_sub(mmu_rb(cpu.regs.hl));

    cpu.cycles += 8;
}

void instruction_sbc(uint8_t value)
{
    if (CHECK_FLAG(FLAG_CARRY)) value++;

    if (cpu.regs.a == value) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    SET_FLAG(FLAG_SUBTRACTION);
    if ((value & 0x0F) > (cpu.regs.a & 0x0F)) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (value > cpu.regs.a) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.regs.a -= value;
}

void instruction_sbc_a_a()
{
    instruction_sbc(cpu.regs.a);

    cpu.cycles += 4;
}

void instruction_sbc_a_b()
{
    instruction_sbc(cpu.regs.b);

    cpu.cycles += 4;
}

void instruction_sbc_a_c()
{
    instruction_sbc(cpu.regs.c);

    cpu.cycles += 4;
}

void instruction_sbc_a_d()
{
    instruction_sbc(cpu.regs.d);

    cpu.cycles += 4;
}

void instruction_sbc_a_e()
{
    instruction_sbc(cpu.regs.e);

    cpu.cycles += 4;
}

void instruction_sbc_a_h()
{
    instruction_sbc(cpu.regs.h);

    cpu.cycles += 4;
}

void instruction_sbc_a_l()
{
    instruction_sbc(cpu.regs.l);

    cpu.cycles += 4;
}

void instruction_sbc_a_n()
{
    instruction_sbc(mmu_rb(cpu.regs.pc));

    cpu.regs.pc += 1;
    cpu.cycles += 8;
}

void instruction_sbc_a_hlp()
{
    instruction_sbc(mmu_rb(cpu.regs.hl));

    cpu.cycles += 8;
}

void instruction_and_a()
{
    cpu.regs.a &= cpu.regs.a;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_and_b()
{
    cpu.regs.a &= cpu.regs.b;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_and_c()
{
    cpu.regs.a &= cpu.regs.c;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_and_d()
{
    cpu.regs.a &= cpu.regs.d;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_and_e()
{
    cpu.regs.a &= cpu.regs.e;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_and_h()
{
    cpu.regs.a &= cpu.regs.h;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_and_l()
{
    cpu.regs.a &= cpu.regs.l;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_and_n()
{
    cpu.regs.a &= mmu_rb(cpu.regs.pc);

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 8;
    cpu.regs.pc += 1;
}

void instruction_and_hlp()
{
    cpu.regs.a &= mmu_rb(cpu.regs.hl);

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 8;
}

void instruction_xor_a()
{
    cpu.regs.a ^= cpu.regs.a;
    
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_xor_b()
{
    cpu.regs.a ^= cpu.regs.b;
    
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_xor_c()
{
    cpu.regs.a ^= cpu.regs.c;
    
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_xor_d()
{
    cpu.regs.a ^= cpu.regs.d;
    
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_xor_e()
{
    cpu.regs.a ^= cpu.regs.e;
    
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_xor_h()
{
    cpu.regs.a ^= cpu.regs.h;
    
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_xor_l()
{
    cpu.regs.a ^= cpu.regs.l;
    
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_xor_n()
{
    cpu.regs.a ^= mmu_rb(cpu.regs.pc);
    
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 8;
    cpu.regs.pc += 1;
}

void instruction_xor_hlp()
{
    cpu.regs.a ^= mmu_rb(cpu.regs.hl);
    
    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 8;
}

void instruction_or_a()
{
    cpu.regs.a |= cpu.regs.a;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_or_b()
{
    cpu.regs.a |= cpu.regs.b;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_or_c()
{
    cpu.regs.a |= cpu.regs.c;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_or_d()
{
    cpu.regs.a |= cpu.regs.d;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_or_e()
{
    cpu.regs.a |= cpu.regs.e;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_or_h()
{
    cpu.regs.a |= cpu.regs.h;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_or_l()
{
    cpu.regs.a |= cpu.regs.l;

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 4;
}

void instruction_or_n()
{
    cpu.regs.a |= mmu_rb(cpu.regs.pc);

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 8;
    cpu.regs.pc += 1;
}

void instruction_or_hlp()
{
    cpu.regs.a |= mmu_rb(cpu.regs.hl);

    if (cpu.regs.a == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    CLEAR_FLAG(FLAG_CARRY);

    cpu.cycles += 8;
}

void instruction_cp(uint8_t value)
{
    uint8_t result = cpu.regs.a - value;

    if (result == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    SET_FLAG(FLAG_SUBTRACTION);
    if (((cpu.regs.a & 0x0F) - (value & 0x0F)) < 0) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);
    if (cpu.regs.a < value) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);
}

void instruction_cp_a()
{
    instruction_cp(cpu.regs.a); 

    cpu.cycles += 4;
}

void instruction_cp_b()
{
    instruction_cp(cpu.regs.b);

    cpu.cycles += 4;
}

void instruction_cp_c()
{
    instruction_cp(cpu.regs.c);

    cpu.cycles += 4;
}

void instruction_cp_d()
{
    instruction_cp(cpu.regs.d);

    cpu.cycles += 4;
}

void instruction_cp_e()
{
    instruction_cp(cpu.regs.e);

    cpu.cycles += 4;
}

void instruction_cp_h()
{
    instruction_cp(cpu.regs.h);

    cpu.cycles += 4;
}

void instruction_cp_l()
{
    instruction_cp(cpu.regs.l);

    cpu.cycles += 4;
}

void instruction_cp_n()
{
    instruction_cp(mmu_rb(cpu.regs.pc)); 

    cpu.cycles += 8;
    cpu.regs.pc += 1; 
}

void instruction_cp_hlp()
{
    instruction_cp(mmu_rb(cpu.regs.hl)); 

    cpu.cycles += 8;
}

uint8_t instruction_inc(uint8_t value)
{
    uint8_t result = value + 1;

    if (result == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if ((result & 0x0F) == 0x00) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);

    return result;
}

uint8_t instruction_dec(uint8_t value)
{
    uint8_t result = value - 1;

    if (result == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if ((result & 0x0F) == 0x00) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);

    return result;
}

void instruction_inc_a()
{
    cpu.regs.a = instruction_inc(cpu.regs.a);

    cpu.cycles += 4;
}

void instruction_inc_b()
{
    cpu.regs.b = instruction_inc(cpu.regs.b);

    cpu.cycles += 4;
}

void instruction_inc_c()
{
    cpu.regs.c = instruction_inc(cpu.regs.c);

    cpu.cycles += 4;
}

void instruction_inc_d()
{
    cpu.regs.d = instruction_inc(cpu.regs.d);

    cpu.cycles += 4;
}

void instruction_inc_e()
{
    cpu.regs.e = instruction_inc(cpu.regs.e);

    cpu.cycles += 4;
}

void instruction_inc_h()
{
    cpu.regs.h = instruction_inc(cpu.regs.h);

    cpu.cycles += 4;
}

void instruction_inc_l()
{
    cpu.regs.l = instruction_inc(cpu.regs.l);

    cpu.cycles += 4;
}

void instruction_inc_hlp()
{
    mmu_wb(cpu.regs.hl, instruction_inc(mmu_rb(cpu.regs.hl)));

    cpu.cycles += 12;
}

void instruction_dec_a()
{
    cpu.regs.a = instruction_dec(cpu.regs.a);

    cpu.cycles += 4;
}

void instruction_dec_b()
{
    cpu.regs.b = instruction_dec(cpu.regs.b);

    cpu.cycles += 4;
}

void instruction_dec_c()
{
    cpu.regs.c = instruction_dec(cpu.regs.c);

    cpu.cycles += 4;
}

void instruction_dec_d()
{
    cpu.regs.d = instruction_dec(cpu.regs.d);

    cpu.cycles += 4;
}

void instruction_dec_e()
{
    cpu.regs.e = instruction_dec(cpu.regs.e);

    cpu.cycles += 4;
}

void instruction_dec_h()
{
    cpu.regs.h = instruction_dec(cpu.regs.h);

    cpu.cycles += 4;
}

void instruction_dec_l()
{
    cpu.regs.l = instruction_dec(cpu.regs.l);

    cpu.cycles += 4;
}

void instruction_dec_hlp()
{
    mmu_wb(cpu.regs.hl, instruction_dec(mmu_rb(cpu.regs.hl)));

    cpu.cycles += 12;
}

void instruction_daa()
{
    /*
    uint8_t result = cpu.regs.a;
    if (CHECK_FLAG(FLAG_SUBTRACTION)) {
        if (CHECK_FLAG(FLAG_HALFCARRY)) {
            result -= 0x06;
        }
    } else {
        if (CHECK_FLAG(FLAG_HALFCARRY)) {
            result += 0x06;
        }
    }
    if (result == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_HALFCARRY);
    if (result > 0x99) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);
    cpu.regs.a = result;
    cpu.cycles += 4;
    */

    uint8_t reg = cpu.regs.a;

    uint16_t correction = CHECK_FLAG(FLAG_CARRY) ? 0x60 : 0x00;

    if (CHECK_FLAG(FLAG_HALFCARRY) || (!CHECK_FLAG(FLAG_SUBTRACTION) && ((reg & 0x0F) > 9))) {
        correction |= 0x06;
    }

    if (CHECK_FLAG(FLAG_CARRY) || (!CHECK_FLAG(FLAG_SUBTRACTION) && (reg > 0x99))) {
        correction |= 0x60;
    }

    if (CHECK_FLAG(FLAG_SUBTRACTION)) {
        reg = (uint8_t) (reg - correction);
    } else {
        reg = (uint8_t) (reg + correction);
    }

    if (((correction << 2) & 0x100) != 0) {
        SET_FLAG(FLAG_CARRY);
    }

    CLEAR_FLAG(FLAG_HALFCARRY);
    if (reg == 0) SET_FLAG(FLAG_ZERO); else CLEAR_FLAG(FLAG_ZERO);

    cpu.regs.a = (uint8_t) reg;

    cpu.cycles += 4;
}

void instruction_cpl()
{
    cpu.regs.a = ~cpu.regs.a;

    SET_FLAG(FLAG_SUBTRACTION);
    SET_FLAG(FLAG_HALFCARRY);

    cpu.cycles += 4;
}

void instruction_add_hl_bc()
{
    uint16_t hl = cpu.regs.hl;
    uint32_t result = hl + cpu.regs.bc;

    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (((hl & 0x0F) + (cpu.regs.bc & 0x0F) > 0x0F)) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (result & 0xFFFF0000) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.regs.hl = (uint16_t) result;

    cpu.cycles += 8;
}

void instruction_add_hl_de()
{
    uint16_t hl = cpu.regs.hl;
    uint32_t result = hl + cpu.regs.de;

    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (((hl & 0x0F) + (cpu.regs.de & 0x0F) > 0x0F)) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (result & 0xFFFF0000) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.regs.hl = (uint16_t) result;

    cpu.cycles += 8;
}

void instruction_add_hl_hl()
{
    uint16_t hl = cpu.regs.hl;
    uint32_t result = hl + hl;

    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (((hl & 0x0F) + (cpu.regs.hl & 0x0F) > 0x0F)) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (result & 0xFFFF0000) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.regs.hl = (uint16_t) result;

    cpu.cycles += 8;
}

void instruction_add_hl_sp()
{
    uint16_t hl = cpu.regs.hl;
    uint32_t result = hl + cpu.regs.sp;

    CLEAR_FLAG(FLAG_SUBTRACTION);
    if (((hl & 0x0F) + (cpu.regs.sp & 0x0F) > 0x0F)) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (result & 0xFFFF0000) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.regs.hl = (uint16_t) result;

    cpu.cycles += 8;
}

void instruction_inc_bc()
{
    cpu.regs.bc++;

    cpu.cycles += 8;
}

void instruction_inc_de()
{
    cpu.regs.de++;

    cpu.cycles += 8;
}

void instruction_inc_hl()
{
    cpu.regs.hl++;

    cpu.cycles += 8;
}

void instruction_inc_sp()
{
    cpu.regs.sp++;

    cpu.cycles += 8;
}

void instruction_dec_bc()
{
    cpu.regs.bc--;

    cpu.cycles += 8;
}

void instruction_dec_de()
{
    cpu.regs.de--;

    cpu.cycles += 8;
}

void instruction_dec_hl()
{
    cpu.regs.hl--;

    cpu.cycles += 8;
}

void instruction_dec_sp()
{
    cpu.regs.sp--;

    cpu.cycles += 8;
}

/* Check flags */
void instruction_add_sp_dd()
{
    uint16_t tmp = cpu.regs.sp;
    int32_t result = (int32_t) (tmp + (int8_t) mmu_rb(cpu.regs.pc));

    CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if ((result & 0x00FF) == 0x00FF) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (result & 0xFFFF0000) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.regs.sp = (uint16_t) result;

    cpu.cycles += 16;
    cpu.regs.pc += 1; 
}

void instruction_ld_hl_sp_dd()
{
    int32_t result = (int32_t) (cpu.regs.sp + (int8_t) mmu_rb(cpu.regs.pc));

    CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    if ((result & 0x00FF) == 0x00FF) SET_FLAG(FLAG_HALFCARRY); else CLEAR_FLAG(FLAG_HALFCARRY);
    if (result & 0xFFFF0000) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.regs.hl = (uint16_t) result;

    cpu.cycles += 12;
    cpu.regs.pc += 1;
}

void instruction_rlca()
{
    cpu.cycles += 4;

    cpu.regs.a = instruction_cb_rlc(cpu.regs.a);
}

void instruction_rla()
{
    uint8_t carry = cpu.regs.a & (1 << 7);
    uint8_t result = (cpu.regs.a << 1) | CHECK_FLAG(FLAG_CARRY);

    CLEAR_FLAG(FLAG_ZERO);
    CLEAR_FLAG(FLAG_SUBTRACTION);
    CLEAR_FLAG(FLAG_HALFCARRY);
    if (carry) SET_FLAG(FLAG_CARRY); else CLEAR_FLAG(FLAG_CARRY);

    cpu.regs.a = result;

    cpu.cycles += 4;
}

void instruction_rra()
{
    cpu.cycles += 4;

    cpu.regs.a = instruction_cb_rr(cpu.regs.a);
}

const void (*cb_instruction_pointers[256])(void) = {
    /* Swap */
    [0x37] = &instruction_cb_swap_a,
    [0x30] = &instruction_cb_swap_b,
    [0x31] = &instruction_cb_swap_c,
    [0x32] = &instruction_cb_swap_d,
    [0x33] = &instruction_cb_swap_e,
    [0x34] = &instruction_cb_swap_h,
    [0x35] = &instruction_cb_swap_l,
    [0x36] = &instruction_cb_swap_hlp,

    /* RES */
    [0x87] = &instruction_cb_res_0_a,
    [0x8F] = &instruction_cb_res_1_a,
    [0x97] = &instruction_cb_res_2_a,
    [0x9F] = &instruction_cb_res_3_a,
    [0xA7] = &instruction_cb_res_4_a,
    [0xAF] = &instruction_cb_res_5_a,
    [0xB7] = &instruction_cb_res_6_a,
    [0xBF] = &instruction_cb_res_7_a,

    [0x86] = &instruction_cb_res_0_hlp,
    [0x8E] = &instruction_cb_res_1_hlp,
    [0x96] = &instruction_cb_res_2_hlp,
    [0x9E] = &instruction_cb_res_3_hlp,
    [0xA6] = &instruction_cb_res_4_hlp,
    [0xAE] = &instruction_cb_res_5_hlp,
    [0xB6] = &instruction_cb_res_6_hlp,
    [0xBE] = &instruction_cb_res_7_hlp,

    /* SET */
    [0xC7] = &instruction_cb_set_0_a,
    [0xCF] = &instruction_cb_set_1_a,
    [0xD7] = &instruction_cb_set_2_a,
    [0xDF] = &instruction_cb_set_3_a,
    [0xE7] = &instruction_cb_set_4_a,
    [0xEF] = &instruction_cb_set_5_a,
    [0xF7] = &instruction_cb_set_6_a,
    [0xFF] = &instruction_cb_set_7_a,

    [0xC0] = &instruction_cb_set_0_b,
    [0xC8] = &instruction_cb_set_1_b,
    [0xD0] = &instruction_cb_set_2_b,
    [0xD8] = &instruction_cb_set_3_b,
    [0xE0] = &instruction_cb_set_4_b,
    [0xE8] = &instruction_cb_set_5_b,
    [0xF0] = &instruction_cb_set_6_b,
    [0xF8] = &instruction_cb_set_7_b,

    [0xC1] = &instruction_cb_set_0_c,
    [0xC9] = &instruction_cb_set_1_c,
    [0xD1] = &instruction_cb_set_2_c,
    [0xD9] = &instruction_cb_set_3_c,
    [0xE1] = &instruction_cb_set_4_c,
    [0xE9] = &instruction_cb_set_5_c,
    [0xF1] = &instruction_cb_set_6_c,
    [0xF9] = &instruction_cb_set_7_c,

    [0xC2] = &instruction_cb_set_0_d,
    [0xCA] = &instruction_cb_set_1_d,
    [0xD2] = &instruction_cb_set_2_d,
    [0xDA] = &instruction_cb_set_3_d,
    [0xE2] = &instruction_cb_set_4_d,
    [0xEA] = &instruction_cb_set_5_d,
    [0xF2] = &instruction_cb_set_6_d,
    [0xFA] = &instruction_cb_set_7_d,

    [0xC3] = &instruction_cb_set_0_e,
    [0xCB] = &instruction_cb_set_1_e,
    [0xD3] = &instruction_cb_set_2_e,
    [0xDB] = &instruction_cb_set_3_e,
    [0xE3] = &instruction_cb_set_4_e,
    [0xEB] = &instruction_cb_set_5_e,
    [0xF3] = &instruction_cb_set_6_e,
    [0xFB] = &instruction_cb_set_7_e,

    [0xC4] = &instruction_cb_set_0_h,
    [0xCC] = &instruction_cb_set_1_h,
    [0xD4] = &instruction_cb_set_2_h,
    [0xDC] = &instruction_cb_set_3_h,
    [0xE4] = &instruction_cb_set_4_h,
    [0xEC] = &instruction_cb_set_5_h,
    [0xF4] = &instruction_cb_set_6_h,
    [0xFC] = &instruction_cb_set_7_h,

    [0xC5] = &instruction_cb_set_0_l,
    [0xCD] = &instruction_cb_set_1_l,
    [0xD5] = &instruction_cb_set_2_l,
    [0xDD] = &instruction_cb_set_3_l,
    [0xE5] = &instruction_cb_set_4_l,
    [0xED] = &instruction_cb_set_5_l,
    [0xF5] = &instruction_cb_set_6_l,
    [0xFD] = &instruction_cb_set_7_l,

    [0xC6] = &instruction_cb_set_0_hlp,
    [0xCE] = &instruction_cb_set_1_hlp,
    [0xD6] = &instruction_cb_set_2_hlp,
    [0xDE] = &instruction_cb_set_3_hlp,
    [0xE6] = &instruction_cb_set_4_hlp,
    [0xEE] = &instruction_cb_set_5_hlp,
    [0xF6] = &instruction_cb_set_6_hlp,
    [0xFE] = &instruction_cb_set_7_hlp,

    /* BIT */
    [0x47] = &instruction_cb_bit_0_a,
    [0x4F] = &instruction_cb_bit_1_a,
    [0x57] = &instruction_cb_bit_2_a,
    [0x5F] = &instruction_cb_bit_3_a,
    [0x67] = &instruction_cb_bit_4_a,
    [0x6F] = &instruction_cb_bit_5_a,
    [0x77] = &instruction_cb_bit_6_a,
    [0x7F] = &instruction_cb_bit_7_a,

    [0x40] = &instruction_cb_bit_0_b,
    [0x48] = &instruction_cb_bit_1_b,
    [0x50] = &instruction_cb_bit_2_b,
    [0x58] = &instruction_cb_bit_3_b,
    [0x60] = &instruction_cb_bit_4_b,
    [0x68] = &instruction_cb_bit_5_b,
    [0x70] = &instruction_cb_bit_6_b,
    [0x78] = &instruction_cb_bit_7_b,

    [0x41] = &instruction_cb_bit_0_c,
    [0x49] = &instruction_cb_bit_1_c,
    [0x51] = &instruction_cb_bit_2_c,
    [0x59] = &instruction_cb_bit_3_c,
    [0x61] = &instruction_cb_bit_4_c,
    [0x69] = &instruction_cb_bit_5_c,
    [0x71] = &instruction_cb_bit_6_c,
    [0x79] = &instruction_cb_bit_7_c,

    [0x42] = &instruction_cb_bit_0_d,
    [0x4A] = &instruction_cb_bit_1_d,
    [0x52] = &instruction_cb_bit_2_d,
    [0x5A] = &instruction_cb_bit_3_d,
    [0x62] = &instruction_cb_bit_4_d,
    [0x6A] = &instruction_cb_bit_5_d,
    [0x72] = &instruction_cb_bit_6_d,
    [0x7A] = &instruction_cb_bit_7_d,

    [0x43] = &instruction_cb_bit_0_e,
    [0x4B] = &instruction_cb_bit_1_e,
    [0x53] = &instruction_cb_bit_2_e,
    [0x5B] = &instruction_cb_bit_3_e,
    [0x63] = &instruction_cb_bit_4_e,
    [0x6B] = &instruction_cb_bit_5_e,
    [0x73] = &instruction_cb_bit_6_e,
    [0x7B] = &instruction_cb_bit_7_e,

    [0x44] = &instruction_cb_bit_0_h,
    [0x4C] = &instruction_cb_bit_1_h,
    [0x54] = &instruction_cb_bit_2_h,
    [0x5C] = &instruction_cb_bit_3_h,
    [0x64] = &instruction_cb_bit_4_h,
    [0x6C] = &instruction_cb_bit_5_h,
    [0x74] = &instruction_cb_bit_6_h,
    [0x7C] = &instruction_cb_bit_7_h,

    [0x45] = &instruction_cb_bit_0_l,
    [0x4D] = &instruction_cb_bit_1_l,
    [0x55] = &instruction_cb_bit_2_l,
    [0x5D] = &instruction_cb_bit_3_l,
    [0x65] = &instruction_cb_bit_4_l,
    [0x6D] = &instruction_cb_bit_5_l,
    [0x75] = &instruction_cb_bit_6_l,
    [0x7D] = &instruction_cb_bit_7_l,
 
    [0x46] = &instruction_cb_bit_0_hlp,
    [0x4E] = &instruction_cb_bit_1_hlp,
    [0x56] = &instruction_cb_bit_2_hlp,
    [0x5E] = &instruction_cb_bit_3_hlp,
    [0x66] = &instruction_cb_bit_4_hlp,
    [0x6E] = &instruction_cb_bit_5_hlp,
    [0x76] = &instruction_cb_bit_6_hlp,
    [0x7E] = &instruction_cb_bit_7_hlp,

    [0x2F] = &instruction_cb_sra_a,
    [0x28] = &instruction_cb_sra_h,
    [0x29] = &instruction_cb_sra_c,
    [0x2A] = &instruction_cb_sra_d,
    [0x2B] = &instruction_cb_sra_e,
    [0x2C] = &instruction_cb_sra_h,
    [0x2D] = &instruction_cb_sra_l,
    [0x2E] = &instruction_cb_sra_hlp,

    [0x3F] = &instruction_cb_srl_a,
    [0x38] = &instruction_cb_srl_h,
    [0x39] = &instruction_cb_srl_c,
    [0x3A] = &instruction_cb_srl_d,
    [0x3B] = &instruction_cb_srl_e,
    [0x3C] = &instruction_cb_srl_h,
    [0x3D] = &instruction_cb_srl_l,
    [0x3E] = &instruction_cb_srl_hlp,

    /* RL */
    [0x17] = &instruction_cb_rl_a,
    [0x10] = &instruction_cb_rl_h,
    [0x11] = &instruction_cb_rl_c,
    [0x12] = &instruction_cb_rl_d,
    [0x13] = &instruction_cb_rl_e,
    [0x14] = &instruction_cb_rl_h,
    [0x15] = &instruction_cb_rl_l,
    [0x16] = &instruction_cb_rl_hlp,

    /* RR */
    [0x1F] = &instruction_cb_rr_a,
    [0x18] = &instruction_cb_rr_h,
    [0x19] = &instruction_cb_rr_c,
    [0x1A] = &instruction_cb_rr_d,
    [0x1B] = &instruction_cb_rr_e,
    [0x1C] = &instruction_cb_rr_h,
    [0x1D] = &instruction_cb_rr_l,
    [0x1E] = &instruction_cb_rr_hlp,

    /* RLC */
    [0x07] = &instruction_cb_rlc_a,
    [0x00] = &instruction_cb_rlc_h,
    [0x01] = &instruction_cb_rlc_c,
    [0x02] = &instruction_cb_rlc_d,
    [0x03] = &instruction_cb_rlc_e,
    [0x04] = &instruction_cb_rlc_h,
    [0x05] = &instruction_cb_rlc_l,
    [0x06] = &instruction_cb_rlc_hlp,

    /* SLA */
    [0x27] = &instruction_cb_sla_a,
    [0x20] = &instruction_cb_sla_h,
    [0x21] = &instruction_cb_sla_c,
    [0x22] = &instruction_cb_sla_d,
    [0x23] = &instruction_cb_sla_e,
    [0x24] = &instruction_cb_sla_h,
    [0x25] = &instruction_cb_sla_l,
    [0x26] = &instruction_cb_sla_hlp
};

const char* instruction_labels[256] = {
    /* Control */
    [0x3F] = "CCF",
    [0x37] = "SCF",
    [0x00] = "NOP",
    [0x76] = "HALT",
    [0x10] = "STOP",
    [0xF3] = "DI",
    [0xFB] = "EI",

    /* Jump */
    [0xC3] = "JP nn",
    [0xE9] = "JP HL",
    [0xC2] = "JP NZ,nn",
    [0xCA] = "JP Z,nn",
    [0xD2] = "JP NC,nn",
    [0xDA] = "JP C,nn",
    [0x18] = "JR PC+dd",
    [0x20] = "JR NZ,PC+dd",
    [0x28] = "JR Z,PC+dd",
    [0x30] = "JR NC,PC+dd",
    [0x38] = "JR C,PC+dd",
    [0xCD] = "CALL nn",
    [0xC4] = "CALL NZ,nn",
    [0xCC] = "CALL Z,nn",
    [0xD4] = "CALL NC,nn",
    [0xDC] = "CALL C,nn",
    [0xC9] = "RET",
    [0xC0] = "RET NZ",
    [0xC8] = "RET Z",
    [0xD0] = "RET NC",
    [0xD8] = "RET C",
    [0xD9] = "RETI",
    [0xC7] = "RST 00",
    [0xCF] = "RST 08",
    [0xD7] = "RST 10",
    [0xDF] = "RST 18",
    [0xE7] = "RST 20",
    [0xEF] = "RST 28",
    [0xF7] = "RST 30",
    [0xFF] = "RST 38",

    /* 8-bit Load instructions */
    [0x7F] = "LD A,A",
    [0x78] = "LD A,B",
    [0x79] = "LD A,C",
    [0x7A] = "LD A,D",
    [0x7B] = "LD A,E",
    [0x7C] = "LD A,H",
    [0x7D] = "LD A,L",
    [0x47] = "LD B,A",
    [0x40] = "LD B,B",
    [0x41] = "LD B,C",
    [0x42] = "LD B,D",
    [0x43] = "LD B,E",
    [0x44] = "LD B,H",
    [0x45] = "LD B,L",
    [0x4F] = "LD C,A",
    [0x48] = "LD C,B",
    [0x49] = "LD C,C",
    [0x4A] = "LD C,D",
    [0x4B] = "LD C,E",
    [0x4C] = "LD C,H",
    [0x4D] = "LD C,L",
    [0x57] = "LD D,A",
    [0x50] = "LD D,B",
    [0x51] = "LD D,C",
    [0x52] = "LD D,D",
    [0x53] = "LD D,E",
    [0x54] = "LD D,H",
    [0x55] = "LD D,L",
    [0x5F] = "LD E,A",
    [0x58] = "LD E,B",
    [0x59] = "LD E,C",
    [0x5A] = "LD E,D",
    [0x5B] = "LD E,E",
    [0x5C] = "LD E,H",
    [0x5D] = "LD E,L",
    [0x67] = "LD H,A",
    [0x60] = "LD H,B",
    [0x61] = "LD H,C",
    [0x62] = "LD H,D",
    [0x63] = "LD H,E",
    [0x64] = "LD H,H",
    [0x65] = "LD H,L",
    [0x6F] = "LD L,A",
    [0x68] = "LD L,B",
    [0x69] = "LD L,C",
    [0x6A] = "LD L,D",
    [0x6B] = "LD L,E",
    [0x6C] = "LD L,H",
    [0x6D] = "LD L,L",
    [0x3E] = "LD A,n",
    [0x06] = "LD B,n",
    [0x0E] = "LD C,n",
    [0x16] = "LD D,n",
    [0x1E] = "LD E,n",
    [0x26] = "LD H,n",
    [0x2E] = "LD L,n",
    [0x7E] = "LD A,(HL)",
    [0x46] = "LD B,(HL)",
    [0x4E] = "LD C,(HL)",
    [0x56] = "LD D,(HL)",
    [0x5E] = "LD E,(HL)",
    [0x66] = "LD H,(HL)",
    [0x6E] = "LD L,(HL)",
    [0x77] = "LD (HL),A",
    [0x70] = "LD (HL),B",
    [0x71] = "LD (HL),C",
    [0x72] = "LD (HL),D",
    [0x73] = "LD (HL),E",
    [0x74] = "LD (HL),H",
    [0x75] = "LD (HL),L",
    [0x36] = "LD (HL),n",
    [0x0A] = "LD A,(BC)",
    [0x1A] = "LD A,(DE)",
    [0xFA] = "LD A,(nn)",
    [0x02] = "LD (BC),A",
    [0x12] = "LD (DE),A",
    [0xEA] = "LD (nn),A",
    [0xF0] = "LD A,(FF00+n)",
    [0xE0] = "LD (FF00+n),A",
    [0xF2] = "LD A,(FF00+C)",
    [0xE2] = "LD (FF00+C),A",
    [0x22] = "LDI (HL),A",
    [0x2A] = "LDI A,(HL)",
    [0x32] = "LDD (HL),A",
    [0x3A] = "LDD A,(HL)",

    /* 16-bit Load instructions */
    [0x01] = "LD BC,nn",
    [0x11] = "LD DE,nn",
    [0x21] = "LD HL,nn",
    [0x31] = "LD SP,nn",
    [0x08] = "LD (nn),SP",
    [0xF9] = "LD SP,HL",
    [0xF5] = "PUSH AF",
    [0xC5] = "PUSH BC",
    [0xD5] = "PUSH DE",
    [0xE5] = "PUSH HL",
    [0xF1] = "POP AF",
    [0xC1] = "POP BC",
    [0xD1] = "POP DE",
    [0xE1] = "POP HL",

    /* 8-bit Arithmetic/Logic instructions */
    [0x87] = "ADD A,A",
    [0x80] = "ADD A,B",
    [0x81] = "ADD A,C",
    [0x82] = "ADD A,D",
    [0x83] = "ADD A,E",
    [0x84] = "ADD A,H",
    [0x85] = "ADD A,L",
    [0xC6] = "ADD A,n",
    [0x86] = "ADD A,(HL)",
    [0x8F] = "ADC A,A",
    [0x88] = "ADC A,B",
    [0x89] = "ADC A,C",
    [0x8A] = "ADC A,D",
    [0x8B] = "ADC A,E",
    [0x8C] = "ADC A,H",
    [0x8D] = "ADC A,L",
    [0xCE] = "ADC A,n",
    [0x8E] = "ADC A,(HL)",
    [0x97] = "SUB A,A",
    [0x90] = "SUB A,B",
    [0x91] = "SUB A,C",
    [0x92] = "SUB A,D",
    [0x93] = "SUB A,E",
    [0x94] = "SUB A,H",
    [0x95] = "SUB A,L",
    [0xD6] = "SUB A,n",
    [0x96] = "SUB A,(HL)",
    [0x9F] = "SBC A,A",
    [0x98] = "SBC A,B",
    [0x99] = "SBC A,C",
    [0x9A] = "SBC A,D",
    [0x9B] = "SBC A,E",
    [0x9C] = "SBC A,H",
    [0x9D] = "SBC A,L",
    [0xDE] = "SBC A,n",
    [0x9E] = "SBC A,(HL)",
    [0xA7] = "AND A",
    [0xA0] = "AND B",
    [0xA1] = "AND C",
    [0xA2] = "AND D",
    [0xA3] = "AND E",
    [0xA4] = "AND H",
    [0xA5] = "AND L",
    [0xE6] = "AND n",
    [0xA6] = "AND (HL)",
    [0xAF] = "XOR A",
    [0xA8] = "XOR B",
    [0xA9] = "XOR C",
    [0xAA] = "XOR D",
    [0xAB] = "XOR E",
    [0xAC] = "XOR H",
    [0xAD] = "XOR L",
    [0xEE] = "XOR n",
    [0xAE] = "XOR (HL)",
    [0xB7] = "OR A",
    [0xB0] = "OR B",
    [0xB1] = "OR C",
    [0xB2] = "OR D",
    [0xB3] = "OR E",
    [0xB4] = "OR H",
    [0xB5] = "OR L",
    [0xF6] = "OR n",
    [0xB6] = "OR (HL)",
    [0xBF] = "CP A",
    [0xB8] = "CP B",
    [0xB9] = "CP C",
    [0xBA] = "CP D",
    [0xBB] = "CP E",
    [0xBC] = "CP H",
    [0xBD] = "CP L",
    [0xFE] = "CP n",
    [0xBE] = "CP (HL)",
    [0x3C] = "INC A",
    [0x04] = "INC B",
    [0x0C] = "INC C",
    [0x14] = "INC D",
    [0x1C] = "INC E",
    [0x24] = "INC H",
    [0x2C] = "INC L",
    [0x34] = "INC (HL)",
    [0x3D] = "DEC A",
    [0x05] = "DEC B",
    [0x0D] = "DEC C",
    [0x15] = "DEC D",
    [0x1D] = "DEC E",
    [0x25] = "DEC H",
    [0x2D] = "DEC L",
    [0x35] = "DEC (HL)",
    [0x27] = "DAA",
    [0x2F] = "CPL",

    /* 16-bit Arithmetic/Logic instructions */
    [0x09] = "ADD HL,BC",
    [0x19] = "ADD HL,DE",
    [0x29] = "ADD HL,HL",
    [0x39] = "ADD HL,SP",
    [0x03] = "INC BC",
    [0x13] = "INC DE",
    [0x23] = "INC HL",
    [0x33] = "INC SP",
    [0x0B] = "DEC BC",
    [0x1B] = "DEC DE",
    [0x2B] = "DEC HL",
    [0x3B] = "DEC SP",
    [0xE8] = "ADD SP,dd",
    [0xF8] = "LD HL,SP+dd",

    /* Rotate and Shift */
    [0x07] = "RLCA",
    [0x17] = "RLA",
    [0x1F] = "RRA",

    [0xCB] = "CB"
};

const void (*instruction_pointers[256])(void) = {
    /* Control */
    [0x3F] = &instruction_ccf,
    [0x37] = &instruction_scf,
    [0x00] = &instruction_nop,
    [0x76] = &instruction_halt,
    [0x10] = &instruction_stop,
    [0xF3] = &instruction_di,
    [0xFB] = &instruction_ei,

    /* Jump */
    [0xC3] = &instruction_jp_nn,
    [0xE9] = &instruction_jp_hl,
    [0xC2] = &instruction_jp_nz_nn,
    [0xCA] = &instruction_jp_z_nn,
    [0xD2] = &instruction_jp_nc_nn,
    [0xDA] = &instruction_jp_c_nn,
    [0x18] = &instruction_jr_dd,
    [0x20] = &instruction_jr_nz_dd,
    [0x28] = &instruction_jr_z_dd,
    [0x30] = &instruction_jr_nc_dd,
    [0x38] = &instruction_jr_c_dd,
    [0xCD] = &instruction_call_nn,
    [0xC4] = &instruction_call_nz_nn,
    [0xCC] = &instruction_call_z_nn,
    [0xD4] = &instruction_call_nc_nn,
    [0xDC] = &instruction_call_c_nn,
    [0xC9] = &instruction_ret,
    [0xC0] = &instruction_ret_nz,
    [0xC8] = &instruction_ret_z,
    [0xD0] = &instruction_ret_nc,
    [0xD8] = &instruction_ret_c,
    [0xD9] = &instruction_reti,
    [0xC7] = &instruction_rst_00,
    [0xCF] = &instruction_rst_08,
    [0xD7] = &instruction_rst_10,
    [0xDF] = &instruction_rst_18,
    [0xE7] = &instruction_rst_20,
    [0xEF] = &instruction_rst_28,
    [0xF7] = &instruction_rst_30,
    [0xFF] = &instruction_rst_38,

    /* 8-bit Load instructions */
    [0x7F] = &instruction_ld_a_a,
    [0x78] = &instruction_ld_a_b,
    [0x79] = &instruction_ld_a_c,
    [0x7A] = &instruction_ld_a_d,
    [0x7B] = &instruction_ld_a_e,
    [0x7C] = &instruction_ld_a_h,
    [0x7D] = &instruction_ld_a_l,
    [0x47] = &instruction_ld_b_a,
    [0x40] = &instruction_ld_b_b,
    [0x41] = &instruction_ld_b_c,
    [0x42] = &instruction_ld_b_d,
    [0x43] = &instruction_ld_b_e,
    [0x44] = &instruction_ld_b_h,
    [0x45] = &instruction_ld_b_l,
    [0x4F] = &instruction_ld_c_a,
    [0x48] = &instruction_ld_c_b,
    [0x49] = &instruction_ld_c_c,
    [0x4A] = &instruction_ld_c_d,
    [0x4B] = &instruction_ld_c_e,
    [0x4C] = &instruction_ld_c_h,
    [0x4D] = &instruction_ld_c_l,
    [0x57] = &instruction_ld_d_a,
    [0x50] = &instruction_ld_d_b,
    [0x51] = &instruction_ld_d_c,
    [0x52] = &instruction_ld_d_d,
    [0x53] = &instruction_ld_d_e,
    [0x54] = &instruction_ld_d_h,
    [0x55] = &instruction_ld_d_l,
    [0x5F] = &instruction_ld_e_a,
    [0x58] = &instruction_ld_e_b,
    [0x59] = &instruction_ld_e_c,
    [0x5A] = &instruction_ld_e_d,
    [0x5B] = &instruction_ld_e_e,
    [0x5C] = &instruction_ld_e_h,
    [0x5D] = &instruction_ld_e_l,
    [0x67] = &instruction_ld_h_a,
    [0x60] = &instruction_ld_h_b,
    [0x61] = &instruction_ld_h_c,
    [0x62] = &instruction_ld_h_d,
    [0x63] = &instruction_ld_h_e,
    [0x64] = &instruction_ld_h_h,
    [0x65] = &instruction_ld_h_l,
    [0x6F] = &instruction_ld_l_a,
    [0x68] = &instruction_ld_l_b,
    [0x69] = &instruction_ld_l_c,
    [0x6A] = &instruction_ld_l_d,
    [0x6B] = &instruction_ld_l_e,
    [0x6C] = &instruction_ld_l_h,
    [0x6D] = &instruction_ld_l_l,
    [0x3E] = &instruction_ld_a_n,
    [0x06] = &instruction_ld_b_n,
    [0x0E] = &instruction_ld_c_n,
    [0x16] = &instruction_ld_d_n,
    [0x1E] = &instruction_ld_e_n,
    [0x26] = &instruction_ld_h_n,
    [0x2E] = &instruction_ld_l_n,
    [0x7E] = &instruction_ld_a_hlp,
    [0x46] = &instruction_ld_b_hlp,
    [0x4E] = &instruction_ld_c_hlp,
    [0x56] = &instruction_ld_d_hlp,
    [0x5E] = &instruction_ld_e_hlp,
    [0x66] = &instruction_ld_h_hlp,
    [0x6E] = &instruction_ld_l_hlp,
    [0x77] = &instruction_ld_hlp_a,
    [0x70] = &instruction_ld_hlp_b,
    [0x71] = &instruction_ld_hlp_c,
    [0x72] = &instruction_ld_hlp_d,
    [0x73] = &instruction_ld_hlp_e,
    [0x74] = &instruction_ld_hlp_h,
    [0x75] = &instruction_ld_hlp_l,
    [0x36] = &instruction_ld_hlp_n,
    [0x0A] = &instruction_ld_a_bcp,
    [0x1A] = &instruction_ld_a_dep,
    [0xFA] = &instruction_ld_a_nnp,
    [0x02] = &instruction_ld_bcp_a,
    [0x12] = &instruction_ld_dep_a,
    [0xEA] = &instruction_ld_nnp_a,
    [0xF0] = &instruction_ld_a_io_n,
    [0xE0] = &instruction_ld_io_n_a,
    [0xF2] = &instruction_ld_a_io_c,
    [0xE2] = &instruction_ld_io_c_a,
    [0x22] = &instruction_ldi_hlp_a,
    [0x2A] = &instruction_ldi_a_hlp,
    [0x32] = &instruction_ldd_hlp_a,
    [0x3A] = &instruction_ldd_a_hlp,

    /* 16-bit Load instructions */
    [0x01] = &instruction_ld_bc_nn,
    [0x11] = &instruction_ld_de_nn,
    [0x21] = &instruction_ld_hl_nn,
    [0x31] = &instruction_ld_sp_nn,
    [0x08] = &instruction_ld_nnp_sp,
    [0xF9] = &instruction_ld_sp_hl,
    [0xF5] = &instruction_push_af,
    [0xC5] = &instruction_push_bc,
    [0xD5] = &instruction_push_de,
    [0xE5] = &instruction_push_hl,
    [0xF1] = &instruction_pop_af,
    [0xC1] = &instruction_pop_bc,
    [0xD1] = &instruction_pop_de,
    [0xE1] = &instruction_pop_hl,

    /* 8-bit Arithmetic/Logic instructions */
    [0x87] = &instruction_add_a_a,
    [0x80] = &instruction_add_a_b,
    [0x81] = &instruction_add_a_c,
    [0x82] = &instruction_add_a_d,
    [0x83] = &instruction_add_a_e,
    [0x84] = &instruction_add_a_h,
    [0x85] = &instruction_add_a_l,
    [0xC6] = &instruction_add_a_n,
    [0x86] = &instruction_add_a_hlp,
    [0x8F] = &instruction_adc_a_a,
    [0x88] = &instruction_adc_a_b,
    [0x89] = &instruction_adc_a_c,
    [0x8A] = &instruction_adc_a_d,
    [0x8B] = &instruction_adc_a_e,
    [0x8C] = &instruction_adc_a_h,
    [0x8D] = &instruction_adc_a_l,
    [0xCE] = &instruction_adc_a_n,
    [0x8E] = &instruction_adc_a_hlp,
    [0x97] = &instruction_sub_a_a,
    [0x90] = &instruction_sub_a_b,
    [0x91] = &instruction_sub_a_c,
    [0x92] = &instruction_sub_a_d,
    [0x93] = &instruction_sub_a_e,
    [0x94] = &instruction_sub_a_h,
    [0x95] = &instruction_sub_a_l,
    [0xD6] = &instruction_sub_a_n,
    [0x96] = &instruction_sub_a_hlp,
    [0x9F] = &instruction_sbc_a_a,
    [0x98] = &instruction_sbc_a_b,
    [0x99] = &instruction_sbc_a_c,
    [0x9A] = &instruction_sbc_a_d,
    [0x9B] = &instruction_sbc_a_e,
    [0x9C] = &instruction_sbc_a_h,
    [0x9D] = &instruction_sbc_a_l,
    [0xDE] = &instruction_sbc_a_n,
    [0x9E] = &instruction_sbc_a_hlp,
    [0xA7] = &instruction_and_a,
    [0xA0] = &instruction_and_b,
    [0xA1] = &instruction_and_c,
    [0xA2] = &instruction_and_d,
    [0xA3] = &instruction_and_e,
    [0xA4] = &instruction_and_h,
    [0xA5] = &instruction_and_l,
    [0xE6] = &instruction_and_n,
    [0xA6] = &instruction_and_hlp,
    [0xAF] = &instruction_xor_a,
    [0xA8] = &instruction_xor_b,
    [0xA9] = &instruction_xor_c,
    [0xAA] = &instruction_xor_d,
    [0xAB] = &instruction_xor_e,
    [0xAC] = &instruction_xor_h,
    [0xAD] = &instruction_xor_l,
    [0xEE] = &instruction_xor_n,
    [0xAE] = &instruction_xor_hlp,
    [0xB7] = &instruction_or_a,
    [0xB0] = &instruction_or_b,
    [0xB1] = &instruction_or_c,
    [0xB2] = &instruction_or_d,
    [0xB3] = &instruction_or_e,
    [0xB4] = &instruction_or_h,
    [0xB5] = &instruction_or_l,
    [0xF6] = &instruction_or_n,
    [0xB6] = &instruction_or_hlp,
    [0xBF] = &instruction_cp_a,
    [0xB8] = &instruction_cp_b,
    [0xB9] = &instruction_cp_c,
    [0xBA] = &instruction_cp_d,
    [0xBB] = &instruction_cp_e,
    [0xBC] = &instruction_cp_h,
    [0xBD] = &instruction_cp_l,
    [0xFE] = &instruction_cp_n,
    [0xBE] = &instruction_cp_hlp,
    [0x3C] = &instruction_inc_a,
    [0x04] = &instruction_inc_b,
    [0x0C] = &instruction_inc_c,
    [0x14] = &instruction_inc_d,
    [0x1C] = &instruction_inc_e,
    [0x24] = &instruction_inc_h,
    [0x2C] = &instruction_inc_l,
    [0x34] = &instruction_inc_hlp,
    [0x3D] = &instruction_dec_a,
    [0x05] = &instruction_dec_b,
    [0x0D] = &instruction_dec_c,
    [0x15] = &instruction_dec_d,
    [0x1D] = &instruction_dec_e,
    [0x25] = &instruction_dec_h,
    [0x2D] = &instruction_dec_l,
    [0x35] = &instruction_dec_hlp,
    [0x27] = &instruction_daa,
    [0x2F] = &instruction_cpl,

    /* 16-bit Arithmetic/Logic instructions */
    [0x09] = &instruction_add_hl_bc,
    [0x19] = &instruction_add_hl_de,
    [0x29] = &instruction_add_hl_hl,
    [0x39] = &instruction_add_hl_sp,
    [0x03] = &instruction_inc_bc,
    [0x13] = &instruction_inc_de,
    [0x23] = &instruction_inc_hl,
    [0x33] = &instruction_inc_sp,
    [0x0B] = &instruction_dec_bc,
    [0x1B] = &instruction_dec_de,
    [0x2B] = &instruction_dec_hl,
    [0x3B] = &instruction_dec_sp,
    [0xE8] = &instruction_add_sp_dd,
    [0xF8] = &instruction_ld_hl_sp_dd,

    /* Rotate and Shift */
    [0x07] = &instruction_rlca,
    [0x17] = &instruction_rla,
    [0x1F] = &instruction_rra,
};

void cpu_init()
{
    cpu_reset();
}

void cpu_reset()
{
    cpu.regs.af = 0x0000;
    cpu.regs.bc = 0x0000;
    cpu.regs.de = 0x0000;
    cpu.regs.hl = 0x0000;
    cpu.regs.sp = 0x0000;
    cpu.regs.pc = 0x0000;
    cpu.ie = 0x00;
    cpu.ifr = 0x00;
    cpu.ime = false;
    cpu.cycles = 0;
    cpu.debug_enabled = false;
}

void cpu_enable_interrupts(uint8_t ie)
{
    cpu.ie = ie;

    #if defined CPU_DEBUG && defined CPU_DEBUG_INTERRUPTS
    DEBUG_CPU("-> IE: %x\n", ie);

    if (cpu.ie & CPU_IE_VBLANK) {
        DEBUG_CPU(" - VBLANK\n");
    }

    if (cpu.ie & CPU_IE_LCD_STAT) {
        DEBUG_CPU(" - LCD STAT\n");
    }

    if (cpu.ie & CPU_IE_TIMER) {
        DEBUG_CPU(" - TIMER\n");
    }

    if (cpu.ie & CPU_IE_SERIAL) {
        DEBUG_CPU(" - SERIAL\n");
    }

    if (cpu.ie & CPU_IE_JOYPAD) {
        DEBUG_CPU(" - JOYPAD\n");
    }

    #endif
}

void cpu_request_interrupt(uint8_t ifr)
{
    if (cpu.halted) {
        cpu.halted = false;
    }

    cpu.ifr |= ifr;

    #if defined CPU_DEBUG && defined CPU_DEBUG_INTERRUPTS
    char *source_name;

    switch(ifr) {
        case CPU_IF_VBLANK:
            source_name = "VBLANK";
            break;
        case CPU_IF_LCD_STAT:
            source_name = "STAT";
            break;
        case CPU_IF_TIMER:
            source_name = "TIMER";
            break;
        case CPU_IF_SERIAL:
            source_name = "SERIAL";
            break;
        case CPU_IF_JOYPAD:
            source_name = "JOYPAD";
            break;
    }

    DEBUG_CPU("Requested %s interrupt | IME: %d IE: %02X IF: %02X\n", source_name, (cpu.ime) ? 1 : 0, cpu.ie, cpu.ifr);
    #endif
}

void cpu_serve_interrupts()
{
    if (cpu.ime && (cpu.ifr & cpu.ie)) {
        uint8_t triggered = cpu.ie & cpu.ifr;

        cpu_stack_push(cpu.regs.pc);

        if (triggered & CPU_IF_VBLANK) {
            cpu.regs.pc = 0x0040;
            cpu.ifr &= ~CPU_IF_VBLANK;
        } else if (triggered & CPU_IF_LCD_STAT) {
            cpu.regs.pc = 0x0048;
            cpu.ifr &= ~CPU_IF_LCD_STAT;
        } else if (triggered & CPU_IF_TIMER) {
            cpu.regs.pc = 0x0050;
            cpu.ifr &= ~CPU_IF_TIMER;
        } else if (triggered & CPU_IF_SERIAL) {
            cpu.regs.pc = 0x0058;
            cpu.ifr &= ~CPU_IF_SERIAL;
        } else if (triggered & CPU_IF_JOYPAD) {
            cpu.regs.pc = 0x0060;
            cpu.ifr &= ~CPU_IF_JOYPAD;
        }

        cpu.cycles += 20;

        cpu.ime = false;
        cpu.halted = false;

        #if defined CPU_DEBUG && defined CPU_DEBUG_INTERRUPTS
        DEBUG_CPU("Interrupt after %d cycles | IF: %02X\n", cpu.cycles, cpu.ifr);
        #endif
    }
}

void cpu_step()
{
    if (cpu.stopped) {
        return;
    }

    if (cpu.halted) {
        cpu.cycles += 1;
        return;
    }

    // Breakpoints
    if (mmu.boot_rom_mapped == false && cpu.regs.pc == 0x0100) cpu.debug_enabled = true;

    uint8_t opcode = mmu_rb(cpu.regs.pc++);

    #if defined CPU_DEBUG && defined CPU_DEBUG_INSTRUCTIONS
    if (cpu.debug_enabled) {
        DEBUG_CPU("A: %02X B: %02X C: %02X D: %02X E: %02X H: %02X L: %02X | F: %02X PC: %04X SP: %04X IME: %d IE: %02X IF: %02X Cycles: %d | %02X | %s\n",
            cpu.regs.a,
            cpu.regs.b,
            cpu.regs.c,
            cpu.regs.d,
            cpu.regs.e,
            cpu.regs.h,
            cpu.regs.l,
            cpu.regs.f,
            cpu.regs.pc - 1,
            cpu.regs.sp,
            cpu.ime ? 1 : 0,
            cpu.ie,
            cpu.ifr,
            cpu.cycles,
            opcode,
            instruction_labels[opcode]
        );
    }
    #endif

    if (opcode != 0xCB) {
        if (instruction_pointers[opcode]) {
            (*instruction_pointers[opcode])();
        } else {
            #ifdef CPU_DEBUG
            DEBUG_CPU("Unknown opcode %02X at PC: %04X\n", opcode, cpu.regs.pc - 1);
            #endif

            cpu.stopped = true;
        }
    } else {
        opcode = mmu_rb(cpu.regs.pc++);

        if (cb_instruction_pointers[opcode]) {
            (*cb_instruction_pointers[opcode])();
        } else {
            #ifdef CPU_DEBUG
            DEBUG_CPU("Unknown CB opcode %02X at PC: %04X\n", opcode, cpu.regs.pc - 2);
            #endif

            cpu.stopped = true;
        }
    }

    if (!cpu.ime && cpu.halted) {
        // HALT bug
        cpu.regs.pc--;
    }
}