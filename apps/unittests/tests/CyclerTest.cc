/*
 * (c) 2012 Nils Asmussen <nils@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 */

#include <SList.h>
#include <DList.h>
#include <Cycler.h>

#include "CyclerTest.h"

using namespace nul;
using namespace nul::test;

static void test_cycler_forward();
static void test_cycler_backwards();
static void test_cycler_bidirectional();

const TestCase cyclertest1 = {
	"Forward cycler list",test_cycler_forward
};
const TestCase cyclertest2 = {
	"Backwards cycler list",test_cycler_backwards
};
const TestCase cyclertest3 = {
	"Bidirectional cycler list",test_cycler_bidirectional
};

static void test_cycler_forward() {
	SListItem e1,e2,e3;
	SList<SListItem>::iterator it;
	SList<SListItem> l;
	l.append(&e1);
	l.append(&e2);
	l.append(&e3);

	{
		ForwardCycler<SList<SListItem>::iterator> cyc(l.begin(),l.begin());
		WVPASS(!cyc.valid());
	}

	{
		ForwardCycler<SList<SListItem>::iterator> cyc(l.begin(),l.end());
		it = cyc.current();
		WVPASS(&*it == &e1);
		it = cyc.next();
		WVPASS(&*it == &e2);
		it = cyc.next();
		WVPASS(&*it == &e3);
		it = cyc.next();
		WVPASS(&*it == &e1);
		it = cyc.next();
		WVPASS(&*it == &e2);
		it = cyc.next();
		WVPASS(&*it == &e3);
	}
}

static void test_cycler_backwards() {
	DListItem e1,e2,e3;
	DList<DListItem>::iterator it;
	DList<DListItem> l;
	l.append(&e1);
	l.append(&e2);
	l.append(&e3);

	{
		BackwardsCycler<DList<DListItem>::iterator> cyc(l.begin(),l.end());
		it = cyc.current();
		WVPASS(&*it == &e1);
		it = cyc.prev();
		WVPASS(&*it == &e3);
		it = cyc.prev();
		WVPASS(&*it == &e2);
		it = cyc.prev();
		WVPASS(&*it == &e1);
		it = cyc.prev();
		WVPASS(&*it == &e3);
		it = cyc.prev();
		WVPASS(&*it == &e2);
	}
}

static void test_cycler_bidirectional() {
	DListItem e1,e2,e3;
	DList<DListItem>::iterator it;
	DList<DListItem> l;
	l.append(&e1);
	l.append(&e2);
	l.append(&e3);

	{
		Cycler<DList<DListItem>::iterator> cyc(l.begin(),l.end());
		it = cyc.current();
		WVPASS(&*it == &e1);
		it = cyc.prev();
		WVPASS(&*it == &e3);
		it = cyc.prev();
		WVPASS(&*it == &e2);
		it = cyc.prev();
		WVPASS(&*it == &e1);
		it = cyc.next();
		WVPASS(&*it == &e2);
		it = cyc.next();
		WVPASS(&*it == &e3);
		it = cyc.next();
		WVPASS(&*it == &e1);
	}
}
