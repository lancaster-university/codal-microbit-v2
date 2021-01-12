# Memory Map

This file gives an overview of the flash layout used by the nRF52833 on the micro:bit v2.

Note: there is also a BLE Service named MemoryMapService, which is used to communicate the application's structure over BLE and not the layout of the entire flash storage.

# Memory layout

| Address | Description |
|---------|-------------|
| 7F000 | bootloader_settings_page [settings page](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v15.0.0%2Flib_bootloader.html) |
| 7E000 | mbr_params_page / bootloader settings backup [settings page](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v15.0.0%2Flib_bootloader.html) |
| 77000 | [Bootloader](https://github.com/microbit-foundation/v2-bootloader). [1] |
| 75000 | Flash Data Storage (BLE Bonds + ?) (defined by us, may change depending on implemenation) |
| 74000 | MicroBitStorage  (is where MicroBitStorage puts the things it stores) |
| 73000 | CODAL scratch page (is used as temporary scratch by MicroBitFlash, MicroBitFileSystem and MicroBitStorage) |
| 1C000 | Application |
| 01000 | SoftDevice (S113)|
| 00000 | [Master Boot Record](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fsds_s140%2FSDS%2Fs1xx%2Fmbr_bootloader%2Fmbr_bootloader.html) |

## Notes
[1] Our [current bootloader](https://github.com/microbit-foundation/codal-microbit/blob/master/lib/bootloader.o) requires 24.3 KB. No longer reserving additional space for USB DFU

