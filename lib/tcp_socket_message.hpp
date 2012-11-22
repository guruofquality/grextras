// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.
//! This is not a real header file, only include it in socket_message.cpp

/***********************************************************************
 * Receive from a TCP socket and post tag to output
 **********************************************************************/
struct TCPSocketReceiver : gras::Block
{
    TCPSocketReceiver(const size_t mtu):
        gras::Block("GrExtras TCPSocketReceiver"),
        _mtu(mtu)
    {
        this->set_output_signature(gras::IOSignature(1));
        //TODO allocate pool
    }

    void work(const InputItems &, const OutputItems &)
    {
        //wait for a packet to become available
        if (not wait_for_recv_ready(socket->native())) return;

        //TODO use pool
        gras::SBufferConfig config;
        config.length = _mtu;
        gras::SBuffer b(config);

        //receive into the buffer
        try
        {
            b.length = socket->receive(asio::buffer(b.get(), b.get_actual_length()));
        }
        catch(...)
        {
            std::cerr << "TCPSocketReceiver: socket receive error, continuing..." << std::endl;
            socket.reset();
            return;
        }

        //create a tag for this buffer
        const gras::Tag t(0, DATAGRAM_KEY, PMC::make(b));

        //post the output tag downstream
        this->post_output_tag(0, t);
    }

    const size_t _mtu;
    boost::shared_ptr<asio::ip::tcp::socket> socket;
};

/***********************************************************************
 * Read input tags and send to a UDP socket.
 **********************************************************************/
struct TCPSocketSender : gras::Block
{
    TCPSocketSender(void):
        gras::Block("GrExtras TCPSocketSender")
    {
        //setup the input for messages only
        this->set_input_signature(gras::IOSignature(1));
        gras::InputPortConfig config = this->get_input_config(0);
        config.reserve_items = 0;
        this->set_input_config(0, config);
    }

    void work(const InputItems &ins, const OutputItems &)
    {
        //iterate through all input tags, and post
        BOOST_FOREACH(const gras::Tag &t, this->get_input_tags(0))
        {
            if (t.key == DATAGRAM_KEY and t.value.is<gras::SBuffer>())
            {
                this->send(t.value.as<gras::SBuffer>());
            }
        }

        //erase all input tags from block
        this->erase_input_tags(0);

        //there should be no input items, consume all just in-case
        if (ins[0].size()) this->consume(0, ins[0].size());
    }

    void send(const gras::SBuffer &b)
    {
        try
        {
            socket->send(asio::buffer(b.get(), b.length));
        }
        catch(...)
        {
            std::cerr << "TCPSocketSender: socket receive error, continuing..." << std::endl;
            socket.reset();
        }
    }

    boost::shared_ptr<asio::ip::tcp::socket> socket;
};

/***********************************************************************
 * TCPSocketMessage - hier block with sender and receiver
 **********************************************************************/
struct TCPSocketMessage : SocketMessage
{
    TCPSocketMessage(
        const std::string &type,
        const std::string &addr,
        const std::string &port,
        const size_t mtu
    ):
        gras::HierBlock("GrExtras TCPSocketMessage")
    {
        asio::ip::tcp::resolver resolver(_io_service);
        asio::ip::tcp::resolver::query query(asio::ip::tcp::v4(), addr, port);
        _endpoint = *resolver.resolve(query);

        _receiver = boost::make_shared<TCPSocketReceiver>(mtu);
        _sender = boost::make_shared<TCPSocketSender>();

        if (type == "TCP_SERVER")
        {
            _acceptor.reset(new asio::ip::tcp::acceptor(_io_service, _endpoint));
            _tg.create_thread(boost::bind(&TCPSocketMessage::serve, this));
        }

        if (type == "TCP_CLIENT")
        {
            boost::shared_ptr<asio::ip::tcp::socket> socket;
            socket.reset(new asio::ip::tcp::socket(_io_service));
            _receiver->socket = socket;
            _sender->socket = socket;
        }

        this->connect(*this, 0, _sender, 0);
        this->connect(_receiver, 0, *this, 0);

    }

    ~TCPSocketMessage(void)
    {
        _tg.interrupt_all();
        _tg.join_all();
    }

    void serve(void)
    {
        boost::shared_ptr<asio::ip::tcp::socket> socket;
        while (not boost::this_thread::interruption_requested())
        {
            if (not wait_for_recv_ready(_acceptor->native())) continue;
            socket.reset(new asio::ip::tcp::socket(_io_service));
            _acceptor->accept(*socket);

            //a synchronous switchover to a new client socket
            _receiver->socket = socket;
            _sender->socket = socket;
        }
    }

    boost::thread_group _tg;
    asio::io_service _io_service;
    boost::shared_ptr<asio::ip::tcp::acceptor> _acceptor;
    asio::ip::tcp::endpoint _endpoint;
    boost::shared_ptr<TCPSocketReceiver> _receiver;
    boost::shared_ptr<TCPSocketSender> _sender;
};
