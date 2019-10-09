project(codal-microbit-next)

include("${CODAL_UTILS_LOCATION}")

# find sources and headers
RECURSIVE_FIND_DIR(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/inc" "*.h")
RECURSIVE_FIND_FILE(SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/source" "*.c??")

RECURSIVE_FIND_DIR(INCLUDE_DIRS2 "${CMAKE_CURRENT_SOURCE_DIR}/model" "*.h")
RECURSIVE_FIND_FILE(SOURCE_FILES2 "${CMAKE_CURRENT_SOURCE_DIR}/model" "*.c??")

list(APPEND INCLUDE_DIRS "${INCLUDE_DIRS2}")
list(APPEND SOURCE_FILES "${SOURCE_FILES2}")

# find prebuilt libraries and objects
RECURSIVE_FIND_FILE(LIB_OBJECT_FILES "${CMAKE_CURRENT_LIST_DIR}/lib" "*.o")
RECURSIVE_FIND_FILE(LIB_ARCHIVE_FILES "${CMAKE_CURRENT_LIST_DIR}/lib" "*.a")

set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/ld/nrf52833.ld\"" PARENT_SCOPE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/ld/nrf52833.ld\"" PARENT_SCOPE)
set(CMAKE_SYSTEM_PROCESSOR "armv7-m" PARENT_SCOPE)

# add them
include_directories(${INCLUDE_DIRS})

# create our target
add_library(codal-microbit-next ${SOURCE_FILES})

target_link_libraries(
    codal-microbit-next
    codal-nrf52
    ${LIB_OBJECT_FILES}
    ${LIB_ARCHIVE_FILES}
)

# expose it to parent cmake.
target_include_directories(codal-microbit-next PUBLIC ${INCLUDE_DIRS})
