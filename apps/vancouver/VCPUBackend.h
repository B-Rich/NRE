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
#include <kobj/Thread.h>
#include <kobj/Sm.h>
#include <kobj/Pt.h>
#include <kobj/Sc.h>
#include <kobj/VCPU.h>
#include <utcb/UtcbFrame.h>
#include <Assert.h>
#include <Compiler.h>

#include "bus/motherboard.h"
#include "bus/profile.h"
#include "bus/vcpu.h"

class VCPUBackend {
	enum {
		PT_VMX = 0x100,
		PT_SVM = 0x200
	};
	struct Portal {
		capsel_t offset;
		nul::Pt::portal_func func;
		uint mtd;
	};

public:
	VCPUBackend(Motherboard *mb,VCpu *vcpu,bool use_svm,cpu_t cpu)
			: _ec(cpu), _caps(get_portals(use_svm)), _sm(1), _vcpu(cpu,_caps),
			  _sc(&_vcpu,nul::Qpd()) {
		_ec.set_tls<VCpu*>(nul::Thread::TLS_PARAM,vcpu);
		_sc.start();
		_mb = mb;
	}

	nul::VCPU &vcpu() {
		return _vcpu;
	}
	nul::Sm &sm() {
		return _sm;
	}

private:
	capsel_t get_portals(bool use_svm);

	static void handle_io(bool is_in,unsigned io_order,unsigned port);
	static void handle_vcpu(capsel_t pid,bool skip,CpuMessage::Type type);
	static nul::Crd lookup(uintptr_t base,size_t size,uintptr_t hotspot);
	static bool handle_memory(bool need_unmap);

	static void force_invalid_gueststate_amd(nul::UtcbExcFrameRef &uf);
	static void force_invalid_gueststate_intel(nul::UtcbExcFrameRef &uf);
	static void skip_instruction(CpuMessage &msg);

	PORTAL static void vmx_triple(capsel_t pid);
	PORTAL static void vmx_init(capsel_t pid);
	PORTAL static void vmx_irqwin(capsel_t pid);
	PORTAL static void vmx_cpuid(capsel_t pid);
	PORTAL static void vmx_hlt(capsel_t pid);
	PORTAL static void vmx_rdtsc(capsel_t pid);
	PORTAL static void vmx_vmcall(capsel_t pid);
	PORTAL static void vmx_ioio(capsel_t pid);
	PORTAL static void vmx_rdmsr(capsel_t pid);
	PORTAL static void vmx_wrmsr(capsel_t pid);
	PORTAL static void vmx_invalid(capsel_t pid);
	PORTAL static void vmx_pause(capsel_t pid);
	PORTAL static void vmx_mmio(capsel_t pid);
	PORTAL static void vmx_startup(capsel_t pid);
	PORTAL static void do_recall(capsel_t pid);

	PORTAL static void svm_vintr(capsel_t pid);
	PORTAL static void svm_cpuid(capsel_t pid);
	PORTAL static void svm_hlt(capsel_t pid);
	PORTAL static void svm_ioio(capsel_t pid);
	PORTAL static void svm_msr(capsel_t pid);
	PORTAL static void svm_shutdwn(capsel_t pid);
	PORTAL static void svm_npt(capsel_t pid);
	PORTAL static void svm_invalid(capsel_t pid);
	PORTAL static void svm_startup(capsel_t pid);
	PORTAL static void svm_recall(capsel_t pid);

	nul::LocalThread _ec;
	capsel_t _caps;
	nul::Sm _sm;
	nul::VCPU _vcpu;
	nul::Sc _sc;
	static Motherboard *_mb;
	static bool _tsc_offset;
	static bool _rdtsc_exit;
	static Portal _portals[];
};
