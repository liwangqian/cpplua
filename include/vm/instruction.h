#ifndef CPPLUA_INSTRUCTION_H
#define CPPLUA_INSTRUCTION_H

#include "def.h"

CPPLUA_NS_BEGIN
namespace __detail {

class instruction {
public:
    instruction(uint32_t code)
        : m_code{code} { }

    enum class opmode_t : uint8_t {
        iABC = 0,
        iABx,
        iAsBx,
        iAx
    };

    uint32_t opcode() const
    {
        return m_code & 0x3F;
    }

    uint32_t a() const
    {
        return (m_code >> 6) & 0xFF;
    }

    uint32_t b() const
    {
        return (m_code >> 14) & 0xFF;
    }

    uint32_t c() const
    {
        return (m_code >> 23) & 0xFF;
    }

    uint32_t bx() const
    {
        return m_code >> 14;
    }

    uint32_t sbx() const
    {
        constexpr uint32_t MAXARG_sBx = ((1 << 18) - 1) >> 1;
        return (m_code >> 14) - MAXARG_sBx;
    }

    uint32_t ax() const
    {
        return m_code >> 6;
    }

    opmode_t opmode() const
    {
        return items[opcode()].mode;
    }

    const char *name() const
    {
        return items[opcode()].name;
    }

private:
    uint32_t m_code;

    enum class oparg_t : uint8_t {
        OPARG_N = 0, //! argument is not used
        OPARG_U,     //! argument is used
        OPARG_R,     //! argument is a register or a jump offset
        OPARG_K,     //! argument is a constant or register/constant
    };

    struct instruction_item_t {
        bool is_test   : 1; //! operator is a test
        bool is_set_ra : 1; //! set register A
        oparg_t argb;
        oparg_t argc;
        opmode_t mode;
        const char *name;
    };

