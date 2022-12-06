SET(TARGET 3-basic_lighting)

FILE(GLOB_RECURSE SRC_FILES "${SRC_DIR}/${TARGET}/*.cpp" "${SRC_DIR}/glengine/*.cpp")
ADD_EXECUTABLE(${TARGET} ${SRC_FILES})

ADD_DEPENDENCIES(dev ${TARGET})

SET_TARGET_PROPERTIES(${TARGET} PROPERTIES CXX_EXTENSIONS OFF)
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES CXX_STANDARD 17)
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES CXX_STANDARD_REQUIRED ON)

TARGET_COMPILE_OPTIONS(${TARGET} PUBLIC
  $<$<PLATFORM_ID:Linux>:
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang>: -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>: -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined>
  >
  $<$<PLATFORM_ID:Windows>:
    $<$<COMPILE_LANG_AND_ID:C,GNU>: -Wall -Wextra -Wpedantic -g> # not supported
    $<$<COMPILE_LANG_AND_ID:C,Clang>: -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined> # not supported
    $<$<COMPILE_LANG_AND_ID:C,MSVC>: /Wall /DEBUG /fsanitize=address,undefined>
    $<$<COMPILE_LANG_AND_ID:CXX,GNU>: -Wall -Wextra -Wpedantic -g> # not supported
    $<$<COMPILE_LANG_AND_ID:CXX,Clang>: -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined> # not supported
    $<$<COMPILE_LANG_AND_ID:CXX,MSVC>: /Wall /DEBUG /fsanitize=address,undefined>
  >
)
TARGET_LINK_OPTIONS(${TARGET} PUBLIC
  $<$<PLATFORM_ID:Linux>:
    $<$<LINK_LANG_AND_ID:C,GNU,Clang>: -lX11 -g -fsanitize=address,undefined>
    $<$<LINK_LANG_AND_ID:CXX,GNU,Clang>: -lX11 -g -fsanitize=address,undefined>
  >
  $<$<PLATFORM_ID:Windows>:
    $<$<LINK_LANG_AND_ID:C,GNU>: -g> # not supported
    $<$<LINK_LANG_AND_ID:C,Clang>: -g -fsanitize=address,undefined> # not supported
    $<$<LINK_LANG_AND_ID:C,MSVC>: /DEBUG /fsanitize=address,undefined>
    $<$<LINK_LANG_AND_ID:CXX,GNU>: -g> # not supported
    $<$<LINK_LANG_AND_ID:CXX,Clang>: -g -fsanitize=address,undefined> # not supported
    $<$<LINK_LANG_AND_ID:CXX,MSVC>: /DEBUG /fsanitize=address,undefined>
  >
)

TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE ${INC_DIR} "${SRC_DIR}/glengine")
FILE(GLOB_RECURSE LIB_FILES "${LIB_DIR}/*.a" "${LIB_DIR}/*.so" "${LIB_DIR}/*.lib")
TARGET_LINK_LIBRARIES(${TARGET} PRIVATE ${LIB_FILES})

FIND_PACKAGE(Qt5 COMPONENTS Core REQUIRED)
TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE Qt)
TARGET_LINK_LIBRARIES(${TARGET} PRIVATE ${Qt5Core_LIBRARIES})

ADD_CUSTOM_COMMAND(TARGET ${TARGET} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${SRC_DIR}/${TARGET}/shaders"
    "${BIN_DIR}/shaders"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${RSC_DIR}"
    "${BIN_DIR}/resources"
)
