#pragma once

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <boost/asio.hpp>
#include <string>

namespace tinywebsvr {

	/// The top-level class of the HTTP server.
	class server
	{
	public:
		server(const server&) = delete;
		server& operator=(const server&) = delete;

		/// Construct the server to listen on the specified TCP address and port, and
		/// serve up files from the given directory.
		explicit server(const std::string& address, const std::string& port) : 
			io_service_(),
			signals_(io_service_),
			acceptor_(io_service_),
			connection_manager_(),
			socket_(io_service_)
		{
			// Register to handle the signals that indicate when the server should exit.
			// It is safe to register for the same signal multiple times in a program,
			// provided all registration for the specified signal is made through Asio.
			signals_.add(SIGINT);
			signals_.add(SIGTERM);
#if defined(SIGQUIT)
			signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)

			do_await_stop();

			// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
			boost::asio::ip::tcp::resolver resolver(io_service_);
			boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({ address, port });
			acceptor_.open(endpoint.protocol());
			acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
			acceptor_.bind(endpoint);
			acceptor_.listen();

			do_accept();
		}

		/// Run the server's io_service loop.
		void run() {
			// The io_service::run() call will block until all asynchronous operations
			// have finished. While the server is running, there is always at least one
			// asynchronous operation outstanding: the asynchronous accept call waiting
			// for new incoming connections.
			io_service_.run();
		}

		routing_delegator& route(const std::string url)
		{
			return request_handler_.subscribe(url);
		}

	private:
		/// Perform an asynchronous accept operation.
		void do_accept() {
			acceptor_.async_accept(socket_,
				[this](boost::system::error_code ec)
			{
				// Check whether the server was stopped by a signal before this
				// completion handler had a chance to run.
				if (!acceptor_.is_open())
				{
					return;
				}

				if (!ec)
				{
					connection_manager_.start(std::make_shared<connection>(
						std::move(socket_), connection_manager_, request_handler_));
				}

				do_accept();
			});
		}

		/// Wait for a request to stop the server.
		void do_await_stop() {
			signals_.async_wait(
				[this](boost::system::error_code /*ec*/, int /*signo*/)
			{
				// The server is stopped by cancelling all outstanding asynchronous
				// operations. Once all operations have finished the io_service::run()
				// call will exit.
				acceptor_.close();
				connection_manager_.stop_all();
			});
		}

		/// The io_service used to perform asynchronous operations.
		boost::asio::io_service io_service_;

		/// The signal_set is used to register for process termination notifications.
		boost::asio::signal_set signals_;

		/// Acceptor used to listen for incoming connections.
		boost::asio::ip::tcp::acceptor acceptor_;

		/// The connection manager which owns all live connections.
		connection_manager<std::shared_ptr<connection>> connection_manager_;

		/// The next socket to be accepted.
		boost::asio::ip::tcp::socket socket_;

		/// The handler for all incoming requests.
		request_handler request_handler_;
	};

} // namespace tinywebsvr

#endif // HTTP_SERVER_HPP
