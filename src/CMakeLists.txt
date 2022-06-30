
set(SRC_LIST sync/mutex.cpp
        buffer.cpp
        epoller.cpp
        HTTPconnection.cpp
        HTTPrequest.cpp
        HTTPresponse.cpp
        log.cpp
        timer.cpp
        webserver.cpp)

add_library(myWS STATIC ${SRC_LIST})


target_link_libraries(myWS pthread)