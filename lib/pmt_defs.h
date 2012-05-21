/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GREXTRAS_PMT_DEFS_H
#define INCLUDED_GREXTRAS_PMT_DEFS_H

#include <boost/utility.hpp>
#include <boost/detail/atomic_count.hpp>
#include <boost/cstdint.hpp>
#include <vector>

namespace pmt {

//! Bogus definition of pmt_base so we can extract the counter
class pmt_base : boost::noncopyable {
public:
  mutable boost::detail::atomic_count count_;
protected:
  pmt_base() : count_(0) {};
  virtual ~pmt_base();
};

//! More evil here to get a hold of the d_v to resize it
class pmt_uniform_vector : public pmt_base
{
public:
  bool is_uniform_vector() const { return true; }
  virtual const void *uniform_elements(size_t &len) = 0;
  virtual void *uniform_writable_elements(size_t &len) = 0;
  virtual size_t length() const = 0;
};

class pmt_u8vector : public pmt_uniform_vector
{
public:
  std::vector< boost::uint8_t >	d_v;

public:
  pmt_u8vector(size_t k, boost::uint8_t fill);
  pmt_u8vector(size_t k, const boost::uint8_t *data);
  // ~pmt_u8vector();
};

} //namespace pmt

#endif /* INCLUDED_GREXTRAS_PMT_DEFS_H */
