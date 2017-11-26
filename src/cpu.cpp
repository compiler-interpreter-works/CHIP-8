
#include "cpu.hpp"
#include "memory.hpp"

namespace chip8 {

CPU::CPU(Memory &memory) : memory(memory)
{
}

void CPU::reset()
{
	stack.fill(0);
	v.fill(0);
	i = 0;
	pc = memory.prgStart();
	sp = 0;
	dt = 0;
	st = 0;
}

void CPU::execute()
{
}

void CPU::updateTimers()
{
	if (dt) {
		dt--;
	}

	if (st) {
		st--;
	}
}

void CPU::ins_0nnn(uint16_t)
{
	// unsupported
}

void CPU::ins_00e0(uint16_t)
{
}

void CPU::ins_00ee(uint16_t)
{
	pc = stack[sp--];
}

void CPU::ins_1nnn(uint16_t opcode)
{
	pc = opcodeGetNNN(opcode);
}

void CPU::ins_2nnn(uint16_t opcode)
{
	stack[++sp] = pc;

	pc = opcodeGetNNN(opcode);
}

void CPU::ins_3xnn(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);
	const uint8_t nn = opcodeGetNN(opcode);

	if (v[x] == nn) {
		pc += 2;
	}
}

void CPU::ins_4xnn(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);
	const uint8_t nn = opcodeGetNN(opcode);

	if (v[x] != nn) {
		pc += 2;
	}
}

void CPU::ins_5xy0(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);
	const uint8_t y = opcodeGetY(opcode);

	if (v[x] == v[y]) {
		pc += 2;
	}
}

void CPU::ins_6xnn(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);
	const uint8_t nn = opcodeGetNN(opcode);

	v[x] = nn;
}

void CPU::ins_7xnn(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);
	const uint8_t nn = opcodeGetNN(opcode);

	v[x] += nn;
}

void CPU::ins_8xy0(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);
	const uint8_t y = opcodeGetY(opcode);

	v[x] = v[y];
}

void CPU::ins_8xy1(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);
	const uint8_t y = opcodeGetY(opcode);

	v[x] |= v[y];
}

void CPU::ins_8xy2(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);
	const uint8_t y = opcodeGetY(opcode);

	v[x] &= v[y];
}

void CPU::ins_8xy3(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);
	const uint8_t y = opcodeGetY(opcode);

	v[x] ^= v[y];
}

void CPU::ins_8xy4(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);
	const uint8_t y = opcodeGetY(opcode);

	const uint16_t sum = (uint16_t)v[x] + v[y];

	v[x] = sum & 0xff;
	// set carry
	v[0xf] = sum > 0xff;
}

void CPU::ins_8xy5(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);
	const uint8_t y = opcodeGetY(opcode);

	// set !borrow
	v[0xf] = v[x] >= v[y];

	v[x] -= v[y];
}

void CPU::ins_8xy6(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);

	v[0xf] = v[x] & 0x1;

	v[x] >>= 1;
}

void CPU::ins_8xy7(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);
	const uint8_t y = opcodeGetY(opcode);

	// set !borrow
	v[0xf] = v[y] >= v[x];

	v[x] = v[y] - v[x];
}

void CPU::ins_8xye(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);

	v[0xf] = (v[x] & 0x80) != 0;

	v[x] <<= 1;
}

void CPU::ins_9xy0(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);
	const uint8_t y = opcodeGetY(opcode);

	if (v[x] != v[y]) {
		pc += 2;
	}
}

void CPU::ins_annn(uint16_t opcode)
{
	i = opcodeGetNNN(opcode);
}

void CPU::ins_bnnn(uint16_t opcode)
{
	pc = opcodeGetNNN(opcode) + v[0];
}

void CPU::ins_cxnn(uint16_t)
{
}

void CPU::ins_dxyn(uint16_t)
{
}

void CPU::ins_ex9e(uint16_t)
{
}

void CPU::ins_exa1(uint16_t)
{
}

void CPU::ins_fx07(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);
	
	v[x] = dt;
}

void CPU::ins_fx0a(uint16_t)
{
}

void CPU::ins_fx15(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);

	dt = v[x];
}

void CPU::ins_fx18(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);

	st = v[x];
}

void CPU::ins_fx1e(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);

	i += v[x];
}

void CPU::ins_fx29(uint16_t)
{
}

void CPU::ins_fx33(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);
	const uint8_t vx = v[x];
	const uint8_t hundreds = vx / 100;
	const uint8_t tens = (vx / 10) % 10;
	const uint8_t ones = vx % 10;

	memory[i] = hundreds;
	memory[i + 1] = tens;
	memory[i + 2] = ones;
}

void CPU::ins_fx55(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);

	for (uint8_t reg = 0; reg <= x; reg++) {
		memory[i + reg] = v[reg];
	}
}

void CPU::ins_fx65(uint16_t opcode)
{
	const uint8_t x = opcodeGetX(opcode);

	for (uint8_t reg = 0; reg <= x; reg++) {
		v[reg] = memory[i + reg];
	}
}

}

