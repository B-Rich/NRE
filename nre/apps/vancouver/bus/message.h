/**
 * @file
 * Message Type defintions.
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

#include <arch/Types.h>
#include <services/Console.h>
#include <services/Storage.h>
#include <Compiler.h>
#include <Desc.h>

/****************************************************/
/* IOIO messages                                    */
/****************************************************/
/**
 * An in() from an ioport.
 */
struct MessageIOIn {
    enum Type {
        TYPE_INB = 0,
        TYPE_INW = 1,
        TYPE_INL = 2
    } type;
    unsigned short port;
    unsigned count;
    union {
        unsigned value;
        void *ptr;
    };
    MessageIOIn(Type _type, unsigned short _port)
        : type(_type), port(_port), count(0), value(~0u) {
    }
    MessageIOIn(Type _type, unsigned short _port, unsigned _count, void *_ptr)
        : type(_type), port(_port), count(_count), ptr(_ptr) {
    }
};

struct MessageHwIOIn : public MessageIOIn {
    MessageHwIOIn(Type _type, unsigned short _port)
        : MessageIOIn(_type, _port) {
    }
    MessageHwIOIn(Type _type, unsigned short _port, unsigned _count, void *_ptr)
        : MessageIOIn(_type, _port, _count, _ptr) {
    }
};

/**
 * An out() to an ioport.
 */
struct MessageIOOut {
    enum Type {
        TYPE_OUTB = 0,
        TYPE_OUTW = 1,
        TYPE_OUTL = 2
    } type;
    unsigned short port;
    unsigned count;
    union {
        unsigned value;
        void *ptr;
    };
    MessageIOOut(Type _type, unsigned short _port, unsigned _value)
        : type(_type), port(_port), count(0), value(_value) {
    }
    MessageIOOut(Type _type, unsigned short _port, unsigned _count, void *_ptr)
        : type(_type), port(_port), count(_count), ptr(_ptr) {
    }
};

struct MessageHwIOOut : public MessageIOOut {
    MessageHwIOOut(Type _type, unsigned short _port, unsigned _value)
        : MessageIOOut(_type, _port, _value) {
    }
    MessageHwIOOut(Type _type, unsigned short _port, unsigned _count, void *_ptr)
        : MessageIOOut(_type, _port, _count, _ptr) {
    }
};

/****************************************************/
/* Memory messages                                  */
/****************************************************/

/**
 * A dword aligned memory access.
 */
struct MessageMem {
    enum {
        MSI_ADDRESS = 0xfee00000,
        MSI_DM      = 1 << 2,
        MSI_RH      = 1 << 3
    };
    bool read;
    uintptr_t phys;
    unsigned *ptr;
    MessageMem(bool _read, uintptr_t _phys, unsigned *_ptr)
        : read(_read), phys(_phys), ptr(_ptr) {
    }
};

/**
 * Request a region that is directly mapped into our memory.  Used for
 * mapping it to the user and optimizing internal access.
 *
 * Note, that clients can also return an empty region by not setting
 * the ptr.
 */
struct MessageMemRegion {
    uintptr_t page;
    uintptr_t start_page;
    size_t count;
    char * ptr;
    MessageMemRegion(uintptr_t _page)
        : page(_page), count(0), ptr(0) {
    }
};

/****************************************************/
/* PCI messages                                     */
/****************************************************/

/**
 * A PCI config space transaction.
 */
struct MessagePciConfig {
    enum Type {
        TYPE_READ,
        TYPE_WRITE,
        TYPE_PTR, ///< Return pointer to memory mapped PCI configuration space register
    } type;
    uint32_t bdf;
    unsigned dword;
    unsigned value;
    unsigned *ptr;

    MessagePciConfig(uint32_t _bdf, unsigned _dword)
        : type(TYPE_READ), bdf(_bdf), dword(_dword), value(0xffffffff) {
    }
    MessagePciConfig(uint32_t _bdf, unsigned _dword, unsigned _value)
        : type(TYPE_WRITE), bdf(_bdf), dword(_dword), value(_value) {
    }

