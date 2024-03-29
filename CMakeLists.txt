CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

PROJECT(opengl-practice VERSION 1.0.0 LANGUAGES CXX)

SET(CMAKE_CXX_COMPILER g++)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
SET(INC_DIR ${CMAKE_SOURCE_DIR}/include)
SET(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
SET(RSC_DIR ${CMAKE_SOURCE_DIR}/resources)
SET(BIN_DIR ${CMAKE_BINARY_DIR}/build)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Debug)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Release)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release)

FILE(GLOB_RECURSE SRC_CXX_FILES "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.hpp" "${SRC_DIR}/*.inl")
ADD_CUSTOM_TARGET(checks
  COMMAND cppcheck -q --inconclusive --enable=all --suppress=missingIncludeSystem --error-exitcode=0 ${SRC_CXX_FILES}
  COMMAND clang-tidy -fix -p ${BIN_DIR} ${SRC_CXX_FILES}
  COMMAND clang-format -i ${SRC_CXX_FILES}
)

ADD_CUSTOM_TARGET(dev)
ADD_DEPENDENCIES(dev checks)

FILE(GLOB TARGET_DIRS ${SRC_DIR}/*)
FOREACH(TARGET_DIR ${TARGET_DIRS})
  IF(IS_DIRECTORY ${TARGET_DIR})
    ADD_SUBDIRECTORY(${TARGET_DIR})
  ENDIF()
ENDFOREACH()
