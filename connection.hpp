#pragma once

#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include <array>
#include <memory>
#include <boost/asio.hpp>

namespace tinywebsvr {

	/// Represents a single connection from a client.
	class connection
		: public std::enable_shared_from_this<connection>
	{
	public:
		connection(const connection&) = delete;
		connection& operator=(const connection&) = delete;

		/// Construct a connection with the given socket.
		explicit connection(boost::asio::ip::tcp::socket socket,
			connection_manager<std::shared_ptr<connection>>& manager, request_handler& handler): 
			socket_(std::move(socket)), connection_manager_(manager), request_handler_(handler)
		{
		}

		/// Start the first asynchronous operation for the connection.
		void start()
		{
			do_read();
		}

		/// Stop all asynchronous operations associated with the connection.
		void stop()
		{
			socket_.close();
		}

	private:
		/// Perform an asynchronous read operation.
		void do_read()
		{
			auto self(shared_from_this());
			socket_.async_read_some(boost::asio::buffer(buffer_),
				[this, self](boost::system::error_code ec, std::size_t bytes_transferred)
			{
				if (!ec)
				{
					request_parser::result_type result;
					std::tie(result, std::ignore) = request_parser_.parse(
						request_, buffer_.data(), buffer_.data() + bytes_transferred);

					if (result == request_parser::good)
					{
						request_handler_.handle_request(request_, reply_);
						do_write();
					}
					else if (result == request_parser::bad)
					{
						reply_ = reply::stock_reply(reply::bad_request);
						do_write();
					}
					else
					{
						do_read();
					}
				}
				else if (ec != boost::asio::error::operation_aborted)
				{
					connection_manager_.stop(shared_from_this());
				}
			});
		}

		/// Perform an asynchronous write operation.
		void do_write()
		{
			auto self(shared_from_this());
			boost::asio::async_write(socket_, reply_.to_buffers(),
				[this, self](boost::system::error_code ec, std::size_t)
			{
				if (!ec)
				{
					// Initiate graceful connection closure.
					boost::system::error_code ignored_ec;
					socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
						ignored_ec);
				}

				if (ec != boost::asio::error::operation_aborted)
				{
					connection_manager_.stop(shared_from_this());
				}
			});
		}

		/// Socket for the connection.
		boost::asio::ip::tcp::socket socket_;

		/// The manager for this connection.
		connection_manager<std::shared_ptr<connection>>& connection_manager_;

		/// The handler used to process the incoming request.
		request_handler& request_handler_;

		/// Buffer for incoming data.
		std::array<char, 8192> buffer_;

		/// The incoming request.
		request request_;

		/// The parser for the incoming request.
		request_parser request_parser_;

		/// The reply to be sent back to the client.
		reply reply_;
	};

} // namespace tinywebsvr

#endif // HTTP_CONNECTION_HPPtu