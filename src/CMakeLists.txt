

set(SRC_LIST sync/mutex.cpp
        buffer.cpp
        epoller.cpp
        HTTPconnection.cpp
        HTTPrequest.cpp
        HTTPresponse.cpp
        log.cpp
        timer.cpp
        webserver.cpp
        util.cpp
        threadPool.cpp)
set(LIB_LIB pthread yaml-cpp dl)
add_library(myWS STATIC ${SRC_LIST} )

target_link_libraries(myWS ${LIB_LIB})
