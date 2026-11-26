#pragma once

#include <cstdint>
#include <random>
#include <span>
#include <memory>

#include "Config.hpp"
#include "Instruction.hpp"

namespace Chip8 {
    enum class LoadResult { Ok, TooLarge };
    enum class StepResult { Ok, WaitingForKey, UnknownOpcode, StackOverflow, StackUnderflow };
    enum class AudioResult { Ok, Error };
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
    
    std::unique_ptr<sf::Sound>          buzzer_snd_;
    std::unique_ptr<sf::SoundBuffer>    buzzer_buf_;
    std::vector<std::int16_t>           tone_samples;
    std::vector<sf::SoundChannel>       channels_map;

    const int sample_rate = 48000;
    const int tone_freq = 440;
    const int channels = 2;
    const int sample_periods = sample_rate / tone_freq;
    bool audio_ready_ = false;
    
public:
    explicit Machine(std::uint32_t seed = std::random_device{}());
    LoadResult Load(std::span<const std::uint8_t> rom);
    StepResult Step();

    AudioResult PlaySound();
    AudioResult StopSound();
    
    void TickTimers();
    void SetKey(std::uint8_t key, bool down);
    
    const std::array<std::uint8_t, DISPLAY_HEIGHT * DISPLAY_WIDTH>& Display() const { return display_; }
    bool SoundActive() const { return regs_.sound_timer > 0; }
};


}
