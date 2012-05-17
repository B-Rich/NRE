/*
 * (c) 2012 Nils Asmussen <nils@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 */

#pragma once

#ifdef __i386__
#include <arch/x86_32/SpinLock.h>
#elif defined __x86_64__
#include <arch/x86_64/SpinLock.h>
#else
#error "Unsupported architecture"
#endif
