########################################################################
GR Extras - Misc GNU Radio Blocks
########################################################################

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Easy way to get Gr Extras
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
This package can be included as a gnuradio submodule.

Simply checkout the jblum/next gnuradio branch,
or merge the jblum/next branch into your own branch.

Repository:
git://gnuradio.org/jblum.git

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Otherwise, satify these prerequisites
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
A gnuradio install c138b94f or newer.

UHD (optional for UHD async message passing block):
http://code.ettus.com/redmine/ettus/projects/uhd/wiki

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Build Instructions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
mkdir <build dir>
cd <build dir>
cmake <source dir>
make
sudo make install

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Message passing implementation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Formerly a change-set against gnuradio-core,
this feature is not implemented 100% out of tree.

GrExtras implements a simple PMT-based message passing interface.
The message passing is implemented on top of the stream tags API.
Dummy streams are created after the block's official streams,
to source and sink messages, and can be connected like normal streams.
The GrExtras block class encapsulates all of this functionality,
the user merley needs to connect the message ports with tb.connect().

See include/gnuradio/extras/block.h for reference.

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Write blocks in python
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Formally part of the GrBlocks project.

This project includes the c++ to python block gateway.
This lets users write blocks in python with all
the same API hooks that a c++ block would have.

Users compiling this project against a gnuradio with
the pmt message passing with also get this feature
available in python.

Blocks coding guide (both python and c++ examples):
http://gnuradio.org/redmine/projects/gnuradio/wiki/BlocksCodingGuide

Add the following import to ensure that the API is available:
import gnuradio.extras

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
PMT manager
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The PMT manager is designed to:

* Reduce allocation overhead by re-using mutable PMTs.
* Provide backpressure by waiting on a PMT resource.

Normally, when a PMT dereferences, it gets deleted.
However, PMTs that are assigned to a manager do not delete;
Rather, the deleter returns the PMTs to the manager for re-use.
This allows a calling thread context to wait on the manager
while the downstream blocks consume and return the PMTs.

See include/gruel/pmt_mgr.h for reference.

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Extensions to the PMT blob type
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Extensions onto the existing blob type:

* New factory function creates unitialized blob
* Resize the blob to arbitrary length in bytes
* Get access to read/write pointer to blob buffer

Using the PMT manager, blobs can be continually passed downstream,
returned, and re-used with no extra allocation overhead at runtime.

See include/gruel/pmt_blob.h for reference.

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
PMT extensions documentation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
See lib/* for pmt manager and blob examples.
See python/pkt2.py for python examples.

To use in python:
import gnuradio.extras #loads pmt with new types
from gruel import pmt
#now it works:
mgr = pmt.pmt_make_mgr()
blob = pmt.pmt_make_blob(100)

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Basic blocks list
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* Signal source - look-up table optimized signal source
* Noise source - look-up table optimized noise source
* Stream selector - runtime configurable stream muxing
* Delay - block with runtime configurable delay of output
* Math blocks - f32 and fc32 blocks with volk optimizations
* Math blocks - more fixed point math blocks supported

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Blocks using PMT message passing
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To use these blocks, the user must build grextras against
the msg_passing change-set and the pmt_extensions change-set.

* Blob to stream - message/blob domain to stream domain
* Stream to blob - stream domain to message/blob domain
* Blob to socket - message/blob domain to udp/tcp socket
* Socket to blob - udp/tcp socket to message/blob domain
* Blob to filedes - message/blob domain to file descriptor
* Filedes to blob - file descriptor to message/blob domain
* Tun/Tap - a tuntap interface with blob IO
* Packet framer - blob input, framed bytes output
* Packet deframer - bytes input, deframed blob output
* UHD async msg source - reimplementation with message passing
