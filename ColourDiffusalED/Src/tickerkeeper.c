//$file${.::tickerkeeper.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: ColourDiffusal.qm
// File:  ${.::tickerkeeper.c}
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
//$endhead${.::tickerkeeper.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "central.h"
#include "tickerkeeper.h"
#include "app.h"

/* ask QM to define the TickerKeeper class ---------------------------------------*/
//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${AOs::TickerKeeper} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${AOs::TickerKeeper} .......................................................

//${AOs::TickerKeeper::SM} ...................................................
QState TickerKeeper_initial(TickerKeeper * const me, void const * const par) {
    //${AOs::TickerKeeper::SM::initial}
    me->screenUpdateRate = 150u;
    me->sensorSamplingRate = 100u;
    return Q_TRAN(&TickerKeeper_running);
}

//${AOs::TickerKeeper::SM::running} ..........................................
QState TickerKeeper_running(TickerKeeper * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        //${AOs::TickerKeeper::SM::running}
        case Q_ENTRY_SIG: {
            //Arm both time events
            //USER CODE
#ifdef DEBUG_CFG
			printf("Tick: %i, TickerKeeper_running: Q_ENTRY_SIG\n",OSTimeGet());
#endif

                //Arm both time events
                QTimeEvt_armX(&me->ledScreenTimeEvent, me->screenUpdateRate, me->screenUpdateRate);
                QTimeEvt_armX(&me->sensorDataTimeEvent, me->sensorSamplingRate, me->sensorSamplingRate);
            status_ = Q_HANDLED();
            break;
        }
        //${AOs::TickerKeeper::SM::running}
        case Q_EXIT_SIG: {
            //Disarm both time events
            //USER CODE
#ifdef DEBUG_CFG
			printf("Tick: %i, TickerKeeper_running: Q_EXIT_SIG\n",OSTimeGet());
#endif

			//Disarm both time events
			(void)QTimeEvt_disarm(&me->ledScreenTimeEvent);
			(void)QTimeEvt_disarm(&me->sensorDataTimeEvent);
            status_ = Q_HANDLED();
            break;
        }
        //${AOs::TickerKeeper::SM::running::PAUSE}
        case PAUSE_SIG: {
            status_ = Q_TRAN(&TickerKeeper_paused);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

//${AOs::TickerKeeper::SM::paused} ...........................................
QState TickerKeeper_paused(TickerKeeper * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        //${AOs::TickerKeeper::SM::paused::PAUSE}
        case PAUSE_SIG: {
            status_ = Q_TRAN(&TickerKeeper_running);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
//$enddef${AOs::TickerKeeper} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

/*$define${AO::TickerKeeper_ctor}*/
void TickerKeeper_ctor(TickerKeeper *me){
    Central *Container = (Central*) AO_Central;
    QHsm_ctor(&me->super, Q_STATE_CAST(&TickerKeeper_initial));
    QTimeEvt_ctorX(&me->ledScreenTimeEvent, &Container->super, UPDATESCREEN_SIG, 0U);
    QTimeEvt_ctorX(&me->sensorDataTimeEvent, &Container->super, SENSOROBTAIN_SIG, 0U);
}
