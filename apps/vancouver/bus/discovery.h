/*
 * (c) 2012 Nils Asmussen <nils@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 */

#pragma once

#include "bus.h"
#include "message.h"

/**
 * This template provides easy access to the Discovery Bus.
 */
template<typename Y>
class DiscoveryHelper {
protected:
	/*
	 * Write a string to a resource.
	 */
	bool discovery_write_st(const char *resource,unsigned offset,const void *value,unsigned count) {
		MessageDiscovery msg(resource,offset,value,count);
		return static_cast<Y*>(this)->_mb.bus_discovery.send(msg);
	}

	/**
	 * Write a dword or less than it.
	 */
	bool discovery_write_dw(const char *resource,unsigned offset,unsigned value,unsigned count = 4) {
		return discovery_write_st(resource,offset,&value,count);
	}

	/**
	 * Read a dword.
	 */
	bool discovery_read_dw(const char *resource,unsigned offset,unsigned &value) {
		MessageDiscovery msg(resource,offset,&value);
		return static_cast<Y*>(this)->_mb.bus_discovery.send(msg);
	}

	/**
	 * Return the length of an ACPI table or minlen if it is smaller.
	 */
	unsigned discovery_length(const char *resource,unsigned minlen) {
		unsigned res;
		if(!discovery_read_dw(resource,4,res) || res < minlen)
			return minlen;
		return res;
	}

public:
	static bool discover(Device *o,MessageDiscovery &msg) {
		if(msg.type != MessageDiscovery::DISCOVERY)
			return false;
		static_cast<Y*>(o)->discovery();
		return true;
	}
};
