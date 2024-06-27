//$file${src::qv::qv.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: qpcpp.qm
// File:  ${src::qv::qv.cpp}
//
// This code has been generated by QM 6.1.1 <www.state-machine.com/qm>.
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// This code is covered by the following QP license:
// License #    : LicenseRef-QL-dual
// Issued to    : Any user of the QP/C++ real-time embedded framework
// Framework(s) : qpcpp
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
//$endhead${src::qv::qv.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#define QP_IMPL             // this is QP implementation
#include "qp_port.hpp"      // QP port
#include "qp_pkg.hpp"       // QP package-scope interface
#include "qsafe.h"          // QP Functional Safety (FuSa) Subsystem
#ifdef Q_SPY                // QS software tracing enabled?
    #include "qs_port.hpp"  // QS port
    #include "qs_pkg.hpp"   // QS facilities for pre-defined trace records
#else
    #include "qs_dummy.hpp" // disable the QS software tracing
#endif // Q_SPY

// protection against including this source file in a wrong project
#ifndef QV_HPP_
    #error "Source file included in a project NOT based on the QV kernel"
#endif // QV_HPP_

// unnamed namespace for local definitions with internal linkage
namespace {
Q_DEFINE_THIS_MODULE("qv")
} // unnamed namespace

//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${QV::QV-base} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace QP {
namespace QV {

//${QV::QV-base::priv_} ......................................................
QV::Attr priv_;

//${QV::QV-base::schedDisable} ...............................................
void schedDisable(std::uint_fast8_t const ceiling) {
    QF_CRIT_STAT
    QF_CRIT_ENTRY();
    QF_MEM_SYS();

    QP_ASSERT_INCRIT(102, priv_.schedCeil
        == static_cast<std::uint_fast8_t>(~priv_.schedCeil_dis));

    if (ceiling > priv_.schedCeil) { // raising the scheduler ceiling?

        QS_BEGIN_PRE_(QS_SCHED_LOCK, 0U)
            QS_TIME_PRE_();   // timestamp
            // the previous sched ceiling & new sched ceiling
            QS_2U8_PRE_(static_cast<std::uint8_t>(priv_.schedCeil),
                        static_cast<std::uint8_t>(ceiling));
        QS_END_PRE_()

        priv_.schedCeil = ceiling;
    #ifndef Q_UNSAFE
        priv_.schedCeil_dis = static_cast<std::uint_fast8_t>(~ceiling);
    #endif
    }
    QF_MEM_APP();
    QF_CRIT_EXIT();
}

//${QV::QV-base::schedEnable} ................................................
void schedEnable() {
    QF_CRIT_STAT
    QF_CRIT_ENTRY();
    QF_MEM_SYS();

    QP_ASSERT_INCRIT(202, priv_.schedCeil
        == static_cast<std::uint_fast8_t>(~priv_.schedCeil_dis));

    if (priv_.schedCeil != 0U) { // actually enabling the scheduler?

        QS_BEGIN_PRE_(QS_SCHED_UNLOCK, 0U)
            QS_TIME_PRE_(); // timestamp
            // current sched ceiling (old), previous sched ceiling (new)
            QS_2U8_PRE_(static_cast<std::uint8_t>(priv_.schedCeil), 0U);
        QS_END_PRE_()

        priv_.schedCeil = 0U;
    #ifndef Q_UNSAFE
        priv_.schedCeil_dis = ~static_cast<std::uint_fast8_t>(0U);
    #endif
    }
    QF_MEM_APP();
    QF_CRIT_EXIT();
}

} // namespace QV
} // namespace QP
//$enddef${QV::QV-base} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${QV::QF-cust} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace QP {
namespace QF {

//${QV::QF-cust::init} .......................................................
void init() {
    bzero_(&QF::priv_,                 sizeof(QF::priv_));
    bzero_(&QV::priv_,                 sizeof(QV::priv_));
    bzero_(&QActive::registry_[0],     sizeof(QActive::registry_));

    #ifndef Q_UNSAFE
    QV::priv_.readySet.update_(&QV::priv_.readySet_dis);
    QV::priv_.schedCeil_dis = ~static_cast<std::uint_fast8_t>(0U);
    #endif

    #ifdef QV_INIT
    QV_INIT(); // port-specific initialization of the QV kernel
    #endif
}

//${QV::QF-cust::stop} .......................................................
void stop() {
    onCleanup(); // cleanup callback
    // nothing else to do for the QV kernel
}

//${QV::QF-cust::run} ........................................................
int_t run() {
    #ifdef Q_SPY
    // produce the QS_QF_RUN trace record
    QF_INT_DISABLE();
    QF_MEM_SYS();
    QS::beginRec_(QS_REC_NUM_(QS_QF_RUN));
    QS::endRec_();
    QF_MEM_APP();
    QF_INT_ENABLE();
    #endif // Q_SPY

    onStartup(); // application-specific startup callback

    QF_INT_DISABLE();
    QF_MEM_SYS();

    #ifdef QV_START
    QV_START(); // port-specific startup of the QV kernel
    #endif

    #if (defined QF_ON_CONTEXT_SW) || (defined Q_SPY)
    std::uint_fast8_t pprev = 0U; // previous prio.
    #endif

    for (;;) { // QV event loop...

        // check internal integrity (duplicate inverse storage)
        QP_ASSERT_INCRIT(302,
             QV::priv_.readySet.verify_(&QV::priv_.readySet_dis));
        // check internal integrity (duplicate inverse storage)
        QP_ASSERT_INCRIT(303, QV::priv_.schedCeil
            == static_cast<std::uint_fast8_t>(~QV::priv_.schedCeil_dis));

        // find the maximum prio. AO ready to run
        std::uint_fast8_t const p = (QV::priv_.readySet.notEmpty()
                               ? QV::priv_.readySet.findMax()
                               : 0U);

        if (p > QV::priv_.schedCeil) { // is it above the sched ceiling?
            QActive * const a = QActive::registry_[p];

    #if (defined QF_ON_CONTEXT_SW) || (defined Q_SPY)
            QS_BEGIN_PRE_(QS_SCHED_NEXT, p)
                QS_TIME_PRE_();        // timestamp
                QS_2U8_PRE_(static_cast<std::uint8_t>(p),
                            static_cast<std::uint8_t>(pprev));
            QS_END_PRE_()

    #ifdef QF_ON_CONTEXT_SW
            QF_onContextSw(((pprev != 0U)
                            ? QActive::registry_[pprev]
                            : nullptr), a);
    #endif // QF_ON_CONTEXT_SW

            pprev = p; // update previous prio.
    #endif // (defined QF_ON_CONTEXT_SW) || (defined Q_SPY)

            QF_MEM_APP();
            QF_INT_ENABLE();

            QEvt const * const e = a->get_();
            // NOTE QActive::get_() performs QF_MEM_APP() before return

            // dispatch event (virtual call)
            a->dispatch(e, a->getPrio());
    #if (QF_MAX_EPOOL > 0U)
            gc(e);
    #endif
            QF_INT_DISABLE();
            QF_MEM_SYS();

            if (a->getEQueue().isEmpty()) { // empty queue?
                QV::priv_.readySet.remove(p);
    #ifndef Q_UNSAFE
                QV::priv_.readySet.update_(&QV::priv_.readySet_dis);
    #endif
            }
        }
        else { // no AO ready to run --> idle
    #if (defined QF_ON_CONTEXT_SW) || (defined Q_SPY)
            if (pprev != 0U) {
                QS_BEGIN_PRE_(QS_SCHED_IDLE, pprev)
                    QS_TIME_PRE_();    // timestamp
                    QS_U8_PRE_(static_cast<std::uint8_t>(pprev));
                QS_END_PRE_()

    #ifdef QF_ON_CONTEXT_SW
                QF_onContextSw(QActive::registry_[pprev], nullptr);
    #endif // QF_ON_CONTEXT_SW

                pprev = 0U; // update previous prio
            }
    #endif // (defined QF_ON_CONTEXT_SW) || (defined Q_SPY)

            QF_MEM_APP();

            // QV::onIdle() must be called with interrupts DISABLED
            // because the determination of the idle condition (all event
            // queues empty) can change at any time by an interrupt posting
            // events to a queue.
            //
            // NOTE: QV::onIdle() MUST enable interrupts internally,
            // ideally at the same time as putting the CPU into a power-
            // saving mode.
            QV::onIdle();

            QF_INT_DISABLE();
            QF_MEM_SYS();
        }
    }
    #ifdef __GNUC__ // GNU compiler?
    return 0;
    #endif
}

} // namespace QF
} // namespace QP
//$enddef${QV::QF-cust} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${QV::QActive} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace QP {

//${QV::QActive} .............................................................

//${QV::QActive::start} ......................................................
void QActive::start(
    QPrioSpec const prioSpec,
    QEvt const * * const qSto,
    std::uint_fast16_t const qLen,
    void * const stkSto,
    std::uint_fast16_t const stkSize,
    void const * const par)
{
    Q_UNUSED_PAR(stkSto);  // not needed in QV
    Q_UNUSED_PAR(stkSize); // not needed in QV

    QF_CRIT_STAT
    QF_CRIT_ENTRY();
    Q_REQUIRE_INCRIT(300, stkSto == nullptr);
    QF_CRIT_EXIT();

    m_prio  = static_cast<std::uint8_t>(prioSpec & 0xFFU); //  QF-prio.
    m_pthre = 0U; // not used
    register_(); // make QF aware of this AO

    m_eQueue.init(qSto, qLen); // initialize QEQueue of this AO

    this->init(par, m_prio); // take the top-most initial tran. (virtual)
    QS_FLUSH(); // flush the trace buffer to the host
}

} // namespace QP
//$enddef${QV::QActive} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
