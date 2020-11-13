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

#ifndef MICROBIT_BLE_MANAGER_H
#define MICROBIT_BLE_MANAGER_H

#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#ifndef SOFTDEVICE_PRESENT
#error "Please define SOFTDEVICE_PRESENT as 1"
#endif

#include "MicroBitBLETypes.h"
#include "MicroBitStorage.h"
#include "MicroBitDisplay.h"
#include "ExternalEvents.h"
#include "MicroBitButton.h"

#define MICROBIT_BLE_PAIR_REQUEST 0x01
#define MICROBIT_BLE_PAIR_COMPLETE 0x02
#define MICROBIT_BLE_PAIR_PASSCODE 0x04
#define MICROBIT_BLE_PAIR_SUCCESSFUL 0x08

#define MICROBIT_BLE_PAIRING_TIMEOUT 90
#define MICROBIT_BLE_POWER_LEVELS 8
#define MICROBIT_BLE_MAXIMUM_BONDS 4
#define MICROBIT_BLE_ENABLE_BONDING true

#define MICROBIT_BLE_EDDYSTONE_ADV_INTERVAL     400
#define MICROBIT_BLE_EDDYSTONE_DEFAULT_POWER    0xF0

// CodalComponent status flags
#define MICROBIT_BLE_STATUS_DISCONNECT          0x04
#define MICROBIT_BLE_STATUS_SHUTDOWN            0x08

// micro:bit Modes
// The micro:bit may be in different states: running a user's application or into BLE pairing mode
// These modes can be representeded using these #defines
#define MICROBIT_MODE_PAIRING                   0
#define MICROBIT_MODE_APPLICATION               1

class MicroBitBLEManager;
typedef MicroBitBLEManager BLEDevice;

/**
  * Class definition for the MicroBitBLEManager.
  *
  */
class MicroBitBLEManager : public CodalComponent
{
    public:
    static MicroBitBLEManager *manager;

    //an instance of MicroBitStorage used to store sysAttrs from softdevice
    MicroBitStorage *storage;

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
    MicroBitBLEManager(MicroBitStorage &_storage);

    /**
     * Constructor.
     *
     * Configure and manage the micro:bit's Bluetooth Low Energy (BLE) stack.
     *
     * @note The BLE stack *cannot*  be brought up in a static context (the software simply hangs or corrupts itself).
     * Hence, the init() member function should be used to initialise the BLE stack.
     */
    MicroBitBLEManager();
    
    /**
     * getInstance
     *
     * Allows other objects to easily obtain a pointer to the single instance of this object. By rights the constructor should be made
     * private to properly implement the singleton pattern.
     *
     */
    static MicroBitBLEManager *getInstance();

    /**
      * Post constructor initialisation method as the BLE stack cannot be brought
      * up in a static context.
      *
      * @param deviceName The name used when advertising
      * @param serialNumber The serial number exposed by the device information service
      * @param messageBus An instance of an EventModel, used during pairing.
      * @param keyValuestorage An instance of a MicroBitStorage key/value pair storage class to use to hold bonding metadata
      * @param enableBonding If true, the security manager enabled bonding.
      *
      * @code
      * bleManager.init(uBit.getName(), uBit.getSerial(), uBit.messageBus, true);
      * @endcode
      */
    void init(ManagedString deviceName, ManagedString serialNumber, EventModel &messageBus, MicroBitStorage &keyValuestorage, bool enableBonding);
    
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
    int setTransmitPower(int power);

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
    void pairingMode(MicroBitDisplay &display, Button &authorisationButton);

    /**
     * When called, the micro:bit will begin advertising for a predefined period,
     * MICROBIT_BLE_ADVERTISING_TIMEOUT seconds to bonded devices.
     */
    void advertise();

    /**
     * Determines the number of devices currently bonded with this micro:bit.
     * @return The number of active bonds.
     */
    int getBondCount();

    /**
	 * A request to pair has been received from a BLE device.
     * If we're in pairing mode, display the passkey to the user.
     * Also, purge the bonding table if it has reached capacity.
     *
     * @note for internal use only.
	 */
    void pairingRequested(ManagedString passKey);

    /**
	 * Record pairing progress
     *
     * @note for internal use only.
	 */
    bool pairingComplete( int event);

    /**
     * Periodic callback in thread context.
     * We use this here purely to safely issue a disconnect operation after a pairing operation is complete.
	 */
    void idleCallback();

    /**
	* Stops any currently running BLE advertisements
	*/
    void stopAdvertising();
    
    /**
     * A member function called on disconnection
     * */
    void onDisconnect();

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
    int advertiseEddystoneUrl(const char *url, int8_t calibratedPower = MICROBIT_BLE_EDDYSTONE_DEFAULT_POWER, bool connectable = true, uint16_t interval = MICROBIT_BLE_EDDYSTONE_ADV_INTERVAL);

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
    int advertiseEddystoneUrl(ManagedString url, int8_t calibratedPower = MICROBIT_BLE_EDDYSTONE_DEFAULT_POWER, bool connectable = true, uint16_t interval = MICROBIT_BLE_EDDYSTONE_ADV_INTERVAL);
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
    int advertiseEddystoneUid(const char* uid_namespace, const char* uid_instance, int8_t calibratedPower = MICROBIT_BLE_EDDYSTONE_DEFAULT_POWER, bool connectable = true, uint16_t interval = MICROBIT_BLE_EDDYSTONE_ADV_INTERVAL);
#endif

  /**
   * Restarts in BLE Mode
   *
   */
   void restartInBLEMode();

   /**
    * Get current BLE mode; application, pairing
    * #define MICROBIT_MODE_PAIRING     0x00
    * #define MICROBIT_MODE_APPLICATION 0x01
    */
    uint8_t getCurrentMode();

    /**
     * Control whether advertising will be restarted on disconnection
     */
    void setAdvertiseOnDisconnect( bool f) { advertiseOnDisconnect = f; }
    
    /**
     * Prepare for shutdown or disabling softdevice by stopping advertising and disconnecting
     *
     * Return true if ready for shutdown
     */
    bool prepareForShutdown();

    /**
    * Ensure service changed indication pending for all peers
    */
    void servicesChanged();
      
  private:
    /**
    * Displays the device's ID code as a histogram on the provided MicroBitDisplay instance.
    *
    * @param display The display instance used for displaying the histogram.
    */
    void showNameHistogram(MicroBitDisplay &display);

    /**
    * Displays pairing mode animation
    *
    * @param display The display instance used for displaying the histogram.
    */
    void showManagementModeAnimation(MicroBitDisplay &display);

    int pairingStatus;
    ManagedString passKey;
    ManagedString gapName;
    
    unsigned long pairingTime;
    unsigned long shutdownTime;

    /*
     * Default to Application Mode
     * This variable will be set to MICROBIT_MODE_PAIRING if pairingMode() is executed.
     */
    uint8_t currentMode = MICROBIT_MODE_APPLICATION;

    bool advertiseOnDisconnect = true;
};

#endif

#endif
