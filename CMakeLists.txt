project(codal-microbit)

# find sources and headers
RECURSIVE_FIND_DIR(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/inc" "*.h")
RECURSIVE_FIND_FILE(SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/source" "*.c??")

RECURSIVE_FIND_DIR(INCLUDE_DIRS2 "${CMAKE_CURRENT_SOURCE_DIR}/model" "*.h")
RECURSIVE_FIND_FILE(SOURCE_FILES2 "${CMAKE_CURRENT_SOURCE_DIR}/model" "*.c??")

list(APPEND INCLUDE_DIRS "${INCLUDE_DIRS2}")
list(APPEND SOURCE_FILES "${SOURCE_FILES2}")


# codal-nrf52 includes

string(FIND "${device.processor}" "NRF52840" pos40)
string(FIND "${device.processor}" "NRF52833" pos33)
if (NOT "${pos40}" STREQUAL "-1")
    set(TEMPLATE "nRF52840")
    set(DEV "nrf52840")
else()
if (NOT "${pos33}" STREQUAL "-1")
    set(TEMPLATE "nRF52833")
    set(DEV "nrf52833")
else()
    set(TEMPLATE "nRF52832")
    set(DEV "nrf52")
endif()
endif()

list(APPEND INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/../codal-nrf52/inc"
    "${CMAKE_CURRENT_SOURCE_DIR}/../codal-nrf52/inc/cmsis"
    "${CMAKE_CURRENT_SOURCE_DIR}/../codal-nrf52/nrfx/mdk"
    "${CMAKE_CURRENT_SOURCE_DIR}/../codal-nrf52/nrfx/hal"
    "${CMAKE_CURRENT_SOURCE_DIR}/../codal-nrf52/nrfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/../codal-nrf52/nrfx/templates/"
    "${CMAKE_CURRENT_SOURCE_DIR}/../codal-nrf52/nrfx/templates/${TEMPLATE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../codal-nrf52/nrfx/drivers/include"
)

# find prebuilt libraries and objects
RECURSIVE_FIND_FILE(LIB_OBJECT_FILES "${CMAKE_CURRENT_LIST_DIR}/lib" "*.o")
RECURSIVE_FIND_FILE(LIB_ARCHIVE_FILES "${CMAKE_CURRENT_LIST_DIR}/lib" "*.a")

set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/ld/nrf52833.ld\"" PARENT_SCOPE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/ld/nrf52833.ld\"" PARENT_SCOPE)
set(CMAKE_SYSTEM_PROCESSOR "armv7-m" PARENT_SCOPE)

# add them
include_directories(${INCLUDE_DIRS})

# create our target
add_library(codal-microbit ${SOURCE_FILES})

target_link_libraries(
    codal-microbit
    codal-nrf52
    codal-core
    codal-microbit-nrf5sdk
    ${LIB_OBJECT_FILES}
    ${LIB_ARCHIVE_FILES}
)

# expose it to parent cmake.
target_include_directories(codal-microbit PUBLIC ${INCLUDE_DIRS})
