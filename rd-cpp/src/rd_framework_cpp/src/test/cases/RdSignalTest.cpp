#include <gtest/gtest.h>

#include "base/RdBindableBase.h"
#include "impl/RdSignal.h"
#include "RdFrameworkTestBase.h"

using vi = std::vector<int>;

using namespace rd;
using namespace test;

TEST_F(RdFrameworkTestBase, signal_statics) {
	int signal_id = 1;

	RdSignal<int> client_signal;
	RdSignal<int> server_signal;

	statics(client_signal, signal_id);
	statics(server_signal, signal_id);

	std::vector<int> client_log;
	std::vector<int> server_log;

	client_signal.advise(Lifetime::Eternal(), [&client_log](int v) { client_log.push_back(v); });
	server_signal.advise(Lifetime::Eternal(), [&server_log](int v) { server_log.push_back(v); });

	//not bound
	EXPECT_EQ((vi{}), client_log);
	EXPECT_EQ((vi{}), client_log);

	EXPECT_THROW(client_signal.fire(2), std::exception);
	EXPECT_THROW(server_signal.fire(2), std::exception);

	//bound
	bindStatic(serverProtocol.get(), server_signal, static_name);
	bindStatic(clientProtocol.get(), client_signal, static_name);

	//set from client
	client_signal.fire(2);
	EXPECT_EQ((vi{2}), client_log);
	EXPECT_EQ((vi{2}), server_log);

	//set from client
	server_signal.fire(3);
	EXPECT_EQ((vi{2, 3}), client_log);
	EXPECT_EQ((vi{2, 3}), server_log);

	AfterTest();
}

TEST_F(RdFrameworkTestBase, signal_void_statics) {
	int signal_id = 1;

	RdSignal<Void> client_signal;
	RdSignal<Void> server_signal;

	bindStatic(clientProtocol.get(), statics(client_signal, signal_id),
			   static_name);
	bindStatic(serverProtocol.get(), statics(server_signal, signal_id),
			   static_name);

	int acc = 0;
	LifetimeDefinition::use([&](Lifetime lt) {
		server_signal.advise(lt, [&acc] { acc++; });
		EXPECT_EQ(0, acc);

		client_signal.fire();
		EXPECT_EQ(1, acc);

		client_signal.fire();
		EXPECT_EQ(2, acc);
	});

	client_signal.fire(); //no transmitting
	EXPECT_EQ(2, acc);

	AfterTest();
}

class CustomSerializer {
public:
	static int32_t read(SerializationCtx  &ctx, Buffer &buffer) {
		bool negate = buffer.read_bool();
		int32_t module = buffer.read_integral<int32_t>();
		return negate ? -module : module;
	}

	static void write(SerializationCtx  &ctx, Buffer &buffer, const int &value) {
		buffer.write_integral(value < 0);
		buffer.write_integral(abs(value));
	}
};


TEST_F(RdFrameworkTestBase, signal_custom_serializer) {
	int signal_id = 1;

	RdSignal<int32_t, CustomSerializer> client_signal;
	RdSignal<int32_t, CustomSerializer> server_signal;

	statics(client_signal, signal_id);
	statics(server_signal, signal_id);

	int32_t client_log;
	int32_t server_log;

	client_signal.advise(Lifetime::Eternal(), [&client_log](int32_t v) { client_log = v; });
	server_signal.advise(Lifetime::Eternal(), [&server_log](int32_t v) { server_log = v; });

	bindStatic(serverProtocol.get(), server_signal, static_name);
	bindStatic(clientProtocol.get(), client_signal, static_name);

	//set from client
	client_signal.fire(2);
	EXPECT_EQ(2, client_log);
	EXPECT_EQ(2, server_log);

	//set from client
	server_signal.fire(-3);
	EXPECT_EQ(-3, client_log);
	EXPECT_EQ(-3, server_log);

	AfterTest();
}

