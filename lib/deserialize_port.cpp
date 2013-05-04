// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#define PMC_SERIALIZE_DECLARE
#include "serialize_common.hpp"
#include <grextras/deserialize_port.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/asio.hpp> //gets me ntohl
#include <boost/archive/text_iarchive.hpp>
#include <boost/assert.hpp>
#include <sstream>

using namespace grextras;

static PMCC buffer_to_pmc(const gras::SBuffer &buff)
{
    //convert buffer to stringstream
    const std::string s((const char *)buff.get(), buff.length);
    std::istringstream ss(s);

    //convert stringstream into pmc
    boost::archive::text_iarchive ia(ss);
    PMCC p;
    ia >> p;
    return p;
}

static void unpack_buffer(const gras::SBuffer &packet, size_t &seq, size_t &sid, bool &has_tsf, gras::item_index_t &tsf, bool &is_ext, gras::SBuffer &out_buff)
{
    const boost::uint32_t *p = (const boost::uint32_t *)packet.get();

    //validate vrlp
    BOOST_VERIFY(ntohl(p[0]) == VRLP);
    const size_t pkt_words32 = ntohl(p[1]) & 0xfffff;
    BOOST_VERIFY(pkt_words32*4 <= packet.length);
    const size_t seq12 = ntohl(p[1]) >> 20;

    //validate vita
    const boost::uint32_t vita_hdr = ntohl(p[2]);
    const size_t vita_words32 = vita_hdr & 0xffff;
    BOOST_VERIFY(vita_words32 == pkt_words32 - 3);

    //validate seq
    const size_t seq4 = (vita_hdr >> 16) & 0xf;
    BOOST_VERIFY((seq12 & 0x4) == seq4);

    has_tsf = bool(vita_hdr & VITA_TSF);
    BOOST_VERIFY(bool(vita_hdr & VITA_SID));
    is_ext = bool(vita_hdr & VITA_EXT);

    //assert other fields are blank - expected
    BOOST_VERIFY((vita_hdr & (1 << 30)) == 0);
    BOOST_VERIFY((vita_hdr & (1 << 27)) == 0);
    BOOST_VERIFY((vita_hdr & (1 << 26)) == 0);
    BOOST_VERIFY((vita_hdr & (1 << 23)) == 0);
    BOOST_VERIFY((vita_hdr & (1 << 22)) == 0);

    //extract seq and sid
    seq = seq12;
    sid = ntohl(p[3]);

    //only valid when has_tsf
    tsf = (gras::item_index_t(ntohl(p[4])) << 32) | ntohl(p[5]);

    //vend too
    BOOST_VERIFY(ntohl(p[pkt_words32-1]) == VEND);

    //set out buff
    const size_t hdr_words32 = 4 + has_tsf? 2 : 0;
    out_buff = packet;
    out_buff.offset += hdr_words32*4;
    out_buff.length = (pkt_words32 - hdr_words32 - 1)*4;
}

struct DeserializePortImpl : DeserializePort
{
    DeserializePortImpl(void):
        gras::Block("GrExtras DeserializePort")
    {
        
    }

    void work(const InputItems &ins, const OutputItems &outs)
    {
        for (size_t i = 0; i < ins.size(); i++)
        {
            //validate the pkt message type
            PMCC msg = pop_input_msg(i);
            if (not msg or not msg.is<gras::PacketMsg>()) continue;
            gras::PacketMsg pkt_msg = msg.as<gras::PacketMsg>();

            //TODO things wont keep pkt boundaries -- fix?

            //extract info
            size_t seq = 0;
            size_t sid = 0;
            bool has_tsf = false;
            gras::item_index_t tsf = 0;
            bool is_ext = false;
            gras::SBuffer out_buff;
            unpack_buffer(pkt_msg.buff, seq, sid, has_tsf, tsf, is_ext, out_buff);
            BOOST_VERIFY(sid < outs.size());

            //handle buffs
            if (not is_ext)
            {
                const size_t item_size = this->output_config(sid).item_size;
                BOOST_VERIFY((out_buff.length % item_size) == 0);
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
    }
};

DeserializePort::sptr DeserializePort::make(void)
{
    return boost::make_shared<DeserializePortImpl>();
}
