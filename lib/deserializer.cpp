// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "serialize_common.hpp"
#include <PMC/Serialize.hpp>
#include <grextras/deserializer.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/asio.hpp> //gets me ntohl
#include <boost/assert.hpp>

using namespace grextras;

static PMCC buffer_to_pmc(const gras::SBuffer &buff)
{
    //convert buffer to string
    const std::string s((const char *)buff.get(), buff.length);

    //convert string into pmc
    PMCC p;
    try
    {
        p = PMC::deserialize(s, "TEXT");
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

struct DeserializerImpl : gras::Block
{
    DeserializerImpl(const bool recover):
        gras::Block("GrExtras Deserializer"),
        _recover(recover),
        _num_outs(0) //set by notify
    {
        //NOP
    }

    void work(const InputItems &, const OutputItems &);

    void handle_packet(const gras::SBuffer &in_buff)
    {
        //extract info
        size_t seq = 0;
        size_t sid = 0;
        bool has_tsf = false;
        gras::item_index_t tsf = 0;
        bool is_ext = false;
        gras::SBuffer out_buff;
        unpack_buffer(in_buff, seq, sid, has_tsf, tsf, is_ext, out_buff);
        ASSERT(sid < _num_outs);

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

    void notify_topology(const size_t, const size_t num_outputs)
    {
        _num_outs = num_outputs;
    }

    const bool _recover;
    size_t _num_outs;
    gras::SBuffer _accum_buff;
};


static bool inspect_packet(const void *pkt, const size_t length, bool &fragment, size_t &pkt_len)
{
    const boost::uint32_t *vrlp_pkt = reinterpret_cast<const boost::uint32_t *>(pkt);
    const char *p = reinterpret_cast<const char *>(pkt);
    if ((p[0] == 'V') and (p[1] == 'R') and (p[2] == 'L') and (p[3] == 'P'))
    {
        ASSERT(ntohl(vrlp_pkt[0]) == VRLP);
        const size_t pkt_words32 = ntohl(vrlp_pkt[1]) & 0xfffff;
        pkt_len = pkt_words32*4;
        fragment = pkt_len > length;
        if (pkt_len > MAX_PKT_BYTES) return false; //call this BS
        return fragment or ntohl(vrlp_pkt[pkt_words32-1]) == VEND;
    }
    return false;
}

void DeserializerImpl::work(const InputItems &, const OutputItems &)
{
    //validate the pkt message type
    PMCC msg = pop_input_msg(0);
    if (not msg or not msg.is<gras::PacketMsg>()) return;
    gras::PacketMsg pkt_msg = msg.as<gras::PacketMsg>();

    //handle the packet - non recovery logic
    if (not _recover) return this->handle_packet(pkt_msg.buff);

    ////////////////////////////////////////////////////////////////
    /////////// Recovery logic below ///////////////////////////////
    ////////////////////////////////////////////////////////////////

    //was there a buffer previously? then accumulate it
    if (_accum_buff)
    {
        gras::SBufferConfig config;
        config.length = _accum_buff.length + pkt_msg.buff.length;
        gras::SBuffer new_buff(config);
        std::memcpy(new_buff.get(), _accum_buff.get(), _accum_buff.length);
        std::memcpy(new_buff.get(_accum_buff.length), pkt_msg.buff.get(), pkt_msg.buff.length);
        _accum_buff = new_buff;
    }
    else
    {
        _accum_buff = pkt_msg.buff;
    }

    //character by character recovery search for packet header
    while (_accum_buff.length >= MIN_PKT_BYTES)
    {
        bool fragment = true; size_t pkt_len = 0;
        if (inspect_packet(_accum_buff.get(), _accum_buff.length, fragment, pkt_len))
        {
            if (fragment) return; //wait for more incoming buffers to accumulate
            handle_packet(_accum_buff); //handle the packet, its good probably

            //increment for the next iteration
            ASSERT(pkt_len <= _accum_buff.length);
            _accum_buff.offset += pkt_len;
            _accum_buff.length -= pkt_len;
            ASSERT(_accum_buff.length <= _accum_buff.get_actual_length());
        }
        else
        {
            //the search continues
            _accum_buff.offset++;
            _accum_buff.length--;
        }
    }

    //dont keep a reference if the buffer is empty
    if (_accum_buff.length == 0) _accum_buff.reset();
}

gras::Block *Deserializer::make(const bool recover)
{
    return new DeserializerImpl(recover);
}
