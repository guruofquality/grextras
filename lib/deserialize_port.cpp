// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#define PMC_SERIALIZE_DECLARE
#include "serialize_common.hpp"
#include <grextras/deserialize_port.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <sstream>

using namespace grextras;

struct DeserializePortImpl : DeserializePort
{
    DeserializePortImpl(void):
        gras::Block("GrExtras DeserializePort")
    {
        
    }

    void work(const InputItems &ins, const OutputItems &)
    {
        
    }
};

DeserializePort::sptr DeserializePort::make(void)
{
    return boost::make_shared<DeserializePortImpl>();
}
