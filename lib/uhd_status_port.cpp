//
// Copyright 2012-2013 Ettus Research LLC
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

#include <grextras/uhd_status_port.hpp>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <PMC/Containers.hpp>
#include <stdexcept>

using namespace grextras;

#ifdef HAVE_UHD

#include <uhd/usrp/multi_usrp.hpp>

struct UHDStatusPortImpl : public UHDStatusPort
{
    UHDStatusPortImpl(uhd::usrp::multi_usrp::sptr usrp):
        gras::Block("GrExtras UHDStatusPort")
    {
        _usrp = usrp;
    }

    void work(const InputItems &, const OutputItems &)
    {
        BOOST_FOREACH(const std::string &name, _sensors)
        {
            const PMC sensor_value = do_sensor(name);
            PMCTuple<2> t;
            t[0] = PMC_M(name);
            t[1] = sensor_value;
            this->post_output_msg(0, t);
        }
    }

    PMC do_sensor(const std::string &name)
    {
        const size_t pos = name.find(":");
        if (pos != std::string::npos)
        {
            const std::string action = name.substr(0, pos);
            const std::string what = name.substr(0, 2);
            const std::string which = name.substr(2, pos-2);
            const std::string name_only = name.substr(pos+1);
            const size_t which_num = boost::lexical_cast<size_t>(which);
            if (what == "MB") return sensor_value_to_pmc(_usrp->get_mboard_sensor(name, which_num));
            if (what == "RX") return sensor_value_to_pmc(_usrp->get_rx_sensor(name, which_num));
            if (what == "TX") return sensor_value_to_pmc(_usrp->get_tx_sensor(name, which_num));
        }
        return sensor_value_to_pmc(_usrp->get_mboard_sensor(name));
    }

    PMC sensor_value_to_pmc(const uhd::sensor_value_t &s)
    {
        switch(s.type)
        {
        case uhd::sensor_value_t::BOOLEAN: return PMC_M(s.to_bool());
        case uhd::sensor_value_t::INTEGER: return PMC_M(s.to_int());
        case uhd::sensor_value_t::REALNUM: return PMC_M(s.to_real());
        case uhd::sensor_value_t::STRING:  return PMC_M(s.value);
        }
        return PMC_M(s.value);
    }

    void add_sensor(const std::string &name)
    {
        _sensors.push_back(name);
    }

    uhd::usrp::multi_usrp::sptr _usrp;
    std::vector<std::string> _sensors;
};

UHDStatusPort::sptr UHDStatusPort::make(const std::string &addr)
{
    uhd::usrp::multi_usrp::sptr u = uhd::usrp::multi_usrp::make(addr);
    return boost::make_shared<UHDStatusPortImpl>(u);
}

#else //HAVE_UHD

UHDStatusPort::sptr UHDStatusPort::make(const std::string &)
{
    throw std::runtime_error("UHDStatusPort::make - GrExtras not build with UHD support");
}

#endif //HAVE_UHD
