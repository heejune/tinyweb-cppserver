

namespace tinywebsvr {

	struct routing_record
	{
		virtual reply handle(void*) abstract;
	};

	// in C++17 std::void_t
	template <typename...>
	using void_t = void;

	template <class T, bool = std::is_class<T>::value>
	struct typed_routing_recored
		: public typed_routing_recored<decltype(&T::operator())>
	{
		template <typename F>
		typed_routing_recored(F f) : typed_routing_recored<decltype(&T::operator()), false>(f) {}
	};

	template <typename RetType, typename classTy, typename... ArgType>
	struct typed_routing_recored<RetType(classTy::*)(ArgType...) const, false> : public routing_record
	{
		std::function<RetType(ArgType...)> stdfn;

		typed_routing_recored(RetType(*pfn)(ArgType...))
			: stdfn(pfn) {}

		virtual reply handle(void* args) override
		{
			static const std::size_t typesize = sizeof...(ArgType);
			//auto tuple = static_cast<std::tuple<ArgType...>*> (args);
			auto tuple = static_cast<std::tuple<typename std::decay<ArgType>::type...>*> (args);

			return _handle(*tuple, std::index_sequence_for<ArgType...>());
		}

		template <std::size_t... Is>
		RetType _handle(const std::tuple<ArgType...>& tuple, std::index_sequence<Is...>)
		{
			return (stdfn)(std::get<Is>(tuple)...);
		}
	};

	template <typename RetType, typename... ArgType>
	struct typed_routing_recored<RetType(*)(ArgType...), false> : public routing_record
	{
		std::function<RetType(ArgType...)> stdfn;

		typed_routing_recored(RetType(*pfn)(ArgType...))
			: stdfn(pfn) {}
		void set(RetType(*pfn)(ArgType...)) { stdfn = pfn; }

		virtual reply handle(void* args) override
		{
			static const std::size_t typesize = sizeof...(ArgType);
			//auto tuple = static_cast<std::tuple<ArgType...>*> (args);
			auto tuple = static_cast<std::tuple<typename std::decay<ArgType>::type...>*> (args);

			return _handle(*tuple, std::index_sequence_for<ArgType...>());
		}

		template <std::size_t... Is>
		RetType _handle(const std::tuple<ArgType...>& tuple, std::index_sequence<Is...>)
		{
			return (stdfn)(std::get<Is>(tuple)...);
		}
	};

	template <typename RetType, typename classTy, typename... ArgType>
	struct typed_routing_recored<RetType(classTy::*)(ArgType...), false> : public routing_record
	{
		std::function<RetType(ArgType...)> stdfn;

		typed_routing_recored(RetType(*pfn)(ArgType...))
			: stdfn(pfn) {}
		void set(RetType(*pfn)(ArgType...)) { stdfn = pfn; }

		virtual reply handle(void* args) override
		{
			static const std::size_t typesize = sizeof...(ArgType);
			//auto tuple = static_cast<std::tuple<ArgType...>*> (args);
			auto tuple = static_cast<std::tuple<typename std::decay<ArgType>::type...>*> (args);

			return _handle(*tuple, std::index_sequence_for<ArgType...>());
		}

		template <std::size_t... Is>
		RetType _handle(const std::tuple<ArgType...>& tuple, std::index_sequence<Is...>)
		{
			return (stdfn)(std::get<Is>(tuple)...);
		}
	};

	struct routing_delegator
	{
		routing_delegator() {}	// default
		routing_delegator(std::vector<std::string> r) : ruleset(r) {}

		// register functor handler
		template <typename F>
		void to(F f)
		{
			//routing_data = new typed_routing_recored<decltype(&F::operator())>(f);
			//routing_data = std::make_unique<typed_routing_recored<decltype(&F::operator())>>(f);
			routing_data = std::make_unique<typed_routing_recored<F>>(f);
		}

		// invoke handler
		template <typename... Args>
		reply handle(Args&&... args)
		{
			auto tuple = std::make_tuple<Args...>(args...);
			return routing_data->handle(&tuple);
		}

		std::unique_ptr<routing_record> routing_data = nullptr;
		//routing_record* routing_data = nullptr;

		std::vector<std::string> ruleset;
	};

}// namespace tinywebsvr
