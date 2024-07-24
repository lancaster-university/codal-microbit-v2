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

/**
  * Class definition for the custom MicroBit UART Service.
  * Provides a BLE service that acts as a UART port, enabling the reception and transmission
  * of an arbitrary number of bytes.
  */

#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "ExternalEvents.h"
#include "MicroBitUARTService.h"
#include "MicroBitFiber.h"
#include "ErrorNo.h"
#include "NotifyEvents.h"

using namespace codal;

const uint8_t  MicroBitUARTService::base_uuid[ 16] =
{ 0x6e, 0x40, 0x00, 0x00, 0xb5, 0xa3, 0xf3, 0x93, 0xe0, 0xa9, 0xe5, 0x0e, 0x24, 0xdc, 0xca, 0x9e };

const uint16_t MicroBitUARTService::serviceUUID               = 0x0001;

// The TX/RX characteristics for the Nordic UART services (NUS) have two common
// implementations. The codal code is the same, the only difference is how the characterists
// and their services are advertised.
//
// Enable the MICROBIT_BLE_NORDIC_STYLE_UART setting to build microbit projects for applications that
// connect to generic NUS devices
// https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/include/bluetooth/services/nus.html
//
// Leave the setting undefined (or 0) for standard dal or codal microbit builds

#if CONFIG_ENABLED(MICROBIT_BLE_NORDIC_STYLE_UART)
const uint16_t MicroBitUARTService::charUUID[ mbbs_cIdxCOUNT] = { 0x0003, 0x0002 };
#else
const uint16_t MicroBitUARTService::charUUID[ mbbs_cIdxCOUNT] = { 0x0002, 0x0003 };
#endif 

#define MICROBIT_UART_S_ATTRSIZE            20

/**
 * Constructor for the UARTService.
 * @param _ble an instance of BLEDevice
 * @param rxBufferSize the size of the rxBuffer
 * @param txBufferSize the size of the txBuffer
 *
 * @note defaults to 20
 */
MicroBitUARTService::MicroBitUARTService(BLEDevice &_ble, uint8_t rxBufferSize, uint8_t txBufferSize)
{
    // Initialise our characteristic values.
    txBufferHead = 0;
    txBufferTail = 0;
    
    rxBufferSize += 1;
    txBufferSize += 1;

    // Allocate memory for rxBuffer, rx characteristic, txBuffer, tx characteristic
    int size = rxBufferSize + txBufferSize + 2 * MICROBIT_UART_S_ATTRSIZE;
    rxBuffer = (uint8_t *)malloc(size);
    txBuffer = rxBuffer + rxBufferSize + MICROBIT_UART_S_ATTRSIZE;
    
    memclr( rxBuffer, size);

    rxBufferHead = 0;
    rxBufferTail = 0;
    this->rxBufferSize = rxBufferSize;

    txBufferHead = 0;
    txBufferTail = 0;
    this->txBufferSize = txBufferSize;

    txValueSize  = 0;

    waitingForEmpty = false;

    // Register the base UUID and create the service.
    RegisterBaseUUID( base_uuid);
    CreateService( serviceUUID);

    // Create the data structures that represent each of our characteristics in Soft Device.
    CreateCharacteristic( mbbs_cIdxRX, charUUID[ mbbs_cIdxRX],
                          rxBuffer + rxBufferSize,
                          0, MICROBIT_UART_S_ATTRSIZE,
                          microbit_propWRITE | microbit_propWRITE_WITHOUT);

    CreateCharacteristic( mbbs_cIdxTX, charUUID[ mbbs_cIdxTX],
                          txBuffer + txBufferSize,
                          0, MICROBIT_UART_S_ATTRSIZE,
                          microbit_propINDICATE);
}


/**
  * Invoked when BLE disconnects.
  */
void MicroBitUARTService::onDisconnect( const microbit_ble_evt_t *p_ble_evt)
{
    txValueSize = txBufferTail = txBufferHead = 0;

    if ( waitingForEmpty)
        MicroBitEvent(MICROBIT_ID_NOTIFY, MICROBIT_UART_S_EVT_TX_EMPTY);
}


/**
  * A callback function for whenever a Bluetooth device consumes our TX Buffer
  */
