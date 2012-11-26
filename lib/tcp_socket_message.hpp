// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.
//! This is not a real header file, only include it in socket_message.cpp

/***********************************************************************
 * Receive from a TCP socket and post tag to output
 **********************************************************************/
struct TCPSocketReceiver : gras::Block
{
    TCPSocketReceiver(const size_t mtu):
        gras::Block("GrExtras TCPSocketReceiver")
    {
        this->set_output_signature(gras::IOSignature(1));
        gras::OutputPortConfig config = this->get_output_config(0);
        config.reserve_items = mtu;
        this->set_output_config(0, config);
    }

    void work(const InputItems &, const OutputItems &)
    {
        //wait for a packet to become available
        if (not this->waiter()) return;

        //grab the output buffer to pass downstream as a tag
        gras::SBuffer b = this->pop_output_buffer(0);

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

    boost::shared_ptr<asio::ip::tcp::socket> socket;
    boost::function<bool(void)> waiter;
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
        if (not socket) this->waiter();

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
        if (not socket)
        {
            std::cerr << "sD" << std::flush;
            return;
        }
        try
        {
            socket->send(asio::buffer(b.get(), b.length));
        }
        catch(...)
        {
            std::cerr << "TCPSocketSender: socket send error, continuing..." << std::endl;
            socket.reset();
        }
    }

    boost::shared_ptr<asio::ip::tcp::socket> socket;
    boost::function<bool(void)> waiter;
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
        _receiver->waiter = boost::bind(&TCPSocketMessage::wait, this);
        _sender = boost::make_shared<TCPSocketSender>();
        _sender->waiter = boost::bind(&TCPSocketMessage::wait, this);

        if (type == "TCP_SERVER")
        {
            std::cout << "Creating TCP server..." << std::endl;
            _acceptor.reset(new asio::ip::tcp::acceptor(_io_service, _endpoint));
        }

        if (type == "TCP_CLIENT")
        {
            std::cout << "Creating TCP client..." << std::endl;
        }

        this->connect(*this, 0, _sender, 0);
        this->connect(_receiver, 0, *this, 0);
    }

    bool wait(void)
    {
        boost::mutex::scoped_lock l(_mutex);

        if (not _acceptor and not _socket) //must be a client...
        {
            std::cout << "TCP client connecting... " << std::flush;
            _socket.reset(new asio::ip::tcp::socket(_io_service));
            _socket->connect(_endpoint);
            std::cout << "done" << std::endl;
            _receiver->socket = _socket;
            _sender->socket = _socket;
        }

        //setup timeval for timeout
        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = timeout_us;

        //setup rset for timeout
        fd_set rset;
        FD_ZERO(&rset);
        int max_sock_fd = 0;
        if (_acceptor)
        {
            FD_SET(_acceptor->native(), &rset);
            max_sock_fd = std::max<int>(max_sock_fd, _acceptor->native());
        }
        if (_socket)
        {
            FD_SET(_socket->native(), &rset);
            max_sock_fd = std::max<int>(max_sock_fd, _socket->native());
        }

        //call select with timeout on receive socket
        ::select(max_sock_fd+1, &rset, NULL, NULL, &tv);

        if (_acceptor and FD_ISSET(_acceptor->native(), &rset))
        {
            std::cout << "TCP server accepting... " << std::flush;
            _socket.reset(new asio::ip::tcp::socket(_io_service));
            _acceptor->accept(*_socket);
            std::cout << "done" << std::endl;
            _receiver->socket = _socket;
            _sender->socket = _socket;
            return false; //its a new socket, call again
        }

        if (_socket)
        {
            return FD_ISSET(_socket->native(), &rset);
        }

        return false;
    }

    asio::ip::tcp::endpoint _endpoint;
    boost::mutex _mutex;
    boost::shared_ptr<asio::ip::tcp::acceptor> _acceptor;
    boost::shared_ptr<asio::ip::tcp::socket> _socket;
    boost::shared_ptr<TCPSocketReceiver> _receiver;
    boost::shared_ptr<TCPSocketSender> _sender;
};
