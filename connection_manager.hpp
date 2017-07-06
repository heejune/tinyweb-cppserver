#pragma once

#ifndef HTTP_CONNECTION_MANAGER_HPP
#define HTTP_CONNECTION_MANAGER_HPP

#include <set>

namespace tinywebsvr {

	/// Manages open connections so that they may be cleanly stopped when the server
	/// needs to shut down.
	template <typename T>
	class connection_manager
	{
	public:
		connection_manager(const connection_manager&) = delete;
		connection_manager& operator=(const connection_manager&) = delete;

		/// Construct a connection manager.
		connection_manager() {}

		/// Add the specified connection to the manager and start it.
		void start(T c)
		{
			connections_.insert(c);
			c->start();
		}

		/// Stop the specified connection.
		void stop(T c)
		{
			connections_.erase(c);
			c->stop();
		}

		/// Stop all connections.
		void stop_all()
		{
			for (auto c : connections_)
				c->stop();
			connections_.clear();
		}

	private:
		/// The managed connections.
		std::set<T> connections_;
	};

} // namespace tinywebsvr

#endif // HTTP_CONNECTION_MANAGER_HPP