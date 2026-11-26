#pragma once

#include <cstdint>
#include <random>
#include <span>

#include "Config.hpp"
#include "Instruction.hpp"


namespace Chip8 {
    enum class LoadResult { Ok, TooLarge };
    enum class StepResult { Ok, WaitingForKey, UnknownOpcode, StackOverflow, StackUnderflow };

class Machine {
private:
    struct SystemRegs {
        constexpr SystemRegs(uint16_t _pc = PROGRAM_START, uint16_t _i = 0) noexcept 
            : pc(_pc), I(_i) {}
        
        uint16_t pc;
        uint16_t I;
        uint8_t sp = 0;
        uint8_t delay_timer = 0;
        uint8_t sound_timer = 0;
    };

    bool vblank_ = false;
    
    SystemRegs regs_{};
    std::array<std::uint8_t, 16>                                v_{};
    std::array<std::uint8_t, MEMORY_SIZE>                       memory_{};
    std::array<std::uint8_t, DISPLAY_HEIGHT * DISPLAY_WIDTH>    display_{};
    std::array<std::uint16_t, 16>                               stack_{};
    std::array<bool, 16>                                        keys_{};
    std::mt19937                                                rng_;

    std::uint16_t Fetch();
    
public:
    explicit Machine(std::uint32_t seed = std::random_device{}());
    LoadResult Load(std::span<const std::uint8_t> rom);
    StepResult Step();
    void TickTimers();
    void SetKey(std::uint8_t key, bool down);

    const std::array<std::uint8_t, DISPLAY_HEIGHT * DISPLAY_WIDTH>& Display() const { return display_; }
    bool SoundActive() const { return regs_.sound_timer > 0; }
};


}
