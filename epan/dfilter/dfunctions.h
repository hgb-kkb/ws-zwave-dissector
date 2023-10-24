/** @file
 *
 * Wireshark - Network traffic analyzer
 *
 * Copyright 2006 Gilbert Ramirez <gram@alumni.rice.edu>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef DFUNCTIONS_H
#define DFUNCTIONS_H

#include <glib.h>
#include <ftypes/ftypes.h>
#include "syntax-tree.h"
#include "dfilter-int.h"

/* Functions take any number of arguments and return 1. */

#define dfunc_fail(dfw, node, ...) \
    do { \
        ws_noisy("Semantic check failed here."); \
        dfilter_fail_throw(dfw, DF_ERROR_GENERIC, stnode_location(node), __VA_ARGS__); \
    } while (0)

/* The run-time logic of the dfilter function */
typedef bool (*DFFuncType)(GSList *stack, uint32_t arg_count, df_cell_t *retval);

/* The semantic check for the dfilter function */
typedef ftenum_t (*DFSemCheckType)(dfwork_t *dfw, const char *func_name, ftenum_t lhs_ftype,
                                GSList *param_list, df_loc_t func_loc);

/* This is a "function definition" record, holding everything
 * we need to know about a function */
typedef struct {
    const char      *name;
    DFFuncType      function;
    unsigned        min_nargs;
    unsigned        max_nargs; /* 0 for no limit */
    ftenum_t        return_ftype; /* Can be FT_NONE if the function returns the same type
                                   * as its arguments. */
    DFSemCheckType  semcheck_param_function;
} df_func_def_t;

/* Return the function definition record for a function of named "name" */
WS_DLL_PUBLIC
df_func_def_t* df_func_lookup(const char *name);

#endif
