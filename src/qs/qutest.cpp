//$file${src::qs::qutest.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: qpcpp.qm
// File:  ${src::qs::qutest.cpp}
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
//$endhead${src::qs::qutest.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// only build when Q_UTEST is defined
#ifdef Q_UTEST

#define QP_IMPL             // this is QP implementation
#include "qp_port.hpp"      // QP port
#include "qp_pkg.hpp"       // QP package-scope interface
#include "qsafe.h"          // QP Functional Safety (FuSa) Subsystem
#include "qs_port.hpp"      // QS port
#include "qs_pkg.hpp"       // QS package-scope internal interface

//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${QS::QUTest} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace QP {
namespace QS {

} // namespace QS
} // namespace QP
//$enddef${QS::QUTest} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//============================================================================
//! @cond INTERNAL

namespace QP {
namespace QS {

TestAttr tstPriv_;

//............................................................................
void test_pause_() {
    beginRec_(static_cast<std::uint_fast8_t>(QS_TEST_PAUSED));
    endRec_();
    onTestLoop();
}

//............................................................................
std::uint32_t getTestProbe_(QSpyFunPtr const api) noexcept {
    std::uint32_t data = 0U;
    for (std::uint8_t i = 0U; i < tstPriv_.tpNum; ++i) {
        if (tstPriv_.tpBuf[i].addr == reinterpret_cast<QSFun>(api)) {
            data = tstPriv_.tpBuf[i].data;

            QS_CRIT_STAT
            QS_CRIT_ENTRY();
            QS_MEM_SYS();
            QS::beginRec_(static_cast<std::uint_fast8_t>(QS_TEST_PROBE_GET));
                QS_TIME_PRE_();    // timestamp
                QS_FUN_PRE_(api);  // the calling API
                QS_U32_PRE_(data); // the Test-Probe data
            QS::endRec_();

            QS_REC_DONE(); // user callback (if defined)

            --tstPriv_.tpNum; // one less Test-Probe
            // move all remaining entries in the buffer up by one
            for (std::uint8_t j = i; j < tstPriv_.tpNum; ++j) {
                tstPriv_.tpBuf[j] = tstPriv_.tpBuf[j + 1U];
            }
            QS_MEM_APP();
            QS_CRIT_EXIT();
            break; // we are done (Test-Probe retrieved)
        }
    }
    return data;
}

//............................................................................
QSTimeCtr onGetTime() {
    return (++tstPriv_.testTime);
}

} // namespace QS
} // namespace QP

//............................................................................
extern "C" {

Q_NORETURN Q_onError(
    char const * const module,
    int_t const id)
{
    // NOTE: called in a critical section

    QP::QS::beginRec_(static_cast<std::uint_fast8_t>(QP::QS_ASSERT_FAIL));
        QS_TIME_PRE_();
        QS_U16_PRE_(id);
        QS_STR_PRE_((module != nullptr) ? module : "?");
    QP::QS::endRec_();
    QP::QS::onFlush();   // flush the assertion record to the host

    QP::QS::onCleanup(); // cleanup after the failure
    QP::QS::onReset();   // reset the target to prevent it from continuing
    for (;;) { // onReset() should not return, but to ensure no-return...
    }
}

} // extern "C"

//! @endcond
//============================================================================

// QP-stub for QUTest
// NOTE: The QP-stub is needed for unit testing QP applications, but might
// NOT be needed for testing QP itself. In that case, the build process
// can define Q_UTEST=0 to exclude the QP-stub from the build.
#if Q_UTEST != 0

// unnamed namespace for local definitions with internal linkage
namespace {
Q_DEFINE_THIS_MODULE("qutest")
} // unnamed namespace

