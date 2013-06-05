// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%include "GrExtras_Common.i"

%{

#include <cstdio>
#include <stdexcept>
#include <string.h>

#define VERBOSE 0

/*!
 * \brief Given a stream of bits and access_code flags, assemble packets.
 * \ingroup sink_blk
 *
 * input: stream of bytes from gr_correlate_access_code_bb
 * output: none. Pushes assembled packet into target queue
 *
 * The framer expects a fixed length header of 2 16-bit shorts
 * containing the payload length, followed by the payload. If the
 * 2 16-bit shorts are not identical, this packet is ignored. Better
 * algs are welcome.
 *
 * The input data consists of bytes that have two bits used.
 * Bit 0, the LSB, contains the data bit.
 * Bit 1 if set, indicates that the corresponding bit is the
 * the first bit of the packet. That is, this bit is the first
 * one after the access code.
 */
struct digital_framer_sink_1 : public gras::Block
{

    digital_framer_sink_1(void);

  enum state_t {STATE_SYNC_SEARCH, STATE_HAVE_SYNC, STATE_HAVE_HEADER};

  static const int MAX_PKT_LEN    = 4096;
  static const int HEADERBITLEN   = 32;

  state_t            d_state;
  unsigned int       d_header;			// header bits
  int		     d_headerbitlen_cnt;	// how many so far

  unsigned char      d_packet[MAX_PKT_LEN];	// assembled payload
  unsigned char	     d_packet_byte;		// byte being assembled
  int		     d_packet_byte_index;	// which bit of d_packet_byte we're working on
  int 		     d_packetlen;		// length of packet
  int                d_packet_whitener_offset;  // offset into whitener string to use
  int		     d_packetlen_cnt;		// how many so far

  void enter_search();
  void enter_have_sync();
  void enter_have_header(int payload_len, int whitener_offset);

  bool header_ok()
  {
    // confirm that two copies of header info are identical
    return ((d_header >> 16) ^ (d_header & 0xffff)) == 0;
  }

  void header_payload(int *len, int *offset)
  {
    // header consists of two 16-bit shorts in network byte order
    // payload length is lower 12 bits
    // whitener offset is upper 4 bits
    *len = (d_header >> 16) & 0x0fff;
    *offset = (d_header >> 28) & 0x000f;
  }

  ~digital_framer_sink_1();

  void work(const InputItems &ins, const OutputItems &outs);
};

inline void
digital_framer_sink_1::enter_search()
{
  if (VERBOSE)
    fprintf(stderr, "@ enter_search\n");

  d_state = STATE_SYNC_SEARCH;
}

inline void
digital_framer_sink_1::enter_have_sync()
{
  if (VERBOSE)
    fprintf(stderr, "@ enter_have_sync\n");

  d_state = STATE_HAVE_SYNC;
  d_header = 0;
  d_headerbitlen_cnt = 0;
}

inline void
digital_framer_sink_1::enter_have_header(int payload_len,
					 int whitener_offset)
{
  if (VERBOSE)
    fprintf(stderr, "@ enter_have_header (payload_len = %d) (offset = %d)\n",
	    payload_len, whitener_offset);

  d_state = STATE_HAVE_HEADER;
  d_packetlen = payload_len;
  d_packet_whitener_offset = whitener_offset;
  d_packetlen_cnt = 0;
  d_packet_byte = 0;
  d_packet_byte_index = 0;
}

digital_framer_sink_1::digital_framer_sink_1(void)
  : gras::Block ("extras_framer_sink_1")
{
    this->output_config(0).reserve_items = 4096; //max pkt frame possible
  enter_search();
}

digital_framer_sink_1::~digital_framer_sink_1 ()
{
}

void digital_framer_sink_1::work(const InputItems &ins, const OutputItems &)
{
  const unsigned char *in = ins[0].cast<const unsigned char *>();
  int noutput_items = ins[0].size();
  int count=0;

  if (VERBOSE)
    fprintf(stderr,">>> Entering state machine\n");

  while (count < noutput_items){
    switch(d_state) {

    case STATE_SYNC_SEARCH:    // Look for flag indicating beginning of pkt
      if (VERBOSE)
	fprintf(stderr,"SYNC Search, noutput=%d\n", noutput_items);

      while (count < noutput_items) {
	if (in[count] & 0x2){  // Found it, set up for header decode
	  enter_have_sync();
	  break;
	}
	count++;
      }
      break;

    case STATE_HAVE_SYNC:
      if (VERBOSE)
	fprintf(stderr,"Header Search bitcnt=%d, header=0x%08x\n",
		d_headerbitlen_cnt, d_header);

      while (count < noutput_items) {	// Shift bits one at a time into header
	d_header = (d_header << 1) | (in[count++] & 0x1);
	if (++d_headerbitlen_cnt == HEADERBITLEN) {

	  if (VERBOSE)
	    fprintf(stderr, "got header: 0x%08x\n", d_header);

	  // we have a full header, check to see if it has been received properly
	  if (header_ok()){
	    int payload_len;
	    int whitener_offset;
	    header_payload(&payload_len, &whitener_offset);
	    enter_have_header(payload_len, whitener_offset);

	    if (d_packetlen == 0){	    // check for zero-length payload

        
            gras::PacketMsg msg;
            msg.info = PMC_M(d_packet_whitener_offset);
            this->post_output_msg(0, msg);

	      enter_search();
	    }
	  }
	  else
	    enter_search();				// bad header
	  break;					// we're in a new state
	}
      }
      break;

    case STATE_HAVE_HEADER:
      if (VERBOSE)
	fprintf(stderr,"Packet Build\n");

      while (count < noutput_items) {   // shift bits into bytes of packet one at a time
	d_packet_byte = (d_packet_byte << 1) | (in[count++] & 0x1);
	if (d_packet_byte_index++ == 7) {	  	// byte is full so move to next byte
	  d_packet[d_packetlen_cnt++] = d_packet_byte;
	  d_packet_byte_index = 0;

	  if (d_packetlen_cnt == d_packetlen){		// packet is filled

            gras::PacketMsg msg;
            msg.info = PMC_M(d_packet_whitener_offset);
            msg.buff = this->get_output_buffer(0);
            memcpy(msg.buff.get(), d_packet, d_packetlen_cnt);
            msg.buff.length = d_packetlen_cnt;
            this->post_output_msg(0, PMC_M(msg));

	    enter_search();
	    break;
	  }
	}
      }
      break;

    default:
      assert(0);

    } // switch

  }   // while

  this->consume(0, noutput_items);
  return;
}

%}

%template(grextras_Block) boost::shared_ptr<gras::Block>;

%inline %{

inline boost::shared_ptr<gras::Block> make_digital_framer_sink_1(void)
{
    return boost::shared_ptr<gras::Block>(new digital_framer_sink_1());
}

%}
