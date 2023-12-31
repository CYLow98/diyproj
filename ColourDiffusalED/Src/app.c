//$file${.::app.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: ColourDiffusal.qm
// File:  ${.::app.c}
//
// This code has been generated by QM 5.3.0 <www.state-machine.com/qm>.
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// This generated code is open source software: you can redistribute it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// NOTE:
// Alternatively, this generated code may be distributed under the terms
// of Quantum Leaps commercial licenses, which expressly supersede the GNU
// General Public License and are specifically designed for licensees
// interested in retaining the proprietary status of their code.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//
//$endhead${.::app.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
/* Board Support Package implementation for desktop OS (Windows, Linux, MacOS) */
#include "qpc.h"    /* QP/C framework API */
#ifdef DEBUG_CFG
#include <stdio.h>  /* for printf()/fprintf() */
#endif
#include <stdlib.h> /* for exit() */

/* callback functions needed by the framework ------------------------------*/
void QF_onStartup(void) {}
void QF_onCleanup(void) {}
void QF_onClockTick(void) {
    QF_TICK_X(0U, (void *)0); /* QF clock tick processing for rate 0 */
}
void Q_onAssert(char const * const module, int loc) {
#ifdef DEBUG_CFG
    fprintf(stderr, "Assertion failed in %s:%d", module, loc);
#endif
    exit(-1);
}
