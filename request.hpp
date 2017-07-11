#pragma once
#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <vector>
#include "header.hpp"

namespace tinywebsvr {

	/// A request received from a client.
	struct request
	{
		std::string method;
		std::string uri;
		int http_version_major;
		int http_version_minor;
		std::vector<header> headers;
	};

	struct router_map
	{
		router_map(const std::string& _url) : url(_url) {}
		const std::string url;
		
		//template <typename ...Args>
		std::function<void(const request&)> handler;

		void operator()(std::function<void(const request&)> _handler)
		{
			handler = _handler;
		};
	};

} // namespace tinywebsvr

#endif // HTTP_REQUEST_HPP