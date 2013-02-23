//
// Copyright 2012 Ettus Research LLC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <grextras/uhd_control_port.hpp>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <PMC/Containers.hpp>
#include <stdexcept>

using namespace grextras;

#ifdef HAVE_UHD

#include <uhd/usrp/multi_usrp.hpp>

struct UHDControlPortImpl : public UHDControlPort
{
    UHDControlPortImpl(uhd::usrp::multi_usrp::sptr usrp):
        gras::Block("GrExtras UHDControlPort")
    {
        _usrp = usrp;
        //setup the input for messages only
        gras::InputPortConfig config = this->get_input_config(0);
        config.reserve_items = 0;
        this->set_input_config(0, config);
    }

    void work(const InputItems &, const OutputItems &)
    {
        BOOST_FOREACH(const gras::Tag &t, this->get_input_tags(0))
        {
            //TODO
        }
    }

    uhd::usrp::multi_usrp::sptr _usrp;
};

UHDControlPort::sptr UHDControlPort::make(const std::string &addr)
{
    uhd::usrp::multi_usrp::sptr u = uhd::usrp::multi_usrp::make(addr);
    return boost::make_shared<UHDControlPortImpl>(u);
}

#else //HAVE_UHD

UHDControlPort::sptr UHDControlPort::make(const std::string &)
{
    throw std::runtime_error("UHDControlPort::make - GrExtras not build with UHD support");
}

#endif //HAVE_UHD
