cmake_minimum_required(VERSION 4.1)
project(kylar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

include(cmake/utils.cmake)  # include用于扩展cmake脚本功能，管理cmake工具函数
include_directories()       # 添加第三方库头文件路径
link_directories()          # 添加库文件搜索路径
option(BUILD_TEST "ON for compile test" OFF)   # 定义一个 CMake 选项 BUILD_TEST，默认值为 OFF。用户可以通过 -DBUILD_TEST=ON 来启用测试程序的构建。


# 下面几个内容是通过find_package 搜索包，如果搜索到，就添加对应的头文件到搜索路径中
#find_package(Boost REQUIRED)
#if(Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIRS})
#endif()
#
#find_package(ZLIB REQUIRED)
#if(ZLIB_FOUND)
#    include_directories(${ZLIB_INCLUDE_DIR})
#endif()
#
#find_package(OpenSSL REQUIRED)
#if(OPENSSL_FOUND)
#    include_directories(${OPENSSL_INCLUDE_DIR})
#endif()
#
#find_package(MySQL REQUIRED)
#if(MYSQL_FOUND)
#    include_directories(${MYSQL_INCLUDE_DIR})
#endif()

find_package(Protobuf)
if(Protobuf_FOUND)
    include_directories(${Protobuf_INCLUDE_DIRS})
endif()



set(LIB_SRC
    kylar/log.cpp
    )

add_library(kylar SHARED ${LIB_SRC})

# 设置生成的库文件输出路径、和可执行文件路径
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


