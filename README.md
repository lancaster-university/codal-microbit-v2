# codal-microbit-v2
CODAL target for the micro:bit v2.x series of devices

[![Build Samples](https://github.com/lancaster-university/codal-microbit-v2/actions/workflows/build.yml/badge.svg)](https://github.com/lancaster-university/codal-microbit-v2/actions/workflows/build.yml) [![Build MicroPython](https://github.com/lancaster-university/codal-microbit-v2/actions/workflows/micropython.yml/badge.svg)](https://github.com/lancaster-university/codal-microbit-v2/actions/workflows/micropython.yml) [![Build MakeCode](https://github.com/lancaster-university/codal-microbit-v2/actions/workflows/makecode.yml/badge.svg)](https://github.com/lancaster-university/codal-microbit-v2/actions/workflows/makecode.yml)

[![Autoupdate Changelog.md](https://github.com/lancaster-university/codal-microbit-v2/actions/workflows/update-changelog.yml/badge.svg)](https://github.com/lancaster-university/codal-microbit-v2/actions/workflows/update-changelog.yml) [![Bloaty size diff](https://github.com/lancaster-university/codal-microbit-v2/actions/workflows/size-diff.yml/badge.svg)](https://github.com/lancaster-university/codal-microbit-v2/actions/workflows/size-diff.yml)

See https://github.com/lancaster-university/microbit-v2-samples for details on building and using CODAL.

## Tags and Releases
We tag fairly often, and these may include changes which we are currently testing across the various ecosystems that use CODAL, including Microsoft MakeCode and The Foundation Python editors.
Consequently, these, while generally more stable than the `feature/` or `tests/` branches are also not _guaranteed_ to be stable and complete.

Releases are selected tags intended to be stable and production-ready; and are the recommended ones to use for anyone implementing or using the CODAL codebase.
Each GitHub Release changelog will include the changes since the previous Release (not for just that specific tag only), and the changelog will also include changes in the project dependencies (codal-core, codal-nrf52, codal-nrf-sdk) as these don't have individual changelogs.

## Target Properties
### CONFIG_NFCT_PINS_AS_GPIOS
Controls the NFC functionality of the nRF52833, Comment/remove the line to enable the antenna and NFC capabilities.