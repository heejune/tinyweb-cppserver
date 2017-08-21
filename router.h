

namespace tinywebsvr {

	struct routing_record
	{
		virtual reply handle(void*) abstract;
	};

	// in C++17 std::void_t
	template <typename...>
	using void_t = void;

	// Inferring the call signature of a lambda or arbitrary callable for 'make_function'
	// https://stackoverflow.com/a/12283159
	template<typename T> struct remove_class { };
	template<typename C, typename R, typename... A>
	struct remove_class<R(C::*)(A...)> { using type = R(A...); };
	template<typename C, typename R, typename... A>
	struct remove_class<R(C::*)(A...) const> { using type = R(A...); };
	template<typename C, typename R, typename... A>
	struct remove_class<R(C::*)(A...) volatile> { using type = R(A...); };
	template<typename C, typename R, typename... A>
	struct remove_class<R(C::*)(A...) const volatile> { using type = R(A...); };

	template<typename T>
	struct get_signature_impl {
		using type = typename remove_class<
			decltype(&std::remove_reference<T>::type::operator())>::type;
	};

	template<typename R, typename... A>
	struct get_signature_impl<R(A...)> { using type = R(A...); };
	template<typename R, typename... A>
	struct get_signature_impl<R(&)(A...)> { using type = R(A...); };
	template<typename R, typename... A>
	struct get_signature_impl<R(*)(A...)> { using type = R(A...); };

	template<typename T> using get_signature = typename get_signature_impl<T>::type;

	template <typename T>
	struct typed_routing_record;

	template <typename RetType, typename... ArgType>
	struct typed_routing_record<RetType(ArgType...)> : public routing_record
	{
		std::function<RetType(ArgType...)> stdfn;

		typed_routing_record(RetType(*pfn)(ArgType...)) : stdfn(pfn) {}

		virtual reply handle(void* args) override
		{
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
			routing_data = std::make_unique<typed_routing_record<get_signature<F>>>(f);
		}

		// invoke handler
		template <typename... Args>
		reply handle(Args&&... args)
		{
			auto tuple = std::make_tuple<Args...>(args...);
			return routing_data->handle(&tuple);
		}

		std::unique_ptr<routing_record> routing_data = nullptr;

		std::vector<std::string> ruleset;
	};

}// namespace tinywebsvr
