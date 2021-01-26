# Partial Flashing Service

## Introduction

The Partial Flashing Service allows a BLE client to connect to a micro:bit and read and write the information required to partially update the firmware. For MakeCode programs this is the user's script, and for Python programs this is the file system.

This service exists in partnership with the DFU service that allows a client to fully flash the micro:bit.

Diagrams showing the flow of data during partial flashing can be found at [the bottom of the document](#partial flashing client).

## Bluetooth Service Specification

The micro:bit v2 follows the same BLE specification as the micro:bit v1. The specification can be found [here](https://lancaster-university.github.io/microbit-docs/resources/bluetooth/bluetooth_profile.html)

## Enabling the service

In order to work properly, the partial flashing service requires certain things to be true about the memory map, and it must be possible for the MemoryMapService to be able to build a model of the flash layout. Most significantly, the service is ideal for updating data outside the main C/C++ build containing CODAL. This is currently possible for MakeCode and MicroPython builds, but not trivial for CODAL programs, where the program is linked into a single executable.

The partial flashing service is included by default in MakeCode builds for micro:bit V1 (using microbit-dal, not CODAL) and V2, and MicroPython builds for micro:bit V2 only. The Partial Flashing service is available in both application and pairing modes.

To enable it in a CODAL build the `MICROBIT_BLE_PARTIAL_FLASHING` option is set to `1` in `codal.json`

```
"config":{
        "SOFTDEVICE_PRESENT": 1,
        "DEVICE_BLE": 1,
        "MICROBIT_BLE_ENABLED" : 1,
        "MICROBIT_BLE_PAIRING_MODE": 1,
        "MICROBIT_BLE_DFU_SERVICE": 1,
        "MICROBIT_BLE_DEVICE_INFORMATION_SERVICE": 1,
        "MICROBIT_BLE_EVENT_SERVICE" : 1,
        "MICROBIT_BLE_PARTIAL_FLASHING" : 1,
        "MICROBIT_BLE_SECURITY_LEVEL": "SECURITY_MODE_ENCRYPTION_NO_MITM"
    }
```

## Characteristic Commands
The service consists of a single Bluetooth GATT characteristic that responds to a client's WRITE WITHOUT RESPONSE request with a BLE Notification. The characteristic supports commands to: 

- Read the REGION INFO 
- WRITE DATA to the flash
- Inform the micro:bit that the sevice has reached END OF TRANSMISSION
- Obtain the MICROBIT STATUS (PFS version #, current m:b mode)
- RESET the micro:bit into either application (MakeCode) or BLE mode.

These are documented below:

### Region Info Command

The Region Info command is used to request information from the Memory Map. The client sends a WRITE WITHOUT RESPONSE command with the control byte (byte 0) set to 0x00 (Region Info) and byte 1 set to the region ID:

#### Client Write Request

#### MakeCode

|    | Byte 0 / Command | Byte 1 / Region # |
|---|---|---|
| Read Soft Device | 0x00 | 0x00 |
| Read CODAL | 0x00 | 0x01 |
| Read MakeCode | 0x00 | 0x02 |

#### Python

|    | Byte 0 / Command | Byte 1 / Region # |
|---|---|---|
| Read Soft Device | 0x00 | 0x00 |
| Read MicroPython | 0x00 | 0x01 |
| Read FileSystem | 0x00 | 0x02 |

#### micro:bit Response

The micro:bit looks up the relevant information and returns it using a BLE NOTIFICATION:

| Byte 0 | Byte 1 | Bytes [2,3,4,5] | Bytes [6,7,8,9] | Bytes [10,11,12,13,14,15,16,17] |
|---|---|---|---|---|
| READ_NOTIFICATION / 0x00 | Region # | Region Start Address | Region End Address | Hash |


### Write Data Command
To send the new firmware and write it to the flash a WRITE_WITHOUT_RESPONSE command is used. These packets begin with the command byte (byte 0) set to 0x01. The packet is structured as follows:

#### Client Write Without Response

| 1 Byte    | 2 Bytes | 1 Byte  | 16 Bytes |
|---|---|---|---|
| COMMAND   | OFFSET  | PACKET# | DATA |

The micro:bit handles data in blocks of 4 packets. This allows the micro:bit to keep a 64 byte buffer and ensure that it can be successfully processed before moing onto the next block.

The address of the current block is split between the first two packets. The first packet contains the lower 2 bytes of the address, and the second packet contains the upper 2 bytes of the address.

```
// blockNum is 0: set up the offset
case 0:
    {
        offset = ((data[1] << 8) | data[2] << 0);
        blockNum++;
        break;
    }
// blockNum is 1: complete the offset
case 1:
    {
        offset |= ((data[1] << 24) | data[2] << 16);
        blockNum++;
        break;
    }
```

#### micro:bit Response

**Successful Write:**

| Byte 0 | Byte 1 | 
|---|---|
| WRITE_NOTIFICATION / 0x01 | 0xFF |


**Out Of Order Packet**
If the micro:bit detects an out of order packet the packet count is set to the end of the block (start of block + 3, ready for the next) and a notification is sent to inform the client. The client needs to update the packet # to the start of the next block - add 4 to the start of the block (block packet counts always start with a multiple of 4: 0, 4, 8, 12..).

| Byte 0 | Byte 1 | 
|---|---|
| WRITE NOTIFICATION / 0x01 | 0xAA |

### End Of Transmission

When the start of the embedded source is reached a WRITE_WITHOUT_RESPONSE request is issued with a payload of 0x02. The micro:bit removes the embedded source magic to prevent confusion with the new application, and resets the micro:bit into application mode.

| Byte 0 |
|---|
| END_OF_TX / 0x02 |

### MICROBIT STATUS
A client can fetch the version of the Partial Flashing Service and the micro:bit's current status using the MICROBIT STATUS command.

| Byte 0 |
|---|
| MICROBIT STATUS / 0xEE |

The micro:bit responds with a notification. Partial Flashing Version, currently returns 1 for the intial release. Current mode returns: 0x00 for Pairing Mode, 0x01 for Application Mode.

| Byte 0 | Byte 1 | Byte 2 |
|---|---|---|
| MICROBIT STATUS / 0xEE | Partial Flashing Version | Current Mode |

### MICROBIT RESET
The Partial Flashing Service also allows a client to reset a micro:bit into either Application or Bluetooth Pairing Mode.

Reset Type: 0x00 for Pairing Mode, 0x01 for Application Mode

| Byte 0 | Byte 1
|---|---|
| MICROBIT RESET / 0xFF | Reset Mode |

### UUIDs
```
const uint8_t  MicroBitPartialFlashServiceUUID[] = {
    0xe9,0x7d,0xd9,0x1d,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
};

const uint8_t  MicroBitPartialFlashServiceCharacteristicUUID[] = {
    0xe9,0x7d,0x3b,0x10,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
};
```

## Partial Flashing Client

### Example Client for Android
Please see an example implementation [on GitHub](https://github.com/microbit-foundation/android-partial-flashing-lib).

### Client Process
A client completing the partial flashing process will first need to read the Memory Map from the micro:bit to obtain the region hashes.

Once the hashes have been obtained from the micro:bit they need to be compared with the hashes found within the new hex file to determine if it is possible to partially flash the micro:bit. Partial flashing requires the micro:bit to be using a matching version of the CODAL or MicroPython.

![Determine Partial Flashing](/resources/DeterminePF.png)

If partial flashing is possible the micro:bit needs to be in BLE mode to prevent the flashing process from interfering with instructions that are executing. This is done by sending a `MICROBIT_RESET` command with `0x00` as the payload. 

Once the region is completely transferred, the client needs to send an END OF TRANSMISSION BLE WRITE to inform the micro:bit the transfer is over. The micro:bit will the erase any remaining pages in the region, clear flags keeping it in partial flashing mode, and then reset into the transferred program.

![Do Partial Flashing](/resources/PFFlow.png)