namespace QP {
namespace QS {

void processTestEvts_() {
    QS_TEST_PROBE_DEF(&QS::processTestEvts_)

    // return immediately (do nothing) for Test Probe != 0
    QS_TEST_PROBE(return;)

    while (tstPriv_.readySet.notEmpty()) {
        std::uint_fast8_t const p = tstPriv_.readySet.findMax();
        QActive * const a = QActive::registry_[p];

        QEvt const * const e = a->get_();
        a->dispatch(e, a->getPrio());
    #if (QF_MAX_EPOOL > 0U)
        QF::gc(e);
    #endif
        if (a->getEQueue().isEmpty()) { // empty queue?
            tstPriv_.readySet.remove(p);
#ifndef Q_UNSAFE
            tstPriv_.readySet.update_(&tstPriv_.readySet_dis);
#endif
        }
    }
}

} // namespace QS
} // namespace QP

//$define${QS::QUTest-stub} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace QP {
namespace QF {

//${QS::QUTest-stub::QF::init} ...............................................
void init() {
    // Clear the internal QF variables, so that the framework can start
    // correctly even if the startup code fails to clear the uninitialized
    // data (as is required by the C++ Standard).
    bzero_(&QF::priv_,    sizeof(QF::priv_));
    bzero_(&QS::tstPriv_, sizeof(QS::tstPriv_));
    bzero_(&QActive::registry_[0], sizeof(QActive::registry_));

    #ifndef Q_UNSAFE
    QS::tstPriv_.readySet.update_(&QS::tstPriv_.readySet_dis);
    #endif
}

//${QS::QUTest-stub::QF::stop} ...............................................
void stop() {
    QS::onReset();
}

//${QS::QUTest-stub::QF::run} ................................................
int_t run() {
    // function dictionaries for the standard API
    QS_FUN_DICTIONARY(&QActive::post_);
    QS_FUN_DICTIONARY(&QActive::postLIFO);
    QS_FUN_DICTIONARY(&QS::processTestEvts_);

    QS_CRIT_STAT
    QS_CRIT_ENTRY();
    QS_MEM_SYS();

    // produce the QS_QF_RUN trace record
    QS_BEGIN_PRE_(QS_QF_RUN, 0U)
    QS_END_PRE_()

    QS_MEM_APP();
    QS_CRIT_EXIT();

    QS::processTestEvts_(); // process all events posted so far
    QS::onTestLoop(); // run the unit test
    QS::onCleanup();  // application cleanup
    return 0;         // return no error
}

} // namespace QF

//${QS::QUTest-stub::QActive} ................................................

//${QS::QUTest-stub::QActive::start} .........................................
void QActive::start(
    QPrioSpec const prioSpec,
    QEvt const * * const qSto,
    std::uint_fast16_t const qLen,
    void * const stkSto,
    std::uint_fast16_t const stkSize,
    void const * const par)
{
    Q_UNUSED_PAR(stkSto);
    Q_UNUSED_PAR(stkSize);

    m_prio  = static_cast<std::uint8_t>(prioSpec & 0xFFU); //  QF-priol
    m_pthre = static_cast<std::uint8_t>(prioSpec >> 8U); // preemption-thre.
    register_(); // make QF aware of this AO

    m_eQueue.init(qSto, qLen); // initialize QEQueue of this AO

    this->init(par, m_prio); // take the top-most initial tran. (virtual)
}

//${QS::QUTest-stub::QActive::stop} ..........................................
#ifdef QACTIVE_CAN_STOP
void QActive::stop() {
    unsubscribeAll(); // unsubscribe from all events
    unregister_(); // remove this object from QF
}

#endif // def QACTIVE_CAN_STOP

//${QS::QUTest-stub::QTimeEvt} ...............................................

