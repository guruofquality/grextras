/*
 * Copyright 2011-2012 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GRUEL_PMT_MGR_H
#define INCLUDED_GRUEL_PMT_MGR_H

#include <gnuradio/extras/api.h>
#include <gruel/pmt.h>

namespace pmt{

/*
 * ------------------------------------------------------------------------
 *      Manage a collection of PMTs -> memory wise
 *
 * When a PMT reference count becomes zero, the pmt and contents is deleted.
 * With a manager, the PMT will not be deleted, but released to the manager.
 * Then, the PMT can be acquired again for re-use by the user.
 *
 * This offers 2 benefits:
 *  - Avoids expensive memory allocation overhead (re-use is key)
 *  - An upstream producer can block until resources become released
 * ------------------------------------------------------------------------
 */

//! Return true if \p x is a manager, othewise false.
GR_EXTRAS_API bool pmt_is_mgr(pmt_t x);

//! Make a new pmt manager object
GR_EXTRAS_API pmt_t pmt_make_mgr(void);

/*!
 * \brief Set a pmt to the specified manager.
 *
 * \param mgr the pmt manager object
 * \param x any other object of type pmt
 */
GR_EXTRAS_API void pmt_mgr_set(pmt_t mgr, pmt_t x);

/*!
 * \brief Unset a pmt from the specified manager.
 *
 * \param mgr the pmt manager object
 * \param x any other object of type pmt
 */
GR_EXTRAS_API void pmt_mgr_reset(pmt_t mgr, pmt_t x);

/*!
 * \brief Acquire a pmt from the manager.
 *
 * The order of managed pmts retrieved by this function is not guaranteed.
 * For this reason, the user may want to keep a manager homogeneous.
 * Ex: This manager only manages blobs of size 1500 bytes.
 *
 * \param mgr the pmt manager object
 * \param block when true, block until pmt available
 * \return a managed pmt or empty sptr if not available
 */
GR_EXTRAS_API pmt_t pmt_mgr_acquire(pmt_t mgr, bool block = true);

}

#endif /* INCLUDED_GRUEL_PMT_MGR_H */
