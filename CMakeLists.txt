cmake_minimum_required(VERSION 3.24)
project(leaderless-datastore)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

#googletest------------------------------------------------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
#-----------------------------------------------------------------------------------------------------------------------

if(WIN32 OR WIN64)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
endif()

file(GLOB_RECURSE client-src CONFIGURE_DEPENDS "src/client/*.h" "src/client/*.cpp")
file(GLOB_RECURSE server-src CONFIGURE_DEPENDS "src/server/*.h" "src/server/*.cpp")
file(GLOB_RECURSE common-src CONFIGURE_DEPENDS "src/common/*.h" "src/common/*.cpp")
file(GLOB_RECURSE test-src CONFIGURE_DEPENDS "tests/*.h" "tests/*.cpp"
        "src/client/internal/*.h" "src/client/internal/*.cpp"
        "src/server/internal/*.h" "src/server/internal/*.cpp")

add_executable(client ${client-src} ${common-src})
add_executable(server ${server-src} ${common-src})
add_executable(tests ${test-src} ${common-src})

if(WIN32)
    target_link_libraries(client wsock32 ws2_32)
    target_link_libraries(server wsock32 ws2_32)
    target_link_libraries(tests wsock32 ws2_32)
endif()
if(WIN64)
    target_link_libraries(client wsock64 ws2_64)
    target_link_libraries(server wsock64 ws2_64)
    target_link_libraries(tests wsock64 ws2_64)
endif()

enable_testing()
target_link_libraries(tests GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(tests)
