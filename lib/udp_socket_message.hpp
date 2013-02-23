// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.
//! This is not a real header file, only include it in socket_message.cpp

/***********************************************************************
 * Receive from a UDP socket and post tag to output
 **********************************************************************/
struct UDPSocketReceiver : gras::Block
{
    UDPSocketReceiver(const size_t mtu):
        gras::Block("GrExtras UDPSocketReceiver")
    {
        gras::OutputPortConfig config = this->get_output_config(0);
        config.reserve_items = mtu;
        this->set_output_config(0, config);
    }

    void work(const InputItems &, const OutputItems &)
    {
        //wait for a packet to become available
        if (not this->wait_for_recv_ready()) return;

        //grab the output buffer to pass downstream as a tag
        gras::SBuffer b = this->get_output_buffer(0);

        //receive into the buffer
        b.length = socket->receive_from(asio::buffer(b.get(), b.get_actual_length()), *endpoint);

        //create a message for this buffer
        const gras::PacketMsg msg(b);

        //post the output message downstream
        this->post_output_msg(0, PMC_M(msg));
    }

    bool wait_for_recv_ready(void)
    {
        //setup timeval for timeout
        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = timeout_us;

        //setup rset for timeout
        fd_set rset;
        FD_ZERO(&rset);
        FD_SET(socket->native(), &rset);

        //call select with timeout on receive socket
        return ::select(socket->native()+1, &rset, NULL, NULL, &tv) > 0;
    }

    boost::shared_ptr<asio::ip::udp::socket> socket;
    asio::ip::udp::endpoint *endpoint;
};

/***********************************************************************
 * Read input tags and send to a UDP socket.
 **********************************************************************/
struct UDPSocketSender : gras::Block
{
    UDPSocketSender(void):
        gras::Block("GrExtras UDPSocketSender")
    {
        //setup the input for messages only
        gras::InputPortConfig config = this->get_input_config(0);
        config.reserve_items = 0;
        this->set_input_config(0, config);
    }

    void work(const InputItems &ins, const OutputItems &)
    {
        //read the input message, and check it
        const PMCC msg = this->pop_input_msg(0);
        if (not msg.is<gras::PacketMsg>()) return;

        //write the buffer into the socket
        const gras::SBuffer &b = msg.as<gras::PacketMsg>().buff;
        socket->send_to(asio::buffer(b.get(), b.length), *endpoint);
    }

    boost::shared_ptr<asio::ip::udp::socket> socket;
    asio::ip::udp::endpoint *endpoint;
};

/***********************************************************************
 * UDPSocketMessage - hier block with sender and receiver
 **********************************************************************/
struct UDPSocketMessage : SocketMessage
{
    UDPSocketMessage(
        const std::string &type,
        const std::string &addr,
        const std::string &port,
        const size_t mtu
    ):
        gras::HierBlock("GrExtras UDPSocketMessage")
    {
        asio::ip::udp::resolver resolver(_io_service);
        asio::ip::udp::resolver::query query(asio::ip::udp::v4(), addr, port);
        _endpoint = *resolver.resolve(query);

        _receiver = boost::make_shared<UDPSocketReceiver>(mtu);
        _sender = boost::make_shared<UDPSocketSender>();
        boost::shared_ptr<asio::ip::udp::socket> socket;

        if (type == "UDP_SERVER")
        {
            std::cout << "Creating UDP server..." << std::endl;
            socket.reset(new asio::ip::udp::socket(_io_service, _endpoint));
            //TODO sender and receiver share endpoint - but no locking...

            _receiver->socket = socket;
            _receiver->endpoint = &_recv_ep;
            _sender->socket = socket;
            _sender->endpoint = &_recv_ep; //send to the last recv ep
        }

        if (type == "UDP_CLIENT")
        {
            std::cout << "Creating UDP client..." << std::endl;
            socket.reset(new asio::ip::udp::socket(_io_service));
            socket->open(asio::ip::udp::v4());
            //socket->connect(_endpoint);

            _receiver->socket = socket;
            _receiver->endpoint = &_recv_ep;
            _sender->socket = socket;
            _sender->endpoint = &_endpoint; //always send here
        }

        this->connect(*this, 0, _sender, 0);
        this->connect(_receiver, 0, *this, 0);
    }

    asio::ip::udp::endpoint _endpoint, _recv_ep;
    boost::shared_ptr<UDPSocketReceiver> _receiver;
    boost::shared_ptr<UDPSocketSender> _sender;
};
