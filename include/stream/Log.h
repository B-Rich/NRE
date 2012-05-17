/*
 * TODO comment me
 *
 * Copyright (C) 2012, Nils Asmussen <nils@os.inf.tu-dresden.de>
 * Economic rights: Technische Universitaet Dresden (Germany)
 *
 * This file is part of NUL.
 *
 * NUL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * NUL is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details.
 */

#pragma once

#include <stream/OStream.h>
#include <stream/Screen.h>
#include <stream/Serial.h>

namespace nul {

class Log : public OStream {
public:
	static Log& get() {
		return _inst;
	}

	virtual void write(char c) {
		Screen::get().write(c);
		Serial::get().write(c);
	}

private:
	explicit Log() : OStream() {
	}

	static Log _inst;
};

}
