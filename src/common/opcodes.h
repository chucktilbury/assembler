#ifndef OPCODES_H
#define OPCODES_H

typedef enum {
    OP_ABORT,   // cause the VM to quit with an error found in a variable
    OP_EXIT,    // cause the VM to quit normally
    OP_NOP,     // no operation

    // flow control instructions
    OP_CALL,    // call an absolute address stored in a variable
    OP_TRAP,    // call a pre-determined address for external functionality
    OP_RETURN,  // return from a call or a trap
    OP_JMP,     // unconditional jump to an absolute address stored in a variable
    OP_BR,      // conditional jump to an absolute address in a variable

    // data manipulation instructions
    OP_PUSH,    // push the value on the value stack from a variable
    OP_POP,     // pop the value from the stack and put it in a register

    OP_LOAD,    // Load a register from a variable.
    OP_LOADI,   // load a register from an immediate.
    OP_LOADR,   // load a register from another register (copy)
    OP_STORE,   // Store a register into a variable.

    // comparison operators
    OP_NOT,     // unary not conditional
    OP_EQ,      // equal conditional
    OP_NEQ,     // not equal conditional
    OP_LEQ,     // less-or-equal conditional
    OP_GEQ,     // greater-or-equal conditional
    OP_LESS,    // less than conditional
    OP_GTR,     // greater than conditional

    // arithmetic operators
    OP_NEG,     // unary arithmetic negation
    OP_ADD,     // arithmetic add
    OP_SUB,     // arithmetic subtract
    OP_MUL,     // arithmetic multiply
    OP_DIV,     // arithmetic divide
    OP_MOD,     // arithmetic modulo

} OpCode;

const char* opToStr(OpCode op);

#endif