SET(TARGET glengine)

FILE(GLOB_RECURSE SRC_FILES "${SRC_DIR}/${TARGET}/*.cpp")
ADD_LIBRARY(${TARGET} STATIC ${SRC_FILES})

ADD_DEPENDENCIES(dev ${TARGET})

SET_TARGET_PROPERTIES(${TARGET} PROPERTIES CXX_EXTENSIONS OFF)
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES CXX_STANDARD 17)
SET_TARGET_PROPERTIES(${TARGET} PROPERTIES CXX_STANDARD_REQUIRED ON)

TARGET_COMPILE_OPTIONS(${TARGET} PUBLIC
  $<$<PLATFORM_ID:Linux>:
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang>:
      -Wall -Wextra -Wpedantic
      $<$<CONFIG:Debug>: -g -fsanitize=address,undefined>
      $<$<CONFIG:Release>: -O3>
    >
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:
      -Wall -Wextra -Wpedantic
      $<$<CONFIG:Debug>: -g -fsanitize=address,undefined>
      $<$<CONFIG:Release>: -O3>
    >
  >
  $<$<PLATFORM_ID:Windows>:
    $<$<COMPILE_LANG_AND_ID:C,MSVC>:
      /Wall
      $<$<CONFIG:Debug>: /DEBUG /fsanitize=address,undefined>
      $<$<CONFIG:Release>: /O2>
    >
    $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:
      /Wall
      $<$<CONFIG:Debug>: /DEBUG /fsanitize=address,undefined>
      $<$<CONFIG:Release>: /O2>
    >
  >
)

TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE ${INC_DIR})
FILE(GLOB_RECURSE LIB_FILES "${LIB_DIR}/*.a" "${LIB_DIR}/*.so" "${LIB_DIR}/*.lib")
TARGET_LINK_LIBRARIES(${TARGET} PRIVATE ${LIB_FILES})

FIND_PACKAGE(Qt6 COMPONENTS Core REQUIRED)
TARGET_INCLUDE_DIRECTORIES(${TARGET} PRIVATE Qt)
TARGET_LINK_LIBRARIES(${TARGET} PRIVATE Qt6::Core)
