// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#define PMC_SERIALIZE_IMPLEMENT
#include "serialize_common.hpp"
#include <grextras/serialize_port.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>

using namespace grextras;

struct SerializePortImpl : SerializePort
{
    SerializePortImpl(const size_t mtu):
        gras::Block("GrExtras SerializePort"),
        _mtu(mtu)
    {
        
    }

    void work(const InputItems &ins, const OutputItems &)
    {
        
    }

    void propagate_tags(const size_t, const gras::TagIter &)
    {
        //do not forward tags
    }

    const size_t _mtu;
};

SerializePort::sptr SerializePort::make(const size_t mtu)
{
    return boost::make_shared<SerializePortImpl>(mtu);
}
