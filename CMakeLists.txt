cmake_minimum_required(VERSION 2.8) # 指定版本
project( STEREOPMS ) # 项目名
find_package( OpenCV REQUIRED ) # 依赖项
include_directories( ${OpenCV_INCLUDE_DIRS} ) # 包含的头文件
add_executable( Main main.cc ) # 添加一个可执行文件的编译项
target_link_libraries( Main  ${OpenCV_LIBS} PMS) # 链接可执行文件与链接库
include_directories( . )
add_library( PMS )
file(GLOB PMS_SRCS    
    "PatchMatchStereo/*.cc"
)
target_sources( PMS PUBLIC ${PMS_SRCS} )