template<typename K>
class FooScalar : public ISerializable {
	K x, y;
public:
	//region ctor/dtor

	explicit FooScalar(K x = 0, K y = 0) : x(x), y(y) {};

	FooScalar(FooScalar const &) = delete;

	FooScalar(FooScalar &&other) noexcept = default;

	FooScalar &operator=(FooScalar const &) = default;

	FooScalar &operator=(FooScalar &&) noexcept = default;

	virtual ~FooScalar() = default;
	//endregion

	static FooScalar<K> read(SerializationCtx  &ctx, Buffer &buffer) {
		K x = buffer.read_integral<K>();
		K y = buffer.read_integral<K>();
		return FooScalar(x, y);
	}

	void write(SerializationCtx  &ctx, Buffer &buffer) const override {
		buffer.write_integral(x);
		buffer.write_integral(y);
	}

	friend bool operator==(const FooScalar &lhs, const FooScalar &rhs) {
		return lhs.x == rhs.x &&
			   lhs.y == rhs.y;
	}

	friend bool operator!=(const FooScalar &lhs, const FooScalar &rhs) {
		return !(rhs == lhs);
	}

	friend std::string to_string(FooScalar const &value) {
		return rd::to_string(value.x) + "\n" + rd::to_string(value.y);
	}
};

TEST_F(RdFrameworkTestBase, signal_custom_iserializable) {
	int signal_id = 1;

	RdSignal<FooScalar<wchar_t>> client_signal;
	RdSignal<FooScalar<wchar_t>> server_signal;

	statics(client_signal, signal_id);
	statics(server_signal, signal_id);

	FooScalar<wchar_t> client_log;
	FooScalar<wchar_t> server_log;

	client_signal.advise(Lifetime::Eternal(), [&client_log](FooScalar<wchar_t> const &v) { client_log = v; });
	server_signal.advise(Lifetime::Eternal(), [&server_log](FooScalar<wchar_t> const &v) { server_log = v; });

	bindStatic(serverProtocol.get(), server_signal, static_name);
	bindStatic(clientProtocol.get(), client_signal, static_name);

	//set from client
	client_signal.fire(FooScalar<wchar_t>('2', '0'));
	EXPECT_EQ(FooScalar<wchar_t>('2', '0'), client_log);
	EXPECT_EQ(FooScalar<wchar_t>('2', '0'), server_log);

	//set from client
	server_signal.fire(FooScalar<wchar_t>(1, 8));
	EXPECT_EQ((FooScalar<wchar_t>(1, 8)), client_log);
	EXPECT_EQ((FooScalar<wchar_t>(1, 8)), server_log);

	AfterTest();
}

TEST_F(RdFrameworkTestBase, signal_vector) {
	int signal_id = 1;

	using array = std::vector<int>;

	RdSignal<array> client_signal;
	RdSignal<array> server_signal;

	statics(client_signal, signal_id);
	statics(server_signal, signal_id);

	array client_log;
	array server_log;

	client_signal.advise(Lifetime::Eternal(), [&client_log](array v) { client_log = v; });
	server_signal.advise(Lifetime::Eternal(), [&server_log](array v) { server_log = v; });

	bindStatic(serverProtocol.get(), server_signal, static_name);
	bindStatic(clientProtocol.get(), client_signal, static_name);

	//set from client
	array a{2, 0, 1, 8};
	client_signal.fire(a);
	EXPECT_EQ(a, client_log);
	EXPECT_EQ(a, server_log);

	//set from client
	a = {8, 8, 9, 8, 8, 0, 2, 1, 8, 6, 0};
	server_signal.fire(a);
	EXPECT_EQ(a, client_log);
	EXPECT_EQ(a, server_log);

	AfterTest();
}

TEST_F(RdFrameworkTestBase, signal_move) {
	RdSignal<int> signal1;
	RdSignal<int> signal2(std::move(signal1));

	AfterTest();
}