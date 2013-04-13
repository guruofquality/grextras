// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GREXTRAS_SERIALIZE_COMMON_HPP
#define INCLUDED_GREXTRAS_SERIALIZE_COMMON_HPP

#include <PMC/SerializeTypes.hpp>
#include <gras/tags.hpp>

template <class Archive>
void serialize(Archive &ar, gras::Tag &t, const unsigned int)
{
    ar & t.offset;
    ar & t.object;
}

#endif //INCLUDED_GREXTRAS_SERIALIZE_COMMON_HPP
