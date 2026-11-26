#include "Machine.hpp"
#include <algorithm>


namespace Chip8 {
    Machine::Machine(std::uint32_t seed)
        : rng_(seed)
    {
        std::copy(FONT.begin(), FONT.end(), memory_.begin() + FONT_START);         
        
        tone_samples.clear();
        for (int i = 0; i < sample_periods; i++)
        {
            const int16_t level = (i < (sample_periods >> 1)) ? 16000 : -16000;
            tone_samples.push_back(level);
            tone_samples.push_back(level);
        }

        channels_map = {
            sf::SoundChannel::FrontLeft,
            sf::SoundChannel::FrontRight,
        };

        buzzer_buf_ = std::make_unique<sf::SoundBuffer>();
        bool ok = buzzer_buf_->loadFromSamples(
            tone_samples.data(),
            static_cast<std::uint64_t>(tone_samples.size()),
            channels,
            static_cast<unsigned int>(sample_rate),
            channels_map
        );

        if (ok){
            buzzer_snd_ = std::make_unique<sf::Sound>(*buzzer_buf_);
            buzzer_snd_->setLooping(true);
            audio_ready_ = true;
        }    
    }

    LoadResult Machine::Load(std::span<const std::uint8_t> rom)
    {
        if (rom.size() > MEMORY_SIZE - PROGRAM_START)
            return LoadResult::TooLarge;

        std::copy(rom.begin(), rom.end(), memory_.begin() + PROGRAM_START);
        regs_.pc = PROGRAM_START;
        return LoadResult::Ok;
    }

    void Machine::TickTimers()
    {
        if (regs_.delay_timer > 0) regs_.delay_timer--;
        if (regs_.sound_timer > 0) regs_.sound_timer--;
        vblank_ = true;
    }

    void Machine::SetKey(std::uint8_t key, bool down)
    {
        keys_[key & 0xF] = down;
    }

    std::uint16_t Machine::Fetch() {
        const std::uint16_t hi = memory_[regs_.pc & MEMORY_MASK];
        const std::uint16_t lo = memory_[(regs_.pc + 1) & MEMORY_MASK];
        regs_.pc = static_cast<std::uint16_t>((regs_.pc + 2) & MEMORY_MASK);
        return static_cast<std::uint16_t>((hi << 8) | lo);
    }

    AudioResult Machine::PlaySound()
    {   
        if (!audio_ready_ || !buzzer_snd_) return AudioResult::Error;

        const auto buzzer_status = buzzer_snd_->getStatus();
        if (buzzer_status != sf::SoundSource::Status::Playing)
            buzzer_snd_->play();
        
        return AudioResult::Ok;
    }
    AudioResult Machine::StopSound()
    {  
        if (!audio_ready_ || !buzzer_snd_)
            return AudioResult::Error;

        if (buzzer_snd_)
            buzzer_snd_->stop();
    
        return AudioResult::Ok;
    }

