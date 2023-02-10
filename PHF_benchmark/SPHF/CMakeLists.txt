# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (CheckHash)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED True)

# 指定生成目标 
add_executable(CheckHash main.cpp IntUtils.cpp PerfectHashBuilder.cpp ArrayPre.cpp AllocatorBase.cpp AllocatorC.cpp City64.cpp)

