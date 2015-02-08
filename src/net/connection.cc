template <class req_handler>
connection<req_handler>::connection(boost::asio::ip::tcp::socket socket, connection_manager<req_handler> &manager) :
    socket_(std::move(socket)),
    connection_manager_(manager),
    protocol_handler_(*this)
{
}

template <class req_handler>
void connection<req_handler>::start() {
    do_read();
}

template <class req_handler>
void connection<req_handler>::stop() {
    socket_.close();
}

template <class req_handler>
void connection<req_handler>::do_read() {
    auto self(this->shared_from_this());
    socket_.async_read_some(boost::asio::buffer(buffer_),
            [this, self](boost::system::error_code ec, std::size_t bytes)
            {
                if (!ec) {
                    protocol_status status = protocol_handler_.process(buffer_.data(), bytes);
                    if (status == protocol_status::response) {
                       do_write(protocol_handler_.get_response());
                       std::cerr << "==========================================" << std::endl;
                    }
                    else if (status == protocol_status::error) {
                       std::cerr << "Error received from proto_handler." << std::endl;
                       shutdown();
                       return;
                    }
                }
                else if (ec != boost::asio::error::operation_aborted) {
                    shutdown();
                    return;
                }

                do_read();
            }
    );
}

template <class req_handler>
void connection<req_handler>::do_write(const typename proto_handler::response_buffer &buffer) {
    auto self(this->shared_from_this());
    std::array<boost::asio::const_buffer, 1> buffers = {{ buffer }};
    boost::asio::async_write(socket_, buffers,
            [this, self](boost::system::error_code ec, std::size_t size)
            {
                // TODO: Do not close connection on write.. keep-alive connections.
                //if (!ec) {
                //    boost::system::error_code ignored_ec;
                //    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
                //            ignored_ec);
                //}
                if (ec != boost::asio::error::operation_aborted) {
                    connection_manager_.stop(this->shared_from_this());
                }
            }
    );
}
