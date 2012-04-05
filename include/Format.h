/*
 * (c) 2012 Nils Asmussen <nils@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 */

#pragma once

#include <stdarg.h>
#include <Types.h>

class Format {
private:
	enum {
		PADRIGHT	= 1 << 0,
		FORCESIGN	= 1 << 1,
		SPACESIGN	= 1 << 2,
		PRINTBASE	= 1 << 3,
		PADZEROS	= 1 << 4,
		CAPHEX		= 1 << 5,
		LONGLONG	= 1 << 6,
		LONG		= 1 << 7,
		SIZE_T		= 1 << 8,
		INTPTR_T	= 1 << 9,
	};

public:
	explicit Format() {
	}
	virtual ~Format() {
	}

	void print(const char *fmt,...);
	void vprint(const char *fmt, va_list ap);

protected:
	virtual void printc(char c) = 0;

private:
	void printnpad(llong n, uint pad, uint flags);
	void printupad(ullong u, uint base, uint pad, uint flags);
	int printpad(int count, uint flags);
	int printu(ullong n, uint base, char *chars);
	int printn(llong n);
	int puts(const char *str);

private:
	static char _hexchars_big[];
	static char _hexchars_small[];
};