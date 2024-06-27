//============================================================================
// QP/C++ Real-Time Embedded Framework (RTEF)
//
//                   Q u a n t u m  L e a P s
//                   ------------------------
//                   Modern Embedded Software
//
// Copyright (C) 2005 Quantum Leaps, LLC <state-machine.com>.
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
//============================================================================
//! @date Last updated on: 2023-09-07
//! @version Last updated for: @ref qpcpp_7_3_0
//!
//! @file
//! @brief QP/C++ port to PIC32, cooperative QV kernel, MPLAB-X XC32

#ifndef QP_PORT_HPP_
#define QP_PORT_HPP_

#include <cstdint>  // Exact-width types. C++11 Standard
#include <xc.h>     // for _clz()

#ifdef QP_CONFIG
#include "qp_config.hpp" // external QP configuration
#endif

// no-return function specifier (C++11 Standard)
#define QP_NORETURN  [[ noreturn ]] void

// QF configuration for QK -- data members of the QActive class...

// QActive event queue type
#define QACTIVE_EQUEUE_TYPE     QEQueue
// QACTIVE_OS_OBJ_TYPE not used in this port
// QACTIVE_THREAD_TYPE not used in this port

// QF interrupt disable/enable, see NOTE1
#define QF_INT_DISABLE()        __builtin_disable_interrupts()
#define QF_INT_ENABLE()         __builtin_enable_interrupts()

// QF critical section entry/exit, see NOTE1
#define QF_CRIT_STAT
#define QF_CRIT_ENTRY()         QF_INT_DISABLE()
#define QF_CRIT_EXIT()          QF_INT_ENABLE()

// fast log-base-2 with CLZ instruction
#define QF_LOG2(n_) (static_cast<std::uint8_t>(32U - _clz(n_)))

// include files -------------------------------------------------------------
#include "qequeue.hpp"   // QV kernel uses the native QP event queue
#include "qmpool.hpp"    // QV kernel uses the native QP memory pool
#include "qp.hpp"        // QP framework
#include "qv.hpp"        // QV kernel

//============================================================================
// NOTE1:
// The intrinsic functions __builtin_disable_interrupts() and
// __builtin_enable_interrupts() use the DI (disable interrupts) and
// EI (enable interrupts) instruction, unconditional disabling and enabling
// of interrupts. The DI instruction only disables interrupts with priority
// levels 1-6. Priority level 7 interrupts and all trap events still have
// the ability to interrupt the CPU when the DI instruction is active.
// This means that from the perspective of QP framework, the level 7 interrupts
// are treated "kernel unaware" interrupts or non-maskable interrupts.
// Such "kernel unaware" interrupts **cannot** call any QP services.
// In particular, they cannot post events.
//
// CAUTION: This QP port assumes that interrupt nesting is **enabled**,
// which is the default in the PIC32 processors. Interrupt nesting should
// never be disabled by setting the NSTDIS control bit (INTCON1<15>). If you
// don't want interrupts to nest, you can always prioritize them at the same
// level. For example, the default priority level for all interrupts is 4 out
// of reset. If you don't change this level for any interrupt the nesting of
// interrupt will not occur.
//

#endif // QP_PORT_HPP_

