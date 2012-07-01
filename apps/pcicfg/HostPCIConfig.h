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

#include <kobj/Ports.h>
#include <kobj/UserSm.h>
#include <util/ScopedLock.h>

#include "Config.h"

class HostPCIConfig : public Config {
	enum {
		PORT_ADDR	= 0xCF8,
		PORT_DATA	= 0xCFC
	};

public:
	explicit HostPCIConfig() : _sm(), _addr(PORT_ADDR,4), _data(PORT_DATA,4) {
	}

	virtual bool contains(uint32_t bdf,size_t offset) const {
		return offset < 0x40 && bdf < 0x10000;
	}
	virtual uint32_t read(uint32_t bdf,size_t offset) {
		nul::ScopedLock<nul::UserSm> guard(&_sm);
		select(bdf,offset);
		return _data.in<uint32_t>();
	}
	virtual void write(uint32_t bdf,size_t offset,uint32_t value) {
		nul::ScopedLock<nul::UserSm> guard(&_sm);
		select(bdf,offset);
		_data.out<uint32_t>(value);
	}

private:
	void select(uint32_t bdf,size_t offset) {
		uint32_t addr = 0x80000000 | (bdf << 8) | (offset & 0xFC);
		_addr.out<uint32_t>(addr);
	}

    nul::UserSm _sm;
	nul::Ports _addr;
	nul::Ports _data;
};
