# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (Generate)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED True)

# 指定生成目标 
add_executable(Generate main.cpp Generator.cpp / 
CTimer1.cpp CTimer2.cpp CTimer3.cpp / 
Builder/PerfectRouterBuilder1.cpp Builder/PerfectRouterBuilder2.cpp Builder/PerfectRouterBuilder3.cpp / 
Tester/PerfectRouterTester1.cpp Tester/PerfectRouterTester2.cpp Tester/PerfectRouterTester3.cpp / 
Performance/Performance.cpp IntUtils.cpp ArrayPre.cpp AllocatorBase.cpp AllocatorC.cpp City64.cpp)

