//$file${.::periodic1.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: real-time.qm
// File:  ${.::periodic1.cpp}
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
//$endhead${.::periodic1.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpcpp.hpp"           // QP/C++ real-time embedded framework
#include "bsp.hpp"             // Board Support Package
#include "app.hpp"             // Application interface

namespace {
Q_DEFINE_THIS_MODULE("periodic1")
}

//$declare${AOs::Periodic1} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${AOs::Periodic1} ..........................................................
class Periodic1 : public QP::QMActive {
private:
    QP::QTimeEvt m_te;
    std::uint16_t m_toggles;

public:
    static Periodic1 inst;

public:
    Periodic1();

protected:
    QM_STATE_DECL( initial);
    QM_STATE_DECL( active);
}; // class Periodic1

} // namespace APP
//$enddecl${AOs::Periodic1} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${Shared::AO_Periodic1} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Shared::AO_Periodic1} ....................................................
QP::QActive * const AO_Periodic1 {&Periodic1::inst};

} // namespace APP
//$enddef${Shared::AO_Periodic1} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${AOs::Periodic1} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${AOs::Periodic1} ..........................................................
Periodic1 Periodic1::inst;

//${AOs::Periodic1::Periodic1} ...............................................
Periodic1::Periodic1()
  : QMActive(Q_STATE_CAST(&Periodic1::initial)),
    m_te(this, TIMEOUT_SIG, 0U)
{}

//${AOs::Periodic1::SM} ......................................................
QM_STATE_DEF(Periodic1, initial) {
    //${AOs::Periodic1::SM::initial}
    Q_REQUIRE_ID(300, (QP::QEvt::verify_(e))
                 && (e->sig == PERIODIC_SPEC_SIG));

    m_te.armX(Q_EVT_CAST(PeriodicSpecEvt)->ticks,
              Q_EVT_CAST(PeriodicSpecEvt)->ticks);
    m_toggles = Q_EVT_CAST(PeriodicSpecEvt)->toggles;

    QS_FUN_DICTIONARY(&Periodic1::active);
    static QP::QMTranActTable const tatbl_ = { // tran-action table
        &active_s, // target state
        {
            Q_ACTION_NULL // zero terminator
        }
    };
    return qm_tran_init(&tatbl_);
}

//${AOs::Periodic1::SM::active} ..............................................
QP::QMState const Periodic1::active_s = {
    QM_STATE_NULL, // superstate (top)
    &Periodic1::active,
    Q_ACTION_NULL, // no entry action
    Q_ACTION_NULL, // no exit action
    Q_ACTION_NULL  // no initial tran.
};
//${AOs::Periodic1::SM::active}
QM_STATE_DEF(Periodic1, active) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Periodic1::SM::active::TIMEOUT}
        case TIMEOUT_SIG: {
            for (std::uint16_t i = m_toggles; i > 0U; --i) {
                BSP::d6on();
                BSP::d6off();
            }
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Periodic1::SM::active::PERIODIC_SPEC}
        case PERIODIC_SPEC_SIG: {
            BSP::d6on();
            m_te.disarm();
            m_te.armX(Q_EVT_CAST(PeriodicSpecEvt)->ticks,
                      Q_EVT_CAST(PeriodicSpecEvt)->ticks);
            m_toggles = Q_EVT_CAST(PeriodicSpecEvt)->toggles;
            BSP::d6off();
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = Q_RET_SUPER;
            break;
        }
    }
    return status_;
}

} // namespace APP
//$enddef${AOs::Periodic1} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^