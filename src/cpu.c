#include "emulator.h"

cpu_t cpu;

#define FLAG_CARRY (1 << 4)
#define FLAG_SUBTRACTION (1 << 6)
#define FLAG_ZERO (1 << 7)

#define SET_FLAG(flag) cpu.regs.f |= flag
#define CLEAR_FLAG(flag) cpu.regs.f &= ~flag
#define CHECK_FLAG(flag) ((cpu.regs.f & flag) == flag)

#ifdef CPU_DEBUG
#define DEBUG_CPU(...) printf("[cpu] "); printf(__VA_ARGS__)
#endif

/* Instructions */

/* Control */

void instruction_ccf()
{
    if (cpu.regs.f & FLAG_CARRY) {
        CLEAR_FLAG(FLAG_CARRY);
    } else {
        SET_FLAG(FLAG_CARRY);
    }

    cpu.cycles += 4;
}

void instruction_scf()
{
    SET_FLAG(FLAG_CARRY);
    cpu.cycles += 4;
}

void instruction_nop()
{
    cpu.cycles += 4;
}

void instruction_halt()
{
    cpu.halt = true;
}

void instruction_stop()
{
    // TODO: Stop emulator
    cpu.halt = true;
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
    /* TODO: Check if this is right */
    cpu.regs.pc += (int8_t) mmu_rb(cpu.regs.pc);
    cpu.cycles += 12;
}

void instruction_jr_nz_dd()
{
    if (!CHECK_FLAG(FLAG_ZERO)) {
        cpu.regs.pc += (int8_t) mmu_rb(cpu.regs.pc);
        cpu.cycles += 12;
    } else {
        cpu.regs.pc++;
        cpu.cycles += 8;
    }
}

void instruction_jr_z_dd()
{
    if (CHECK_FLAG(FLAG_ZERO)) {
        cpu.regs.pc += (int8_t) mmu_rb(cpu.regs.pc);
        cpu.cycles += 12;
    } else {
        cpu.regs.pc++;
        cpu.cycles += 8;
    }
}

void instruction_jr_nc_dd()
{
    if (!CHECK_FLAG(FLAG_CARRY)) {
        cpu.regs.pc += (int8_t) mmu_rb(cpu.regs.pc);
        cpu.cycles += 12;
    } else {
        cpu.regs.pc++;
        cpu.cycles += 8;
    }
}

void instruction_jr_c_dd()
{
    if (CHECK_FLAG(FLAG_CARRY)) {
        cpu.regs.pc += (int8_t) mmu_rb(cpu.regs.pc);
        cpu.cycles += 12;
    } else {
        cpu.regs.pc++;
        cpu.cycles += 8;
    }
}

void instruction_call_nn()
{
    cpu.regs.sp -= 2;
    mmu_ww(cpu.regs.sp, cpu.regs.pc - 1); // TODO: Check if thats right
    cpu.regs.pc = mmu_rw(cpu.regs.pc);
    cpu.cycles += 24;
}

void instruction_call_nz_nn()
{
    if (!CHECK_FLAG(FLAG_ZERO)) {
        cpu.regs.sp -= 2;
        mmu_ww(cpu.regs.sp, cpu.regs.pc - 1); // TODO: Check if thats right
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
        mmu_ww(cpu.regs.sp, cpu.regs.pc - 1); // TODO: Check if thats right
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
        mmu_ww(cpu.regs.sp, cpu.regs.pc - 1); // TODO: Check if thats right
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
        mmu_ww(cpu.regs.sp, cpu.regs.pc - 1); // TODO: Check if thats right
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
    cpu.regs.sp += 2;                       // TODO: Check if thats right
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
    cpu.regs.sp -= 2;
    mmu_ww(cpu.regs.sp, cpu.regs.pc - 1); // TODO: Check if thats right
    cpu.regs.pc = 0x0000;
    cpu.cycles += 16;
}

void instruction_rst_08()
{
    cpu.regs.sp -= 2;
    mmu_ww(cpu.regs.sp, cpu.regs.pc - 1); // TODO: Check if thats right
    cpu.regs.pc = 0x0008;
    cpu.cycles += 16;
}

void instruction_rst_10()
{
    cpu.regs.sp -= 2;
    mmu_ww(cpu.regs.sp, cpu.regs.pc - 1); // TODO: Check if thats right
    cpu.regs.pc = 0x0010;
    cpu.cycles += 16;
}

void instruction_rst_18()
{
    cpu.regs.sp -= 2;
    mmu_ww(cpu.regs.sp, cpu.regs.pc - 1); // TODO: Check if thats right
    cpu.regs.pc = 0x0018;
    cpu.cycles += 16;
}

void instruction_rst_20()
{
    cpu.regs.sp -= 2;
    mmu_ww(cpu.regs.sp, cpu.regs.pc - 1); // TODO: Check if thats right
    cpu.regs.pc = 0x0020;
    cpu.cycles += 16;
}

void instruction_rst_28()
{
    cpu.regs.sp -= 2;
    mmu_ww(cpu.regs.sp, cpu.regs.pc - 1); // TODO: Check if thats right
    cpu.regs.pc = 0x0028;
    cpu.cycles += 16;
}

void instruction_rst_30()
{
    cpu.regs.sp -= 2;
    mmu_ww(cpu.regs.sp, cpu.regs.pc - 1); // TODO: Check if thats right
    cpu.regs.pc = 0x0030;
    cpu.cycles += 16;
}

void instruction_rst_38()
{
    cpu.regs.sp -= 2;
    mmu_ww(cpu.regs.sp, cpu.regs.pc - 1); // TODO: Check if thats right
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
    cpu.regs.a = cpu.regs.b;
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
    cpu.regs.a = mmu_rb(cpu.regs.pc);
    cpu.regs.pc += 1;
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
    mmu_wb(cpu.regs.pc, cpu.regs.a);
    cpu.regs.pc += 1;
    cpu.cycles += 16;
}

void instruction_ld_a_io_n()
{
    cpu.regs.a = 0xFF00 + mmu_rb(cpu.regs.pc);
    cpu.regs.pc += 1;
    cpu.cycles += 12;
}

void instruction_ld_io_n_a()
{
    mmu_wb(0xFF00 + cpu.regs.pc, cpu.regs.a);
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
    cpu.ime = false;
}

void cpu_step()
{
    if (cpu.stop) {
        return;
    }

    uint8_t opcode = mmu_rb(cpu.regs.pc);
    cpu.regs.pc++;

    if (instruction_pointers[opcode]) {
        #ifdef CPU_DEBUG
        DEBUG_CPU("%s\n", instruction_labels[opcode]);
        #endif
        
        (*instruction_pointers[opcode])();
    } else {
        #ifdef CPU_DEBUG
        DEBUG_CPU("Unknown opcode: %02X at PC: %04X\n", opcode, cpu.regs.pc - 1);
        #endif

        cpu.stop = true;
    }
}