//${QS::QUTest-stub::QTimeEvt::tick1_} .......................................
void QTimeEvt::tick1_(
    std::uint_fast8_t const tickRate,
    void const * const sender)
{
    QS_CRIT_STAT
    QS_CRIT_ENTRY();
    QS_MEM_SYS();

    QTimeEvt *prev = &QTimeEvt::timeEvtHead_[tickRate];

    QS_BEGIN_PRE_(QS_QF_TICK, 0U)
        prev->m_ctr = (prev->m_ctr + 1U);
        QS_TEC_PRE_(prev->m_ctr); // tick ctr
        QS_U8_PRE_(tickRate);     // tick rate
    QS_END_PRE_()

    // is current Time Event object provided?
    QTimeEvt *t = static_cast<QTimeEvt *>(QS::rxPriv_.currObj[QS::TE_OBJ]);
    if (t != nullptr) {

        // the time event must be armed
        Q_ASSERT_INCRIT(810, t->m_ctr != 0U);

        QActive * const act = static_cast<QActive *>(t->m_act);

        // the recipient AO must be provided
        Q_ASSERT_INCRIT(820, act != nullptr);

        // periodic time evt?
        if (t->m_interval != 0U) {
            t->m_ctr = t->m_interval; // rearm the time event
        }
        else { // one-shot time event: automatically disarm
            t->m_ctr = 0U; // auto-disarm
            // mark time event 't' as NOT linked
            t->refCtr_ = static_cast<std::uint8_t>(t->refCtr_
                         & static_cast<std::uint8_t>(~TE_IS_LINKED));

            QS_BEGIN_PRE_(QS_QF_TIMEEVT_AUTO_DISARM, act->m_prio)
                QS_OBJ_PRE_(t);       // this time event object
                QS_OBJ_PRE_(act);     // the target AO
                QS_U8_PRE_(tickRate); // tick rate
            QS_END_PRE_()
        }

        QS_BEGIN_PRE_(QS_QF_TIMEEVT_POST, act->m_prio)
            QS_TIME_PRE_();           // timestamp
            QS_OBJ_PRE_(t);           // the time event object
            QS_SIG_PRE_(t->sig);      // signal of this time event
            QS_OBJ_PRE_(act);         // the target AO
            QS_U8_PRE_(tickRate);     // tick rate
        QS_END_PRE_()
        QS_MEM_APP();
        QS_CRIT_EXIT(); // exit critical section before posting

        // asserts if queue overflows
        static_cast<void>(act->POST(t, sender));

        QS_CRIT_ENTRY();
        QS_MEM_SYS();
    }

    // update the linked list of time events
    for (;;) {
        t = prev->m_next; // advance down the time evt. list

        // end of the list?
        if (t == nullptr) {

            // any new time events armed since the last run of tick()?
            if (QTimeEvt::timeEvtHead_[tickRate].m_act != nullptr) {

                // sanity check
                Q_ASSERT_INCRIT(830, prev != nullptr);
                prev->m_next = QTimeEvt::timeEvtHead_[tickRate].toTimeEvt();
                QTimeEvt::timeEvtHead_[tickRate].m_act = nullptr;
                t = prev->m_next; // switch to the new list
            }
            else {
                break; // all currently armed time evts. processed
            }
        }

        // time event scheduled for removal?
        if (t->m_ctr == 0U) {
            prev->m_next = t->m_next;
            // mark time event 't' as NOT linked
            t->refCtr_ = static_cast<std::uint8_t>(t->refCtr_
                         & static_cast<std::uint8_t>(~TE_IS_LINKED));
            // do NOT advance the prev pointer
            QS_MEM_APP();
            QS_CRIT_EXIT(); // exit crit. section to reduce latency
        }
        else {
            prev = t; // advance to this time event
            QS_MEM_APP();
            QS_CRIT_EXIT(); // exit crit. section to reduce latency
        }
        QS_CRIT_ENTRY(); // re-enter crit. section to continue
        QS_MEM_SYS();
    }

    QS_MEM_APP();
    QS_CRIT_EXIT();
}

//${QS::QUTest-stub::QHsmDummy} ..............................................

//${QS::QUTest-stub::QHsmDummy::QHsmDummy} ...................................
QHsmDummy::QHsmDummy()
: QAsm()
{}

