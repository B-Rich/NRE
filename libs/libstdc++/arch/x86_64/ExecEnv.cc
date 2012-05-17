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

#include <arch/ExecEnv.h>
#include <Compiler.h>
#include <Util.h>

namespace nul {

void *ExecEnv::_stacks[MAX_STACKS][STACK_SIZE / sizeof(void*)] ALIGNED(ExecEnv::PAGE_SIZE);
size_t ExecEnv::_stack;

void *ExecEnv::setup_stack(Pd *pd,Ec *ec,startup_func start) {
	unsigned stack_top = sizeof(_stacks[_stack]) / sizeof(void*);
	_stacks[_stack][--stack_top] = ec;
	_stacks[_stack][--stack_top] = pd;
	_stacks[_stack][--stack_top] = 0;
	_stacks[_stack][--stack_top] = reinterpret_cast<void*>(start);
	return _stacks[_stack++] + stack_top;
}

size_t ExecEnv::collect_backtrace(uintptr_t *frames,size_t max) {
	uintptr_t ebp;
	asm volatile ("mov %%rbp,%0" : "=a" (ebp));
	return collect_backtrace(Util::rounddown<uintptr_t>(ebp,ExecEnv::STACK_SIZE),ebp,frames,max);
}

size_t ExecEnv::collect_backtrace(uintptr_t stack,uintptr_t ebp,uintptr_t *frames,size_t max) {
	uintptr_t end,start;
	uintptr_t *frame = frames;
	size_t count = 0;
	end = Util::roundup<uintptr_t>(ebp,ExecEnv::STACK_SIZE);
	start = end - ExecEnv::STACK_SIZE;
	for(size_t i = 0; i < max - 1; i++) {
		// prevent page-fault
		if(ebp < start || ebp >= end)
			break;
		ebp = stack + (ebp & (ExecEnv::STACK_SIZE - 1));
		*frame = *(reinterpret_cast<uintptr_t*>(ebp) + 1) - CALL_INSTR_SIZE;
		frame++;
		count++;
		ebp = *reinterpret_cast<uintptr_t*>(ebp);
	}
	// terminate
	*frame = 0;
	return count;
}

}
