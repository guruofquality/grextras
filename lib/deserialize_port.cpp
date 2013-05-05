// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "serialize_common.hpp"
#include <PMC/Serialize.hpp>
#include <grextras/deserialize_port.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/asio.hpp> //gets me ntohl
#include <boost/archive/polymorphic_text_iarchive.hpp>
#include <boost/assert.hpp>
#include <sstream>

using namespace grextras;

static PMCC buffer_to_pmc(const gras::SBuffer &buff)
{
    //convert buffer to stringstream
    const std::string s((const char *)buff.get(), buff.length);
    std::istringstream ss(s);

    //convert stringstream into pmc
    PMCC p;
    try
    {
        boost::archive::polymorphic_text_iarchive ia(ss);
        ia >> p;
    }
    catch(...)
    {
        std::cerr << "cannot deserialize " << "" << std::endl;
    }
    return p;
}

static void unpack_buffer(const gras::SBuffer &packet, size_t &seq, size_t &sid, bool &has_tsf, gras::item_index_t &tsf, bool &is_ext, gras::SBuffer &out_buff)
{
    const boost::uint32_t *p = (const boost::uint32_t *)packet.get();

    //validate vrlp
    ASSERT(ntohl(p[0]) == VRLP);
    const size_t pkt_words32 = ntohl(p[1]) & 0xfffff;
    ASSERT(pkt_words32*4 <= packet.length);
    const size_t seq12 = ntohl(p[1]) >> 20;

    //validate vita
    const boost::uint32_t vita_hdr = ntohl(p[2]);
    const size_t vita_words32 = vita_hdr & 0xffff;
    ASSERT(vita_words32 == pkt_words32 - 3);

    //validate seq
    const size_t seq4 = (vita_hdr >> 16) & 0xf;
    ASSERT((seq12 & 0x4) == seq4);

    has_tsf = bool(vita_hdr & VITA_TSF);
    ASSERT(bool(vita_hdr & VITA_SID));
    is_ext = bool(vita_hdr & VITA_EXT);

    //assert other fields are blank - expected
    ASSERT((vita_hdr & (1 << 30)) == 0);
    ASSERT((vita_hdr & (1 << 27)) == 0);
    ASSERT((vita_hdr & (1 << 26)) == 0);
    ASSERT((vita_hdr & (1 << 23)) == 0);
    ASSERT((vita_hdr & (1 << 22)) == 0);

    //extract seq and sid
    seq = seq12;
    sid = ntohl(p[3]);

    //only valid when has_tsf
    tsf = (gras::item_index_t(ntohl(p[4])) << 32) | ntohl(p[5]);

    //vend too
    ASSERT(ntohl(p[pkt_words32-1]) == VEND);

    //set out buff
    const size_t hdr_words32 = has_tsf? 6 : 4;
    out_buff = packet;
    out_buff.offset += hdr_words32*4;
    out_buff.length = (pkt_words32 - hdr_words32 - 1)*4;
}

struct DeserializePortImpl : DeserializePort
{
    DeserializePortImpl(const bool recover):
        gras::Block("GrExtras DeserializePort"),
        _recover(recover)
    {
        //NOP
    }

    gras::SBuffer recover_logic(const gras::SBuffer &in_buff)
    {
        if (not _recover) return in_buff;
        return in_buff;
    }

    void work(const InputItems &ins, const OutputItems &outs)
    {
        //validate the pkt message type
        PMCC msg = pop_input_msg(0);
        if (not msg or not msg.is<gras::PacketMsg>()) return;
        gras::PacketMsg pkt_msg = msg.as<gras::PacketMsg>();
        gras::SBuffer in_buff = recover_logic(pkt_msg.buff);
        if (not in_buff) return; //need more

        //extract info
        size_t seq = 0;
        size_t sid = 0;
        bool has_tsf = false;
        gras::item_index_t tsf = 0;
        bool is_ext = false;
        gras::SBuffer out_buff;
        unpack_buffer(in_buff, seq, sid, has_tsf, tsf, is_ext, out_buff);
        ASSERT(sid < outs.size());

        //handle buffs
        if (not is_ext)
        {
            const size_t item_size = this->output_config(sid).item_size;
            ASSERT((out_buff.length % item_size) == 0);
            this->post_output_buffer(sid, out_buff);
        }

        //handle tags
        else if (has_tsf)
        {
            gras::Tag tag;
            tag.offset = tsf;
            tag.object = buffer_to_pmc(out_buff);
            this->post_output_tag(sid, tag);
        }

        //handle msgs
        else
        {
            PMCC msg = buffer_to_pmc(out_buff);
            this->post_output_msg(sid, msg);
        }
    }

    const bool _recover;
};

DeserializePort::sptr DeserializePort::make(const bool recover)
{
    return boost::make_shared<DeserializePortImpl>(recover);
}