//${QS::QUTest-stub::QHsmDummy::init} ........................................
void QHsmDummy::init(
    void const * const e,
    std::uint_fast8_t const qsId)
{
    Q_UNUSED_PAR(e);

    #ifdef Q_SPY
    if ((QS::priv_.flags & 0x01U) == 0U) {
        QS::priv_.flags |= 0x01U;
        QS_FUN_DICTIONARY(&QP::QHsm::top);
    }
    #else
    Q_UNUSED_PAR(qsId);
    #endif

    QS_CRIT_STAT
    QS_CRIT_ENTRY();
    QS_MEM_SYS();
    QS_BEGIN_PRE_(QS_QEP_STATE_INIT, qsId)
        QS_OBJ_PRE_(this);          // this state machine object
        QS_FUN_PRE_(m_state.fun);   // the source state
        QS_FUN_PRE_(m_temp.fun);    // the target of the initial tran.
    QS_END_PRE_()
    QS_MEM_APP();
    QS_CRIT_EXIT();
}

//${QS::QUTest-stub::QHsmDummy::dispatch} ....................................
void QHsmDummy::dispatch(
    QEvt const * const e,
    std::uint_fast8_t const qsId)
{
    QS_CRIT_STAT
    QS_CRIT_ENTRY();
    QS_MEM_SYS();
    QS_BEGIN_PRE_(QS_QEP_DISPATCH, qsId)
        QS_TIME_PRE_();             // time stamp
        QS_SIG_PRE_(e->sig);        // the signal of the event
        QS_OBJ_PRE_(this);          // this state machine object
        QS_FUN_PRE_(m_state.fun);   // the current state
    QS_END_PRE_()
    QS_MEM_APP();
    QS_CRIT_EXIT();
}

//${QS::QUTest-stub::QActiveDummy} ...........................................

//${QS::QUTest-stub::QActiveDummy::QActiveDummy} .............................
QActiveDummy::QActiveDummy()
: QActive(nullptr)
{}

//${QS::QUTest-stub::QActiveDummy::init} .....................................
void QActiveDummy::init(
    void const * const e,
    std::uint_fast8_t const qsId)
{
    Q_UNUSED_PAR(e);
    Q_UNUSED_PAR(qsId);

    #ifdef Q_SPY
    if ((QS::priv_.flags & 0x01U) == 0U) {
        QS::priv_.flags |= 0x01U;
        QS_FUN_DICTIONARY(&QP::QHsm::top);
    }
    #endif

    QS_CRIT_STAT
    QS_CRIT_ENTRY();
    QS_MEM_SYS();
    QS_BEGIN_PRE_(QS_QEP_STATE_INIT, m_prio)
        QS_OBJ_PRE_(this);        // this state machine object
        QS_FUN_PRE_(m_state.fun); // the source state
        QS_FUN_PRE_(m_temp.fun);  // the target of the initial tran.
    QS_END_PRE_()
    QS_MEM_APP();
    QS_CRIT_EXIT();
}

//${QS::QUTest-stub::QActiveDummy::dispatch} .................................
void QActiveDummy::dispatch(
    QEvt const * const e,
    std::uint_fast8_t const qsId)
{
    QS_CRIT_STAT
    QS_CRIT_ENTRY();
    QS_MEM_SYS();
    QS_BEGIN_PRE_(QS_QEP_DISPATCH, qsId)
        QS_TIME_PRE_();           // time stamp
        QS_SIG_PRE_(e->sig);      // the signal of the event
        QS_OBJ_PRE_(this);        // this state machine object
        QS_FUN_PRE_(m_state.fun); // the current state
    QS_END_PRE_()
    QS_MEM_APP();
    QS_CRIT_EXIT();
}