    explicit MessagePciConfig(uint32_t _bdf)
        : type(TYPE_PTR), bdf(_bdf), dword(0), ptr(0) {
    }
};

struct MessageHwPciConfig : public MessagePciConfig {
    MessageHwPciConfig(uint32_t _bdf, unsigned _dword)
        : MessagePciConfig(_bdf, _dword) {
    }
    MessageHwPciConfig(uint32_t _bdf, unsigned _dword, unsigned _value)
        : MessagePciConfig(_bdf, _dword, _value) {
    }

    explicit MessageHwPciConfig(uint32_t _bdf)
        : MessagePciConfig(_bdf) {
    }
};

/****************************************************/
/* SATA messages                                    */
/****************************************************/

class FisReceiver;

// XXX Use SATA bus to comunicated FISes
/**
 * Set a drive on a port of an AHCI controller.
 */
struct MessageAhciSetDrive {
    FisReceiver *drive;
    unsigned port;
    MessageAhciSetDrive(FisReceiver *_drive, unsigned _port)
        : drive(_drive), port(_port) {
    }
};

/****************************************************/
/* IRQ messages                                     */
/****************************************************/

/**
 * Raise an IRQ.
 */
struct MessageIrq {
    enum Type {
        ASSERT_IRQ,
        ASSERT_NOTIFY,
        DEASSERT_IRQ
    } type;
    unsigned char line;

    MessageIrq(Type _type, unsigned char _line)
        : type(_type), line(_line) {
    }
};

struct MessageIrqLines : public MessageIrq {
    MessageIrqLines(Type _type, unsigned char _line)
        : MessageIrq(_type, _line) {
    }
};

/**
 * Notify that a level-triggered IRQ can be reraised.
 */
struct MessageIrqNotify {
    unsigned char baseirq;
    unsigned char mask;
    MessageIrqNotify(unsigned char _baseirq, unsigned char _mask)
        : baseirq(_baseirq), mask(_mask) {
    }
};

/**
 * Message on the PIC bus.
 */
struct MessagePic {
    unsigned char slave;
    unsigned char vector;
    MessagePic(unsigned char _slave)
        : slave(_slave) {
    }
};

/**
 * IPI-Message on the APIC bus.
 */
struct MessageApic {
    enum {
        /**
         * The HW uses a special cycle for broadcast EOIs. We model that
         * by performing a write transaction to the first IOAPIC EOI
         * registers that is snooped by all IOApics.
         */
        IOAPIC_EOI      = 0xfec00040,
        ICR_DM          = 1 << 11,
        ICR_ASSERT      = 1 << 14,
        ICR_LEVEL       = 1 << 15
    };
    unsigned icr; // only bits 0xcfff are used
    unsigned dst; // 32bit APIC ID
    void *ptr; // to distinguish loops
    MessageApic(unsigned _icr, unsigned _dst, void *_ptr)
        : icr(_icr), dst(_dst), ptr(_ptr) {
    }
};

/****************************************************/
/* Legacy messages                                  */
/****************************************************/

/**
 * Various messages of the legacy chips such as PIT, PPI...
 */
struct MessageLegacy {
    enum Type {
        GATE_A20,
        FAST_A20,
        RESET,
        INIT,
        NMI,
        INTR,
        DEASS_INTR,
        INTA,
    } type;
    unsigned value;
    MessageLegacy(Type _type, unsigned _value = 0)
        : type(_type), value(_value) {
    }
};

/**
 * Pit messages.
 */
struct MessagePit {
    enum Type {
        GET_OUT,
        SET_GATE
    } type;
    unsigned pit;
    bool value;
    MessagePit(Type _type, unsigned _pit, bool _value = false)
        : type(_type), pit(_pit), value(_value) {
    }
};

