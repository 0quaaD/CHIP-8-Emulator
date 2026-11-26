#include <SFML/Graphics.hpp>
#include "Machine.hpp"

#include <iostream>
#include <cstdint>
#include <iterator>
#include <fstream>
#include <string>
#include <optional>
#include <vector>

constexpr float SCALE = 12.f;
constexpr int CYCLES_PER_FRAME = 10;

constexpr sf::Keyboard::Key KEYMAP[16] = {
	sf::Keyboard::Key::Num1,    sf::Keyboard::Key::Num2, sf::Keyboard::Key::Num3, sf::Keyboard::Key::Num4, 		// 1 2 3 C
	sf::Keyboard::Key::Q,    sf::Keyboard::Key::W,    sf::Keyboard::Key::E,    sf::Keyboard::Key::R,    		// 4 5 6 D
	sf::Keyboard::Key::A,    sf::Keyboard::Key::S,    sf::Keyboard::Key::D,    sf::Keyboard::Key::F,    		// 7 8 9 E
	sf::Keyboard::Key::Z, 	 sf::Keyboard::Key::X,    sf::Keyboard::Key::C,    sf::Keyboard::Key::V     		// A 0 B F
};

template<typename T>
T ReadFile(const std::string& path)
{
	std::ifstream file(path, std::ios::binary);
	if (!file) return std::nullopt;
	return std::vector<std::uint8_t>(
                std::istreambuf_iterator<char>(file),
                std::istreambuf_iterator<char>()
        );
}

int main(int argc, char** argv)
{
	if (argc < 2) { std::cerr << "usage: Chip8 <rom>\n"; return 1; }
	const auto rom = ReadFile<std::optional<std::vector<std::uint8_t>>>(argv[1]);
	if (!rom) { std::cerr << "couldn't read " << argv[1] <<"\n"; return 1; }

	Chip8::Machine machine;
	if (machine.Load(*rom) != Chip8::LoadResult::Ok)
	{
		std::cerr << "ROM too large to load!\n";
		return 1;
	}

	sf::RenderWindow window(
		sf::VideoMode({ static_cast<unsigned>(Chip8::DISPLAY_WIDTH * SCALE),
		                static_cast<unsigned>(Chip8::DISPLAY_HEIGHT * SCALE) }),
		                "CHIP-8 Emulator"
        );
	window.setFramerateLimit(60);

	sf::RectangleShape pixel({ SCALE, SCALE });
	pixel.setFillColor(sf::Color::White);

	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
			if (event->is<sf::Event::Closed>())
				window.close();

		for (int k = 0; k < 16; k++)
			machine.SetKey(
                                static_cast<std::uint8_t>(k), 
                                sf::Keyboard::isKeyPressed(KEYMAP[k])
                        );

		for (int i = 0; i < CYCLES_PER_FRAME; i++)
		{
			const Chip8::StepResult r = machine.Step();
			if (r == Chip8::StepResult::UnknownOpcode ||
				r == Chip8::StepResult::StackOverflow ||
				r == Chip8::StepResult::StackUnderflow)
			{
					std::cerr << "Emulation stopped (error code " << static_cast<int>(r) << ")\n";
					window.close();
					break;
			}
		}
		machine.TickTimers();

		window.clear(sf::Color::Black);
		const auto& display = machine.Display();
		for (int y = 0; y < Chip8::DISPLAY_HEIGHT; y++)
			for (int x = 0; x < Chip8::DISPLAY_WIDTH; x++)
				if (display[y * Chip8::DISPLAY_WIDTH + x])
				{
					pixel.setPosition({ x * SCALE, y * SCALE });
					window.draw(pixel);
				}
		window.display();
	}
	return 0;
}