void MicroBitUARTService::onConfirmation( const microbit_ble_evt_hvc_t *params)
{
    if ( params->handle == valueHandle( mbbs_cIdxTX))
    {
        txBufferTail = ( (int)txBufferTail + txValueSize) % txBufferSize;
        txValueSize = 0;
        bool async = !waitingForEmpty;
        MicroBitEvent(MICROBIT_ID_NOTIFY, MICROBIT_UART_S_EVT_TX_EMPTY);
        if ( async)
            sendNext();
    }
}


/**
  * A callback function for whenever a Bluetooth device writes to our RX characteristic.
  */
void MicroBitUARTService::onDataWritten(const microbit_ble_evt_write_t *params)
{
    if (params->handle == valueHandle( mbbs_cIdxRX))
    {
        uint16_t bytesWritten = params->len;

        for(int byteIterator = 0; byteIterator <  bytesWritten; byteIterator++)
        {
            int newHead = (rxBufferHead + 1) % rxBufferSize;

            if(newHead != rxBufferTail)
            {
                char c = params->data[byteIterator];

                int delimeterOffset = 0;
                int delimLength = this->delimeters.length();

                //iterate through our delimeters (if any) to see if there is a match
                while(delimeterOffset < delimLength)
                {
                    //fire an event if there is to block any waiting fibers
                    if(this->delimeters.charAt(delimeterOffset) == c)
                        MicroBitEvent(MICROBIT_ID_BLE_UART, MICROBIT_UART_S_EVT_DELIM_MATCH);

                    delimeterOffset++;
                }

                rxBuffer[rxBufferHead] = c;

                rxBufferHead = newHead;

                if(rxBufferHead == rxBuffHeadMatch)
                {
                    rxBuffHeadMatch = -1;
                    MicroBitEvent(MICROBIT_ID_BLE_UART, MICROBIT_UART_S_EVT_HEAD_MATCH);
                }
            }
            else
                MicroBitEvent(MICROBIT_ID_BLE_UART, MICROBIT_UART_S_EVT_RX_FULL);
        }
    }
}

/**
  * An internal method that copies values from a circular buffer to a linear buffer.
  *
  * @param circularBuff a pointer to the source circular buffer
  * @param circularBuffSize the size of the circular buffer
  * @param linearBuff a pointer to the destination linear buffer
  * @param tailPosition the tail position in the circular buffer you want to copy from
  * @param headPosition the head position in the circular buffer you want to copy to
  *
  * @note this method assumes that the linear buffer has the appropriate amount of
  *       memory to contain the copy operation
  */
void MicroBitUARTService::circularCopy(uint8_t *circularBuff, uint8_t circularBuffSize, uint8_t *linearBuff, uint16_t tailPosition, uint16_t headPosition)
{
    int toBuffIndex = 0;

    while(tailPosition != headPosition)
    {
        linearBuff[toBuffIndex++] = circularBuff[tailPosition];

        tailPosition = (tailPosition + 1) % circularBuffSize;
    }
}

/**
  * Retreives a single character from our RxBuffer.
  *
  * @param mode the selected mode, one of: ASYNC, SYNC_SPINWAIT, SYNC_SLEEP. Each mode
  *        gives a different behaviour:
  *
  *            ASYNC - Will attempt to read a single character, and return immediately
  *
  *            SYNC_SPINWAIT - will return MICROBIT_INVALID_PARAMETER
  *
  *            SYNC_SLEEP - Will configure the event and block the current fiber until the
  *                         event is received.
  *
  * @return MICROBIT_INVALID_PARAMETER if the mode given is SYNC_SPINWAIT, a character or MICROBIT_NO_DATA
  */
int MicroBitUARTService::getc(MicroBitSerialMode mode)
{
    if(mode == SYNC_SPINWAIT)
        return MICROBIT_INVALID_PARAMETER;

    if(mode == ASYNC)
    {
        if(!isReadable())
            return MICROBIT_NO_DATA;
    }

    if(mode == SYNC_SLEEP)
    {
        if(!isReadable())
            eventAfter(1, mode);
    }

    char c = rxBuffer[rxBufferTail];

    rxBufferTail = (rxBufferTail + 1) % rxBufferSize;

    return c;
}

