/** @file
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 2001 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef DFVM_H
#define DFVM_H

#include <wsutil/regex.h>
#include <epan/proto.h>
#include "dfilter-int.h"
#include "syntax-tree.h"
#include "drange.h"
#include "dfunctions.h"

typedef enum {
	EMPTY,
	FVALUE,
	HFINFO,
	RAW_HFINFO,
	INSN_NUMBER,
	REGISTER,
	INTEGER,
	DRANGE,
	FUNCTION_DEF,
	PCRE,
	VALUE_STRING,
} dfvm_value_type_t;

/* These are mapped from a convoluted combination of
 * hfinfo->display flags in proto.h to an enum. */
typedef enum {
	DFVM_VS_NONE = 0,
	DFVM_VS_RANGE,
	DFVM_VS_VALS64,
	DFVM_VS_VALS,
	DFVM_VS_VALS_EXT,
	DFVM_VS_CUSTOM, // Not a value string but treat it like one
	DFVM_VS_CUSTOM64,
} dfvm_vs_type_t;

typedef struct {
	dfvm_vs_type_t type;
	const void *strings; // "strings" is the name used in header_field_info
} dfvm_value_string_t;

typedef struct {
	dfvm_value_type_t	type;

	union {
		GPtrArray		*fvalue_p; /* Always has length == 1 */
		uint32_t		numeric;
		drange_t		*drange;
		header_field_info	*hfinfo;
		df_func_def_t		*funcdef;
		ws_regex_t		*pcre;
		dfvm_value_string_t	value_string;
	} value;

	int ref_count;
} dfvm_value_t;

#define dfvm_value_get_fvalue(val) ((val)->value.fvalue_p->pdata[0])

typedef enum {

	DFVM_IF_TRUE_GOTO,
	DFVM_IF_FALSE_GOTO,
	DFVM_CHECK_EXISTS,
	DFVM_CHECK_EXISTS_R,
	DFVM_NOT,
	DFVM_RETURN,
	DFVM_READ_TREE,
	DFVM_READ_TREE_R,
	DFVM_READ_REFERENCE,
	DFVM_READ_REFERENCE_R,
	DFVM_PUT_FVALUE,
	DFVM_ALL_EQ,
	DFVM_ANY_EQ,
	DFVM_ALL_NE,
	DFVM_ANY_NE,
	DFVM_ALL_GT,
	DFVM_ANY_GT,
	DFVM_ALL_GE,
	DFVM_ANY_GE,
	DFVM_ALL_LT,
	DFVM_ANY_LT,
	DFVM_ALL_LE,
	DFVM_ANY_LE,
	DFVM_ALL_CONTAINS,
	DFVM_ANY_CONTAINS,
	DFVM_ALL_MATCHES,
	DFVM_ANY_MATCHES,
	DFVM_SET_ALL_IN,
	DFVM_SET_ANY_IN,
	DFVM_SET_ALL_NOT_IN,
	DFVM_SET_ANY_NOT_IN,
	DFVM_SET_ADD,
	DFVM_SET_ADD_RANGE,
	DFVM_SET_CLEAR,
	DFVM_SLICE,
	DFVM_LENGTH,
	DFVM_VALUE_STRING,
	DFVM_BITWISE_AND,
	DFVM_UNARY_MINUS,
	DFVM_ADD,
	DFVM_SUBTRACT,
	DFVM_MULTIPLY,
	DFVM_DIVIDE,
	DFVM_MODULO,
	DFVM_CALL_FUNCTION,
	DFVM_STACK_PUSH,
	DFVM_STACK_POP,
	DFVM_NOT_ALL_ZERO,
} dfvm_opcode_t;

const char *
dfvm_opcode_tostr(dfvm_opcode_t code);

typedef struct {
	int		id;
	dfvm_opcode_t	op;
	dfvm_value_t	*arg1;
	dfvm_value_t	*arg2;
	dfvm_value_t	*arg3;
} dfvm_insn_t;

dfvm_insn_t*
dfvm_insn_new(dfvm_opcode_t op);

void
dfvm_insn_free(dfvm_insn_t *insn);

dfvm_value_t*
dfvm_value_new(dfvm_value_type_t type);

dfvm_value_t*
dfvm_value_ref(dfvm_value_t *v);

void
dfvm_value_unref(dfvm_value_t *v);

dfvm_value_t*
dfvm_value_new_fvalue(fvalue_t *fv);

dfvm_value_t*
dfvm_value_new_hfinfo(header_field_info *hfinfo, bool raw);

dfvm_value_t*
dfvm_value_new_register(int reg);

dfvm_value_t*
dfvm_value_new_drange(drange_t *dr);

dfvm_value_t*
dfvm_value_new_funcdef(df_func_def_t *funcdef);

dfvm_value_t*
dfvm_value_new_pcre(ws_regex_t *re);

dfvm_value_t*
dfvm_value_new_guint(unsigned num);

dfvm_value_t*
dfvm_value_new_value_string(dfvm_vs_type_t type, const void *strings);

void
dfvm_dump(FILE *f, dfilter_t *df, uint16_t flags);

char *
dfvm_dump_str(wmem_allocator_t *alloc, dfilter_t *df,  uint16_t flags);

bool
dfvm_apply(dfilter_t *df, proto_tree *tree);

fvalue_t *
dfvm_get_raw_fvalue(const field_info *fi);

#endif
