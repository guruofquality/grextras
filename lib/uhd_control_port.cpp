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

#include <grextras/uhd_control_port.hpp>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <PMC/Containers.hpp>
#include <stdexcept>

using namespace grextras;

#ifdef HAVE_UHD

#include <uhd/usrp/multi_usrp.hpp>

//! dummy == operator so we can use this type in PMC
inline bool operator==(const uhd::tune_request_t &, const uhd::tune_request_t &)
{
    return false;
}

struct UHDControlPortImpl : public UHDControlPort
{
    UHDControlPortImpl(uhd::usrp::multi_usrp::sptr usrp):
        gras::Block("GrExtras UHDControlPort")
    {
        _usrp = usrp;
        //register handlers
        this->register_setter("command_time", &UHDControlPortImpl::set_command_time);
        this->register_getter("rx_gain", &UHDControlPortImpl::get_rx_gain);
        this->register_setter("rx_gain", &UHDControlPortImpl::set_rx_gain);
        this->register_getter("tx_gain", &UHDControlPortImpl::get_tx_gain);
        this->register_setter("tx_gain", &UHDControlPortImpl::set_tx_gain);
        this->register_getter("rx_freq", &UHDControlPortImpl::get_rx_freq);
        this->register_setter("rx_freq", &UHDControlPortImpl::set_rx_freq);
        this->register_getter("tx_freq", &UHDControlPortImpl::get_tx_freq);
        this->register_setter("tx_freq", &UHDControlPortImpl::set_tx_freq);
    }

    void work(const InputItems &, const OutputItems &)
    {
        //there is no work, only handlers
    }

    /*******************************************************************
     * The RX gain
     ******************************************************************/
    void set_rx_gain(const double &gain)
    {
        _usrp->set_rx_gain(gain);
    }

    double get_rx_gain(void)
    {
        return _usrp->get_rx_gain();
    }

    /*******************************************************************
     * The TX gain
     ******************************************************************/
    void set_tx_gain(const double &gain)
    {
        _usrp->set_tx_gain(gain);
    }

    double get_tx_gain(void)
    {
        return _usrp->get_tx_gain();
    }

    /*******************************************************************
     * The RX freq
     ******************************************************************/
    void set_rx_freq(const uhd::tune_request_t &tr)
    {
        _usrp->set_rx_freq(tr);
    }

    double get_rx_freq(void)
    {
        return _usrp->get_rx_freq();
    }

    /*******************************************************************
     * The TX freq
     ******************************************************************/
    void set_tx_freq(const uhd::tune_request_t &tr)
    {
        _usrp->set_tx_freq(tr);
    }

    uhd::tune_request_t get_tx_freq(void)
    {
        return _usrp->get_tx_freq();
    }

    /*******************************************************************
     * Command time
     ******************************************************************/
    void set_command_time(const uhd::time_spec_t &time_spec)
    {
        if (time_spec == uhd::time_spec_t(0.0))
            _usrp->clear_command_time();
        else
            _usrp->set_command_time(time_spec);
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
