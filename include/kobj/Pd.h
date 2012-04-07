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

#include <arch/Startup.h>
#include <kobj/KObject.h>
#include <cap/CapSpace.h>
#include <cap/ResourceSpace.h>

namespace nul {

class Pd : public KObject {
	friend void ::_setup();

	explicit Pd(cap_t cap,bool) : KObject(this,cap), _io(0,0), _mem(0,0), _obj() {
	}

public:
	static Pd *current();

	explicit Pd(cap_t cap) : KObject(current(),cap), _io(0,0), _mem(0,0), _obj() {
	}

	ResourceSpace &io() {
		return _io;
	}
	ResourceSpace &mem() {
		return _mem;
	}
	CapSpace &obj() {
		return _obj;
	}

private:
	Pd(const Pd&);
	Pd& operator=(const Pd&);

	ResourceSpace _io;
	ResourceSpace _mem;
	CapSpace _obj;
};

}
