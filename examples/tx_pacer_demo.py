#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: TX Pacer Demo
# Author: Josh Blum
# Generated: Thu Oct  4 18:21:05 2012
##################################################

from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from optparse import OptionParser
import gnuradio.extras as gr_extras

class tx_pacer_demo(gr.top_block):

	def __init__(self):
		gr.top_block.__init__(self, "TX Pacer Demo")

		##################################################
		# Variables
		##################################################
		self.samp_rate = samp_rate = 1e6

		##################################################
		# Blocks
		##################################################
		self.uhd_usrp_source_0 = uhd.usrp_source(
			device_addr="",
			stream_args=uhd.stream_args(
				cpu_format="fc32",
				channels=range(1),
			),
		)
		self.uhd_usrp_source_0.set_samp_rate(samp_rate)
		self.uhd_usrp_source_0.set_center_freq(0, 0)
		self.uhd_usrp_source_0.set_gain(0, 0)
		self.uhd_usrp_sink_0 = uhd.usrp_sink(
			device_addr="",
			stream_args=uhd.stream_args(
				cpu_format="fc32",
				channels=range(1),
			),
		)
		self.uhd_usrp_sink_0.set_samp_rate(samp_rate)
		self.uhd_usrp_sink_0.set_center_freq(0, 0)
		self.uhd_usrp_sink_0.set_gain(0, 0)
		self.gr_sig_source_x_0 = gr.sig_source_c(samp_rate, gr.GR_COS_WAVE, 1000, .3, 0)
		self.gr_null_sink_0 = gr.null_sink(gr.sizeof_gr_complex*1)
		self.gr_noise_source_x_0 = gr.noise_source_c(gr.GR_GAUSSIAN, .01, 0)
		self.gr_add_xx_0 = gr.add_vcc(1)
		self.extras_tx_pacer_0 = gr_extras.tx_pacer(8, 8)
		self.extras_tx_pacer_0.set_tx_sample_rate(samp_rate)
		self.extras_tx_pacer_0.set_rx_sample_rate(samp_rate)
		self.extras_tx_pacer_0.set_tx_window_duration(.001)

		##################################################
		# Connections
		##################################################
		self.connect((self.uhd_usrp_source_0, 0), (self.extras_tx_pacer_0, 1))
		self.connect((self.extras_tx_pacer_0, 0), (self.uhd_usrp_sink_0, 0))
		self.connect((self.gr_add_xx_0, 0), (self.extras_tx_pacer_0, 0))
		self.connect((self.uhd_usrp_source_0, 0), (self.gr_null_sink_0, 0))
		self.connect((self.gr_noise_source_x_0, 0), (self.gr_add_xx_0, 1))
		self.connect((self.gr_sig_source_x_0, 0), (self.gr_add_xx_0, 0))

	def get_samp_rate(self):
		return self.samp_rate

	def set_samp_rate(self, samp_rate):
		self.samp_rate = samp_rate
		self.uhd_usrp_sink_0.set_samp_rate(self.samp_rate)
		self.uhd_usrp_source_0.set_samp_rate(self.samp_rate)
		self.extras_tx_pacer_0.set_tx_sample_rate(self.samp_rate)
		self.extras_tx_pacer_0.set_rx_sample_rate(self.samp_rate)
		self.gr_sig_source_x_0.set_sampling_freq(self.samp_rate)

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = tx_pacer_demo()
	tb.start()
	raw_input('Press Enter to quit: ')
	tb.stop()

