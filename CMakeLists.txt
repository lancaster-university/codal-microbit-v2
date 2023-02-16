project(codal-microbit-v2)

# Actually merge our multiple configs into a single set of values so we can query them easily.
# Note: This will be removed when we move to a library-ified version of the build system
#       so do not rely on it for anything.
#
# Note: This form of loop isn't available until we move to at least CMake 3.17+, so the second form
#       below has been added to do the same, in a more verbose way.
#foreach( _key _val IN ZIP_LISTS FINAL_FIELDS FINAL_VALUES )
#    set( "__merged.config.${_key}" "${_val}" )
#endforeach()



LIST( LENGTH FINAL_FIELDS _idxMax )
MATH( EXPR _idxMax "${_idxMax} - 1" )
foreach( _index RANGE ${_idxMax} )
    LIST( GET FINAL_FIELDS ${_index} _key )
    LIST( GET FINAL_VALUES ${_index} _val )
    set( "__merged.config.${_key}" "${_val}" )
endforeach()

if( DEFINED __merged.config.SOFTDEVICE_PRESENT AND NOT "${__merged.config.DEVICE_BLE}" STREQUAL "1" )
    message( FATAL_ERROR "Do not define SOFTDEVICE_PRESENT in your configuration, use DEVICE_BLE instead." )
endif()


# Determine if we need SOFTDEVICE support by checking if BLE is enabled.
if( "${__merged.config.DEVICE_BLE}" STREQUAL "1" )
    message( "Building WITH softdevice support" )

    if( DEFINED codal.config.S140 )
        message( "Softdevice S140 selected" )
        set( NRF52833_SOFTDEVICE_VERSION "s140")
    else()
        message( "Softdevice S113 selected" )
        set( NRF52833_SOFTDEVICE_VERSION "s113")
    endif()

    file( APPEND "${EXTRA_INCLUDES_PATH}" " #define SOFTDEVICE_PRESENT    1" )
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/ld/nrf52833-softdevice-${NRF52833_SOFTDEVICE_VERSION}.ld\"" PARENT_SCOPE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/ld/nrf52833-softdevice-${NRF52833_SOFTDEVICE_VERSION}.ld\"" PARENT_SCOPE)

    RECURSIVE_FIND_FILE(LIB_OBJECT_FILES "${CMAKE_CURRENT_LIST_DIR}/lib/${NRF52833_SOFTDEVICE_VERSION}" "*.o")
    
else()
    message( "Building WITHOUT softdevice support" )
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/ld/nrf52833.ld\"" PARENT_SCOPE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T\"${CMAKE_CURRENT_LIST_DIR}/ld/nrf52833.ld\"" PARENT_SCOPE)

    RECURSIVE_FIND_FILE(LIB_OBJECT_FILES "${CMAKE_CURRENT_LIST_DIR}/lib/no_softdevice" "*.o")
endif()

# find sources and headers
RECURSIVE_FIND_DIR(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/inc" "*.h")
RECURSIVE_FIND_FILE(SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/source" "*.c??")

RECURSIVE_FIND_DIR(INCLUDE_DIRS2 "${CMAKE_CURRENT_SOURCE_DIR}/model" "*.h")
RECURSIVE_FIND_FILE(SOURCE_FILES2 "${CMAKE_CURRENT_SOURCE_DIR}/model" "*.c??")

list(APPEND INCLUDE_DIRS "${INCLUDE_DIRS2}")
list(APPEND SOURCE_FILES "${SOURCE_FILES2}")

# find prebuilt libraries and objects

# Add object files form lib folder (not recursively)
# FILE(GLOB new_o_files "${CMAKE_CURRENT_LIST_DIR}/lib/*.o")
# FOREACH(o_file ${new_o_files})
#     LIST(APPEND LIB_OBJECT_FILES ${o_file})
# ENDFOREACH()


# if( "${__merged.config.DEVICE_BLE}" STREQUAL "1" )
#     # Add object files from specific softdevice lib folder (not recursively)
#     FILE(GLOB new_drivers_o_files "${CMAKE_CURRENT_LIST_DIR}/lib/${NRF52833_SOFTDEVICE_VERSION}/*.o")
#     FOREACH(o_file ${new_drivers_o_files})
#         LIST(APPEND LIB_OBJECT_FILES ${o_file})
#     ENDFOREACH()
# endif()

# LIST(REMOVE_DUPLICATES LIB_OBJECT_FILES)


RECURSIVE_FIND_FILE(LIB_ARCHIVE_FILES "${CMAKE_CURRENT_LIST_DIR}/lib" "*.a")

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

message(STATUS "===================== DEBUG VARIABLE =====================")
get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()
message(STATUS "=========================================================")

# expose it to parent cmake.
target_include_directories(codal-microbit-v2 PUBLIC ${INCLUDE_DIRS})