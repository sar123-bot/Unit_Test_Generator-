cmake_minimum_required(VERSION 3.10)
project(UnitTestGenerator)

set(CMAKE_CXX_STANDARD 17)
enable_testing()

# JWT Configuration (header-only)
set(JWT_CPP_DIR ${CMAKE_SOURCE_DIR}/third_party/jwt-cpp)
include_directories(${JWT_CPP_DIR}/include)

# Find required packages
find_package(Drogon REQUIRED)
find_package(GTest REQUIRED)
find_package(Jsoncpp REQUIRED)

# BCrypt Configuration
set(BCRYPT_ROOT ${CMAKE_SOURCE_DIR}/third_party/libbcrypt)
set(BCRYPT_INCLUDE_DIR ${BCRYPT_ROOT}/include)

# List available BCrypt source files (adjusted to your structure)
set(BCRYPT_SOURCES
    ${BCRYPT_ROOT}/src/bcrypt.c
    ${BCRYPT_ROOT}/src/bcrypt.cc
    ${BCRYPT_ROOT}/src/crypt_blowfish.c
    ${BCRYPT_ROOT}/src/crypt_gensalt.c
    ${BCRYPT_ROOT}/src/wrapper.c
    ${BCRYPT_ROOT}/src/x86.S
)

# Verify sources exist
set(VALID_BCRYPT_SOURCES)
foreach(SOURCE_FILE ${BCRYPT_SOURCES})
    if(EXISTS ${SOURCE_FILE})
        list(APPEND VALID_BCRYPT_SOURCES ${SOURCE_FILE})
        message(STATUS "Found BCrypt source: ${SOURCE_FILE}")
    else()
        message(WARNING "Missing BCrypt source: ${SOURCE_FILE}")
    endif()
endforeach()

if(VALID_BCRYPT_SOURCES)
    add_library(bcrypt STATIC ${VALID_BCRYPT_SOURCES})
    target_include_directories(bcrypt PUBLIC ${BCRYPT_INCLUDE_DIR})
else()
    message(FATAL_ERROR "No valid BCrypt sources found")
endif()

# Gather all sources
file(GLOB_RECURSE PLUGIN_SOURCES "plugins/*.cc")
file(GLOB_RECURSE CONTROLLER_SOURCES "inputs/*.cc")
file(GLOB_RECURSE MODEL_SOURCES "models/*.cc")
file(GLOB_RECURSE TEST_SOURCES "tests/*.cc")
file(GLOB_RECURSE UTILS_SOURCES "utils/*.cc")

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/dot_h
    ${CMAKE_SOURCE_DIR}/inputs
    ${CMAKE_SOURCE_DIR}/models
    ${CMAKE_SOURCE_DIR}/utils
    ${BCRYPT_INCLUDE_DIR}
    ${DROGON_INCLUDE_DIRS}
    ${JWT_CPP_DIR}/include
)

# Create test executable
add_executable(test_suite
    ${TEST_SOURCES}
    ${MODEL_SOURCES}
    ${CONTROLLER_SOURCES}
    ${PLUGIN_SOURCES}
    ${UTILS_SOURCES}
    tests/test_main.cpp
)

# Link libraries (removed jwt-cpp as it's header-only)
target_link_libraries(test_suite PRIVATE
    Drogon::Drogon
    GTest::gtest
    GTest::gtest_main
    Jsoncpp_lib
    bcrypt
    ssl crypto
)

add_test(NAME unit_tests COMMAND test_suite)

option(ENABLE_COVERAGE "Enable coverage reporting" ON)

if(ENABLE_COVERAGE)
    message(STATUS "Enabling coverage instrumentation")
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif()