project(codal-microbit)

include("${CODAL_UTILS_LOCATION}")

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


# nRF5SDK includes

list(APPEND INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/sdk_config"
    "${CMAKE_CURRENT_SOURCE_DIR}/nRF5SDK_mods"
)

list(APPEND INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/scheduler"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/pwr_mgmt"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/sortlist"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/strerror"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/softdevice/common"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/crc16"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/bootloader/dfu"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/toolchain/cmsis/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/util"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/common"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/balloc"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/peer_manager"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/ringbuf"
)

list(APPEND INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/timer"
)

list(APPEND INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/log"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/nrf_ble_gatt"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/nrf_ble_qwr"
)

list(APPEND INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/bootloader"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/fstorage"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/experimental_section_vars"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/softdevice/s140/headers"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/mutex"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/delay"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/bootloader/ble_dfu"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/atomic_fifo"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/atomic"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/boards"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/integration/nrfx/legacy"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/memobj"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/integration/nrfx"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/fds"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/ble_advertising"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/atomic_flags"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/softdevice/s140/headers/nrf52"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/ble_services/ble_dfu"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/external/fprintf"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/svc"
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/log/src"
)

list(APPEND INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/ble_services/ble_dis"
)

# needed for NRF_LOG_XXX
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/nRF5SDK_mods/nrf_log_backend_dmesg.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/log/src/nrf_log_backend_serial.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/log/src/nrf_log_frontend.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/log/src/nrf_log_str_formatter.c")

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/util/app_error.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/util/app_error_handler_gcc.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/util/app_error_weak.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/util/app_util_platform.c")
#list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/util/nrf_assert.c")

#list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/scheduler/app_scheduler.c")

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/timer/app_timer2.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/nRF5SDK_mods/drv_rtc.c")

#list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/crc16/crc16.c")

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/fds/fds.c")

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/atomic_fifo/nrf_atfifo.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/atomic_flags/nrf_atflags.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/atomic/nrf_atomic.c")

# needed for NRF_LOG_XXX
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/balloc/nrf_balloc.c")

# needed for NRF_LOG_XXX
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/external/fprintf/nrf_fprintf.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/external/fprintf/nrf_fprintf_format.c")

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/fstorage/nrf_fstorage.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/fstorage/nrf_fstorage_sd.c")

# needed for NRF_LOG_XXX
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/memobj/nrf_memobj.c")

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/nRF5SDK_mods/nrf_pwr_mgmt.c")

# needed for NRF_LOG_XXX
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/ringbuf/nrf_ringbuf.c")

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/experimental_section_vars/nrf_section_iter.c")

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/sortlist/nrf_sortlist.c")

# needed for NRF_LOG_XXX
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/strerror/nrf_strerror.c")

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/libraries/bootloader/dfu/nrf_dfu_svci.c")

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/common/ble_advdata.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/common/ble_conn_params.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/common/ble_conn_state.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/common/ble_srv_common.c")

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/nrf_ble_gatt/nrf_ble_gatt.c")

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/nrf_ble_qwr/nrf_ble_qwr.c")

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/peer_manager/gatt_cache_manager.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/peer_manager/gatts_cache_manager.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/peer_manager/id_manager.c")
#list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/peer_manager/auth_status_tracker.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/peer_manager/peer_data_storage.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/peer_manager/peer_database.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/peer_manager/peer_id.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/peer_manager/peer_manager.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/peer_manager/peer_manager_handler.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/peer_manager/pm_buffer.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/peer_manager/security_dispatcher.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/peer_manager/security_manager.c")

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/nRF5SDK_mods/nrf_sdh.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/softdevice/common/nrf_sdh_ble.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/nRF5SDK_mods/nrf_sdh_soc.c")

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/ble_services/ble_dfu/ble_dfu.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/nRF5SDK_mods/ble_dfu_bonded.c")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/ble_services/ble_dfu/ble_dfu_unbonded.c")

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../nRF5SDK/components/ble/ble_services/ble_dis/ble_dis.c")

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
    ${LIB_OBJECT_FILES}
    ${LIB_ARCHIVE_FILES}
)

# expose it to parent cmake.
target_include_directories(codal-microbit PUBLIC ${INCLUDE_DIRS})
