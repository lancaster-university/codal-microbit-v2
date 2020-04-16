# BLE Object Files

This directory contains the necessary objects for bringing up SoftDevice (S140 7.0.1) and the Bootloader on the nrf52833. I've split everything into individual object files as I encountered some issues with padding between addresses when converting from IHEX to BIN files.

A breakdown of generating the required hex files, and then converting them into .o files is below.

## Creating the hex files

### Bootloader

The bootloader is build using Martin's bootloader program, which can be found [here](https://github.com/microbit-foundation/v2-bootloader/).

There is two bootloader .o files, one (bootloader.o) is a standard bootloader using the nRF SDK, and the second (bootloader_skip_validation.o) uses a modified nRFSDK to skip validation.

bootloader_skip_validation.o is required so that we don't have to regenerate the settings page every time we create a new CODAL program.

The modification to nRFSDK is as follows:

nRF5SDK/components/libraries/bootloader/dfu/nrf_dfu_validation.c
```
bool nrf_dfu_validation_boot_validate(boot_validation_t const * p_validation, uint32_t dat
{
    return true; // Modified to always return true

    uint8_t const * p_data = (uint8_t*) data_addr;
    switch(p_validation->type)
    {
        case NO_VALIDATION:
            return true;

        case VALIDATE_CRC:
        {
            uint32_t current_crc = *(uint32_t *)p_validation->bytes;
            uint32_t crc = crc32_compute(p_data, data_len, NULL);

            if (crc != current_crc)
            {
                // CRC does not match with what is stored.
                NRF_LOG_DEBUG("CRC check of app failed. Return %d", NRF_DFU_DEBUG);
                return NRF_DFU_DEBUG;
            }
            return true;
        }
...
```

Once this change has been made compile the bootloader following the instructions in the repo.

Split the resulting hex file into seperate bootloader and UICR hex files. This is done to avoid generating large BIN files, caused by the UICR address of 0x10001000.

For example the last few lines of the bootloader hex file go from:

```
:10DA180000000000000000000000000000000000FE
:10DA280000000000000000000000000000000000EE
:10DA380000000000000000000000000000000000DE
:10DA480000000000000000000000000000000000CE
:0CDA58000000000025820700018207008A
:10DA6400000000000000000061AF0700001000008B
:04DA740000000800A6
:020000020000FC
:020000041000EA
:041014000080070051
:0410180000E00700ED
:04000003700082B552
:00000001FF
```

to:

```
:10DA180000000000000000000000000000000000FE
:10DA280000000000000000000000000000000000EE
:10DA380000000000000000000000000000000000DE
:10DA480000000000000000000000000000000000CE
:0CDA58000000000025820700018207008A
:10DA6400000000000000000061AF0700001000008B
:04DA740000000800A6
:00000001FF
```

And a UICR hex file is created:

```
:020000020000FC
:020000041000EA
:041014000080070051
:0410180000E00700ED
:04000003700082B552
:00000001FF
```

### Softdevice

Split into MBR and Softdevice. This done to prevent the BIN file padding with 0s which appears to cause an issue in getting out of Softdevice on start up.

Using `s140_nrf52_7.0.1_softdevice.hex` as an example. The jump between MBR and SD is at line 177.

```
:100AF000001000000000000000FFFFFF0090D00386 # MBR end  0x00AF
:10100000C8130020415F020075C10000A75E020006 # SD start 0x1000
```

Add an end of file here and create MBR.hex, and add a start to the SD section in a new file called softdevice.hex

### Settings

Finally create a settings.hex. I'm not sure whether this is required at the moment, but include it for now.

To do so follow Martin's instructions in v2-bootloader, and generate it with the test-app. Modify the `./merge_hex` script to not delete the settings.hex.

## Creating .o from .hex

Gather the hex files together into the ./lib folder. You should have bootloader_skip_validation.hex, mbr.hex, settings.hex, softdevice.hex, and uicr.hex.

Convert each into a BIN file. Example command for bootloader_skip_validation below. If any of the bin files are very large (e.g. 256Mb) you probably haven't split something correctly.

```
arm-none-eabi-objcopy -I ihex --output-target=binary bootloader_skip_validation.hex bootloader_skip_validation.bin
```

Then convert each bin into an object file. 

```
arm-none-eabi-objcopy --rename-section .data=.bootloader_skip_validation -I binary -O elf32-littlearm -B arm bootloader_skip_validation.bin bootloader_skip_validation.o
```

Once all the object files are created and in the ./lib folder the linker script can use them.

It has been modified to put them in their correct positions:

```
MEMORY
{
  MBR (rx) : ORIGIN = 0x0000, LENGTH = 0x1000
  SD (rx) : ORIGIN = 0x1000, LENGTH = 0x26000
  FLASH (rx) : ORIGIN = 0x27000, LENGTH = 0x59000
  BOOTLOADER (rx) : ORIGIN = 0x78000, LENGTH = 0x6000
  SETTINGS (rx) : ORIGIN = 0x7E000, LENGTH = 0x2000
  UICR (rx) : ORIGIN = 0x10001014, LENGTH = 0x8
  RAM (rwx) : ORIGIN = 0x20002270, LENGTH = 0x1DD90
}
OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
ENTRY(Reset_Handler)
SECTIONS
{
    . = ALIGN(4);
    .mbr :
    {
        KEEP(*(.mbr))
    } > MBR

    . = ALIGN(4);
    .softdevice :
    {
        KEEP(*(.softdevice))
    } > SD

    . = ALIGN(4);
    .bootloader_skip_validation :
    {
        KEEP(*(.bootloader_skip_validation))
    } > BOOTLOADER

    . = ALIGN(4);
    .uicr :
    {
        KEEP(*(.uicr))
    } > UICR

    . = ALIGN(4);
    .settings :
    {
        KEEP(*(.settings))
    } > SETTINGS
```