    static constexpr instruction_item_t items[] = {
        {0, 1, oparg_t::OPARG_R, oparg_t::OPARG_N, opmode_t::iABC, "MOVE    "},
        {0, 1, oparg_t::OPARG_K, oparg_t::OPARG_N, opmode_t::iABx, "LOADK   "},
        {0, 1, oparg_t::OPARG_N, oparg_t::OPARG_N, opmode_t::iABx, "LOADKX  "},
        {0, 1, oparg_t::OPARG_U, oparg_t::OPARG_U, opmode_t::iABC, "LOADBOOL"},
        {0, 1, oparg_t::OPARG_U, oparg_t::OPARG_N, opmode_t::iABC, "LOADNIL "},
        {0, 1, oparg_t::OPARG_U, oparg_t::OPARG_N, opmode_t::iABC, "GETUPVAL"},
        {0, 1, oparg_t::OPARG_U, oparg_t::OPARG_K, opmode_t::iABC, "GETTABUP"},
        {0, 1, oparg_t::OPARG_R, oparg_t::OPARG_K, opmode_t::iABC, "GETTABLE"},
        {0, 0, oparg_t::OPARG_K, oparg_t::OPARG_K, opmode_t::iABC, "SETTABUP"},
        {0, 0, oparg_t::OPARG_U, oparg_t::OPARG_N, opmode_t::iABC, "SETUPVAL"},
        {0, 0, oparg_t::OPARG_K, oparg_t::OPARG_K, opmode_t::iABC, "SETTABLE"},
        {0, 1, oparg_t::OPARG_U, oparg_t::OPARG_U, opmode_t::iABC, "NEWTABLE"},
        {0, 1, oparg_t::OPARG_R, oparg_t::OPARG_K, opmode_t::iABC, "SELF    "},
        {0, 1, oparg_t::OPARG_K, oparg_t::OPARG_K, opmode_t::iABC, "ADD     "},
        {0, 1, oparg_t::OPARG_K, oparg_t::OPARG_K, opmode_t::iABC, "SUB     "},
        {0, 1, oparg_t::OPARG_K, oparg_t::OPARG_K, opmode_t::iABC, "MUL     "},
        {0, 1, oparg_t::OPARG_K, oparg_t::OPARG_K, opmode_t::iABC, "MOD     "},
        {0, 1, oparg_t::OPARG_K, oparg_t::OPARG_K, opmode_t::iABC, "POW     "},
        {0, 1, oparg_t::OPARG_K, oparg_t::OPARG_K, opmode_t::iABC, "DIV     "},
        {0, 1, oparg_t::OPARG_K, oparg_t::OPARG_K, opmode_t::iABC, "IDIV    "},
        {0, 1, oparg_t::OPARG_K, oparg_t::OPARG_K, opmode_t::iABC, "BAND    "},
        {0, 1, oparg_t::OPARG_K, oparg_t::OPARG_K, opmode_t::iABC, "BOR     "},
        {0, 1, oparg_t::OPARG_K, oparg_t::OPARG_K, opmode_t::iABC, "BXOR    "},
        {0, 1, oparg_t::OPARG_K, oparg_t::OPARG_K, opmode_t::iABC, "SHL     "},
        {0, 1, oparg_t::OPARG_K, oparg_t::OPARG_K, opmode_t::iABC, "SHR     "},
        {0, 1, oparg_t::OPARG_R, oparg_t::OPARG_N, opmode_t::iABC, "UNM     "},
        {0, 1, oparg_t::OPARG_R, oparg_t::OPARG_N, opmode_t::iABC, "BNOT    "},
        {0, 1, oparg_t::OPARG_R, oparg_t::OPARG_N, opmode_t::iABC, "NOT     "},
        {0, 1, oparg_t::OPARG_R, oparg_t::OPARG_N, opmode_t::iABC, "LEN     "},
        {0, 1, oparg_t::OPARG_R, oparg_t::OPARG_R, opmode_t::iABC, "CONCAT  "},
        {0, 0, oparg_t::OPARG_R, oparg_t::OPARG_N, opmode_t::iAsBx,"JMP     "},
        {1, 0, oparg_t::OPARG_K, oparg_t::OPARG_K, opmode_t::iABC, "EQ      "},
        {1, 0, oparg_t::OPARG_K, oparg_t::OPARG_K, opmode_t::iABC, "LT      "},
        {1, 0, oparg_t::OPARG_K, oparg_t::OPARG_K, opmode_t::iABC, "LE      "},
        {1, 0, oparg_t::OPARG_N, oparg_t::OPARG_U, opmode_t::iABC, "TEST    "},
        {1, 1, oparg_t::OPARG_R, oparg_t::OPARG_U, opmode_t::iABC, "TESTSET "},
        {0, 1, oparg_t::OPARG_U, oparg_t::OPARG_U, opmode_t::iABC, "CALL    "},
        {0, 1, oparg_t::OPARG_U, oparg_t::OPARG_U, opmode_t::iABC, "TAILCALL"},
        {0, 0, oparg_t::OPARG_U, oparg_t::OPARG_N, opmode_t::iABC, "RETURN  "},
        {0, 1, oparg_t::OPARG_R, oparg_t::OPARG_N, opmode_t::iAsBx,"FORLOOP "},
        {0, 1, oparg_t::OPARG_R, oparg_t::OPARG_N, opmode_t::iAsBx,"FORPREP "},
        {0, 0, oparg_t::OPARG_N, oparg_t::OPARG_U, opmode_t::iABC, "TFORCALL"},
        {0, 1, oparg_t::OPARG_R, oparg_t::OPARG_N, opmode_t::iAsBx,"TFORLOOP"},
        {0, 0, oparg_t::OPARG_U, oparg_t::OPARG_U, opmode_t::iABC, "SETLIST "},
        {0, 1, oparg_t::OPARG_U, oparg_t::OPARG_N, opmode_t::iABx, "CLOSURE "},
        {0, 1, oparg_t::OPARG_U, oparg_t::OPARG_N, opmode_t::iABC, "VARARG  "},
        {0, 0, oparg_t::OPARG_U, oparg_t::OPARG_U, opmode_t::iAx,  "EXTRAARG"},
    };
};

using instruction_t = __detail::instruction;

} // namespace __detail


CPPLUA_NS_END

#endif
