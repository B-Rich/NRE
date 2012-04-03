/*
 * (c) 2012 Nils Asmussen <nils@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 */

#include <Log.h>
#include <Ports.h>

Log::Log() : Format() {
	Ports::out<uint8_t>(port + 1,0x00); // Disable all interrupts
	Ports::out<uint8_t>(port + 3,0x80); // Enable DLAB (set baud rate divisor)
	Ports::out<uint8_t>(port + 0,0x03); // Set divisor to 3 (lo byte) 38400 baud
	Ports::out<uint8_t>(port + 1,0x00); //                  (hi byte)
	Ports::out<uint8_t>(port + 3,0x03); // 8 bits, no parity, one stop bit
	Ports::out<uint8_t>(port + 2,0xC7); // Enable FIFO, clear them, with 14-byte threshold
	Ports::out<uint8_t>(port + 4,0x0B); // IRQs enabled, RTS/DSR set
}

void Log::printc(char c) {
	while((Ports::in<uint8_t>(port + 5) & 0x20) == 0)
		;
	Ports::out<uint8_t>(port,c);
}
