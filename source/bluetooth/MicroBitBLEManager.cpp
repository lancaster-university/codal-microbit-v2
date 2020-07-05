/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_dis.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdm.h"
#include "app_timer.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_conn_state.h"
#include "ble_dfu.h"
#include "nrf_ble_gatt.h"
#include "fds.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_power.h"
#include "nrf_bootloader_info.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#if ( defined(NRF_LOG_BACKEND_RTT_ENABLED) && NRF_LOG_BACKEND_RTT_ENABLED) || ( defined(NRF_LOG_BACKEND_UART_ENABLED) && NRF_LOG_BACKEND_UART_ENABLED)
#include "nrf_log_default_backends.h"
#endif

#include "MicroBitBLEManager.h"
#include "MicroBitEddystone.h"
#include "MicroBitStorage.h"
#include "MicroBitFiber.h"
#include "MicroBitSystemTimer.h"
#include "MicroBitDevice.h"
#include "MicroBitEventService.h"
#include "MicroBitPartialFlashingService.h"

#include "CodalDmesg.h"
#include "nrf_log_backend_dmesg.h"


#define MICROBIT_PAIRING_FADE_SPEED 4

//
// Local enumeration of valid security modes. Used only to optimise pre‐processor comparisons.
//
#define __SECURITY_MODE_ENCRYPTION_OPEN_LINK 0
#define __SECURITY_MODE_ENCRYPTION_NO_MITM 1
#define __SECURITY_MODE_ENCRYPTION_WITH_MITM 2
//
// Some Black Magic to compare the definition of our security mode in MicroBitConfig with a given parameter.
// Required as the MicroBitConfig option is actually an mbed enum, that is not normally comparable at compile time.
//

#define __CAT(a, ...) a##__VA_ARGS__
#define SECURITY_MODE(x) __CAT(__, x)
#define SECURITY_MODE_IS(x) (SECURITY_MODE(MICROBIT_BLE_SECURITY_LEVEL) == SECURITY_MODE(x))

//
// Times for pairing (ms): delay between pairing events and delay before disconnecting
//
#ifndef MICROBIT_BLE_DISCONNECT_AFTER_PAIRING_DELAY
#define MICROBIT_BLE_DISCONNECT_AFTER_PAIRING_DELAY  2000
#endif

#ifndef MICROBIT_BLE_PAIRING_EVENT_DELAY
#define MICROBIT_BLE_PAIRING_EVENT_DELAY 2000
#endif

//
// Time (ms) to delay shutdown or disabling softdevice
//
#ifndef MICROBIT_BLE_SHUTDOWN_DELAY
#define MICROBIT_BLE_SHUTDOWN_DELAY 500
#endif

const char *MICROBIT_BLE_MANUFACTURER = NULL;
const char *MICROBIT_BLE_MODEL = "BBC micro:bit";
const char *MICROBIT_BLE_HARDWARE_VERSION = NULL;
const char *MICROBIT_BLE_FIRMWARE_VERSION = MICROBIT_DAL_VERSION;
const char *MICROBIT_BLE_SOFTWARE_VERSION = NULL;
const int8_t MICROBIT_BLE_POWER_LEVEL[] = { -20, -16, -12, -8, -4, 0, 4, 8};

/*
 * Many of the interfaces we need to use only support callbacks to plain C functions, rather than C++ methods.
 * So, we maintain a pointer to the MicroBitBLEManager that's in use. Ths way, we can still access resources on the micro:bit
 * whilst keeping the code modular.
 */
MicroBitBLEManager *MicroBitBLEManager::manager = NULL; // Singleton reference to the BLE manager. many BLE API callbacks still do not support member functions. :-(

#define microbit_ble_OBSERVER_PRIO           3
#define microbit_ble_CONN_CFG_TAG            1


static int                  m_power         = MICROBIT_BLE_DEFAULT_TX_POWER;
static uint8_t              m_adv_handle    = BLE_GAP_ADV_SET_HANDLE_NOT_SET;
static uint8_t              m_enc_advdata[ BLE_GAP_ADV_SET_DATA_SIZE_MAX];

static pm_peer_id_t         m_failed_peer_id = PM_PEER_ID_INVALID;

NRF_BLE_GATT_DEF( m_gatt);


static void const_ascii_to_utf8(ble_srv_utf8_str_t * p_utf8, const char * p_ascii);

static void microbit_ble_for_each_connected_disconnect( uint16_t conn_handle, void *p_context);
static void microbit_ble_for_each_connected_tx_power_set( uint16_t conn_handle, void *p_context);

static void bleConnectionCallback( microbit_gaphandle_t handle);
static void passkeyDisplayCallback( microbit_gaphandle_t handle, ManagedString passKey);

static void microbit_ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);
static void microbit_ble_pm_evt_handler(pm_evt_t const * p_evt);
static void microbit_ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);

static void microbit_dfu_init(void);

static void microbit_ble_configureAdvertising( bool connectable, bool discoverable, bool whitelist, uint16_t interval_ms, int timeout_seconds);

#if CONFIG_ENABLED(MICROBIT_BLE_EDDYSTONE_URL) || CONFIG_ENABLED(MICROBIT_BLE_EDDYSTONE_UID)
static void microbit_ble_configureAdvertising( bool connectable, bool discoverable, bool whitelist, uint16_t interval_ms, int timeout_seconds,
                                               uint8_t *frameData, uint16_t frameSize);
#endif


/**
 * Constructor.
 *
 * Configure and manage the micro:bit's Bluetooth Low Energy (BLE) stack.
 *
 * @param _storage an instance of MicroBitStorage used to persist sys attribute information. (This is required for compatability with iOS).
 *
 * @note The BLE stack *cannot*  be brought up in a static context (the software simply hangs or corrupts itself).
 * Hence, the init() member function should be used to initialise the BLE stack.
 */
MicroBitBLEManager::MicroBitBLEManager(MicroBitStorage &_storage) : storage(&_storage)
{
    manager = this;
    this->pairingStatus = 0;
        
#if CONFIG_ENABLED(MICROBIT_BLE_DFU_SERVICE)
    // Initialize buttonless SVCI bootloader interface before interrupts are enabled
    MICROBIT_BLE_ECHK( ble_dfu_buttonless_async_svci_init());
#endif
}

/**
 * Constructor.
 *
 * Configure and manage the micro:bit's Bluetooth Low Energy (BLE) stack.
 *
 * @note The BLE stack *cannot*  be brought up in a static context (the software simply hangs or corrupts itself).
 * Hence, the init() member function should be used to initialise the BLE stack.
 */
MicroBitBLEManager::MicroBitBLEManager() : storage(NULL)
{
    manager = this;
    this->pairingStatus = 0;
    
#if CONFIG_ENABLED(MICROBIT_BLE_DFU_SERVICE)
    // Initialize buttonless SVCI bootloader interface before interrupts are enabled
    MICROBIT_BLE_ECHK( ble_dfu_buttonless_async_svci_init());
#endif
}


/**
 * When called, the micro:bit will begin advertising for a predefined period,
 * MICROBIT_BLE_ADVERTISING_TIMEOUT seconds to bonded devices.
 */
MicroBitBLEManager *MicroBitBLEManager::getInstance()
{
    if (manager == 0)
    {
        manager = new MicroBitBLEManager;
    }
    return manager;
}


/**
  * Post constructor initialisation method as the BLE stack cannot be brought
  * up in a static context.
  *
  * @param deviceName The name used when advertising
  * @param serialNumber The serial number exposed by the device information service
  * @param messageBus An instance of an EventModel, used during pairing.
  * @param enableBonding If true, the security manager enabled bonding.
  *
  * @code
  * bleManager.init(uBit.getName(), uBit.getSerial(), uBit.messageBus, true);
  * @endcode
  */
