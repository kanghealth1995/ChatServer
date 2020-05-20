#配置头文件搜索路径
# include_directories()

# 配置库文件搜索路径
# link_libraries()

#设置需要编译的源文件列表
set(SRC_LIST ./muduo_server.cpp)

# 把.指定路径下的所有源文件名字放入变量名SRC_LIST里面
# aux_source_directory(. SRC_LIST)

#表示生成server可执行文件，是由SRC_LIST变量所定义的源文件编译而来的
add_executable(server ${SRC_LIST})

#设置生成可执行文件所在的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#表示server这个目标程序，需要连接以下几个库文件
target_link_libraries(server muduo_net muduo_base pthread)