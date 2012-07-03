/*
 * (c) 2012 Nils Asmussen <nils@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 */

#pragma once

class HostRebootMethod {
public:
	explicit HostRebootMethod() {
	}
	virtual ~HostRebootMethod() {
	}

	virtual void reboot() = 0;
};
