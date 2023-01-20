//$file${src::qf::qf_mem.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: qpcpp.qm
// File:  ${src::qf::qf_mem.cpp}
//
// This code has been generated by QM 5.3.0 <www.state-machine.com/qm>.
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
//$endhead${src::qf::qf_mem.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
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

// unnamed namespace for local definitions with internal linkage
namespace {
Q_DEFINE_THIS_MODULE("qf_mem")
} // unnamed namespace

//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${QF::QMPool} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace QP {

//${QF::QMPool} ..............................................................

//${QF::QMPool::init} ........................................................
void QMPool::init(
    void * const poolSto,
    std::uint_fast32_t const poolSize,
    std::uint_fast16_t const blockSize) noexcept
{
    QF_CRIT_STAT
    QF_CRIT_ENTRY();
    QF_MEM_SYS();

    Q_REQUIRE_INCRIT(100, (poolSto != nullptr)
        && (poolSize >= static_cast<std::uint_fast32_t>(sizeof(QFreeBlock)))
        && (static_cast<std::uint_fast16_t>(blockSize + sizeof(QFreeBlock))
            > blockSize));

    m_free_head = static_cast<QFreeBlock *>(poolSto);

    // find # free blocks in a memory block, NO DIVISION
    m_blockSize = static_cast<QMPoolSize>(sizeof(QFreeBlock));
    std::uint_fast16_t nblocks = 1U;
    while (m_blockSize < static_cast<QMPoolSize>(blockSize)) {
        m_blockSize += static_cast<QMPoolSize>(sizeof(QFreeBlock));
        ++nblocks;
    }

    // the pool buffer must fit at least one rounded-up block
    Q_ASSERT_INCRIT(110, poolSize >= m_blockSize);

    // start at the head of the free list
    QFreeBlock *fb = m_free_head;
    m_nTot = 1U; // the last block already in the list

    // chain all blocks together in a free-list...
    for (std::uint_fast32_t size = poolSize - m_blockSize;
         size >= static_cast<std::uint_fast32_t>(m_blockSize);
         size -= static_cast<std::uint_fast32_t>(m_blockSize))
    {
        fb->m_next = &fb[nblocks]; // point next link to next block
    #ifndef Q_UNSAFE
        fb->m_next_dis = ~Q_UINTPTR_CAST_(fb->m_next);
    #endif
        fb = fb->m_next; // advance to the next block
        ++m_nTot;        // one more free block in the pool
    }

    fb->m_next  = nullptr; // the last link points to NULL
    #ifndef Q_UNSAFE
    fb->m_next_dis = ~Q_UINTPTR_CAST_(fb->m_next);
    #endif

    fb->m_next = nullptr; // the last link points to NULL
    m_nFree = m_nTot;  // all blocks are free
    m_nMin  = m_nTot;  // the minimum # free blocks
    m_start = static_cast<QFreeBlock *>(poolSto); // original start
    m_end   = fb;      // the last block in this pool

    QF_MEM_APP();
    QF_CRIT_EXIT();
}

//${QF::QMPool::get} .........................................................
void * QMPool::get(
    std::uint_fast16_t const margin,
    std::uint_fast8_t const qs_id) noexcept
{
    #ifndef Q_SPY
    Q_UNUSED_PAR(qs_id);
    #endif

    QF_CRIT_STAT
    QF_CRIT_ENTRY();
    QF_MEM_SYS();

    // have more free blocks than the requested margin?
    QFreeBlock *fb;
    if (m_nFree > static_cast<QMPoolCtr>(margin)) {
        fb = m_free_head;  // get a free block

        //  a free block must be valid
        Q_ASSERT_INCRIT(300, fb != nullptr);

        QFreeBlock * const fb_next = fb->m_next;

        // the free block must have integrity (duplicate inverse storage)
        Q_ASSERT_INCRIT(302, Q_UINTPTR_CAST_(fb_next)
                              == static_cast<std::uintptr_t>(~fb->m_next_dis));

        m_nFree = (m_nFree - 1U); // one free block less
        if (m_nFree == 0U) { // is the pool becoming empty?
            // pool is becoming empty, so the next free block must be NULL
            Q_ASSERT_INCRIT(320, fb_next == nullptr);

            m_nMin = 0U; // remember that the pool got empty
        }
        else {
            // invariant:
            // The pool is not empty, so the next free-block pointer,
            // so the next free block must be in range.

            // NOTE: The next free block pointer can fall out of range
            // when the client code writes past the memory block, thus
            // corrupting the next block.
            Q_ASSERT_INCRIT(330,
                QF_PTR_RANGE_(fb_next, m_start, m_end));

            // is the # free blocks the new minimum so far?
            if (m_nMin > m_nFree) {
                m_nMin = m_nFree; // remember the minimum so far
            }
        }

        m_free_head = fb_next; // set the head to the next free block

        QS_BEGIN_PRE_(QS_QF_MPOOL_GET, qs_id)
            QS_TIME_PRE_();         // timestamp
            QS_OBJ_PRE_(this);      // this memory pool
            QS_MPC_PRE_(m_nFree);   // # of free blocks in the pool
            QS_MPC_PRE_(m_nMin);    // min # free blocks ever in the pool
        QS_END_PRE_()
    }
    else { // don't have enough free blocks at this point
        fb = nullptr;

        QS_BEGIN_PRE_(QS_QF_MPOOL_GET_ATTEMPT, qs_id)
            QS_TIME_PRE_();         // timestamp
            QS_OBJ_PRE_(this);      // this memory pool
            QS_MPC_PRE_(m_nFree);   // # of free blocks in the pool
            QS_MPC_PRE_(margin);    // the requested margin
        QS_END_PRE_()
    }

    QF_MEM_APP();
    QF_CRIT_EXIT();

    return fb; // return the block or nullptr to the caller
}

//${QF::QMPool::put} .........................................................
void QMPool::put(
    void * const block,
    std::uint_fast8_t const qs_id) noexcept
{
    #ifndef Q_SPY
    Q_UNUSED_PAR(qs_id);
    #endif

    QFreeBlock * const fb = static_cast<QFreeBlock *>(block);

    QF_CRIT_STAT
    QF_CRIT_ENTRY();
    QF_MEM_SYS();

    Q_REQUIRE_INCRIT(200, (m_nFree < m_nTot)
                           && QF_PTR_RANGE_(fb, m_start, m_end));

    fb->m_next = m_free_head; // link into list
    #ifndef Q_UNSAFE
    fb->m_next_dis = static_cast<std::uintptr_t>(
                         ~Q_UINTPTR_CAST_(fb->m_next));
    #endif

    // set as new head of the free list
    m_free_head = static_cast<QFreeBlock *>(block);

    m_nFree = m_nFree + 1U; // one more free block in this pool

    QS_BEGIN_PRE_(QS_QF_MPOOL_PUT, qs_id)
        QS_TIME_PRE_();         // timestamp
        QS_OBJ_PRE_(this);      // this memory pool
        QS_MPC_PRE_(m_nFree);   // the # free blocks in the pool
    QS_END_PRE_()

    QF_MEM_APP();
    QF_CRIT_EXIT();
}

//${QF::QMPool::getBlockSize} ................................................
QMPoolSize QMPool::getBlockSize() const noexcept {
    return m_blockSize;
}

} // namespace QP
//$enddef${QF::QMPool} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
