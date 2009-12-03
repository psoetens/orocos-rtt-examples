
SET(SRCS nAxisTest.cpp)

SET(target robotdemo)

ADD_EXECUTABLE(${target} ${SRCS})

TARGET_LINK_LIBRARIES(${target} ${APP_LIBRARIES} ACE) 



