#pragma once

#ifndef HTTP_HEADER_HPP
#define HTTP_HEADER_HPP

#include <string>

namespace tinywebsvr {

	struct header
	{
		std::string name;
		std::string value;
	};

} // namespace tinywebsvr

#endif // HTTP_HEADER_HPP