/**
  * places a single character into our transmission buffer,
  *
  * @param c the character to transmit
  *
  * @param mode the selected mode, one of: ASYNC, SYNC_SPINWAIT, SYNC_SLEEP. Each mode
  *        gives a different behaviour:
  *
  *            ASYNC - Will copy as many characters as it can into the buffer for transmission,
  *                    and return control to the user.
  *
  *            SYNC_SPINWAIT - will return MICROBIT_INVALID_PARAMETER
  *
  *            SYNC_SLEEP - Will perform a cooperative blocking wait until all
  *                         given characters have been received by the connected
  *                         device.
  *
  * @return the number of characters written, or MICROBIT_NOT_SUPPORTED if there is
  *         no connected device, or the connected device has not enabled indications.
  */
int MicroBitUARTService::putc(char c, MicroBitSerialMode mode)
{
    return send( (uint8_t *)&c, 1, mode);
}

/**
  * Copies characters into the buffer used for Transmitting to the central device.
  *
  * @param buf a buffer containing length number of bytes.
  * @param length the size of the buffer.
  * @param mode the selected mode, one of: ASYNC, SYNC_SPINWAIT, SYNC_SLEEP. Each mode
  *        gives a different behaviour:
  *
  *            ASYNC - Will copy as many characters as it can into the buffer for transmission,
  *                    and return control to the user.
  *
  *            SYNC_SPINWAIT - will return MICROBIT_INVALID_PARAMETER
  *
  *            SYNC_SLEEP - Will perform a cooperative blocking wait until all
  *                         given characters have been received by the connected
  *                         device.
  *
  * @return the number of characters written, or MICROBIT_NOT_SUPPORTED if there is
  *         no connected device, or the connected device has not enabled indications.
  */
int MicroBitUARTService::send(const uint8_t *buf, int length, MicroBitSerialMode mode)
{
    if(length < 1 || mode == SYNC_SPINWAIT)
        return MICROBIT_INVALID_PARAMETER;

    if( !getConnected() || !indicateChrValueEnabled( mbbs_cIdxTX))
        return MICROBIT_NOT_SUPPORTED;

    int bytesWritten = 0;

    while ( getConnected() && indicateChrValueEnabled( mbbs_cIdxTX))
    {
        // Add new data that fits in the tx buffer
        while ( bytesWritten < length)
        {
            int nextHead = (txBufferHead + 1) % txBufferSize;
            if( nextHead == txBufferTail)
                break;

            txBuffer[ txBufferHead] = buf[ bytesWritten++];
            txBufferHead = nextHead;
        }
        
        if ( mode == SYNC_SLEEP)
        {
            waitingForEmpty = true;
            fiber_wake_on_event(MICROBIT_ID_NOTIFY, MICROBIT_UART_S_EVT_TX_EMPTY);
            sendNext();
            schedule();
            waitingForEmpty = false;
            if ( bytesWritten == length && txBufferTail == txBufferHead)
                break;
        }
        else
        {
            sendNext();
            break;
        }
    }

    return bytesWritten;
}

/**
  * An internal method that sends the next block from the tx buffer.
  * @return true if a block is sent
  */
bool MicroBitUARTService::sendNext()
{ 
    if ( txValueSize != 0 || txBufferTail == txBufferHead)
        return false;

    if( !getConnected() || !indicateChrValueEnabled( mbbs_cIdxTX))
        return false;

    // Duplicate the next tx data into the attribute buffer
    uint8_t *value = txBuffer + txBufferSize;
    int txBufferNext = txBufferTail;
    while ( txValueSize < MICROBIT_UART_S_ATTRSIZE && txBufferNext != txBufferHead)
    {
        value[ txValueSize++] = txBuffer[ txBufferNext];
        txBufferNext = ( txBufferNext + 1) % txBufferSize;
    }

    indicateChrValue( mbbs_cIdxTX, value, txValueSize);
    return true;
}

/**
  * Copies characters into the buffer used for Transmitting to the central device.
  *
  * @param s the string to transmit
  * @param mode the selected mode, one of: ASYNC, SYNC_SPINWAIT, SYNC_SLEEP. Each mode
  *        gives a different behaviour:
  *
  *            ASYNC - Will copy as many characters as it can into the buffer for transmission,
  *                    and return control to the user.
  *
  *            SYNC_SPINWAIT - will return MICROBIT_INVALID_PARAMETER
  *
  *            SYNC_SLEEP - Will perform a cooperative blocking wait until all
  *                         given characters have been received by the connected
  *                         device.
  *
  * @return the number of characters written, or MICROBIT_NOT_SUPPORTED if there is
  *         no connected device, or the connected device has not enabled indications.
  */
int MicroBitUARTService::send(ManagedString s, MicroBitSerialMode mode)
{
    return send((uint8_t *)s.toCharArray(), s.length(), mode);
}

/**
  * Reads a number of characters from the rxBuffer and fills user given buffer.
  *
  * @param buf a pointer to a buffer of len bytes.
  * @param len the size of the user allocated buffer
  * @param mode the selected mode, one of: ASYNC, SYNC_SPINWAIT, SYNC_SLEEP. Each mode
  *        gives a different behaviour:
  *
  *            ASYNC - Will attempt to read all available characters, and return immediately
  *                    until the buffer limit is reached
  *
  *            SYNC_SPINWAIT - will return MICROBIT_INVALID_PARAMETER
  *
  *            SYNC_SLEEP - Will first of all determine whether the given number of characters
  *                         are available in our buffer, if not, it will set an event and sleep
  *                         until the number of characters are avaialable.
  *
  * @return the number of characters digested
  */
int MicroBitUARTService::read(uint8_t *buf, int len, MicroBitSerialMode mode)
{
    if(mode == SYNC_SPINWAIT)
        return MICROBIT_INVALID_PARAMETER;

    int i = 0;

    if(mode == ASYNC)
    {
        int c;

        while(i < len && (c = getc(mode)) >= 0)
        {
            buf[i] = c;
            i++;
        }
    }

    if(mode == SYNC_SLEEP)
    {
        if(len > rxBufferedSize())
            eventAfter(len - rxBufferedSize(), mode);

        while(i < len)
        {
            buf[i] = (char)getc(mode);
            i++;
        }
    }

    return i;
}

/**
  * Reads a number of characters from the rxBuffer and returns them as a ManagedString
  *
  * @param len the number of characters to read.
  * @param mode the selected mode, one of: ASYNC, SYNC_SPINWAIT, SYNC_SLEEP. Each mode
  *        gives a different behaviour:
  *
  *            ASYNC - Will attempt to read all available characters, and return immediately
  *                    until the buffer limit is reached
  *
  *            SYNC_SPINWAIT - will return MICROBIT_INVALID_PARAMETER
  *
  *            SYNC_SLEEP - Will first of all determine whether the given number of characters
  *                         are available in our buffer, if not, it will set an event and sleep
  *                         until the number of characters are avaialable.
  *
  * @return an empty ManagedString on error, or a ManagedString containing characters
  */
ManagedString MicroBitUARTService::read(int len, MicroBitSerialMode mode)
{
    uint8_t buf[len + 1];

    memclr(&buf, len + 1);

    int ret = read(buf, len, mode);

    if(ret < 1)
        return ManagedString();

    return ManagedString((const char *)buf);
}

/**
  * Reads characters until a character matches one of the given delimeters
  *
  * @param delimeters the number of characters to match against
  * @param mode the selected mode, one of: ASYNC, SYNC_SPINWAIT, SYNC_SLEEP. Each mode
  *        gives a different behaviour:
  *
  *            ASYNC - Will attempt read the immediate buffer, and look for a match.
  *                    If there isn't, an empty ManagedString will be returned.
  *
  *            SYNC_SPINWAIT - will return MICROBIT_INVALID_PARAMETER
  *
  *            SYNC_SLEEP - Will first of all consider the characters in the immediate buffer,
  *                         if a match is not found, it will block on an event, fired when a
  *                         character is matched.
  *
  * @return an empty ManagedString on error, or a ManagedString containing characters
  */
ManagedString MicroBitUARTService::readUntil(ManagedString delimeters, MicroBitSerialMode mode)
{
    if(mode == SYNC_SPINWAIT)
        return MICROBIT_INVALID_PARAMETER;

    int localTail = rxBufferTail;
    int preservedTail = rxBufferTail;

    int foundIndex = -1;

    //ASYNC mode just iterates through our stored characters checking for any matches.
    while(localTail != rxBufferHead && foundIndex  == -1)
    {
        //we use localTail to prevent modification of the actual tail.
        char c = rxBuffer[localTail];

        for(int delimeterIterator = 0; delimeterIterator < delimeters.length(); delimeterIterator++)
            if(delimeters.charAt(delimeterIterator) == c)
                foundIndex = localTail;

        localTail = (localTail + 1) % rxBufferSize;
    }

    //if our mode is SYNC_SLEEP, we set up an event to be fired when we see a
    //matching character.
    if(mode == SYNC_SLEEP && foundIndex == -1)
    {
        eventOn(delimeters, mode);

        foundIndex = rxBufferHead - 1;

        this->delimeters = ManagedString();
    }

    if(foundIndex >= 0)
    {
        //calculate our local buffer size
        int localBuffSize = (preservedTail > foundIndex) ? (rxBufferSize - preservedTail) + foundIndex : foundIndex - preservedTail;

        uint8_t localBuff[localBuffSize + 1];

        memclr(&localBuff, localBuffSize + 1);

        circularCopy(rxBuffer, rxBufferSize, localBuff, preservedTail, foundIndex);

        //plus one for the character we listened for...
        rxBufferTail = (rxBufferTail + localBuffSize + 1) % rxBufferSize;

        return ManagedString((char *)localBuff, localBuffSize);
    }

    return ManagedString();
}

