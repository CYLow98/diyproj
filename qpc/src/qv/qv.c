//$file${src::qv::qv.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: qpc.qm
// File:  ${src::qv::qv.c}
//
// This code has been generated by QM 5.3.0 <www.state-machine.com/qm>.
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// This code is covered by the following QP license:
// License #    : LicenseRef-QL-dual
// Issued to    : Any user of the QP/C real-time embedded framework
// Framework(s) : qpc
// Support ends : 2024-12-31
// License scope:
//
// Copyright (C) 2005 Quantum Leaps, LLC <state-machine.com>.
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
//
// This software is dual-licensed under the terms of the open source GNU
// General Public License version 3 (or any later version), or alternatively,
// under the terms of one of the closed source Quantum Leaps commercial
// licenses.
//
// The terms of the open source GNU General Public License version 3
// can be found at: <www.gnu.org/licenses/gpl-3.0>
//
// The terms of the closed source Quantum Leaps commercial licenses
// can be found at: <www.state-machine.com/licensing>
//
// Redistributions in source code must retain this top-level comment block.
// Plagiarizing this software to sidestep the license obligations is illegal.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//
//$endhead${src::qv::qv.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#define QP_IMPL           // this is QP implementation
#include "qp_port.h"      // QP port
#include "qp_pkg.h"       // QP package-scope internal interface
#include "qsafe.h"        // QP Functional Safety (FuSa) Subsystem
#ifdef Q_SPY              // QS software tracing enabled?
    #include "qs_port.h"  // QS port
    #include "qs_pkg.h"   // QS facilities for pre-defined trace records
#else
    #include "qs_dummy.h" // disable the QS software tracing
#endif // Q_SPY

// protection against including this source file in a wrong project
#ifndef QV_H_
    #error "Source file included in a project NOT based on the QV kernel"
#endif // QV_H_

Q_DEFINE_THIS_MODULE("qv")

//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${QV::QV-base} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${QV::QV-base::priv_} ......................................................
QV_Attr QV_priv_;
//$enddef${QV::QV-base} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${QV::QF-cust} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${QV::QF-cust::init} .......................................................
//! @static @public @memberof QF
void QF_init(void) {
    QF_bzero_(&QF_priv_,                 sizeof(QF_priv_));
    QF_bzero_(&QV_priv_,                 sizeof(QV_priv_));
    QF_bzero_(&QTimeEvt_timeEvtHead_[0], sizeof(QTimeEvt_timeEvtHead_));
    QF_bzero_(&QActive_registry_[0],     sizeof(QActive_registry_));

    #ifndef Q_UNSAFE
    QPSet_update_(&QV_priv_.readySet, &QV_priv_.readySet_dis);
    #endif

    #ifdef QV_INIT
    QV_INIT(); // port-specific initialization of the QV kernel
    #endif
}

//${QV::QF-cust::stop} .......................................................
//! @static @public @memberof QF
void QF_stop(void) {
    QF_onCleanup(); // application-specific cleanup callback
    // nothing else to do for the cooperative QV kernel
}

