add_library(rd_framework_cpp_test_util
        #others
        others/entities_util.cpp others/entities_util.h
        others/ExtProperty.h
        #scheduler
        scheduler/TestSingleThreadScheduler.cpp scheduler/TestSingleThreadScheduler.h
        #test_fixture
        test_fixture/SocketWireTestBase.cpp test_fixture/SocketWireTestBase.h
        test_fixture/RdFrameworkTestBase.cpp test_fixture/RdFrameworkTestBase.h
        test_fixture/RdFrameworkDynamicPolymorphicTestBase.cpp test_fixture/RdFrameworkDynamicPolymorphicTestBase.h
        test_fixture/InterningTestBase.cpp test_fixture/InterningTestBase.h
        test_fixture/RdAsyncTestBase.cpp test_fixture/RdAsyncTestBase.h
        #wire
        wire/SocketProxy.cpp wire/SocketProxy.h

        Linearization.cpp Linearization.h
        PropertyHolderWithInternRoot.cpp PropertyHolderWithInternRoot.h
        SimpleWire.cpp SimpleWire.h
        )

add_subdirectory(entities)
add_subdirectory(interning)

target_include_directories(rd_framework_cpp_test_util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        entities
        models
        test_fixture
        others
        scheduler)

target_link_libraries(rd_framework_cpp_test_util PUBLIC
        gtest gtest_main
        rd_framework_cpp
        rd_core_cpp_test_util
        entities
        interning_test_model)

target_link_libraries(rd_framework_cpp_test_util PRIVATE clsocket)