/**
  * Configures an event to be fired on a match with one of the delimeters.
  *
  * @param delimeters the characters to match received characters against e.g. ManagedString("\r\n")
  * @param mode the selected mode, one of: ASYNC, SYNC_SPINWAIT, SYNC_SLEEP. Each mode
  *        gives a different behaviour:
  *
  *            ASYNC - Will configure the event and return immediately.
  *
  *            SYNC_SPINWAIT - will return MICROBIT_INVALID_PARAMETER
  *
  *            SYNC_SLEEP - Will configure the event and block the current fiber until the
  *                         event is received.
  *
  * @return MICROBIT_INVALID_PARAMETER if the mode given is SYNC_SPINWAIT, otherwise MICROBIT_OK.
  *
  * @note delimeters are matched on a per byte basis.
  */
int MicroBitUARTService::eventOn(ManagedString delimeters, MicroBitSerialMode mode)
{
    if(mode == SYNC_SPINWAIT)
        return MICROBIT_INVALID_PARAMETER;

    //configure our head match...
    this->delimeters = delimeters;

    //block!
    if(mode == SYNC_SLEEP)
        fiber_wait_for_event(MICROBIT_ID_BLE_UART, MICROBIT_UART_S_EVT_DELIM_MATCH);

    return MICROBIT_OK;
}

/**
  * Configures an event to be fired after "len" characters.
  *
  * @param len the number of characters to wait before triggering the event
  * @param mode the selected mode, one of: ASYNC, SYNC_SPINWAIT, SYNC_SLEEP. Each mode
  *        gives a different behaviour:
  *
  *            ASYNC - Will configure the event and return immediately.
  *
  *            SYNC_SPINWAIT - will return MICROBIT_INVALID_PARAMETER
  *
  *            SYNC_SLEEP - Will configure the event and block the current fiber until the
  *                         event is received.
  *
  * @return MICROBIT_INVALID_PARAMETER if the mode given is SYNC_SPINWAIT, otherwise MICROBIT_OK.
  */
int MicroBitUARTService::eventAfter(int len, MicroBitSerialMode mode)
{
    if(mode == SYNC_SPINWAIT)
        return MICROBIT_INVALID_PARAMETER;

    //configure our head match...
    this->rxBuffHeadMatch = (rxBufferHead + len) % rxBufferSize;

    //block!
    if(mode == SYNC_SLEEP)
        fiber_wait_for_event(MICROBIT_ID_BLE_UART, MICROBIT_UART_S_EVT_HEAD_MATCH);

    return MICROBIT_OK;
}

/**
  * Determines if we have space in our rxBuff.
  *
  * @return 1 if we have space, 0 if we do not.
  *
  * @note the reason we do not wrap the super's readable() method is so that we
  *       don't interfere with communities that use manual calls to uBit.serial.readable()
  */
int MicroBitUARTService::isReadable()
{
    return (rxBufferTail != rxBufferHead) ? 1 : 0;
}

/**
  * @return The currently buffered number of bytes in our rxBuff.
  */
int MicroBitUARTService::rxBufferedSize()
{
    if(rxBufferTail > rxBufferHead)
        return (rxBufferSize - rxBufferTail) + rxBufferHead;

    return rxBufferHead - rxBufferTail;
}

/**
  * @return The currently buffered number of bytes in our txBuff.
  */
int MicroBitUARTService::txBufferedSize()
{
    if(txBufferTail > txBufferHead)
        return (txBufferSize - txBufferTail) + txBufferHead;

    return txBufferHead - txBufferTail;
}

#endif
