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

message( "Checking device spec for extra compiler definitions..." )
foreach( directive ${DEVICE_FIELDS} )
    if( "${directive}" MATCHES "^SET_" )
        STRING( SUBSTRING ${directive} 4 -1 flag_name )
        LIST( FIND DEVICE_FIELDS ${directive} offset )
        LIST( GET DEVICE_VALUES ${offset} value )
        message( ${value} )
        if( ${value} EQUAL "1" )
            message( "Enabling " "${flag_name}" )
            add_definitions( "-D${flag_name}" )
        else()
            message( "Disabling " "${flag_name}" )
            remove_definitions( "-D${flag_name}" )
        endif()
    endif()
endforeach()

message( "Checking application spec for extra compiler definitions..." )
foreach( directive ${CODAL_FIELDS} )
    if( "${directive}" MATCHES "^SET_" )
        STRING( SUBSTRING ${directive} 4 -1 flag_name )
        LIST( FIND CODAL_FIELDS ${directive} offset )
        LIST( GET CODAL_VALUES ${offset} value )
        message( ${value} )
        if( ${value} EQUAL "1" )
            message( "Enabling " "${flag_name}" )
            add_definitions( "-D${flag_name}" )
        else()
            message( "Disabling " "${flag_name}" )
            remove_definitions( "-D${flag_name}" )
        endif()
    endif()
endforeach()

# Conditionally switch out the softdevice linker script, if we're using softdevice in this build
if( "${codal.config.SET_SOFTDEVICE_PRESENT}" EQUAL "1" OR "${device.config.SET_SOFTDEVICE_PRESENT}" EQUAL "1" )
    message( "Building with softdevice support" )
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/ld/nrf52833-softdevice.ld\"" PARENT_SCOPE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/ld/nrf52833-softdevice.ld\"" PARENT_SCOPE)
else()
    message( "Building without softdevice support" )
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/ld/nrf52833.ld\"" PARENT_SCOPE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/ld/nrf52833.ld\"" PARENT_SCOPE)
endif()
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

# expose it to parent cmake.
target_include_directories(codal-microbit-v2 PUBLIC ${INCLUDE_DIRS})