void MicroBitBLEManager::init( ManagedString deviceName, ManagedString serialNumber, EventModel &messageBus, bool enableBonding)
{
    if ( this->status & DEVICE_COMPONENT_RUNNING)
      return;

    MICROBIT_DEBUG_DMESG( "MicroBitBLEManager::init");
    
    pairingTime = 0;
    shutdownTime = 0;

#if NRF_LOG_ENABLED
    MICROBIT_BLE_ECHK( NRF_LOG_INIT(NULL));
#if ( defined(NRF_LOG_BACKEND_RTT_ENABLED) && NRF_LOG_BACKEND_RTT_ENABLED) || ( defined(NRF_LOG_BACKEND_UART_ENABLED) && NRF_LOG_BACKEND_UART_ENABLED)
    NRF_LOG_DEFAULT_BACKENDS_INIT();
#endif
#if defined(NRF_LOG_BACKEND_DMESG_ENABLED) && NRF_LOG_BACKEND_DMESG_ENABLED
    nrf_log_backend_dmesg_init();
#endif
#endif // NRF_LOG_ENABLED

    MICROBIT_BLE_ECHK( app_timer_init());
    
    nrf_sdh_soc_init();
  
    // Start the BLE stack.
    uint32_t ram_start = 0;
    MICROBIT_BLE_ECHK( nrf_pwr_mgmt_init());
    MICROBIT_BLE_ECHK( nrf_sdh_enable_request());
    MICROBIT_BLE_ECHK( nrf_sdh_ble_default_cfg_set( microbit_ble_CONN_CFG_TAG, &ram_start));
    
    // set fixed gap name
    gapName = MICROBIT_BLE_MODEL;
    if ( enableBonding || !CONFIG_ENABLED(MICROBIT_BLE_WHITELIST))
    {
        ManagedString namePrefix(" [");
        ManagedString namePostfix("]");
        gapName = gapName + namePrefix + deviceName + namePostfix;
    }
    ble_cfg_t ble_cfg;
    memset(&ble_cfg, 0, sizeof(ble_cfg));
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS( &ble_cfg.gap_cfg.device_name_cfg.write_perm);
    ble_cfg.gap_cfg.device_name_cfg.vloc        = BLE_GATTS_VLOC_USER;
    ble_cfg.gap_cfg.device_name_cfg.p_value     = (uint8_t *)gapName.toCharArray();
    ble_cfg.gap_cfg.device_name_cfg.current_len = gapName.length();
    ble_cfg.gap_cfg.device_name_cfg.max_len     = gapName.length();
    MICROBIT_BLE_ECHK( sd_ble_cfg_set( BLE_GAP_CFG_DEVICE_NAME, &ble_cfg, ram_start));

    MICROBIT_BLE_ECHK( nrf_sdh_ble_enable(&ram_start));
    NRF_SDH_BLE_OBSERVER( microbit_ble_observer, microbit_ble_OBSERVER_PRIO, microbit_ble_evt_handler, NULL);

    MICROBIT_BLE_ECHK( sd_ble_gap_appearance_set( BLE_APPEARANCE_UNKNOWN));
    
//#ifdef MICROBIT_V1_MBED_BLE_PATCHES
//    // Configure the stack to hold onto the CPU during critical timing events.
//    // mbed-classic performs __disable_irq() calls in its timers that can cause
//    // MIC failures on secure BLE channels...
//    ble_common_opt_radio_cpu_mutex_t opt;
//    opt.enable = 1;
//    sd_ble_opt_set(BLE_COMMON_OPT_RADIO_CPU_MUTEX, (const ble_opt_t *)&opt);
//#endif
//
//#if CONFIG_ENABLED(MICROBIT_BLE_PRIVATE_ADDRESSES)
//    // Configure for private addresses, so kids' behaviour can't be easily tracked.
//    ble->gap().setAddress(BLEProtocol::AddressType::RANDOM_PRIVATE_RESOLVABLE, {0});
//#endif
    
    // Setup our security requirements.
    // @bluetooth_mdw: select either passkey pairing (more secure),
    // "just works" pairing (less secure but nice and simple for the user)
    // or no security
    // Default to passkey pairing with MITM protection
    
    ble_gap_sec_params_t sec_param;
    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

#if MICROBIT_BLE_SECURITY_MODE == 3
#if defined(MICROBIT_BLE_SECURITY_LEVEL) && !(SECURITY_MODE_IS(SECURITY_MODE_ENCRYPTION_WITH_MITM))
#error "MICROBIT_BLE_SECURITY_MODE == 2 but MICROBIT_BLE_SECURITY_LEVEL != SECURITY_MODE_ENCRYPTION_WITH_MITM"
#endif
#elif MICROBIT_BLE_SECURITY_MODE == 2
#if defined(MICROBIT_BLE_SECURITY_LEVEL) && !(SECURITY_MODE_IS(SECURITY_MODE_ENCRYPTION_NO_MITM))
#error "MICROBIT_BLE_SECURITY_MODE == 2 but MICROBIT_BLE_SECURITY_LEVEL != SECURITY_MODE_ENCRYPTION_NO_MITM"
#endif
#elif MICROBIT_BLE_SECURITY_MODE == 1
#if defined(MICROBIT_BLE_SECURITY_LEVEL) && !(SECURITY_MODE_IS(SECURITY_MODE_ENCRYPTION_OPEN_LINK))
#error "MICROBIT_BLE_SECURITY_MODE == 2 but MICROBIT_BLE_SECURITY_LEVEL != SECURITY_MODE_ENCRYPTION_OPEN_LINK"
#endif
#else
#error "Unknown MICROBIT_BLE_SECURITY_MODE"
#endif

#if (MICROBIT_BLE_SECURITY_MODE == 2)
    MICROBIT_INFO_DMESG( "Just Works security");
    sec_param.bond = true;
    sec_param.mitm = false;
    sec_param.lesc = 0;
    sec_param.keypress = 0;
    sec_param.io_caps = BLE_GAP_IO_CAPS_NONE;
    sec_param.oob = false;
    sec_param.min_key_size = 7;
    sec_param.max_key_size = 16;
    sec_param.kdist_own.enc = 1;
    sec_param.kdist_own.id = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id = 1;
#elif (MICROBIT_BLE_SECURITY_MODE == 1)
    MICROBIT_INFO_DMESG( "No security");
    sec_param.bond = false;
    sec_param.mitm = false;
    sec_param.lesc = 0;
    sec_param.keypress = 0;
    sec_param.io_caps = BLE_GAP_IO_CAPS_NONE;
    sec_param.oob = false;
    sec_param.min_key_size = 7;
    sec_param.max_key_size = 16;
    sec_param.kdist_own.enc = 0;
    sec_param.kdist_own.id = 0;
    sec_param.kdist_peer.enc = 0;
    sec_param.kdist_peer.id = 0;
#elif (MICROBIT_BLE_SECURITY_MODE == 3)
    MICROBIT_INFO_DMESG( "Passkey security");
    sec_param.bond = true;
    sec_param.mitm = true;
    sec_param.lesc = 0;
    sec_param.keypress = 0;
    sec_param.io_caps = BLE_GAP_IO_CAPS_DISPLAY_ONLY;
    sec_param.oob = false;
    sec_param.min_key_size = 7;
    sec_param.max_key_size = 16;
    sec_param.kdist_own.enc = 1;
    sec_param.kdist_own.id = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id = 1;
#else
#error "Unknown MICROBIT_BLE_SECURITY_MODE"
#endif

    MICROBIT_BLE_ECHK( pm_init());
    MICROBIT_BLE_ECHK( pm_sec_params_set( &sec_param));
    MICROBIT_BLE_ECHK( pm_register( microbit_ble_pm_evt_handler));

    // Set up GAP
    // Configure for high speed mode where possible.
    ble_gap_conn_params_t   gap_conn_params;
    memset(&gap_conn_params, 0, sizeof(gap_conn_params));
    gap_conn_params.min_conn_interval = 8;      // 10 ms
    gap_conn_params.max_conn_interval = 16;     // 20 ms
    gap_conn_params.slave_latency     = 0;
    gap_conn_params.conn_sup_timeout  = 400;    // 4s
    MICROBIT_BLE_ECHK( sd_ble_gap_ppcp_set( &gap_conn_params));
    
    // Set up GATT
    MICROBIT_BLE_ECHK( nrf_ble_gatt_init( &m_gatt, NULL));
        
    if ( enableBonding)
    {
        MICROBIT_DEBUG_DMESG( "enableBonding");
        // If we're in pairing mode, review the size of the bond table.
        // If we're full, delete the lowest ranked.
        if ( getBondCount() >= MICROBIT_BLE_MAXIMUM_BONDS)
        {
            MICROBIT_DEBUG_DMESG( "delete the lowest ranked peer");
            pm_peer_id_t highest_ranked_peer;
            uint32_t     highest_rank;
            pm_peer_id_t lowest_ranked_peer;
            uint32_t     lowest_rank;
            pm_peer_ranks_get( &highest_ranked_peer, &highest_rank, &lowest_ranked_peer, &lowest_rank);
            pm_peer_delete( lowest_ranked_peer);
        }
    }

    bool connectable = true;
    bool discoverable = true;
    bool whitelist = false;
    
#if CONFIG_ENABLED(MICROBIT_BLE_WHITELIST)
    // Configure a whitelist to filter all connection requetss from unbonded devices.
    // Most BLE stacks only permit one connection at a time, so this prevents denial of service attacks.
//    ble->gap().setScanningPolicyMode(Gap::SCAN_POLICY_IGNORE_WHITELIST);
//    ble->gap().setAdvertisingPolicyMode(Gap::ADV_POLICY_FILTER_CONN_REQS);
    
    pm_peer_id_t peer_list[ MICROBIT_BLE_MAXIMUM_BONDS];
    uint32_t list_size = MICROBIT_BLE_MAXIMUM_BONDS;
    MICROBIT_BLE_ECHK( pm_peer_id_list( peer_list, &list_size, PM_PEER_ID_INVALID, PM_PEER_ID_LIST_ALL_ID ));
    MICROBIT_BLE_ECHK( pm_whitelist_set( list_size ? peer_list : NULL, list_size));
    MICROBIT_BLE_ECHK( pm_device_identities_list_set( list_size ? peer_list : NULL, list_size));
    connectable = discoverable = whitelist = list_size > 0;
    MICROBIT_INFO_DMESG( "whitelist size = %d", list_size);
#endif
    
    // Bring up core BLE services.
#if CONFIG_ENABLED(MICROBIT_BLE_DFU_SERVICE)
    MICROBIT_INFO_DMESG( "DFU_SERVICE");
    microbit_dfu_init();
#endif

#if CONFIG_ENABLED(MICROBIT_BLE_PARTIAL_FLASHING)
    MICROBIT_INFO_DMESG( "PARTIAL_FLASHING");
//    new MicroBitPartialFlashingService(*ble, messageBus);
#endif

    ManagedString modelVersion("V2.0");  // TODO use a calculated version
    ManagedString disName( MICROBIT_BLE_MODEL);
    disName = disName + " " + modelVersion;

#if CONFIG_ENABLED(MICROBIT_BLE_DEVICE_INFORMATION_SERVICE)
    MICROBIT_INFO_DMESG( "DEVICE_INFORMATION_SERVICE");
    ble_dis_init_t disi;
    memset( &disi, 0, sizeof(disi));
    disi.dis_char_rd_sec = SEC_OPEN;
    const_ascii_to_utf8( &disi.manufact_name_str,  MICROBIT_BLE_MANUFACTURER);
    const_ascii_to_utf8( &disi.model_num_str,      disName.toCharArray());
    const_ascii_to_utf8( &disi.serial_num_str,     serialNumber.toCharArray());
    const_ascii_to_utf8( &disi.hw_rev_str,         MICROBIT_BLE_HARDWARE_VERSION);
    const_ascii_to_utf8( &disi.fw_rev_str,         MICROBIT_BLE_FIRMWARE_VERSION);
    const_ascii_to_utf8( &disi.sw_rev_str,         MICROBIT_BLE_SOFTWARE_VERSION);
    //ble_dis_sys_id_t *             p_sys_id;                    /**< System ID. */
    //ble_dis_reg_cert_data_list_t * p_reg_cert_data_list;        /**< IEEE 11073-20601 Regulatory Certification Data List. */
    //ble_dis_pnp_id_t *             p_pnp_id;                    /**< PnP ID. */
    ble_dis_init( &disi);
#else
    (void)serialNumber;
#endif

#if CONFIG_ENABLED(MICROBIT_BLE_EVENT_SERVICE)
    MICROBIT_INFO_DMESG( "EVENT_SERVICE");
    new MicroBitEventService( *this, messageBus);
#else
    (void)messageBus;
#endif

    // Setup advertising.
    microbit_ble_configureAdvertising( connectable, discoverable, whitelist,
                                       MICROBIT_BLE_ADVERTISING_INTERVAL, MICROBIT_BLE_ADVERTISING_TIMEOUT);

    // Configure the radio at our default power level
    setTransmitPower( MICROBIT_BLE_DEFAULT_TX_POWER);

    ble_conn_params_init_t cp_init;
    memset(&cp_init, 0, sizeof(cp_init));
    cp_init.p_conn_params                  = &gap_conn_params;
    cp_init.first_conn_params_update_delay = APP_TIMER_TICKS(5000);     // 5 seconds
    cp_init.next_conn_params_update_delay  = APP_TIMER_TICKS(30000);    // 30 seconds
    cp_init.max_conn_params_update_count   = 3;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = true;
    MICROBIT_BLE_ECHK( ble_conn_params_init(&cp_init));

    setAdvertiseOnDisconnect( true);

// If we have whitelisting enabled, then prevent only enable advertising of we have any binded devices...
// This is to further protect kids' privacy. If no-one initiates BLE, then the device is unreachable.
// If whiltelisting is disabled, then we always advertise.
#if CONFIG_ENABLED(MICROBIT_BLE_WHITELIST)
    if ( getBondCount() > 0)
#endif
        advertise();

    this->status |= DEVICE_COMPONENT_RUNNING;
}