/****************************************************/
/* Keyboard and Serial messages                     */
/****************************************************/
/**
 * Message on the PS2 bus between a KeyboardController and a connected Keyboard/Mouse
 */
struct MessagePS2 {
    unsigned port;
    enum Type {
        NOTIFY,
        READ_KEY,
        SEND_COMMAND
    } type;
    unsigned char value;
    MessagePS2(unsigned char _port, Type _type, unsigned char _value)
        : port(_port), type(_type), value(_value) {
    }
};

/**
 * A keycode or a mouse packet. See keyboard.h for the format.
 */
struct MessageInput {
    unsigned device;
    unsigned data;
    MessageInput(unsigned _device = 0, unsigned _data = 0)
        : device(_device), data(_data) {
    }
};

/**
 * An ascii character from the serial port.
 */
struct MessageSerial {
    unsigned serial;
    unsigned char ch;
    MessageSerial(unsigned _serial, unsigned char _ch)
        : serial(_serial), ch(_ch) {
    }
};

/****************************************************/
/* Console messages                                 */
/****************************************************/

#if 0
#    include "host/vesa.h"
#endif

struct VgaRegs {
    unsigned short mode;
    unsigned short cursor_style;
    unsigned cursor_pos;
    size_t offset;
};

struct MessageConsoleView {
    enum Type {
        TYPE_GET_INFO
    } type;
    nre::ConsoleSession *sess;
    MessageConsoleView(Type _type)
        : type(_type), sess() {
    }
};

#if 0
typedef Vbe::ModeInfoBlock ConsoleModeInfo;

/**
 * VGA Console.
 */
struct MessageConsole {
    enum Type {
        // allocate a new client
        TYPE_ALLOC_CLIENT,
        // allocate a new view for a client
        TYPE_ALLOC_VIEW,
        // get information about a mode
        TYPE_GET_MODEINFO,
        // get a FONT
        TYPE_GET_FONT,
        // switch to another view
        TYPE_SWITCH_VIEW,
        // the user pressed a key
        TYPE_KEY,
        // the user requests a reset
        TYPE_RESET,
        // the user requests to start a new PD
        TYPE_START,
        // the user requests to kill a PD
        TYPE_KILL,
        // the user requests a debug feature
        TYPE_DEBUG
    } type;
    unsigned short id;
    unsigned short view;
    union {
        struct {
            const char *clientname;
        };
        struct {
            const char *name;
            char * ptr;
            unsigned size;
            VgaRegs *regs;
        };
        struct {
            unsigned index;
            ConsoleModeInfo *info;
        };
        struct {
            unsigned input_device;
            unsigned input_data;
        };
        struct { //used with TYPE_START
            cap_sel cap_sc_usage;
            unsigned long mem;
            unsigned res;
            char const * cmdline;
        };
    };
    MessageConsole(Type _type = TYPE_ALLOC_CLIENT,
                   unsigned short _id = 0) : type(_type), id(_id), ptr(0) {
    }
    MessageConsole(unsigned _index,
                   ConsoleModeInfo *_info) : type(TYPE_GET_MODEINFO), index(_index), info(_info) {
    }
    MessageConsole(const char *_name, char * _ptr, unsigned _size, VgaRegs *_regs)
        : type(TYPE_ALLOC_VIEW), id(~0), name(_name), ptr(_ptr), size(_size), regs(_regs) {
    }
    MessageConsole(unsigned short _id, unsigned short _view, unsigned _input_device,
                   unsigned _input_data)
        : type(TYPE_KEY), id(_id), view(_view), input_device(_input_device), input_data(_input_data) {
    }
};

/**
 * VESA support.
 */
struct MessageVesa {
    enum Type {
        // return available modes
        TYPE_GET_MODEINFO,
        // switch mode
        TYPE_SWITCH_MODE
    } type;
    unsigned index;
    Vbe::ModeInfoBlock *info;
    MessageVesa(unsigned _index,
                Vbe::ModeInfoBlock *_info) : type(TYPE_GET_MODEINFO), index(_index), info(_info) {
    }
    MessageVesa(unsigned _index) : type(TYPE_SWITCH_MODE), index(_index) {
    }
};
#endif