//${QS::QUTest-stub::QActiveDummy::fakePost} .................................
bool QActiveDummy::fakePost(
    QEvt const * const e,
    std::uint_fast16_t const margin,
    void const * const sender) noexcept
{
    QS_TEST_PROBE_DEF(&QActive::post_)

    QF_CRIT_STAT
    QF_CRIT_ENTRY();
    QF_MEM_SYS();

    // test-probe#1 for faking queue overflow
    bool status = true;
    QS_TEST_PROBE_ID(1,
        status = false;
        if (margin == QF::NO_MARGIN) {
            // fake assertion Mod=qf_actq,Loc=110
            Q_onError("qf_actq", 110);
        }
    )

    // is it a mutable event?
    if (e->getPoolNum_() != 0U) {
        QEvt_refCtr_inc_(e);
    }

    std::uint_fast8_t const rec =
        (status ? static_cast<std::uint8_t>(QS_QF_ACTIVE_POST)
                : static_cast<std::uint8_t>(QS_QF_ACTIVE_POST_ATTEMPT));
    QS_BEGIN_PRE_(rec, m_prio)
        QS_TIME_PRE_();      // timestamp
        QS_OBJ_PRE_(sender); // the sender object
        QS_SIG_PRE_(e->sig); // the signal of the event
        QS_OBJ_PRE_(this);   // this active object
        QS_2U8_PRE_(e->getPoolNum_(), e->refCtr_); // poolNum & refCtr
        QS_EQC_PRE_(0U);     // # free entries
        QS_EQC_PRE_(margin); // margin requested
    QS_END_PRE_()

    QF_MEM_APP();
    QF_CRIT_EXIT();

    // callback to examine the posted event under the same conditions
    // as producing the #QS_QF_ACTIVE_POST trace record, which are:
    // the local filter for this AO ('m_prio') is set
    if (QS_LOC_CHECK_(m_prio)) {
        QS::onTestPost(sender, this, e, status);
    }

    // recycle the event immediately, because it was not really posted
    #if (QF_MAX_EPOOL > 0U)
    QF::gc(e);
    #endif

    return status;
}

//${QS::QUTest-stub::QActiveDummy::fakePostLIFO} .............................
void QActiveDummy::fakePostLIFO(QEvt const * const e) noexcept {
    QS_TEST_PROBE_DEF(&QActive::postLIFO)

    QF_CRIT_STAT
    QF_CRIT_ENTRY();
    QF_MEM_SYS();

    // test-probe#1 for faking queue overflow
    QS_TEST_PROBE_ID(1,
        // fake assertion Mod=qf_actq,Loc=210
        Q_onError("qf_actq", 210);
    )

    // is it a mutable event?
    if (e->getPoolNum_() != 0U) {
        QEvt_refCtr_inc_(e);
    }

    QS_BEGIN_PRE_(QS_QF_ACTIVE_POST_LIFO, m_prio)
        QS_TIME_PRE_();      // timestamp
        QS_SIG_PRE_(e->sig); // the signal of this event
        QS_OBJ_PRE_(this);   // this active object
        QS_2U8_PRE_(e->getPoolNum_(), e->refCtr_); // poolNum & refCtr
        QS_EQC_PRE_(0U);     // # free entries
        QS_EQC_PRE_(0U);     // min # free entries
    QS_END_PRE_()

    QF_MEM_APP();
    QF_CRIT_EXIT();

    // callback to examine the posted event under the same conditions
    // as producing the #QS_QF_ACTIVE_POST trace record, which are:
    // the local filter for this AO ('m_prio') is set
    if (QS_LOC_CHECK_(m_prio)) {
        QS::onTestPost(nullptr, this, e, true);
    }
    // recycle the event immediately, because it was not really posted
    #if (QF_MAX_EPOOL > 0U)
    QF::gc(e);
    #endif
}

} // namespace QP
//$enddef${QS::QUTest-stub} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#endif // Q_UTEST != 0

#endif // def Q_UTEST