/**
 * Change the output power level of the transmitter to the given value.
 *
 * @param power a value in the range 0..7, where 0 is the lowest power and 7 is the highest.
 *
 * @return DEVICE_OK on success, or DEVICE_INVALID_PARAMETER if the value is out of range.
 *
 * @code
 * // maximum transmission power.
 * bleManager.setTransmitPower(7);
 * @endcode
 */
int MicroBitBLEManager::setTransmitPower(int power)
{
    if ( power < 0 || power >= MICROBIT_BLE_POWER_LEVELS)
        return DEVICE_INVALID_PARAMETER;

    MICROBIT_DEBUG_DMESG( "setTransmitPower %d", power);
    
    m_power = power;
    
    ble_conn_state_for_each_connected( microbit_ble_for_each_connected_tx_power_set, &m_power);
    
    if ( m_adv_handle != BLE_GAP_ADV_SET_HANDLE_NOT_SET)
    {
        MICROBIT_DEBUG_DMESG( " BLE_GAP_TX_POWER_ROLE_ADV");
        MICROBIT_BLE_ECHK( sd_ble_gap_tx_power_set( BLE_GAP_TX_POWER_ROLE_ADV, m_adv_handle, MICROBIT_BLE_POWER_LEVEL[ m_power]));
    }

    return DEVICE_OK;
}

