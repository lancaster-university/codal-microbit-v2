project(codal-microbit-v2)

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
add_library(codal-microbit-v2 ${SOURCE_FILES})

target_link_libraries(
    codal-microbit-v2
    codal-nrf52
    codal-core
    codal-microbit-nrf5sdk
    ${LIB_OBJECT_FILES}
    ${LIB_ARCHIVE_FILES}
)

# Check if we need to rebuild the HTML sources based on an URL change via the variable MICROBIT_URL
if("${codal.config.MICROBIT_URL}" STRGREATER "")
    message( INFO "Using non-standard upstream URL, building with custom HTML stages!" )
    add_custom_target(
        htmlRebuildTarget
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources/logfs 
        COMMAND node ${CMAKE_CURRENT_SOURCE_DIR}/resources/logfs/build.js
    )
    add_dependencies(codal-microbit-v2 htmlRebuildTarget)
endif()

# expose it to parent cmake.
target_include_directories(codal-microbit-v2 PUBLIC ${INCLUDE_DIRS})
