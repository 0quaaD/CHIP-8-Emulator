#pragma once

#include <cstdint>

namespace Chip8 {

    struct Instruction
    {
        constexpr explicit Instruction(std::uint16_t op) noexcept
            : Opcode(op),
              family(static_cast<std::uint8_t>(op >> 12)),
              x(static_cast<std::uint8_t>((op >> 8) & 0xF)),
              y(static_cast<std::uint8_t>((op >> 4) & 0xF)),
              n(static_cast<std::uint8_t>(op & 0xF)),
              nn(static_cast<std::uint8_t>(op & 0xFF)),
              nnn(static_cast<std::uint16_t>(op & 0xFFF)) {}

        uint16_t Opcode;
        uint8_t family, x, y, n, nn;
        uint16_t nnn; 
    };

    static_assert(Instruction(0x8A42).family    == 0x8);
    static_assert(Instruction(0x8A42).x         == 0xA);
    static_assert(Instruction(0x8A42).y         == 0x4);
    static_assert(Instruction(0x8A42).n         == 0x2);
    static_assert(Instruction(0x8A42).nn        == 0x42);
    static_assert(Instruction(0x8A42).nnn       == 0xA42);
} // namespace Chip8