/**
 * Determines the number of devices currently bonded with this micro:bit.
 * @return The number of active bonds.
 */
int MicroBitBLEManager::getBondCount()
{
    MICROBIT_DEBUG_DMESG( "getBondCount %d", pm_peer_count());

    return pm_peer_count();
}

/**
 * A request to pair has been received from a BLE device.
 * If we're in pairing mode, display the passkey to the user.
 * Also, purge the bonding table if it has reached capacity.
 *
 * @note for internal use only.
 */
void MicroBitBLEManager::pairingRequested(ManagedString passKey)
{
    MICROBIT_DEBUG_DMESG( "pairingRequested %s", passKey.toCharArray());
    
    // Update our mode to display the passkey.
    this->passKey = passKey;
    this->pairingStatus = MICROBIT_BLE_PAIR_REQUEST;
}

#define MICROBIT_BLE_PAIR_FAILURE   0
#define MICROBIT_BLE_PAIR_SUCCESS   1
#define MICROBIT_BLE_PAIR_AUTH      2
#define MICROBIT_BLE_PAIR_UPDATE    3
#define MICROBIT_BLE_PAIR_CHECK     4

/**
 * Record pairing progress
 *
 * @note for internal use only.
 */
bool MicroBitBLEManager::pairingComplete( int event)
{
    if ( currentMode != MICROBIT_MODE_PAIRING)
        return true;
    
    if ( this->pairingStatus & MICROBIT_BLE_PAIR_COMPLETE)
        return true;
    
    switch ( event)
    {
        case MICROBIT_BLE_PAIR_FAILURE:
            MICROBIT_DEBUG_DMESG( "pairingComplete FAILURE");
            this->pairingStatus = MICROBIT_BLE_PAIR_COMPLETE;
            this->status |= MICROBIT_BLE_STATUS_DELETE_BOND;
            fiber_add_idle_component(this);
            break;
            
        case MICROBIT_BLE_PAIR_SUCCESS:
            MICROBIT_DEBUG_DMESG( "pairingComplete SUCCESS");
            this->pairingStatus = MICROBIT_BLE_PAIR_COMPLETE | MICROBIT_BLE_PAIR_SUCCESSFUL;
            if ( MICROBIT_BLE_DISCONNECT_AFTER_PAIRING_DELAY > 0)
            {
                this->status |= MICROBIT_BLE_STATUS_DISCONNECT;
                fiber_add_idle_component(this);
            }
            break;
                
        case MICROBIT_BLE_PAIR_AUTH:
            MICROBIT_DEBUG_DMESG( "pairingComplete AUTH");
            pairingTime = system_timer_current_time();
            break;
            
        case MICROBIT_BLE_PAIR_UPDATE:
            MICROBIT_DEBUG_DMESG( "pairingComplete UPDATE");
            if ( pairingTime)
                pairingTime = system_timer_current_time();
            break;

        case MICROBIT_BLE_PAIR_CHECK:
            //MICROBIT_DEBUG_DMESG( "pairingComplete CHECK");
            if ( !(pairingStatus & MICROBIT_BLE_PAIR_COMPLETE)
                && pairingTime > 0
                && (system_timer_current_time() - pairingTime) >= MICROBIT_BLE_PAIRING_EVENT_DELAY)
            {
                pairingComplete( MICROBIT_BLE_PAIR_SUCCESS);
            }
            break;
            
        default:
            break;
    }
    
    return this->pairingStatus & MICROBIT_BLE_PAIR_COMPLETE;
}

/**
 * Periodic callback in thread context.
 * We use this here purely to safely issue a disconnect operation after a pairing operation is complete.
 */
void MicroBitBLEManager::idleCallback()
{
    if ( this->status & MICROBIT_BLE_STATUS_DISCONNECT)
    {
        if ( (system_timer_current_time() - pairingTime) >= MICROBIT_BLE_DISCONNECT_AFTER_PAIRING_DELAY)
        {
            MICROBIT_DEBUG_DMESG( "%d:MicroBitBLEManager::idleCallback", (int)system_timer_current_time());
            MICROBIT_DEBUG_DMESG( "MICROBIT_BLE_STATUS_DISCONNECT");
            ble_conn_state_for_each_connected( microbit_ble_for_each_connected_disconnect, NULL);
            this->status &= ~MICROBIT_BLE_STATUS_DISCONNECT;
        }
    }

    if ( this->status & MICROBIT_BLE_STATUS_DELETE_BOND)
    {
        MICROBIT_DEBUG_DMESG( "MicroBitBLEManager::idleCallback");
        MICROBIT_DEBUG_DMESG( "MICROBIT_BLE_STATUS_DELETE_BOND %x", m_failed_peer_id);
        ble_conn_state_for_each_connected( microbit_ble_for_each_connected_disconnect, NULL);

        if ( m_failed_peer_id != PM_PEER_ID_INVALID)
        {
            MICROBIT_BLE_ECHK( pm_peer_delete( m_failed_peer_id));
            m_failed_peer_id = PM_PEER_ID_INVALID;
        }
        else
        {
            MICROBIT_BLE_ECHK( pm_peers_delete());
        }
        this->status &= ~MICROBIT_BLE_STATUS_DELETE_BOND;
    }
    
    if ( this->status & MICROBIT_BLE_STATUS_SHUTDOWN)
    {
        //MICROBIT_DEBUG_DMESG( "MicroBitBLEManager::idleCallback");
        //MICROBIT_DEBUG_DMESG( "MICROBIT_BLE_STATUS_SHUTDOWN");
        nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_CONTINUE);
    }
}


/**
 * When called, the micro:bit will begin advertising for a predefined period,
 * MICROBIT_BLE_ADVERTISING_TIMEOUT seconds to bonded devices.
 */
void MicroBitBLEManager::advertise()
{
    MICROBIT_DEBUG_DMESG( "advertise");
    MICROBIT_BLE_ECHK( sd_ble_gap_adv_start( m_adv_handle, microbit_ble_CONN_CFG_TAG));
}


/**
* Stops any currently running BLE advertisements
*/
void MicroBitBLEManager::stopAdvertising()
{
    MICROBIT_DEBUG_DMESG( "stopAdvertising");
    MICROBIT_BLE_ECHK( sd_ble_gap_adv_stop( m_adv_handle));
}


/**
 * A member function used to restart advertising
 * */
void MicroBitBLEManager::onDisconnect()
{
    MICROBIT_DEBUG_DMESG( "onDisconnect");
        
    MicroBitEvent(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED);
    
    if ( advertiseOnDisconnect && ble_conn_state_peripheral_conn_count() == 0)
        advertise();
}


    
#if CONFIG_ENABLED(MICROBIT_BLE_EDDYSTONE_URL)
/**
  * Set the content of Eddystone URL frames
  *
  * @param url The url to broadcast
  *
  * @param calibratedPower the transmission range of the beacon (Defaults to: 0xF0 ~10m).
  *
  * @param connectable true to keep bluetooth connectable for other services, false otherwise. (Defaults to true)
  *
  * @param interval the rate at which the micro:bit will advertise url frames. (Defaults to MICROBIT_BLE_EDDYSTONE_ADV_INTERVAL)
  *
  * @note The calibratedPower value ranges from -100 to +20 to a resolution of 1. The calibrated power should be binary encoded.
  * More information can be found at https://github.com/google/eddystone/tree/master/eddystone-uid#tx-power
  */
