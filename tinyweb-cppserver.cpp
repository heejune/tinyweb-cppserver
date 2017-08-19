#include <iostream>
#include <string>
#include <boost/asio.hpp>

#include "request.hpp"
#include "reply.hpp"
#include "mime_types.hpp"
#include "request_handler.hpp"
#include "request_parser.hpp"
#include "connection_manager.hpp"
#include "connection.hpp"
#include "server.hpp"

/// Based on the boost asio C++11 http server sample
/// http://www.boost.org/doc/libs/1_64_0/doc/html/boost_asio/example/cpp11/http/server/
///

// function pointer test
tinywebsvr::reply handle(const tinywebsvr::request& req)
{
	return tinywebsvr::reply::reponse_static("mingw32.PNG");
}

int main(int argc, char* argv[])
{
	try
	{
		tinywebsvr::server s("0.0.0.0", "80");

		s.route("/").to(
			[](const tinywebsvr::request& req) {
			return tinywebsvr::reply::reponse_static("index.html");
		});

		s.route("/hello/<int>").to(
			[](const tinywebsvr::request& req, const std::string& scount) {
			auto count = std::stoi(scount);
				if (count > 100)
					return tinywebsvr::reply::stock_reply(tinywebsvr::reply::bad_request);
			std::ostringstream os;
			os << count << " bottles of beer!";
			return tinywebsvr::reply::response(os.str());
		});

		// function pointer
		s.route("/image").to(handle);

		// Run the server until stopped.
		s.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
