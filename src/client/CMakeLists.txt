#定义了一个SRC_LIST变量，其中包含该目录下所有的源文件
aux_source_directory(. SRC_LIST)

#指定生成可执行文件
add_executable(ChatClient ${SRC_LIST})
#指定可执行文件编译所需要的链接库
target_link_libraries(ChatClient pthread)