int MicroBitBLEManager::advertiseEddystoneUrl(const char* url, int8_t calibratedPower, bool connectable, uint16_t interval)
{
    MICROBIT_DEBUG_DMESG( "advertiseEddystoneUrl");
    
    uint8_t frameData[ MicroBitEddystone::frameSizeURL];
    uint16_t frameSize;
    
    int ret = MicroBitEddystone::getInstance()->getURL( frameData, &frameSize, url, calibratedPower);

    if ( ret == MICROBIT_OK)
    {
      stopAdvertising();
      
      microbit_ble_configureAdvertising( connectable, true /*discoverable*/, false /*whitelist*/, interval, MICROBIT_BLE_ADVERTISING_TIMEOUT, frameData + 2, frameSize - 2);

      advertise();
    }

    return ret;
}

/**
  * Set the content of Eddystone URL frames, but accepts a ManagedString as a url.
  *
  * @param url The url to broadcast
  *
  * @param calibratedPower the transmission range of the beacon (Defaults to: 0xF0 ~10m).
  *
  * @param connectable true to keep bluetooth connectable for other services, false otherwise. (Defaults to true)
  *
  * @param interval the rate at which the micro:bit will advertise url frames. (Defaults to MICROBIT_BLE_EDDYSTONE_ADV_INTERVAL)
  *
  * @note The calibratedPower value ranges from -100 to +20 to a resolution of 1. The calibrated power should be binary encoded.
  * More information can be found at https://github.com/google/eddystone/tree/master/eddystone-uid#tx-power
  */
int MicroBitBLEManager::advertiseEddystoneUrl(ManagedString url, int8_t calibratedPower, bool connectable, uint16_t interval)
{
    return advertiseEddystoneUrl((char *)url.toCharArray(), calibratedPower, connectable, interval);
}
#endif

#if CONFIG_ENABLED(MICROBIT_BLE_EDDYSTONE_UID)
/**
 * Set the content of Eddystone UID frames
 *
 * @param uid_namespace: the uid namespace. Must 10 bytes long.
 *
 * @param uid_instance:  the uid instance value. Must 6 bytes long.
 *
 * @param calibratedPower the transmission range of the beacon (Defaults to: 0xF0 ~10m).
 *
 * @param connectable true to keep bluetooth connectable for other services, false otherwise. (Defaults to true)
 *
 * @param interval the rate at which the micro:bit will advertise url frames. (Defaults to MICROBIT_BLE_EDDYSTONE_ADV_INTERVAL)
 *
 * @note The calibratedPower value ranges from -100 to +20 to a resolution of 1. The calibrated power should be binary encoded.
 * More information can be found at https://github.com/google/eddystone/tree/master/eddystone-uid#tx-power
 */
int MicroBitBLEManager::advertiseEddystoneUid(const char* uid_namespace, const char* uid_instance, int8_t calibratedPower, bool connectable, uint16_t interval)
{
    MICROBIT_DEBUG_DMESG( "advertiseEddystoneUid");
    
    uint8_t frameData[ MicroBitEddystone::frameSizeUID];
    uint16_t frameSize;
    
    int ret = MicroBitEddystone::getInstance()->getUID( frameData, &frameSize, uid_namespace, uid_instance, calibratedPower);

    if ( ret == MICROBIT_OK)
    {
      stopAdvertising();
      
      microbit_ble_configureAdvertising( connectable, true /*discoverable*/, false /*whitelist*/, interval, MICROBIT_BLE_ADVERTISING_TIMEOUT, frameData + 2, frameSize - 2);

      advertise();
    }

    return ret;
}
#endif

/**
 * Enter pairing mode. This is mode is called to initiate pairing, and to enable FOTA programming
 * of the micro:bit in cases where BLE is disabled during normal operation.
 *
 * @param display An instance of MicroBitDisplay used when displaying pairing information.
 * @param authorizationButton The button to use to authorise a pairing request.
 *
 * @code
 * // initiate pairing mode
 * bleManager.pairingMode(uBit.display, uBit.buttonA);
 * @endcode
 */
void MicroBitBLEManager::pairingMode(MicroBitDisplay &display, MicroBitButton &authorisationButton)
{
    MICROBIT_DEBUG_DMESG( "pairingMode");
    
    // Do not page this fiber!
    currentFiber->flags |= DEVICE_FIBER_FLAG_DO_NOT_PAGE;

    int timeInPairingMode = 0;
    int brightness = 255;
    int fadeDirection = 0;

    currentMode = MICROBIT_MODE_PAIRING;
    
    pairingTime = 0;

    stopAdvertising();

#if CONFIG_ENABLED(MICROBIT_BLE_WHITELIST)
    // Clear the whitelist (if we have one), so that we're discoverable by all BLE devices.
    MICROBIT_BLE_ECHK( pm_whitelist_set( NULL, 0));
    MICROBIT_BLE_ECHK( pm_device_identities_list_set( NULL, 0));
#endif
    
    microbit_ble_configureAdvertising( true /*connectable*/, true /*discoverable*/, false /*whitelist*/, 200, 0);

    advertise();

    // Stop any running animations on the display
    display.stopAnimation();

    showManagementModeAnimation(display);

    // Display our name, visualised as a histogram in the display to aid identification.
    showNameHistogram(display);
    
    while (1)
    {
        pairingComplete( MICROBIT_BLE_PAIR_CHECK);

        if (pairingStatus & MICROBIT_BLE_PAIR_REQUEST)
        {
            timeInPairingMode = 0;
            MicroBitImage arrow("0,0,255,0,0\n0,255,0,0,0\n255,255,255,255,255\n0,255,0,0,0\n0,0,255,0,0\n");
            display.print(arrow, 0, 0, 0);

            if (fadeDirection == 0)
                brightness -= MICROBIT_PAIRING_FADE_SPEED;
            else
                brightness += MICROBIT_PAIRING_FADE_SPEED;

            if (brightness <= 40)
                display.clear();

            if (brightness <= 0)
                fadeDirection = 1;

            if (brightness >= 255)
                fadeDirection = 0;

            if (authorisationButton.isPressed())
            {
                pairingStatus &= ~MICROBIT_BLE_PAIR_REQUEST;
                pairingStatus |= MICROBIT_BLE_PAIR_PASSCODE;
            }
        }

        if (pairingStatus & MICROBIT_BLE_PAIR_PASSCODE)
        {
            timeInPairingMode = 0;
            display.setBrightness(255);
            for (int i = 0; i < passKey.length(); i++)
            {
                display.image.print(passKey.charAt(i), 0, 0);
                
                if ( pairingComplete( MICROBIT_BLE_PAIR_CHECK))
                    break;
                
                fiber_sleep(800);
                display.clear();

                if ( pairingComplete( MICROBIT_BLE_PAIR_CHECK))
                    break;
                
                fiber_sleep(200);
            }

            if ( !pairingComplete( MICROBIT_BLE_PAIR_CHECK))
                fiber_sleep(1000);
        }

        if (pairingStatus & MICROBIT_BLE_PAIR_COMPLETE)
        {
            if (pairingStatus & MICROBIT_BLE_PAIR_SUCCESSFUL)
            {
                MicroBitImage tick("0,0,0,0,0\n0,0,0,0,255\n0,0,0,255,0\n255,0,255,0,0\n0,255,0,0,0\n");
                display.print(tick, 0, 0, 0);
                fiber_sleep(15000);
                timeInPairingMode = MICROBIT_BLE_PAIRING_TIMEOUT * 30;

                /*
                 * Disabled, as the API to return the number of active bonds is not reliable at present...
                 *
                display.clear();
                ManagedString c(getBondCount());
                ManagedString c2("/");
                ManagedString c3(MICROBIT_BLE_MAXIMUM_BONDS);
                ManagedString c4("USED");

                display.scroll(c+c2+c3+c4);
                *
                *
                */
            }
            else
            {
                MicroBitImage cross("255,0,0,0,255\n0,255,0,255,0\n0,0,255,0,0\n0,255,0,255,0\n255,0,0,0,255\n");
                display.print(cross, 0, 0, 0);
            }
        }

        fiber_sleep(100);
        timeInPairingMode++;

        if (timeInPairingMode >= MICROBIT_BLE_PAIRING_TIMEOUT * 30)
        {
            MICROBIT_DEBUG_DMESGF( "Pairing mode reset");
            microbit_reset();
        }
    }
}