    StepResult Machine::Step()
    {
        const Instruction ins{ Fetch() };

        switch (ins.family)
        {
            case 0x0:
            {
                if (ins.Opcode == 0x00E0) { display_.fill(0); return StepResult::Ok; }
                else if (ins.Opcode == 0x00EE) {
                    if (regs_.sp == 0) return StepResult::StackUnderflow;

                    regs_.sp--;
                    regs_.pc = stack_[regs_.sp];
                } else {
                    return StepResult::UnknownOpcode;
                }

                break;
            }
            case 0x1:
            {
                regs_.pc = (ins.nnn);
                break;
            }
            case 0x2:
            {
                if (regs_.sp >= stack_.size()) return StepResult::StackOverflow;
                stack_[regs_.sp] = regs_.pc;
                regs_.sp++;
                regs_.pc = (ins.nnn);
                break;
            }
            case 0x3:
            {
                if (v_[ins.x] == ins.nn)
                    regs_.pc += 2;
                break;
            }
            case 0x4:
            {
                if (v_[ins.x] != ins.nn) regs_.pc += 2;
                break;
            }
            case 0x5:
            {
                if (v_[ins.x] == v_[ins.y]) regs_.pc += 2;
                break;
            }
            case 0x6:
            {
                v_[ins.x] = ins.nn;
                break;
            }
            case 0x7:
            {
                v_[ins.x] += ins.nn;
                break;
            }
            case 0x8:
            {
                switch (ins.n)
                {
                    case 0:
                    {
                        v_[ins.x] = v_[ins.y];
                        break;
                    }
                    case 1:
                    {
                        v_[ins.x] = v_[ins.x] | v_[ins.y];
                        v_[VF] = 0;
                        break;
                    }
                    case 2:
                    {
                        v_[ins.x] = v_[ins.x] & v_[ins.y];
                        v_[VF] = 0;
                        break;
                    }
                    case 3:
                    {
                        v_[ins.x] = v_[ins.x] ^ v_[ins.y];
                        v_[VF] = 0;
                        break;
                    }
                    case 4:
                    {
                        uint16_t sum = v_[ins.x] + v_[ins.y];
                        v_[ins.x] = sum & 0xFF;
                        v_[VF] = (sum > 0xFF) ? 1 : 0;
                        break;
                    }
                    case 5:
                    {
                        uint8_t vx = v_[ins.x], vy = v_[ins.y];
                        v_[ins.x] = vx - vy;
                        v_[VF] = (vx >= vy) ? 1 : 0;
                        break;
                    }
                    case 6:
                    {
                        const std::uint8_t src = v_[ins.y];
                        v_[ins.x] = static_cast<std::uint8_t>(src >> 1);
                        v_[VF] = src & 1;
                        break;
                    }
                    case 7:
                    {
                        uint8_t vx = v_[ins.x], vy = v_[ins.y];
                        v_[ins.x] = (vy - vx);
                        v_[VF] = (vy >= vx) ? 1 : 0;
                        break;
                    }
                    case 0xE:
                    {
                        const std::uint8_t src = v_[ins.y];
                        v_[ins.x] = static_cast<uint8_t>(src << 1); 
                        v_[VF] = (src >> 7) & 1;
                        break;
                    }
                    default: {
                        return StepResult::UnknownOpcode;
                    }
                }
                break;
            }
            case 0x9:
            {
                if (v_[ins.x] != v_[ins.y]) regs_.pc += 2;
                break;
            }
            case 0xA:
            {
                regs_.I = ins.nnn;
                break;
            }
            case 0xB:
            {
                regs_.pc = (ins.nnn + v_[0]) & 0xFFF;
                break;
            }
            case 0xC:
            {
                v_[ins.x] = static_cast<std::uint8_t>(rng_() & ins.nn);
                break;
            }
            case 0xD:
            {
                if (!vblank_) {
                    regs_.pc -= 2;
                    return StepResult::Ok;
                }
                vblank_ = false;

                uint8_t x_idx = (ins.Opcode >> 8) & 0x0F;
                uint8_t y_idx = (ins.Opcode >> 4) & 0x0F;
                uint8_t h = ins.Opcode & 0x0F;

                uint8_t pos_x = v_[x_idx] % DISPLAY_WIDTH;
                uint8_t pos_y = v_[y_idx] % DISPLAY_HEIGHT;
                v_[VF] = 0;

                for (uint8_t row = 0; row < h; row++)
                {
                    if (pos_y + row >= DISPLAY_HEIGHT) break;

                    uint16_t addr = (regs_.I + row) & 0xFFF;
                    uint8_t sprite_byte = memory_[addr];

                    for (uint8_t bit = 0; bit < 8; bit++)
                    {
                        if (pos_x + bit >= DISPLAY_WIDTH) break;
                        if ((sprite_byte >> (7 - bit)) & 1)
                        {
                            size_t idx = (pos_y + row) * DISPLAY_WIDTH + (pos_x + bit);
                            if (display_[idx]) v_[VF] = 1;
                            display_[idx] ^= 1;
                        }
                    }
                }
                break;
            }
            case 0xE:
            {
                if (ins.nn == 0x9E) {
                    if (keys_[v_[ins.x] & 0xF])
                        regs_.pc += 2;
                }
                else if (ins.nn == 0xA1)
                {
                    if (!keys_[v_[ins.x] & 0xF])
                        regs_.pc += 2;
                }
                else return StepResult::UnknownOpcode;
                break;
            }
            case 0xF:
            {
                switch (ins.nn)
                {
                    case 0x07:
                    {
                        v_[ins.x] = regs_.delay_timer;
                        break;
                    }
                    case 0x0A:
                    {
                        bool found = false;
                        for (uint8_t i = 0; i < 16; i++)
                        {
                            if(keys_[i])
                            {
                                v_[ins.x] = i;
                                found = true;
                                break;
                            }
                        }

                        if (!found) regs_.pc -= 2;

                        return StepResult::WaitingForKey;
                    }
                    case 0x15:
                    {
                        regs_.delay_timer = v_[ins.x];
                        break;
                    }
                    case 0x18:
                    {
                        regs_.sound_timer = v_[ins.x];
                        break;
                    }
                    case 0x1E:
                    {
                        uint16_t sum = regs_.I + v_[ins.x];
                        regs_.I = sum & 0xFFF;
                        break;
                    }
                    case 0x29:
                    {
                        regs_.I = FONT_START + 5 * (v_[ins.x] & 0xF);
                        break;
                    }
                    case 0x33:
                    {
                        uint8_t vx = v_[ins.x];
                        memory_[regs_.I & 0xFFF] = vx / 100;
                        memory_[(regs_.I + 1) & 0xFFF] = (vx / 10) % 10;
                        memory_[(regs_.I + 2) & 0xFFF] = vx % 10;

                        break;
                    }
                    case 0x55:
                    {
                        for (uint8_t i = 0; i <= ins.x; i++)
                            memory_[(regs_.I + i) & 0xFFF] = v_[i];
                        regs_.I = static_cast<std::uint16_t>((regs_.I + ins.x + 1) & MEMORY_MASK);
                        break;
                    }
                    case 0x65:
                    {
                        for (uint8_t i = 0; i <= ins.x; i++)
                            v_[i] = memory_[(regs_.I + i) & 0xFFF];
                        regs_.I = static_cast<std::uint16_t>((regs_.I + ins.x + 1) & MEMORY_MASK);
                        break;
                    }
                    default:
                    {
                        return StepResult::UnknownOpcode;
                    }
                }
                break;
            }
            default:
            {
                return StepResult::UnknownOpcode;
            }
        }

        return StepResult::Ok;
    }


}   
