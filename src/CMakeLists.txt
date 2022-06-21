aux_source_directory(. SRC_LIST)


add_library(myWS STATIC ${SRC_LIST})


target_link_libraries(myWS pthread)