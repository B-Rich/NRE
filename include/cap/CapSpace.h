/*
 * Copyright (C) 2012, Nils Asmussen <nils@os.inf.tu-dresden.de>
 * Economic rights: Technische Universitaet Dresden (Germany)
 *
 * This file is part of NRE (NOVA runtime environment).
 *
 * NRE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * NRE is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details.
 */

#pragma once

#include <arch/SpinLock.h>
#include <arch/Types.h>
#include <Exception.h>
#include <util/ScopedLock.h>
#include <Hip.h>

namespace nre {

class CapException : public Exception {
public:
	explicit CapException(ErrorCode code) throw() : Exception(code) {
	}
};

class CapSpace {
public:
	enum Caps {
		EV_DIVIDE		= 0x0,
		EV_DEBUG		= 0x1,
		EV_BREAKPOINT	= 0x3,
		EV_OVERFLOW		= 0x4,
		EV_BOUNDRANGE	= 0x5,
		EV_UNDEFOP		= 0x6,
		EV_NOMATHPROC	= 0x7,
		EV_DBLFAULT		= 0x8,
		EV_TSS			= 0xA,
		EV_INVSEG		= 0xB,
		EV_STACK		= 0xC,
		EV_GENPROT		= 0xD,
		EV_PAGEFAULT	= 0xE,
		EV_MATHFAULT	= 0x10,
		EV_ALIGNCHK		= 0x11,
		EV_MACHCHK		= 0x12,
		EV_SIMD			= 0x13,
		EV_STARTUP		= 0x1E,
		EV_RECALL		= 0x1F,

		INIT_PD			= 0x20,
		INIT_EC			= 0x21,
		INIT_SC			= 0x22,

		SRV_INIT		= 0x23,		// get initial caps
		SRV_REG			= 0x24,		// register service
		SRV_UNREG		= 0x25,		// unregister service
		SRV_GET			= 0x26,		// get service
		SRV_IO			= 0x27,		// alloc/release io ports
		SRV_GSI			= 0x28,		// alloc/release global system interrupt
		SRV_DS			= 0x29,		// dataspace portal
	};

	static CapSpace& get() {
		return _inst;
	}

private:
	explicit CapSpace(capsel_t off = Hip::get().object_caps()) : _lck(), _off(off) {
	}

public:
	capsel_t allocate(uint count = 1,uint align = 1) {
		ScopedLock<SpinLock> lock(&_lck);
		capsel_t res = (_off + align - 1) & ~(align - 1);
		if(res + count < res || res + count > Hip::get().cfg_cap)
			throw CapException(E_NO_CAP_SELS);
		_off = res + count;
		return res;
	}
	void free(capsel_t,uint = 1) {
		// TODO implement me
	}

private:
	CapSpace(const CapSpace&);
	CapSpace& operator=(const CapSpace&);

	static CapSpace _inst;
	SpinLock _lck;
	capsel_t _off;
};

}
