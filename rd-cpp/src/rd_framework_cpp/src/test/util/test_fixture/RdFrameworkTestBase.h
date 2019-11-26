#ifndef RD_CPP_RDFRAMEWORKTESTBASE_H
#define RD_CPP_RDFRAMEWORKTESTBASE_H

#include <gtest/gtest.h>

#include <memory>

#include "SimpleWire.h"
#include "protocol/Identities.h"
#include "protocol/Protocol.h"
#include "scheduler/SimpleScheduler.h"

namespace rd {
	namespace test {
		class RdFrameworkTestBase : public ::testing::Test {
			bool after_test_called = false;
		public:
			int static_entity_id = 1;
			std::string static_name = "top";

			Serializers serializers;

			LifetimeDefinition clientLifetimeDef;
			LifetimeDefinition serverLifetimeDef;

			Lifetime clientLifetime;
			Lifetime serverLifetime;

			std::unique_ptr<IProtocol> clientProtocol;
			std::unique_ptr<IProtocol> serverProtocol;

			SimpleScheduler clientScheduler;
			SimpleScheduler serverScheduler;

			std::shared_ptr<SimpleWire> clientWire;
			std::shared_ptr<SimpleWire> serverWire;

			//    /*std::unique_ptr<IWire>*/SimpleWire clientWire{&clientScheduler};
			//    /*std::unique_ptr<IWire>*/SimpleWire serverSimpleWire{&serverScheduler};

			std::shared_ptr<Identities> clientIdentities = std::make_shared<Identities>(Identities::CLIENT);
			std::shared_ptr<Identities> serverIdentities = std::make_shared<Identities>(Identities::SERVER);

			//    private var disposeLoggerFactory: Closeable? = null

			//    @BeforeTest
			RdFrameworkTestBase();

			//    @AfterTest
			virtual void AfterTest();

			template<typename T>
			T &bindStatic(IProtocol *protocol, T &x, std::string const &name) const {
				Lifetime lf = (protocol == clientProtocol.get() ? clientLifetime : serverLifetime);
				x.bind(lf, protocol, name);
				return x;
			}

			template<typename T>
			T &bindStatic(IProtocol *protocol, T &x, int id) const {
				Lifetime lf = (protocol == clientProtocol.get() ? clientLifetime : serverLifetime);
				statics(x, id).bind(lf, protocol, static_name);
				return x;
			}

			void setWireAutoFlush(bool flag);

            virtual ~RdFrameworkTestBase();
        };
	}
}


#endif //RD_CPP_RDFRAMEWORKTESTBASE_H