/**
 * Displays the management mode animation on the provided MicroBitDisplay instance.
 *
 * @param display The Display instance used for displaying the animation.
 */
void MicroBitBLEManager::showManagementModeAnimation(MicroBitDisplay &display)
{
    // Animation for display object
    // https://makecode.microbit.org/93264-81126-90471-58367

    const uint8_t mgmt_animation[] __attribute__ ((aligned (4))) =
    {
         0xff, 0xff, 20, 0, 5, 0,
         255,255,255,255,255,   255,255,255,255,255,   255,255,  0,255,255,   255,  0,  0,  0,255,
         255,255,255,255,255,   255,255,  0,255,255,   255,  0,  0,  0,255,     0,  0,  0,  0,  0,
         255,255,  0,255,255,   255,  0,  0,  0,255,     0,  0,  0,  0,  0,     0,  0,  0,  0,  0,
         255,255,255,255,255,   255,255,  0,255,255,   255,  0,  0,  0,255,     0,  0,  0,  0,  0,
         255,255,255,255,255,   255,255,255,255,255,   255,255,  0,255,255,   255,  0,  0,  0,255
    };

    MicroBitImage mgmt((ImageData*)mgmt_animation);
    display.animate(mgmt,100,5);

    const uint8_t bt_icon_raw[] =
    {
          0,  0,255,255,  0,
        255,  0,255,  0,255,
          0,255,255,255,  0,
        255,  0,255,  0,255,
          0,  0,255,255,  0
    };

    MicroBitImage bt_icon(5,5,bt_icon_raw);
    display.print(bt_icon,0,0,0,0);

    for(int i=0; i < 255; i = i + 5){
        display.setBrightness(i);
        fiber_sleep(5);
    }
    fiber_sleep(1000);

}


// visual ID code constants
#define MICROBIT_DFU_HISTOGRAM_WIDTH        5
#define MICROBIT_DFU_HISTOGRAM_HEIGHT       5


/**
 * Displays the device's ID code as a histogram on the provided MicroBitDisplay instance.
 *
 * @param display The display instance used for displaying the histogram.
 */
void MicroBitBLEManager::showNameHistogram(MicroBitDisplay &display)
{
    uint32_t n = NRF_FICR->DEVICEID[1];
    int ld = 1;
    int d = MICROBIT_DFU_HISTOGRAM_HEIGHT;
    int h;

    display.clear();
    for (int i = 0; i < MICROBIT_DFU_HISTOGRAM_WIDTH; i++)
    {
        h = (n % d) / ld;

        n -= h;
        d *= MICROBIT_DFU_HISTOGRAM_HEIGHT;
        ld *= MICROBIT_DFU_HISTOGRAM_HEIGHT;

        for (int j = 0; j < h + 1; j++)
            display.image.setPixelValue(MICROBIT_DFU_HISTOGRAM_WIDTH - i - 1, MICROBIT_DFU_HISTOGRAM_HEIGHT - j - 1, 255);
    }
}

/**
 * Restarts into BLE Mode
 *
 */
void MicroBitBLEManager::restartInBLEMode()
{
   MICROBIT_DEBUG_DMESG( "restartInBLEMode");
   
    if ( storage)
    {
       KeyValuePair* RebootMode = storage->get("RebootMode");
       if ( RebootMode == NULL)
       {
         uint8_t RebootModeValue = MICROBIT_MODE_PAIRING;
         storage->put("RebootMode", &RebootModeValue, sizeof(RebootMode));
         delete RebootMode;
       }
    }
    microbit_reset();
 }

/**
 * Get BLE mode. Returns the current mode: application, pairing mode
 */
uint8_t MicroBitBLEManager::getCurrentMode()
{
  MICROBIT_DEBUG_DMESG( "getCurrentMode %d", (int) currentMode);
  return currentMode;
}



/**
 * Prepare for shutdown or disabling softdevice by stopping advertising and disconnecting
 *
 * Return true if ready for shutdown
 */
bool MicroBitBLEManager::prepareForShutdown()
{
    bool shutdownOK = true;
        
    MicroBitBLEManager::manager->stopAdvertising();
    MicroBitBLEManager::manager->setAdvertiseOnDisconnect( false);

    if ( ble_conn_state_conn_count()) // TODO: anything else we need to wait for?
    {
        shutdownOK = false;
        ble_conn_state_for_each_connected( microbit_ble_for_each_connected_disconnect, NULL);
    }
    
    if ( shutdownOK)
    {
        if ( !shutdownTime)
            shutdownTime = system_timer_current_time();

        if ( (system_timer_current_time() - shutdownTime) < MICROBIT_BLE_SHUTDOWN_DELAY)
            shutdownOK = false;
    }
    
    return shutdownOK;
}


/**
 * Function to configure advertising
 *
 * @param connectable Choose connectable advertising events.
 * @param discoverable Choose LE General Discoverable Mode.
 * @param whitelist Filter scan and connect requests with whitelist.
 * @param interval_ms Advertising interval in milliseconds.
 * @param timeout_seconds Advertising timeout in seconds
 */
static void microbit_ble_configureAdvertising( bool connectable, bool discoverable, bool whitelist,
                                               uint16_t interval_ms, int timeout_seconds,
                                               ble_advdata_t *p_advdata)
{
    MICROBIT_INFO_DMESG( "configureAdvertising connectable %d, discoverable %d", (int) connectable, (int) discoverable);
    MICROBIT_INFO_DMESG( "whitelist %d, interval_ms %d, timeout_seconds %d", (int) whitelist, (int) interval_ms, (int) timeout_seconds);

    ble_gap_adv_params_t    gap_adv_params;
    memset( &gap_adv_params, 0, sizeof( gap_adv_params));
    gap_adv_params.properties.type  = connectable
                                    ? BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED
                                    : BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED;
    gap_adv_params.interval         = ( 1000 * interval_ms) / 625;  // 625 us units
    if ( gap_adv_params.interval < BLE_GAP_ADV_INTERVAL_MIN) gap_adv_params.interval = BLE_GAP_ADV_INTERVAL_MIN;
    if ( gap_adv_params.interval > BLE_GAP_ADV_INTERVAL_MAX) gap_adv_params.interval = BLE_GAP_ADV_INTERVAL_MAX;
    gap_adv_params.duration         = timeout_seconds * 100;              //10 ms units
    gap_adv_params.filter_policy    = whitelist
                                    ? BLE_GAP_ADV_FP_FILTER_BOTH
                                    : BLE_GAP_ADV_FP_ANY;
    gap_adv_params.primary_phy      = BLE_GAP_PHY_1MBPS;
                
    ble_gap_adv_data_t  gap_adv_data;
    memset( &gap_adv_data, 0, sizeof( gap_adv_data));
    gap_adv_data.adv_data.p_data    = m_enc_advdata;
    gap_adv_data.adv_data.len       = BLE_GAP_ADV_SET_DATA_SIZE_MAX;
    MICROBIT_BLE_ECHK( ble_advdata_encode( p_advdata, gap_adv_data.adv_data.p_data, &gap_adv_data.adv_data.len));
    NRF_LOG_HEXDUMP_INFO( gap_adv_data.adv_data.p_data, gap_adv_data.adv_data.len);
    MICROBIT_BLE_ECHK( sd_ble_gap_adv_set_configure( &m_adv_handle, &gap_adv_data, &gap_adv_params));
}


