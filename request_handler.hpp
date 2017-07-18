#pragma once

#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP

#include <string>
#include <map>
#include "router.h"
#include <memory>
#include <tuple>

namespace tinywebsvr {

	struct routerMapComparer
	{
		bool operator () (const std::string&l, const std::string& r) { return l.length() < r.length(); }
	};

	/// The common handler for all incoming requests.
	class request_handler
	{
	public:
		request_handler(const request_handler&) = delete;
		request_handler& operator=(const request_handler&) = delete;

		request_handler() {}

		// TODO: multiple tag support
		routing_delegator& subscribe(const std::string& rule)
		{
			std::vector<std::string> ruleset;
			auto open_tag = rule.find_first_of("<");
			std::string url = rule;
			if (open_tag != std::string::npos) {
				url = rule.substr(0, open_tag);

				auto close_tag = rule.find_first_of(">");
				if (close_tag != std::string::npos && close_tag > open_tag) {
					auto param_type = rule.substr(open_tag, close_tag);
					ruleset.emplace_back(param_type);
				}
			}
			
			_router.insert(std::make_pair(url, std::make_unique<routing_delegator>(ruleset)));

			return *_router[url].get();
		}

		routing_delegator emptydelegator;

		routing_delegator& match(const std::string& requested_url, std::string& matched_url) {

			auto urls = url2vector(requested_url);

			for (size_t i = 0; i < urls.size(); i++)
			{
				auto it = _router.find(urls[i]);
				if (it != _router.end()) {
					// match
					matched_url = urls[i];
					return *_router[urls[i]].get();
				}
			}

			return emptydelegator;
		}

		/// Handle a request and produce a reply.
		void handle_request(const request& req, reply& rep)
		{
			// Decode url to path.
			std::string request_path;
			if (!url_decode(req.uri, request_path))
			{
				rep = reply::stock_reply(reply::bad_request);
				return;
			}
			// Request path must be absolute and not contain "..".
			if (request_path.empty() || request_path[0] != '/'
				|| request_path.find("..") != std::string::npos)
			{
				rep = reply::stock_reply(reply::bad_request);
				return;
			}

			std::string matched_url;
			auto& routing_delegator = match(req.uri, matched_url);

			if (routing_delegator.routing_data != nullptr) {
				// url to values
				auto params = req.uri.substr(req.uri.find(matched_url) + matched_url.size(), std::string::npos);
				auto vparams = url2vector(params);
				rep = invoke_with_collection(routing_delegator, req, vparams);
			}

			// url routing not found
			rep = reply::stock_reply(reply::not_found);
			return;
		}

	private:
		reply invoke_with_collection(routing_delegator& delegator, const request& req, const std::vector<std::string>& urls) {
			switch (urls.size()) {
			case 1: return delegator.handle(req, urls[0]); break;
			case 2: return delegator.handle(req, urls[0], urls[1]); break;
			case 3: return delegator.handle(req, urls[0], urls[1], urls[2]); break;
			default:
				return reply();
			}
		}

		std::size_t get_param_tag(const std::string& url, std::size_t const begin_pos = 0L) {
			auto open_tag = url.find_first_of('<', begin_pos);
			if (open_tag != std::string::npos) {
				auto close_tag = url.find_first_of('>', open_tag);
				if (close_tag != std::string::npos)
					return open_tag;
			}

			return std::string::npos;
		}

		struct url_info {
			std::string url;
			std::vector<std::string> params;
		};

		std::vector<std::string> url2vector(const std::string& requesturl) {
			std::vector<std::string> urls = { requesturl};
			auto pos = requesturl.find_last_of('/');
			std::string url = requesturl;

			while (pos != std::string::npos) {
				url = url.substr(0, pos + 1); // including trailing /
				urls.emplace_back(url);
				url.pop_back();	// delete the trailing /
				pos = url.find_last_of('/'); // but search excluding trailing /
			}

			return urls;
		}

		/// The directory containing the files to be served.
		std::string doc_root_;

		std::map<std::string, std::unique_ptr<routing_delegator>> _router;

		/// Perform URL-decoding on a string. Returns false if the encoding was
		/// invalid.
		static bool url_decode(const std::string& in, std::string& out)
		{
			out.clear();
			out.reserve(in.size());
			for (std::size_t i = 0; i < in.size(); ++i)
			{
				if (in[i] == '%')
				{
					if (i + 3 <= in.size())
					{
						int value = 0;
						std::istringstream is(in.substr(i + 1, 2));
						if (is >> std::hex >> value)
						{
							out += static_cast<char>(value);
							i += 2;
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				else if (in[i] == '+')
				{
					out += ' ';
				}
				else
				{
					out += in[i];
				}
			}
			return true;
		}
	};

} // namespace tinywebsvr

#endif // HTTP_REQUEST_HANDLER_HPP
