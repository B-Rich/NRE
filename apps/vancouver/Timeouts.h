/*
 * (c) 2012 Nils Asmussen <nils@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 */

#pragma once

#include <kobj/UserSm.h>
#include <kobj/GlobalThread.h>
#include <kobj/Sc.h>
#include <services/Timer.h>
#include <util/TimeoutList.h>
#include <util/ScopedLock.h>

#include "bus/motherboard.h"

class Timeouts {
	enum {
		NO_TIMEOUT	= ~0ULL
	};

public:
	Timeouts(Motherboard &mb) : _mb(mb), _timeouts(), _timercon("timer"), _timer(_timercon),
		_last_to(NO_TIMEOUT), _ec(timer_thread,nre::CPU::current().log_id()), _sc(&_ec,nre::Qpd()) {
		_ec.set_tls<Timeouts*>(nre::Thread::TLS_PARAM,this);
		_sc.start();
	}

	size_t alloc() {
		nre::ScopedLock<nre::UserSm> guard(&_sm);
		return _timeouts.alloc();
	}

	void request(size_t nr,timevalue_t to) {
		nre::ScopedLock<nre::UserSm> guard(&_sm);
		_timeouts.request(nr,to);
		program();
	}

	void time(timevalue_t &uptime,timevalue_t &unixtime) {
		_timer.get_time(uptime,unixtime);
	}

private:
	NORETURN static void timer_thread(void*);
	void trigger();
	void program();

	Motherboard &_mb;
	nre::UserSm _sm;
	nre::TimeoutList<32,void> _timeouts;
	nre::Connection _timercon;
	nre::TimerSession _timer;
	timevalue_t _last_to;
	nre::GlobalThread _ec;
	nre::Sc _sc;
};
