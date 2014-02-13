#ifndef BACH_CPU_OPCODES_H
#define BACH_CPU_OPCODES_H

#include "cpu_config.h"

enum {
    /* Arithmetic (integer)
     */

    OPCODE_ARITH,       /* ADD, SUB, MUL, DIV */
                        /* Set by flag in OPCODE_ARITH instruction */

    OPCODE_SHFT,        /* SHL, SHR, ROL, ROR */
                        /* Set by flag in OPCODE_SHFT instruction */
    /* Flow control
     */
    OPCODE_JMP,

    OPCODE_JSR,

    OPCODE_SKIP,    /* Skip next instruction depending on flags */

    /* Load/Store (RAM access)
     */
    OPCODE_MOV,     /* LD ST (i.e. direction) Set by flags in the instr. */

    /* Register modify
     */
    OPCODE_SET,

    /* Register bitwise
     */
    OPCODE_BOP,     /* Actual operation (AND, OR, XOR, NOT) set by
                     * flags in the instruction */
    /* Misc
     */
    OPCODE_SWI,
    OPCODE_PUSH,    /* PUSHA set by flag */
    OPCODE_POP,     /* POPA set by flag */
    OPCODE_HALT,
    OPCODE_ERR,     /* Raise Exception */

    /* Comparative
     */
    OPCODE_TST,     /* Will test condition flags */
                    /* Set CPU condition flags based on the tests
                     * specified by the instr. condition flags */

    /* Logical ops
     */
    OPCODE_BTST,     /* Boolean test */
                     /* Test registers based on boolen flag in instr. */

    /* Misc
     */
    OPCODE_NOP
};

/* ======================================================================
 * Condition flags
 * (4 bits)
 *
 * 0000                 No flags
 *
 * 0001     EQ      =   Equal
 * 0010     GT      >   Greater than
 * 0100     LT      <   Less than
 *
 * 0110     C           Carry
 * 0111     V           Overflow
 *
 * 1000     Z           Previous operation was "zero"
 *
 * Note that C and V reuse bits already used by EQ, GT and LT. The
 * reasoning behind this is that GT and LT are mutually exclusive.
 * Similarly the statement "n > m and n < m and n == m" doesn't make
 * sense either, so if all three (EQ, GT, LT) flags are set, this is
 * interpreted as overflow.
 *
 * The following combinations are also valid:
 *
 * 0011     GTE     >=
 * 0101     LTE     <=
 *
 * The zero flag (Z) may be combined with any of the other combinations.
 */

/* ======================================================================
 * Instruction Set and Bit Layouts
 *
 * General
 *
 *      DESC         : BITS
 *      -------------------
 *
 *      Opcode       : 6    64 opcodes... why would you need more?
 *      CondFlags    : 4    Condition flags
 *      SetCondFlags : 1    The instruction (may) update the CPU cond flags
 *      Instr data   : 22   Op-code dependant
 *
 * For all instructions, the instruction will only be executed if the
 * CPU context's condition flags and any of the instruction's condition
 * flags are both set. For example, if the instruction has the NE (not
 * equal) flag set then the instruction will only be executed if the
 * CPU's NE condition flag is also set.
 *
 * =====================================================================
 *
 * Arithmetic (integer) operations
 *
 *      Opcode       : 6
 *      CondFlags    : 4
 *      SetCondFlags : 1
 *
 *      Address mode : 1        0 = operand(s) registers
 *                              1 = operand(s) immediate
 *      Op           : 2        0 = Add, 1 = Subract
 *                              2 = Multiply, 3 = Divide
 *      R<dest>      : 4
 *
 *      -------- Mode 0  R<dest> := R<srcA> op R<srcB>
 *
 *      R<srcA>      : 4
 *      R<srcB>      : 4
 *      Shift        : 6        Multiply result by 2^n (max 64)
 *
 *          NB: The shift is applied after the the operation
 *              on R<srcA> and R<srcB> has been performed and before
 *              the final result is stored in R<dest>
 *
 *      -------- Mode 1  Add operand to R<dest>
 *
 *      Operand      : 16       R<dest> := R<dest> op Operand
 *
 * =====================================================================
 *
 * Flow control
 *
 *      NOTE: Indirect "mode" always takes a register as an operand.
 *
 *      Branch will occur based on the condition flags, therefore a
 *      comparison operation must be performed before the brach instruction
 *      or, for an unconditional branch, the (instruction) condition flags
 *      cleared.
 *
 *      Opcode       : 6
 *      CondFlags    : 4
 *      Mode         : 2    0 = Relative direct, 1 = Absolute indirect
 *                          2 = Relative indirect, 3 = RESERVED
 *
 *                          Note that the mode field has replaced the
 *                          "SetCondFlags" field common to many other
 *                          instructions.
 *
 *      -------- Mode: Relative direct
 *
 *      Address      : 20   +/- 2^20 *instructions*
 *
 *      -------- Mode: Absolute indirect
 *               Mode: Relative indirect
 *
 *      Rn           : 4
 *      Add          : 17   Add value to register to calculate final
 *                          address
 */

#endif /* BACH_CPU_OPCODES_H */
