# Memory Map

This file gives an overview of the flash layout used by the nRF52833 on the micro:bit v2, and was written at tag [v0.2.24](https://github.com/lancaster-university/codal-microbit-v2/releases/tag/v0.2.24).

Note: there is also a BLE Service named MemoryMapService, which is used to communicate the application's structure over BLE and not the layout of the entire flash storage.

# Memory layout

| Address | Description |
|---------|-------------|
| 7F000 | bootloader_settings_page [settings page](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v15.0.0%2Flib_bootloader.html) |
| 7E000 | mbr_params_page / bootloader settings backup [settings page](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v15.0.0%2Flib_bootloader.html) |
| 77000 | [Bootloader](https://github.com/microbit-foundation/v2-bootloader). Our [current bootloader](https://github.com/microbit-foundation/codal-microbit/blob/master/lib/bootloader.o) requires 24.3 KB. |
| 75000 | Flash Data Storage. A Nordic SDK component, with a minimum size of 2 pages, that stores the bond data. Currently CODAL doesn't store anything else here. [View more in Nordic SDK](https://infocenter.nordicsemi.com/topic/sdk_nrf5_v16.0.0/lib_fds.html) |
| 74000 | MicroBitStorage  (is where MicroBitStorage puts the things it stores) |
| 73000 | CODAL scratch page (is used as temporary scratch by MicroBitFlash, MicroBitFileSystem and MicroBitStorage) |
| 1C000 | Application |
| 01000 | SoftDevice (S113)|
| 00000 | [Master Boot Record](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fsds_s140%2FSDS%2Fs1xx%2Fmbr_bootloader%2Fmbr_bootloader.html) |


