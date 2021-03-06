/*
 * This file is part of the CHIP-8 emulator (https://github.com/VladimirMakeev/CHIP-8).
 * Copyright (C) 2017 Vladimir Makeev.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <array>
#include <random>
#include <unordered_map>

namespace chip8 {

class Memory;
class Display;
class Keyboard;

class CPU
{
public:
	CPU(Memory &memory, Display &display, Keyboard &keyboard);

	void reset();
	// execute single instruction
	void execute();
	void updateTimers();

private:
	uint16_t fetch();
	uint8_t rand() const;

	// instruction handlers
	void ins_misc(uint16_t opcode);
	void ins_arith(uint16_t opcode);
	void ins_key(uint16_t opcode);
	void ins_special(uint16_t opcode);

	// jump to machine code routine at nnn
	void ins_0nnn(uint16_t opcode);
	// clear the display
	void ins_00e0(uint16_t opcode);
	// return from a subroutine
	void ins_00ee(uint16_t opcode);
	// jump to nnn
	void ins_1nnn(uint16_t opcode);
	// call subroutine at nnn
	void ins_2nnn(uint16_t opcode);
	// skip next instruction if vx == nn
	void ins_3xnn(uint16_t opcode);
	// skip next instruction if vx != nn
	void ins_4xnn(uint16_t opcode);
	// skip next instruction if vx == vy
	void ins_5xy0(uint16_t opcode);
	// set vx = nn
	void ins_6xnn(uint16_t opcode);
	// set vx = vx + nn
	void ins_7xnn(uint16_t opcode);
	// set vx = vy
	void ins_8xy0(uint16_t opcode);
	// set vx = vx | vy
	void ins_8xy1(uint16_t opcode);
	// set vx = vx & vy
	void ins_8xy2(uint16_t opcode);
	// set vx = vx ^ vy
	void ins_8xy3(uint16_t opcode);
	// set vx = vx + vy, set vf = carry
	void ins_8xy4(uint16_t opcode);
	// set vx = vx - vy, set vf = !borrow
	void ins_8xy5(uint16_t opcode);
	// set vx = vx >> 1
	void ins_8xy6(uint16_t opcode);
	// set vx = vy - vx, set vf = !borrow
	void ins_8xy7(uint16_t opcode);
	// set vx = vx << 1
	void ins_8xye(uint16_t opcode);
	// skip next instruction if vx != vy
	void ins_9xy0(uint16_t opcode);
	// set i = nnn
	void ins_annn(uint16_t opcode);
	// jump to nnn + v0
	void ins_bnnn(uint16_t opcode);
	// set vx = rand() & nn
	void ins_cxnn(uint16_t opcode);
	// display n-byte sprite starting at memory location i
	void ins_dxyn(uint16_t opcode);
	// skip next instruction if key with the value of vx is pressed
	void ins_ex9e(uint16_t opcode);
	// skip next instruction if key with the value of vx is not pressed
	void ins_exa1(uint16_t opcode);
	// set vx = dt
	void ins_fx07(uint16_t opcode);
	// wait for a key press, store the value of the key in vx
	void ins_fx0a(uint16_t opcode);
	// set dt = vx
	void ins_fx15(uint16_t opcode);
	// set st = vx
	void ins_fx18(uint16_t opcode);
	// set i = i + vx
	void ins_fx1e(uint16_t opcode);
	// set i = memory location of sprite for digit vx
	void ins_fx29(uint16_t opcode);
	// store BCD representation of vx in memory
	void ins_fx33(uint16_t opcode);
	// store registers v0 through vx in memory starting at i
	void ins_fx55(uint16_t opcode);
	// read registers v0 through vx from memory starting at i
	void ins_fx65(uint16_t opcode);
	
	uint16_t opcodeGetNNN(uint16_t opcode) const
	{
		return opcode & 0xfff;
	}

	uint8_t opcodeGetNN(uint16_t opcode) const
	{
		return opcode & 0xff;
	}

	uint8_t opcodeGetN(uint16_t opcode) const
	{
		return opcode & 0xf;
	}

	uint8_t opcodeGetX(uint16_t opcode) const
	{
		return (opcode >> 8) & 0xf;
	}

	uint8_t opcodeGetY(uint16_t opcode) const
	{
		return (opcode >> 4) & 0xf;
	}


	mutable std::mt19937 engine;
	mutable std::uniform_int_distribution<uint8_t> dist;

	Memory &memory;
	Display &display;
	Keyboard &keyboard;

	using InstructionHandler = void (CPU::*)(uint16_t);

	std::array<InstructionHandler, 16> instructions;
	// arithmetic instructions 8xy0 - 8xye
	std::array<InstructionHandler, 9> arithmetic;
	// special instructions fx07 - fx65
	std::unordered_map<uint8_t, InstructionHandler> special;

	std::array<uint16_t, 16> stack;
	// general purpose registers
	std::array<uint8_t, 16> v;
	// I register
	uint16_t i;
	// program counter
	uint16_t pc;
	// delay timer
	uint8_t dt;
	// sound timer
	uint8_t st;
	// stack pointer
	uint8_t sp;
};

}

#endif /* CPU_HPP */