/****************************************************/
/* HOST messages                                    */
/****************************************************/

class VCVCpu;
typedef void (*ServiceThreadFn)(void *) REGPARMS(0) NORETURN;
typedef void (*ServicePortalFn)(void *, nre::Utcb *) REGPARMS(0);

/**
 * Request to the host, such as notify irq or request IO region.
 */
struct MessageHostOp {
    enum Type {
        OP_ATTACH_IRQ,
        OP_NOTIFY_IRQ,
        OP_ATTACH_MSI,
        OP_ALLOC_IOIO_REGION,
        OP_ALLOC_IOMEM,
        OP_ALLOC_SEMAPHORE,
        OP_ALLOC_SERVICE_THREAD,
        OP_ALLOC_SERVICE_PORTAL,
        OP_ASSIGN_PCI,
        OP_VIRT_TO_PHYS,
        OP_GET_MODULE,
        OP_GET_MAC,
        OP_GUEST_MEM,
        OP_ALLOC_FROM_GUEST,
        OP_VCPU_CREATE_BACKEND,
        OP_VCPU_BLOCK,
        OP_VCPU_RELEASE,
        OP_REGISTER_SERVICE,
        OP_WAIT_CHILD,
        OP_CREATE_EC4PT,
    } type;
    union {
        unsigned long value;
        void * obj;
    };
    union {
        struct {
            uintptr_t phys;
            size_t phys_len;
        };
        struct {
            char const *service_name;
            uintptr_t portal_func;
            bool cap;
            uintptr_t portal_pf;
            unsigned excbase;
            unsigned excinc;
            unsigned crd_t;
            char * revoke_mem;
        };
        struct {
            char *ptr;
            size_t len;
            cpu_t cpu;
            char const * desc;
        };
        struct {
            unsigned module;
            char * start;
            size_t size;
            char * cmdline;
            size_t cmdlen;
        };
        struct {
            unsigned msi_gsi;
            unsigned msi_value;
            uint64_t msi_address;
        };
        struct {
            uint64_t mac;
        };
        struct {
            VCVCpu *vcpu;
        };
        struct {
            ServiceThreadFn work;
            void *work_arg;
            unsigned prio;
            cpu_t cpu;
            char const * name;
        } _alloc_service_thread;
        struct {
            ServicePortalFn pt;
            void *pt_arg;
            unsigned crd;
            cpu_t cpu;
            unsigned *pt_out;
        } _alloc_service_portal;
        struct {
            cpu_t cpu;
            nre::Utcb **utcb_out;
            capsel_t ec; // In/Out parameter; if ~0, then a new cap will be allocated by the calee.
        } _create_ec4pt;
    };

    static MessageHostOp alloc_service_thread(ServiceThreadFn work, void *arg, char const * name = 0,
                                              unsigned prio = ~0U, cpu_t cpu = ~0U) {
        MessageHostOp n(OP_ALLOC_SERVICE_THREAD, 0UL);
        n._alloc_service_thread.work = work;
        n._alloc_service_thread.work_arg = arg;
        n._alloc_service_thread.prio = prio;
        n._alloc_service_thread.cpu = cpu;
        n._alloc_service_thread.name = name;
        return n;
    }

    static MessageHostOp alloc_service_portal(unsigned *pt_out, ServicePortalFn pt, void *pt_arg,
                                              nre::Crd crd, cpu_t cpu = ~0U) {
        MessageHostOp n(OP_ALLOC_SERVICE_PORTAL, 0UL);
        n._alloc_service_portal.pt = pt;
        n._alloc_service_portal.pt_arg = pt_arg;
        n._alloc_service_portal.crd = crd.value();
        n._alloc_service_portal.cpu = cpu;
        n._alloc_service_portal.pt_out = pt_out;
        return n;
    }

