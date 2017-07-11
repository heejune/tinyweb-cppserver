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

int main(int argc, char* argv[])
{
	try
	{
		tinywebsvr::server s("0.0.0.0", "80");

		s.route("/hello/<int>")(
			[](const tinywebsvr::request& req) {
			//	if (count > 100)
			//		return crow::response(400);
			//std::ostringstream os;
			//os << count << " bottles of beer!";
			//return response(os.str());
		});

		s.route("/add_json")
			//.methods("POST")
			([](const tinywebsvr::request& req) {
			//auto x = json::load(req.body);
			//if (!x)
			//	return response(400);
			//int sum = x["a"].i() + x["b"].i();
			//std::ostringstream os;
			//os << sum;
			//return response{ os.str() };
		});

		// Run the server until stopped.
		s.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