static void microbit_ble_configureAdvertising( bool connectable, bool discoverable, bool whitelist,
                                               uint16_t interval_ms, int timeout_seconds)
{
    ble_advdata_t advdata;
    memset( &advdata, 0, sizeof( advdata));
    advdata.name_type = BLE_ADVDATA_FULL_NAME;
    advdata.flags     = !whitelist && discoverable
                      ? BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED | BLE_GAP_ADV_FLAG_LE_GENERAL_DISC_MODE
                      : BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
            
    microbit_ble_configureAdvertising( connectable, discoverable, whitelist, interval_ms, timeout_seconds, &advdata);
}


#if CONFIG_ENABLED(MICROBIT_BLE_EDDYSTONE_URL) || CONFIG_ENABLED(MICROBIT_BLE_EDDYSTONE_UID)

static void microbit_ble_configureAdvertising( bool connectable, bool discoverable, bool whitelist,
                                               uint16_t interval_ms, int timeout_seconds,
                                               uint8_t *frameData, uint16_t frameSize)
{
    ble_uuid_t  esUuid = { 0xFEAA, BLE_UUID_TYPE_BLE};
    
    ble_advdata_service_data_t service_data;
    memset( &service_data, 0, sizeof( service_data));
    service_data.service_uuid = esUuid.uuid;
    service_data.data.size    = frameSize;
    service_data.data.p_data  = frameSize ? frameData : NULL;

    ble_advdata_t advdata;
    memset( &advdata, 0, sizeof( advdata));
    advdata.name_type               = BLE_ADVDATA_NO_NAME;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.uuids_complete.uuid_cnt = 1;
    advdata.uuids_complete.p_uuids  = &esUuid;

    if ( service_data.data.size)
    {
        advdata.service_data_count   = 1;
        advdata.p_service_data_array = &service_data;
    }

    microbit_ble_configureAdvertising( connectable, discoverable, whitelist, interval_ms, timeout_seconds, &advdata);
}

#endif


/**
  * Callback when a BLE connection is established.
  */
static void bleConnectionCallback( microbit_gaphandle_t handle)
{
    MICROBIT_DEBUG_DMESG( "bleConnectionCallback %d", (int) handle);
    
    if ( handle != BLE_CONN_HANDLE_INVALID)
        sd_ble_gap_tx_power_set( BLE_GAP_TX_POWER_ROLE_CONN, handle, MICROBIT_BLE_POWER_LEVEL[ m_power]);
    
    MicroBitEvent(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED);
}


static void passkeyDisplayCallback( microbit_gaphandle_t handle, ManagedString passKey)
{
    MICROBIT_DEBUG_DMESG( "passkeyDisplayCallback %d", (int) handle);
    
    (void)handle; /* -Wunused-param */

    if (MicroBitBLEManager::manager)
        MicroBitBLEManager::manager->pairingRequested(passKey);
}

// NOTE: Event handlers may be called from SD_EVT_IRQHandler
// TODO: Check what they call. Consider other dispatch modes

/**
 * Callback for handling BLE events.
 */
static void microbit_ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    //MICROBIT_DEBUG_DMESG( "%d:microbit_ble_evt_handler %x %d", (int)system_timer_current_time(), (unsigned int) p_ble_evt->header.evt_id);
    
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
        {
            if ( MicroBitBLEManager::manager)
                MicroBitBLEManager::manager->onDisconnect();
            break;
        }
        case BLE_GAP_EVT_CONNECTED:
        {
            MICROBIT_DEBUG_DMESG( "BLE_GAP_EVT_CONNECTED %d", ble_conn_state_conn_count());
            bleConnectionCallback( p_ble_evt->evt.gap_evt.conn_handle);
            break;
        }
        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            ble_gap_phys_t const phys =
            {
                .tx_phys = BLE_GAP_PHY_AUTO,
                .rx_phys = BLE_GAP_PHY_AUTO,
            };
            MICROBIT_BLE_ECHK( sd_ble_gap_phy_update( p_ble_evt->evt.gap_evt.conn_handle, &phys));
            break;
        }
        case BLE_GAP_EVT_PASSKEY_DISPLAY:
        {
            ManagedString passKey( (const char *)p_ble_evt->evt.gap_evt.params.passkey_display.passkey, BLE_GAP_PASSKEY_LEN);
            passkeyDisplayCallback( p_ble_evt->evt.gap_evt.conn_handle, passKey);
            break;
        }
        case BLE_GATTC_EVT_TIMEOUT:
            MICROBIT_BLE_ECHK( sd_ble_gap_disconnect( p_ble_evt->evt.gattc_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION));
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            MICROBIT_BLE_ECHK( sd_ble_gap_disconnect( p_ble_evt->evt.gatts_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION));
            break;

        case BLE_GAP_EVT_AUTH_STATUS:
            MICROBIT_DEBUG_DMESG( "BLE_GAP_EVT_AUTH_STATUS %d", (int) (p_ble_evt->evt.gap_evt.params.auth_status.auth_status == BLE_GAP_SEC_STATUS_SUCCESS));
            if ( p_ble_evt->evt.gap_evt.params.auth_status.auth_status == BLE_GAP_SEC_STATUS_SUCCESS)
            {
              if ( MicroBitBLEManager::manager)
                  MicroBitBLEManager::manager->pairingComplete( MICROBIT_BLE_PAIR_AUTH);
            }
            break;
        
        default:
            break;
    }
}


/**
 * Callback for handling Peer Manager events.
 *
 * @param p_evt Peer Manager event.
 */
static void microbit_ble_pm_evt_handler(pm_evt_t const * p_evt)
{
    //MICROBIT_DEBUG_DMESG( "%d:microbit_ble_pm_evt_handler %d", (int)system_timer_current_time(), (int) p_evt->evt_id);

    bool bondingFailed = false;
    
    pm_handler_on_pm_evt( p_evt);
    pm_handler_flash_clean( p_evt);

    switch ( p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
            //MICROBIT_DEBUG_DMESG( "PM_EVT_BONDED_PEER_CONNECTED");
            break;

        case PM_EVT_CONN_SEC_START:
            //MICROBIT_DEBUG_DMESG( "PM_EVT_CONN_SEC_START");
            break;

        case PM_EVT_CONN_SEC_CONFIG_REQ:
        {
            //MICROBIT_DEBUG_DMESG( "PM_EVT_CONN_SEC_CONFIG_REQ");
            pm_conn_sec_config_t conn_sec_config = { .allow_repairing = true };
            pm_conn_sec_config_reply( p_evt->conn_handle, &conn_sec_config);
            break;
        }
        case PM_EVT_CONN_SEC_PARAMS_REQ:
            //MICROBIT_DEBUG_DMESG( "PM_EVT_CONN_SEC_PARAMS_REQ");
            // Optionally call pm_conn_sec_params_reply
            // By default, params passed to pm_sec_params_set are used
            break;

        case PM_EVT_CONN_SEC_SUCCEEDED:
            //MICROBIT_DEBUG_DMESG( "PM_EVT_CONN_SEC_SUCCEEDED");
            if ( MicroBitBLEManager::manager)
                MicroBitBLEManager::manager->pairingComplete( MICROBIT_BLE_PAIR_UPDATE);
            break;
        
        case PM_EVT_CONN_SEC_FAILED:
            MICROBIT_DEBUG_DMESG( "PM_EVT_CONN_SEC_FAILED");
            bondingFailed = true;
            break;

        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
            MICROBIT_DEBUG_DMESG( "PM_EVT_PEER_DATA_UPDATE_SUCCEEDED");
            if ( MicroBitBLEManager::manager)
                MicroBitBLEManager::manager->pairingComplete( MICROBIT_BLE_PAIR_UPDATE);
            break;
        
        case PM_EVT_PEER_DATA_UPDATE_FAILED:
            MICROBIT_DEBUG_DMESG( "PM_EVT_PEER_DATA_UPDATE_FAILED");
            bondingFailed = true;
            break;

        default:
            break;
    }
    
    if ( bondingFailed)
    {
        pm_peer_id_t peer_id;
        if ( MICROBIT_BLE_ECHK( pm_peer_id_get( p_evt->conn_handle, &peer_id)) == NRF_SUCCESS)
            m_failed_peer_id = peer_id;
        if ( MicroBitBLEManager::manager)
            MicroBitBLEManager::manager->pairingComplete( MICROBIT_BLE_PAIR_FAILURE);
    }
}


