#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

namespace Chip8 {

    inline constexpr std::size_t MEMORY_SIZE =          0x1000;
    inline constexpr std::size_t MEMORY_MASK =          MEMORY_SIZE - 1;
    inline constexpr std::uint16_t FONT_START =         0x050;
    inline constexpr std::uint16_t PROGRAM_START =      0x200;
    inline constexpr std::uint8_t DISPLAY_WIDTH =       0x40;
    inline constexpr std::uint8_t DISPLAY_HEIGHT =      0x20;
    inline constexpr std::uint8_t VF =                  0xF;

    inline constexpr std::array<std::uint8_t, 80> FONT = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F

    };
    constexpr sf::Keyboard::Key KEYMAP[16] = {
        sf::Keyboard::Key::X,    		sf::Keyboard::Key::Num1, sf::Keyboard::Key::Num2, sf::Keyboard::Key::Num3, 		// 1 2 3 C
        sf::Keyboard::Key::Q,    		sf::Keyboard::Key::W,    sf::Keyboard::Key::E,    sf::Keyboard::Key::A,    		// 4 5 6 D
        sf::Keyboard::Key::S,    		sf::Keyboard::Key::D,    sf::Keyboard::Key::Z,    sf::Keyboard::Key::C,    		// 7 8 9 E
        sf::Keyboard::Key::Num4, 		sf::Keyboard::Key::R,    sf::Keyboard::Key::F,    sf::Keyboard::Key::V     		// A 0 B F
    };

static_assert(FONT_START + FONT.size() <= PROGRAM_START, "Font overlaps program memory.");
static_assert(PROGRAM_START < MEMORY_SIZE, "Program starts outside of memory.");
} // namespace Chip8
