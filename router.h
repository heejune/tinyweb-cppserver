

namespace tinywebsvr {

	struct routing_record
	{
		virtual reply handle(void*) abstract;
	};

	template <typename T>
	struct typed_routing_recored;

	template <typename RetType, typename classTy, typename... ArgType>
	struct typed_routing_recored<RetType(classTy::*)(ArgType...) const> : public routing_record
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

	struct routing_delegator
	{
		routing_delegator() {}	// default
		routing_delegator(std::vector<std::string> r) : ruleset(r) {}

		// register functor handler
		template <typename F>
		void to(F&& f)
		{
			//routing_data = new typed_routing_recored<decltype(&F::operator())>(f);
			routing_data = std::make_unique<typed_routing_recored<decltype(&F::operator())>>(f);
		}

		// invoke handler
		template <typename... Args>
		reply handle(Args&&... args)
		{
			auto tuple = std::make_tuple<Args...>(args...);
			return routing_data->handle(&tuple);
		}

		//routing_record* routing_data = nullptr;
		std::unique_ptr<routing_record> routing_data = nullptr;

		std::vector<std::string> ruleset;
	};


}// namespace tinywebsvr
