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

#include <gnuradio/extras/msg_many_to_one.h>
#include <gr_io_signature.h>
#include <boost/foreach.hpp>

using namespace gnuradio::extras;

class msg_many_to_one_impl : public msg_many_to_one
{
public:
    msg_many_to_one_impl(const size_t num_inputs):
        block(
            "message many to one",
            gr_make_io_signature(num_inputs, num_inputs, 1),
            gr_make_io_signature(1, 1, 1)
        )
    {
        _tags.reserve(1024); //something reasonably large so we dont malloc
    }

    int work(
        const InputItems &input_items,
        const OutputItems &output_items
    ){
        std::vector<gr_tag_t> tags;
        for (size_t i = 0; i < input_items.size(); i++)
        {
            const uint64_t nread = this->nitems_read(i); //number of items read on port i

            //read all tags associated with port i for items in this work function
            this->get_tags_in_range(_tags, i, nread, nread+input_items[i].size());

            BOOST_FOREACH(const gr_tag_t &tag, _tags)
            {
                this->add_item_tag(0, tag);
            }
        }

        return output_items[0].size();
    }

    std::vector<gr_tag_t> _tags;
};

msg_many_to_one::sptr msg_many_to_one::make(const size_t num_inputs)
{
    return gnuradio::get_initial_sptr(new msg_many_to_one_impl(num_inputs));
}