    static MessageHostOp attach_msi(cpu_t cpu, bool locked, uint16_t rid, char const * name) {
        MessageHostOp n(OP_ATTACH_MSI, rid, !locked, cpu);
        n.desc = name;
        return n;
    }

    static MessageHostOp attach_irq(unsigned irq, cpu_t cpu, bool locked, char const * name) {
        MessageHostOp n(OP_ATTACH_IRQ, irq, !locked, cpu);
        n.desc = name;
        return n;
    }

    static MessageHostOp create_ec4pt(capsel_t &ec, void *obj, cpu_t cpu, nre::Utcb **utcb_out) {
        MessageHostOp n(OP_CREATE_EC4PT, obj);
        n._create_ec4pt.ec = ec;
        n._create_ec4pt.cpu = cpu;
        n._create_ec4pt.utcb_out = utcb_out;
        return n;
    }

    explicit MessageHostOp(VCVCpu *_vcpu)
        : type(OP_VCPU_CREATE_BACKEND), value(0), vcpu(_vcpu) {
    }
    explicit MessageHostOp(unsigned _module, char * _start, size_t _size = 0)
        : type(OP_GET_MODULE), module(_module), start(_start), size(_size), cmdlen(0) {
    }
    explicit MessageHostOp(Type _type, unsigned long _value, size_t _len = 0, unsigned _cpu =
                               ~0U)
        : type(_type), value(_value), ptr(0), len(_len), cpu(_cpu) {
    }
    explicit MessageHostOp(Type _type, void * _value, size_t _len = 0)
        : type(_type), obj(_value), ptr(0), len(_len) {
    }
    explicit MessageHostOp(void * _obj, char const * _name, uintptr_t _pfu, char * _revoke_mem = 0,
                           bool _cap = true)
        : type(OP_REGISTER_SERVICE), obj(_obj), service_name(_name), portal_func(_pfu), cap(_cap),
          portal_pf(0), excbase(0), excinc(0), crd_t(0), revoke_mem(_revoke_mem) {
    }
};

struct MessageAcpi {
    enum Type {
        ACPI_GET_TABLE,
        ACPI_GET_IRQ
    } type;
    union {
        struct {
            const char *name;
            unsigned instance;
            char *table;
            size_t len;
        };
        struct {
            uint32_t parent_bdf;
            uint32_t bdf;
            unsigned char pin;
            unsigned gsi;
        };
    };
    MessageAcpi(const char *_name)
        : type(ACPI_GET_TABLE), name(_name), instance(0), table(0), len(0) {
    }
    MessageAcpi(uint32_t _parent_bdf, uint32_t _bdf, unsigned char _pin)
        : type(ACPI_GET_IRQ), parent_bdf(_parent_bdf), bdf(_bdf), pin(_pin), gsi(~0u) {
    }
};

/**
 * Resource discovery between device models is done by the virtual
 * BIOS.
 *
 * Resources can be ACPI tables, BIOS BDA, EBDA,...
 */
struct MessageDiscovery {
    enum Type {
        DISCOVERY,
        WRITE,
        READ
    } type;
    struct {
        const char * resource;
        size_t offset;
        union {
            const void * data;
            unsigned * dw;
        };
        size_t count;
    };
    MessageDiscovery()
        : type(DISCOVERY) {
    }
    MessageDiscovery(const char * _resource, size_t _offset, const void * _data, size_t _count)
        : type(WRITE), resource(_resource), offset(_offset), data(_data), count(_count) {
    }
    MessageDiscovery(const char * _resource, size_t _offset, unsigned * _dw)
        : type(READ), resource(_resource), offset(_offset), dw(_dw) {
    }
};

/****************************************************/
/* DISK messages                                    */
/****************************************************/

class DmaDescriptor;
class DiskParameter;

/**
 * Request/read from the disk.
 */
