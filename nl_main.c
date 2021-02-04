
/*--------------------------------------------------------------------*/
/*--- Nulgrind: The minimal Valgrind tool.               nl_main.c ---*/
/*--------------------------------------------------------------------*/

/*
   This file is part of Nulgrind, the minimal Valgrind tool,
   which does no instrumentation or analysis.

   Copyright (C) 2002-2017 Nicholas Nethercote
   njn@valgrind.org

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307, USA.

   The GNU General Public License is contained in the file COPYING.
 */

#include "pub_tool_basics.h"
#include "pub_tool_tooliface.h"
#include "pub_core-libcproc.h"
#include "global.h"
#include "events.h"
#include "costs.h"

static const HChar& anonymous_obj = "???";

static void print_inst(UInt len, Addr addr) {

	int i;
	UChar* inst_pointer;
	UChar inst_byte;

	inst_pointer = (UChar*)addr;
	i = 0;
	inst_byte = 0;

	while (i < len) {

		inst_byte = *inst_pointer;

		vex_printf("%x", (int)inst_byte);

		i++;

		inst_pointer++;

	}

}


static void nl_post_clo_init(void)
{
}

	static
IRSB* nl_instrument ( VgCallbackClosure* closure,
		IRSB* bb_in,
		const VexGuestLayout* layout, 
		const VexGuestExtents* vge,
		const VexArchInfo* archinfo_host,
		IRType gWordTy, IRType hWordTy )
{
	DebugInfo* di;
	Diepoch ep;

	IRStmt* st;
	IRSB* bb        = emptyIRSB();
	bb->tyenv       = deepCopyIRTypeEnv(bb_in->tyenv);
	bb->next        = deepCopyIRExpr(bb_in->next);
	bb->jumpkind = bb_in->jumpkind;
	bb->offsIP      = bb_in->offsIP;

	int flag = 0;
	int libc_check;
	int ld_check;
	int i;
	const HChar* obj_name;

	for(i = 0 ; i < bb_in->stmts_used ; i++) {
		st = bb_in->stmts[i];
		if(!st)
			continue;
		switch(st->tag) {
			case Ist_NoOp:
				break;
				case Ist_IMark
					ep = VG_(current_DiEpoch)();
					di = VG_(find_DebugInfo)(ep, st->Ist.IMark.addr);
					obj_name = di ? VG_(DebugInfo_get_file_name)(di) : anonymous_obj;

					libc_check = VG_(strcmp)(obj_name, "/lib/i386-linux-gnu/libc-2.23.so");
					ld_check = VG_(strcmp)(obj_name, "/lib/i386-linux-gnu/ld-2.23.so");

					if (libc_check == 0 || ld_check == 0) {
						flag = 1;
						break;
					}
					else
						flag = 0;
					if (flag == 0) {
						vex_printf("%d th", i);
						vex_printf(" ");
						ppIRStmt(st);

					}
					vex_printf("ASSM : ");
					print_inst(st->Ist.IMark.len, st->Ist.IMark.addr);
				break;
			case Ist_AbiHint:
				break;
			case Ist_Put:
				break;
			case Ist_PutI:
				break;
			case Ist_WrTmp:
				break;
			case Ist_Store:
				break;
			case Ist_LoadG:
				break;
			case Ist_StoreG:
				break;
			case Ist_CAS:
				break;
			case Ist_LLSC:
				break;
			case Ist_Dirty:
				break;
			case Ist_MBE:
				break;
			case Ist_Exit:
				break;
			default:
				break;
		}
		if (flag == 0 && st->tag != Ist_IMark) {
			vex_printf("%d th", i);
			vex_printf(" ");
			ppIRStmt(st);
		}
		vex_printf("\n");
		addStmtToIRSB(bb, deepCopyIRStmt(st));
	}
	return bb;
}

static void nl_fini(Int exitcode)
{
}

static void nl_pre_clo_init(void)
{
	VG_(details_name)            ("Nulgrind");
	VG_(details_version)         (NULL);
	VG_(details_description)     ("the minimal Valgrind tool");
	VG_(details_copyright_author)(
			"Copyright (C) 2002-2017, and GNU GPL'd, by Nicholas Nethercote.");
	VG_(details_bug_reports_to)  (VG_BUGS_TO);

	VG_(details_avg_translation_sizeB) ( 275 );

	VG_(basic_tool_funcs)        (nl_post_clo_init,
			nl_instrument,
			nl_fini);

	/* No needs, no core events to track */
}

VG_DETERMINE_INTERFACE_VERSION(nl_pre_clo_init)

	/*--------------------------------------------------------------------*/
	/*--- end                                                          ---*/
	/*--------------------------------------------------------------------*/
