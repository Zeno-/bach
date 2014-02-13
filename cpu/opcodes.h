#ifndef BACH_CPU_OPCODES_H
#define BACH_CPU_OPCODES_H

#include "cpu_config.h"

enum {
    /* Arithmetic (integer)
     */

    OPCODE_ALU,     /* Integer Arithemetic */
                    /* ADD, SUB, MUL, DIV */
                    /* Set by flag in OPCODE_ARITH instruction */

    OPCODE_SHFT,    /* SHL, SHR, ROL, ROR */
                    /* Set by flag in OPCODE_SHFT instruction */

    /* Flow control
     */
    OPCODE_JMP,     /* JSR indicated by flag in instruction */
    OPCODE_RET,     /* Return from subroutine, Return from interrupt etc
                       all indicated by instruction flags */

    OPCODE_SKP,     /* Skip next instruction depending on flags */

    /* Load/Store (RAM access)
     */
    OPCODE_MOV,     /* LD ST (i.e. direction) Set by flags in the instr. */

    /* Register modify
     */
    OPCODE_SET,     /* SET, SET.h, SET.l */
                    /* Same addressing modes as flow control and load/store
                     * Load/store instructions.
                     *
                     * ?? Is this opcode necessary, or can everything be
                     * done with MOV?
                     */

    /* Register bitwise
     */
    OPCODE_BOP,     /* Actual operation (AND, OR, XOR, NOT) set by
                     * flags in the instruction */
    /* Misc
     */
    OPCODE_SWI,     /* Software Interrupt */

    OPCODE_USTK,    /* (User) Stack operations: PUSH, POP, PUSHA, POPA */
                    /* Actual operation based on flags in instr. */

    OPCODE_FSTK,    /* (Frame) Stack operations: PUSH, POP */
                    /* Actual operation based on flags in instr. */

    OPCODE_HLT,     /* Halt program */
    OPCODE_ERR,     /* Raise Exception */

    /* Comparative
     */
    OPCODE_TST,     /* Will test condition flags */
                    /* Set CPU condition flags based on the tests
                     * specified by the instr. condition flags */

    /* Logical ops
     */
    OPCODE_LOP,      /* Logical (boolean) test */
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
 *
 * Note:    For all instructions these comprise the first 4 bits of the
 *          instruction.  Originally the opcode bits came first but it
 *          makes more sense with the condition flags being first as they
 *          determine whether or not the instruction is even executed.
 */

/* ======================================================================
 * Instruction Set and Bit Layouts
 *
 * General
 *
 *      DESC         : BITS
 *      -------------------
 *
 *      CondFlags    : 4    Condition flags
 *      Opcode       : 6    64 opcodes... why would you need more?
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
 *      CondFlags    : 4        Condition flags
 *      Opcode       : 6
 *      SetCondFlags : 1
 *
 *      AddrMode     : 1        0 = operand(s) registers
 *                              1 = operand(s) immediate
 *      Op           : 2        0 = Add, 1 = Subract
 *                              2 = Multiply, 3 = Divide
 *      R<dest>      : 4
 *
 *      -------- AddrMode 0  (registers)    R<dest> := R<srcA> op R<srcB>
 *
 *      R<srcA>      : 4
 *      R<srcB>      : 4
 *      Shift        : 6        Multiply result by 2^n (max 64)
 *
 *          NB: The shift is applied after the the operation
 *              on R<srcA> and R<srcB> has been performed and before
 *              the final result is stored in R<dest>
 *
 *      -------- AddrMode 1 (immediate)     R<dest> := R<dest> op Operand
 *
 *      Operand      : 16
 *
 *      Notes:
 *          * The remainder of a division is stored in XXXXX special
 *            purpose register
 *
 * =====================================================================
 *
 * Flow control
 *
 *      NOTE: Indirect "modes" always take a register as an operand. At
 *            this point there is no "relative indirect".
 *
 *      Branch will occur based on the condition flags, therefore a
 *      comparison operation must be performed before the brach instruction
 *      or, for an unconditional branch, the (instruction) condition flags
 *      cleared.
 *
 *      CondFlags    : 4    Condition flags
 *      Opcode       : 6
 *      AddrMode     : 2    0 = Relative direct, 1 = Absolute indirect
 *                          2 = Relative indirect, 3 = RESERVED
 *
 *                          Note that the AddrMode field has replaced the
 *                          "SetCondFlags" field common to many other
 *                          instructions. As a consequence none of the
 *                          flow control instructions can modify the CPU
 *                          condition flags.
 *
 *      -------- AddrMode: Relative direct
 *
 *      Address      : 20   +/- 2^20
 *
 *      -------- AddrMode: Absolute indirect
 *            -- AddrMode: Relative indirect
 *
 *      Rn           : 4
 *      Add          : 17   Value to add to register to calculate final
 *                          address
 */

#endif /* BACH_CPU_OPCODES_H */
