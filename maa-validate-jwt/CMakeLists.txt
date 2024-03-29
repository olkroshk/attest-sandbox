cmake_minimum_required(VERSION 3.10.0)

if(CMAKE_HOST_WIN32)
    set(WINDOWS 1)
elseif(CMAKE_HOST_UNIX)
    set(LINUX 1)
endif()

file(TO_CMAKE_PATH $ENV{LOCAL_ROOT} LOCAL_ROOT)
set(VCPKG_ROOT "${LOCAL_ROOT}/vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
if(WINDOWS)
    set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "")
elseif(LUNUX)
    set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(maa-validate-jwt LANGUAGES CXX)

set(SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/context.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/curl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jwks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jwt.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/verify.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/x509.cpp
)

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/include/ascii_escape_code.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/base64.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/context.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/curl.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jwks.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/jwt.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/utils.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/verify.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/x509.hpp
)

set(OPENSSL_USE_STATIC_LIBS TRUE)

find_package(OpenEnclave CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
#find_package(ZLIB REQUIRED)
find_package(CURL CONFIG REQUIRED)


add_executable(maa-validate-jwt ${SOURCES} ${HEADERS})

target_link_libraries(maa-validate-jwt PRIVATE openenclave::oeenclave openenclave::oelibcxx)
target_link_libraries(maa-validate-jwt PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(maa-validate-jwt PRIVATE CURL::libcurl)
target_include_directories(maa-validate-jwt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(WINDOWS)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT maa-validate-jwt)
    target_compile_options(maa-validate-jwt PRIVATE /W4 /WX)
elseif(LINUX)
    target_compile_options(maa-validate-jwt PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
