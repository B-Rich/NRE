/** @file
 * Common BIOS code.
 *
 * Copyright (C) 2009, Bernhard Kauer <bk@vmmon.org>
 * Economic rights: Technische Universitaet Dresden (Germany)
 *
 * This file is part of Vancouver.
 *
 * Vancouver is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Vancouver is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details.
 */

#pragma once

#include <Assert.h>

#include "../bus/discovery.h"
#include "../bus/vcpu.h"

#define DEBUG(cpu)      \
    nre::Serial::get().writef("\t%s eax %x ebx %x ecx %x edx %x eip %x efl %x\n", \
                              __func__, cpu->eax, cpu->ebx, cpu->ecx, cpu->edx, cpu->eip, cpu->efl)

class BiosCommon : public DiscoveryHelper<BiosCommon> {
public:
    Motherboard &_mb;

    enum {
        RESET_VECTOR = 0x100, MAX_VECTOR
    };

protected:

#include "../model/simplemem.h"

    /**
     * Write bios data helper.
     */
    void write_bda(unsigned short offset, unsigned value, size_t len) {
        assert(len <= sizeof(value));
        copy_out(0x400 + offset, &value, len);
    }

    /**
     * Read bios data helper.
     */
    unsigned read_bda(size_t offset) {
        unsigned res;
        copy_in(0x400 + offset, &res, sizeof(res));
        return res;
    }

    /**
     * Jump to another realmode INT handler.
     */
    bool jmp_int(MessageBios &msg, unsigned char number) {
        unsigned short v[2];
        copy_in(number * 4, v, 4);

        msg.cpu->cs.sel = v[1];
        msg.cpu->cs.base = v[1] << 4;
        msg.cpu->rip = v[0];
        msg.mtr_out |= nre::Mtd::RIP_LEN | nre::Mtd::CS_SS;
        return true;
    }

    bool jmp_hlt(MessageBios &msg) {
        msg.cpu->rip--;
        return true;
    }

    /**
     * Set the usual error indication.
     */
    void error(MessageBios &msg, unsigned char errorcode) {
        msg.cpu->rfl |= 1;
        msg.cpu->ah = errorcode;
        msg.mtr_out |= nre::Mtd::RFLAGS | nre::Mtd::GPR_ACDB;
    }

    /**
     * Out to IO-port.
     */
    void outb(unsigned short port, unsigned value) {
        MessageIOOut msg(MessageIOOut::TYPE_OUTB, port, value);
        _mb.bus_ioout.send(msg);
    }

    BiosCommon(Motherboard &mb)
        : _mb(mb), _bus_memregion(&mb.bus_memregion), _bus_mem(&mb.bus_mem) {
    }
};
