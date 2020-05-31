cmake_minimum_required(VERSION 3.10)

project(demo)

set(CMAKE_CXX_COMPILER g++)    # 解决对'@GLIBCXX_3.4.20'未定义的引用的问题

# 自定义变量
set(DEPENDENCIES realsense2)

# 确定需要的头文件所在文件夹
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

# 多个文件相关，添加源代码
aux_source_directory(./src DIR_SRCS)

# 动态链接库路径
link_directories(${CMAKE_CURRENT_LIST_DIR}/lib)

add_executable(demo ${DIR_SRCS})

find_package(OpenCV REQUIRED)
target_link_libraries(demo ${OpenCV_LIBS})
target_link_libraries(demo ${DEPENDENCIES})    # 加上之前的set等同于下一条命令
# target_link_libraries(demo -lrealsense2)    # Link lib of realsense