static void const_ascii_to_utf8(ble_srv_utf8_str_t * p_utf8, const char * p_ascii)
{
    // ble_srv_ascii_to_utf8() doesn't check for p_ascii == NULL;
    // cast away const or allocate temporary buffer?
    p_utf8->p_str  = (uint8_t *)p_ascii;
    p_utf8->length = p_ascii ? (uint16_t)strlen(p_ascii) : 0;
}


static void microbit_ble_for_each_connected_disconnect( uint16_t conn_handle, void * /*p_context*/)
{
    MICROBIT_DEBUG_DMESGF( "microbit_ble_for_each_connected_disconnect %d", (int) conn_handle);
    MICROBIT_BLE_ECHK( sd_ble_gap_disconnect( conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION));
}


static void microbit_ble_for_each_connected_tx_power_set( uint16_t conn_handle, void *p_context)
{
    int power = *( int *) p_context;
    MICROBIT_DEBUG_DMESGF( "microbit_ble_for_each_connected_tx_power_set conn_handle %d power %d", (int) conn_handle, (int) power);
    MICROBIT_BLE_ECHK( sd_ble_gap_tx_power_set( BLE_GAP_TX_POWER_ROLE_CONN, conn_handle, MICROBIT_BLE_POWER_LEVEL[ power]));
}


/**
 * Callback for handling shutdown preparation.
 *
 * @param event Power manager event.
 *
 * @return True if shutdown is allowed by this power manager handler, otherwise false.
 */
static bool microbit_ble_shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    bool shutdownOK = true; // Allow the shutdown, unless other handlers object
    
    MICROBIT_DEBUG_DMESG( "%d:microbit_ble_shutdown_handler %d", (int)system_timer_current_time(), (int) event);
    
    switch (event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_RESET:
        case NRF_PWR_MGMT_EVT_PREPARE_DFU:
            if ( MicroBitBLEManager::manager)
            {
                // Use idleCallback rather than a timer to restart the shutdown
                MicroBitBLEManager::manager->status |= MICROBIT_BLE_STATUS_SHUTDOWN;
                fiber_add_idle_component( MicroBitBLEManager::manager);
                
                shutdownOK = MicroBitBLEManager::manager->prepareForShutdown();
            }

// TODO: sd_softdevice_disable hangs
//            if ( shutdownOK)
//            {
//                // Allow the NRF_SDH_REQUEST_OBSERVERs (e.g. fstorage) to delay the shutdown
//                MICROBIT_BLE_ECHK( nrf_sdh_disable_request());
//                shutdownOK = !nrf_sdh_is_enabled();
//            }
            break;

        case NRF_PWR_MGMT_EVT_PREPARE_WAKEUP:
        case NRF_PWR_MGMT_EVT_PREPARE_SYSOFF:
            break;
            
        default:
            break;
    }

    MICROBIT_DEBUG_DMESG( "%d:microbit_ble_shutdown_handler shutdownOK = %d", (int)system_timer_current_time(), (int) shutdownOK);
    return shutdownOK;
}

//lint -esym(528, m_ble_dfu_shutdown_handler)
NRF_PWR_MGMT_HANDLER_REGISTER( microbit_ble_shutdown_handler, 0);


/**
* Callback for handling softdevice state change.
*
* @param req Softdevice manager event.
*
* @return True to allow the change, otherwise false.
*/
static bool microbit_ble_sdh_req_handler(nrf_sdh_req_evt_t req, void * /*p_context*/)
{
    bool shutdownOK = true; // Allow the SoftDevice state change, unless other handlers object
    
    switch ( req)
    {
        case NRF_SDH_EVT_ENABLE_REQUEST:
            break;
            
        case NRF_SDH_EVT_DISABLE_REQUEST:
            if ( MicroBitBLEManager::manager)
            {
                shutdownOK = MicroBitBLEManager::manager->prepareForShutdown();
            }
            else
            {
                MICROBIT_BLE_ECHK( sd_ble_gap_adv_stop( m_adv_handle));
                if ( ble_conn_state_conn_count()) // TODO: anything else we need to wait for?
                {
                    shutdownOK = false;
                    ble_conn_state_for_each_connected( microbit_ble_for_each_connected_disconnect, NULL);
                }
            }
            break;
    }
    
    MICROBIT_DEBUG_DMESGF( "%d:microbit_ble_sdh_req_handler shutdownOK = %d", (int)system_timer_current_time(), (int) shutdownOK);
    return shutdownOK;
}

NRF_SDH_REQUEST_OBSERVER(microbit_dfu_sdh_req_obs, 0) =
{
    .handler = microbit_ble_sdh_req_handler,
    .p_context = NULL
};


/**
 * Nordic DFU Service support
 */
#if CONFIG_ENABLED(MICROBIT_BLE_DFU_SERVICE)

static void microbit_dfu_evt_handler( ble_dfu_buttonless_evt_type_t event);

/**
  * Initialise the Device Firmware Update service.
  */
static void microbit_dfu_init(void)
{
    MICROBIT_DEBUG_DMESG( "microbit_dfu_init");
    ble_dfu_buttonless_init_t dfus_init = {0};
    dfus_init.evt_handler = microbit_dfu_evt_handler;
    MICROBIT_BLE_ECHK( ble_dfu_buttonless_init(&dfus_init));
}

/**
* Callback for handling Buttonless Secure DFU service events.
*
* @param p_evt Buttonless Secure DFU service event.
*/
static void microbit_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event)
{
    MICROBIT_DEBUG_DMESG( "microbit_dfu_evt_handler %d", (int) event);

    switch (event)
    {
        case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE:
        {
            MICROBIT_DEBUG_DMESG( "%d:BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE", (int)system_timer_current_time());
            if ( MicroBitBLEManager::manager)
                MicroBitBLEManager::manager->prepareForShutdown();
            break;
        }

        case BLE_DFU_EVT_BOOTLOADER_ENTER:
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
            //microbit_panic( DEVICE_HARDWARE_CONFIGURATION_ERROR);
            break;

        case BLE_DFU_EVT_RESPONSE_SEND_ERROR:
            //microbit_panic( DEVICE_HARDWARE_CONFIGURATION_ERROR);
            break;

        default:
            break;
    }
}

#endif // CONFIG_ENABLED(MICROBIT_BLE_DFU_SERVICE)

#endif // CONFIG_ENABLED(DEVICE_BLE)