struct MessageDisk {
    enum Type {
        DISK_CONNECT,
        DISK_READ,
        DISK_WRITE,
        DISK_FLUSH_CACHE
    } type;
    size_t disknr;
    union {
        nre::Storage::Parameter *params;
        struct {
            nre::Storage::sector_type sector;
            nre::Storage::tag_type usertag;
            const nre::Storage::dma_type *dma;
        };
    };
    enum Status {
        DISK_OK = 0,
        DISK_STATUS_BUSY,
        DISK_STATUS_DEVICE,
        DISK_STATUS_DMA,
        DISK_STATUS_USERTAG,
        DISK_STATUS_SHIFT = 4,
        DISK_STATUS_MASK = (1 << DISK_STATUS_SHIFT) - 1
    } error;
    MessageDisk(size_t _disknr, nre::Storage::Parameter *_params)
        : type(DISK_CONNECT), disknr(_disknr), params(_params), error(DISK_OK) {
    }
    MessageDisk(Type _type, size_t _disknr, nre::Storage::tag_type _usertag,
                nre::Storage::sector_type _sector,
                const nre::Storage::dma_type *_dma)
        : type(_type), disknr(_disknr), sector(_sector), usertag(_usertag), dma(_dma) {
    }
};

/**
 * A disk.request is completed.
 */
struct MessageDiskCommit {
    size_t disknr;
    nre::Storage::tag_type usertag;
    MessageDisk::Status status;
    MessageDiskCommit(size_t _disknr = 0, nre::Storage::tag_type _usertag = 0,
                      MessageDisk::Status _status = MessageDisk::DISK_OK)
        : disknr(_disknr), usertag(_usertag), status(_status) {
    }
};

/****************************************************/
/* Executor messages                                */
/****************************************************/

class CpuState;

struct MessageBios {
    VCVCpu *vcpu;
    CpuState *cpu;
    unsigned irq;
    unsigned mtr_out;
    MessageBios(VCVCpu *_vcpu, CpuState *_cpu, unsigned _irq)
        : vcpu(_vcpu), cpu(_cpu), irq(_irq), mtr_out() {
    }
};

/****************************************************/
/* Timer messages                                   */
/****************************************************/

typedef timevalue_t timevalue;

/**
 * Timer infrastructure.
 *
 * There is no frequency and clock here, as all is based on the same
 * clocksource.
 */
struct MessageTimer {
    enum Type {
        TIMER_NEW,
        TIMER_REQUEST_TIMEOUT
    } type;
    unsigned nr;
    timevalue abstime;
    MessageTimer()
        : type(TIMER_NEW) {
    }
    MessageTimer(unsigned _nr, timevalue _abstime)
        : type(TIMER_REQUEST_TIMEOUT), nr(_nr), abstime(_abstime) {
    }
};

/**
 * A timeout triggered.
 */
struct MessageTimeout {
    unsigned nr;
    timevalue time;
    MessageTimeout(unsigned _nr, timevalue _time)
        : nr(_nr), time(_time) {
    }
};

/**
 * Returns the wall clock time in microseconds.
 *
 * It also contains a timestamp of the Motherboard clock in
 * microseconds, to be able to adjust to the time already passed and
 * to detect out-of-date values.
 */
struct MessageTime {
    enum {
        FREQUENCY = 1000000
    };
    timevalue wallclocktime;
    timevalue timestamp;
    MessageTime()
        : wallclocktime(0), timestamp(0) {
    }
};

/****************************************************/
/* Network messages                                 */
/****************************************************/

struct MessageNetwork {
    enum ops {
        PACKET,
        QUERY_MAC
    };

    unsigned type;

    union {
        struct {
            const unsigned char *buffer;
            size_t len;
        };
        uint64_t mac;
    };

    unsigned client;

    MessageNetwork(const unsigned char *buffer, size_t len, unsigned client)
        : type(PACKET), buffer(buffer), len(len), client(client) {
    }
    MessageNetwork(unsigned type, unsigned client)
        : type(type), mac(0), client(client) {
    }
};

/* EOF */