//${QV::QF-cust::run} ........................................................
//! @static @public @memberof QF
int_t QF_run(void) {
    #ifdef Q_SPY
    // produce the QS_QF_RUN trace record
    QF_INT_DISABLE();
    QF_MEM_SYS();
    QS_beginRec_((uint_fast8_t)QS_QF_RUN);
    QS_endRec_();
    QF_MEM_APP();
    QF_INT_ENABLE();
    #endif // Q_SPY

    QF_onStartup(); // application-specific startup callback

    QF_INT_DISABLE();
    QF_MEM_SYS();

    #ifdef QV_START
    QV_START(); // port-specific startup of the QV kernel
    #endif

    #if (defined QF_ON_CONTEXT_SW) || (defined Q_SPY)
    uint8_t pprev = 0U; // previously used prio.
    #endif

    for (;;) { // QV event loop...

         // check internal integrity (duplicate inverse storage)
         Q_ASSERT_INCRIT(202, QPSet_verify_(&QV_priv_.readySet,
                                             &QV_priv_.readySet_dis));

        // find the maximum prio. AO ready to run
        if (QPSet_notEmpty(&QV_priv_.readySet)) {

            uint8_t const p = (uint8_t)QPSet_findMax(&QV_priv_.readySet);
            QActive * const a = QActive_registry_[p];

    #if (defined QF_ON_CONTEXT_SW) || (defined Q_SPY)
            QS_BEGIN_PRE_(QS_SCHED_NEXT, p)
                QS_TIME_PRE_();     // timestamp
                QS_2U8_PRE_(p, pprev); // scheduled prio & previous prio
            QS_END_PRE_()

    #ifdef QF_ON_CONTEXT_SW
            QF_onContextSw(((pprev != 0U)
                            ? QActive_registry_[pprev]
                            : (QActive *)0), a);
    #endif // QF_ON_CONTEXT_SW

            pprev = p; // update previous prio.
    #endif // (defined QF_ON_CONTEXT_SW) || (defined Q_SPY)

            QF_MEM_APP();
            QF_INT_ENABLE();

            QEvt const * const e = QActive_get_(a);
            // NOTE QActive_get_() performs QS_MEM_APP() before return

            // dispatch event (virtual call)
            (*a->super.vptr->dispatch)(&a->super, e, p);
    #if (QF_MAX_EPOOL > 0U)
            QF_gc(e);
    #endif
            QF_INT_DISABLE();
            QF_MEM_SYS();

            if (a->eQueue.frontEvt == (QEvt *)0) { // empty queue?
                QPSet_remove(&QV_priv_.readySet, p);
    #ifndef Q_UNSAFE
                QPSet_update_(&QV_priv_.readySet, &QV_priv_.readySet_dis);
    #endif
            }
        }
        else { // no AO ready to run --> idle
    #if (defined QF_ON_CONTEXT_SW) || (defined Q_SPY)
            if (pprev != 0U) {
                QS_BEGIN_PRE_(QS_SCHED_IDLE, pprev)
                    QS_TIME_PRE_();    // timestamp
                    QS_U8_PRE_(pprev); // previous prio
                QS_END_PRE_()

    #ifdef QF_ON_CONTEXT_SW
                QF_onContextSw(QActive_registry_[pprev], (QActive *)0);
    #endif // QF_ON_CONTEXT_SW

                pprev = 0U; // update previous prio.
            }
    #endif // (defined QF_ON_CONTEXT_SW) || (defined Q_SPY)

            QF_MEM_APP();

            // QV_onIdle() must be called with interrupts DISABLED
            // because the determination of the idle condition (all event
            // queues empty) can change at any time by an interrupt posting
            // events to a queue.
            //
            // NOTE: QV_onIdle() MUST enable interrupts internally,
            // ideally at the same time as putting the CPU into a power-
            // saving mode.
            QV_onIdle();

            QF_INT_DISABLE(); // disable interrupts before looping back
            QF_MEM_SYS();
        }
    }
    #ifdef __GNUC__ // GNU compiler?
    return 0;
    #endif
}
//$enddef${QV::QF-cust} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${QV::QActive} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${QV::QActive} .............................................................

//${QV::QActive::start_} .....................................................
//! @public @memberof QActive
void QActive_start_(QActive * const me,
    QPrioSpec const prioSpec,
    QEvt const * * const qSto,
    uint_fast16_t const qLen,
    void * const stkSto,
    uint_fast16_t const stkSize,
    void const * const par)
{
    Q_UNUSED_PAR(stkSto);  // not needed in QV
    Q_UNUSED_PAR(stkSize); // not needed in QV

    QF_CRIT_STAT
    QF_CRIT_ENTRY();
    Q_REQUIRE_INCRIT(300, stkSto == (void *)0);
    QF_CRIT_EXIT();

    me->prio  = (uint8_t)(prioSpec & 0xFFU); // QF-prio. of the AO
    me->pthre = (uint8_t)(prioSpec >> 8U);   // preemption-threshold
    QActive_register_(me); // make QF aware of this active object

    QEQueue_init(&me->eQueue, qSto, qLen); // init the built-in queue

    // top-most initial tran. (virtual call)
    (*me->super.vptr->init)(&me->super, par, me->prio);
    QS_FLUSH(); // flush the trace buffer to the host
}
//$enddef${QV::QActive} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
