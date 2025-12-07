## [v0.3.4](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.3.3...v0.3.4)

 - CI: Update Changelog script to skip listing changelog edit commits. (by Carlos Pereira Atencio)
 - CI: Updates due to latest arm compiler not supporting Intel Mac. (by Carlos Pereira Atencio)
 - Snapshot v0.3.4 (by Carlos Pereira Atencio)

## [v0.3.3](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.3.2...v0.3.3)

 - Add additional validation checks around SoundEmjiSynthesizer, and guarantee mutual exclusion when IRQ triggers (by Joe Finney)
 - Bump to head of codal-core (by Joe Finney)

### codal-core ([8c8366f...1bc6d4d](https://github.com/lancaster-university/codal-core/compare/8c8366f9a1e92da69f90fe816456c4b9c42ffd13...1bc6d4d10c47f4fe34e2a54004530d5071e892d4))

 - ST7735 driver fix for screen hanging. (#186) (by KierPalin)

## [v0.3.2](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.3.1...v0.3.2)

 - CI: Update Windows runner from deprecated 2019 to oldest available 2022. (by Carlos Pereira Atencio)
 - Make writes to mManagedBuffer safe (by Joe Finney)
 - Snapshot v0.3.2 (by Carlos Pereira Atencio)

### codal-core ([a2f5051...8c8366f](https://github.com/lancaster-university/codal-core/compare/a2f5051dc60f89276546517f1077e75659ae6741...8c8366f9a1e92da69f90fe816456c4b9c42ffd13))

 - Prevent microphone LED from being onn all the time (bug #499) (by Joe Finney)
 - Smooth values in the 35..56dB range (by Joe Finney)

## [v0.3.1](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.3.0...v0.3.1)

 - CI: Now that all audio-refactor branches are merged, revert CI temp changes. (by Carlos Pereira Atencio)
 - Snapshot v0.3.1 (by Carlos Pereira Atencio)

### codal-core ([24f1284...a2f5051](https://github.com/lancaster-university/codal-core/compare/24f1284018657fd01a276ef554d16f9550d65d7f...a2f5051dc60f89276546517f1077e75659ae6741))

 - Ensure LevelDetectorSPL::getValue() waits for valid data before returning (#486) (by Joe Finney)
 - Fix bug related to quiet received buffer (by Joe Finney)
 - Fix bug #487 (by Joe Finney)

## [v0.3.0](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.71...v0.3.0)

 - Remove legacy code relater to runtime reconfiguration of audio pipeline (by Joe Finney)
 - Chnages ito support audio-refactor. (by Joe Finney)
 - Bugfix minor errors in pipeline (by Joe Finney)
 - bugfix (by Joe Finney)
 - Set default sample rate back to 11000 (by Joe Finney)
 - Reduce clicking noise on microphone acivation (by Joe Finney)
 - Increase start up delay to 3 frames, to avoid occasional clicks. (by Joe Finney)
 - Reduce sensitivity of microphone to permit detection of 35db (by Joe Finney)
 - Introduce multitrack audio playback capability (by Joe Finney)
 - Expose address that marks the usable top of flash memory (by Joe Finney)
 - Update target-locked to allow for a fake tagged-released for micromusic testing (by Joe Finney)
 - Use 32 bit addressing for limit macros (by Joe Finney)
 - Update target-locked hashes (by Joe Finney)
 - Add static definitions of TOP_OF_FLASH (by Joe Finney)
 - Update target-locked hashes. (by Carlos Pereira Atencio)
 - ci: Run CI workslflows with projects using the latest audio refactor. (by Carlos Pereira Atencio)
 - Add message bus listener removal handler for LevelDetectorSPL instance (by Joe Finney)
 - Update target-locked hashes. (by Carlos Pereira Atencio)
 - Return levelDetectorSPL settings to previous values (bug #472) (by Joe Finney)
 - Default Mixerchannel rate to upstream default on creation. (by Joe Finney)
 - Set default microphone samplerate by a CONFIG option (by Joe Finney)
 - Update target-locked to latest revision (by Joe Finney)
 - Filter audio data that can arrive when hardware is disabled (#478) (by Joe Finney)
 - bump version (by Joe Finney)
 - Snapshot v0.3.0 (by Carlos Pereira Atencio)

### codal-core ([238e938...24f1284](https://github.com/lancaster-university/codal-core/compare/238e93846b434b730d4d0bfde99b43ccfde29cef...24f1284018657fd01a276ef554d16f9550d65d7f))

 - Remove runtime configuration of bitrate for StreamSplitter (by Joe Finney)
 - Remove legacy code. (by Joe Finney)
 - Introduce DataSourceSink base class (by Joe Finney)
 - More refactoring. (by Joe Finney)
 - Bugfix minor errors (by Joe Finney)
 - Add tri-state for connectedness of audio filters (by Joe Finney)
 - Refactored stream recording class (by Joe Finney)
 - Bugfix for smaller packets being received from the microphone (by Joe Finney)
 - Enable back to back recordAsync / playbackAsync (by Joe Finney)
 - Fix typo in test expression. classic! (by Joe Finney)
 - Introduce (optional) StreamAnalyzer class (by Joe Finney)
 - Update LevelDetectorSPL to always wake sleeping fibers. (by Joe Finney)
 - Fix bug relating to reset of LevelDetectorSPL state (by Joe Finney)
 - Add support for detecting removal of MessageBus listeners (by Joe Finney)
 - Fix synchronous playback bug via SerialStreamer API (bug #477) (by Joe Finney)
 - Introduce SerialStreamer class into codal-core (by Joe Finney)
 - Register SerialStreamer component as always wantingData (by Joe Finney)
 - Remove stale data stored in DataStream (by Joe Finney)
 - Add noise gate to level detector SPL (by Joe Finney)
 - Add optional support to filter unwanted packets at source (by Joe Finney)
 - Create tailored dataWanted() behaviour for SerialStremaer (#485) (by Joe Finney)

### codal-nrf52 ([8802eb4...1fbb724](https://github.com/lancaster-university/codal-nrf52/compare/8802eb49140e0389e535cb6160d9080efd951ba7...1fbb7240290fe36a55c61378f5cdeb7640f3ec4a))

 - Updated documentation for NRF52I2C and NRF52Pin (both .h & .cpp) (#55) (by KierPalin)
 - Eliminate compiler warning about derived method signature (#57) (by Kevin Walsh)
 - Minor change to support debugging (by Joe Finney)
 - Changes to support audio-refactor. (by Joe Finney)
 - Bugfix minor errors (by Joe Finney)
 - Bugfix (by Joe Finney)
 - Add configurable startup delay to ADC channels (by Joe Finney)
 - Fix audio channel release (bug #476) (by Joe Finney)
 - Add interface to allow sample rate of microphone to be defined (#474) (by Joe Finney)
 - Add basic validation for defining microphone ADC samplerate (by Joe Finney)

### codal-microbit-nrf5sdk ([d41d5c7...4b8abc6](https://github.com/microbit-foundation/codal-microbit-nrf5sdk/compare/d41d5c7ebe53a1d01935e61d4ffa891e5112e119...4b8abc690f6c9fca6132e6db5ee13a795a263f88))

 - Suppress all warnings from this target as it's 3rd party code. (#18) (by Carlos Pereira Atencio)

## [v0.2.71](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.70...v0.2.71)

 - Fix first use of io.logo.isTouched() (#480) (by Martin Williams)
 - Save a bit of memory from MicroBitIO. (#466) (by Carlos Pereira Atencio)
 - Snapshot v0.2.71 (by Carlos Pereira Atencio)

## [v0.2.70](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.69...v0.2.70)

 - CI: Update mac runner version to macOS 13, as 12 is deprecated. (by Carlos Pereira Atencio)
 - Fix Pi value typo in SoundSynthesizerEffects. (#456) (by Carlos Pereira Atencio)
 - CI: Reduce the min GCC version used to match what MakeCode (PXT) uses. (#461) (by Carlos Pereira Atencio)
 - Update MicroBitAudio.isPlaying() to take into account initial silence. (#452) (by Carlos Pereira Atencio)
 - Add 'show_size' option to target.json file to print build size info. (#372) (by Carlos Pereira Atencio)
 - ci: Also build MakeCode & MicroPython on PRs. (#473) (by Carlos Pereira Atencio)
 - Eliminate noreturn compiler warnings (#467) (by Kevin Walsh)
 - CI: Update deprecated GH Actions runner image ubuntu-20.4 to 22.04. (by Carlos Pereira Atencio)
 - Ensure float operations in Sound Expression/Synth & Compass cal. (#454) (by Carlos Pereira Atencio)
 - Overwrite default CODAL `uBit.audio.levelSPL` 8-bit conversion range. (#481) (by Carlos Pereira Atencio)
 - Snapshot v0.2.70 (by Carlos Pereira Atencio)

### codal-core ([c43b899...238e938](https://github.com/lancaster-university/codal-core/compare/c43b89936ed72c022f2649f5540a9a74ef48fd04...238e93846b434b730d4d0bfde99b43ccfde29cef))

 - Ensure float literals and float versions of math functions are used. (#176) (by Carlos Pereira Atencio)
 - Fix MemorySource::playAsync for ManagedBuffer (#180) (by Martin Williams)
 - Fix TouchSensor::removeTouchButton (#179) (by Martin Williams)
 - Fix SerialStreamer::duration() (#172) (by Martin Williams)
 - Add compiler specific attributes for FORCE_RAM_FUNC. (#166) (by Carlos Pereira Atencio)
 - Fix Compass::init() warning and array delete bug (#181) (by Kevin Walsh)
 - LevelDetectorSPL: Cast `LEVEL_DETECTOR_SPL_8BIT_X_POINT` to floats. (#182) (by Carlos Pereira Atencio)

## [v0.2.69](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.68...v0.2.69)

 - Fix CODAL_VERSION config value defined in target.json (by Carlos Pereira Atencio)
 - CI: Changelog script to also include libs when created from scratch. (by Carlos Pereira Atencio)
 - CI: Update Changelog script to clone libs in tmp dir to get history. (by Carlos Pereira Atencio)
 - Snapshot v0.2.69 (by Carlos Pereira Atencio)

### codal-core ([509086c...c43b899](https://github.com/lancaster-university/codal-core/compare/509086cc8590465041b15493ab52b56e7071c110...c43b89936ed72c022f2649f5540a9a74ef48fd04))

 - StreamRecording - calculate memory overhead of buffers (#175) (by Martin Williams)

## [v0.2.68](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.67...v0.2.68)

 - Allow log full FUL to be written to last byte (#438) (by Martin Williams)
 - Temporarily set CODAL_VERSION back to "unknown". (#445) (by Carlos Pereira Atencio)
 - Snapshot v0.2.68 (by Carlos Pereira Atencio)

## [v0.2.67](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.66...v0.2.67)

 - Update Changelog.md (by Dr John Vidler)
 - CI: Restore building MakeCode with Docker. (#407) (by Carlos Pereira Atencio)
 - CI: Update versions of actions to remove warnings about node 16. (#408) (by Carlos Pereira Atencio)
 - Remove uBit.io.face as it was a temp additional that is no longer needed. (#410) (by Carlos Pereira Atencio)
 - Added definitions to match the Makecode effects parameters (#417) (by Dr John Vidler)
 - CI:Update Changelog script to include changes in lib dependencies. (#420) (by Carlos Pereira Atencio)
 - Set uBit.io.logo to capacitive touch mode by default. (#418) (by Carlos Pereira Atencio)
 - CI: PXT build workaround due to deprecated PXT docker image type. (#435) (by Carlos Pereira Atencio)
 - CI: Update macOS runner version as 11 is deprecated. (#436) (by Carlos Pereira Atencio)
 - Populate MICROBIT_DAL_VERSION and add microbit_dal_version(). (#434) (by Carlos Pereira Atencio)
 - Addition of getRows(n, k) and getNumberOfRows(n) to the datalogger.  (#431) (by KierPalin)
 - Ensure header files and .cpp files use the codal namespace appropriately. (#437) (by Carlos Pereira Atencio)
 - Snapshot v0.2.67 (by Carlos Pereira Atencio)

### codal-core ([992c0b1...509086c](https://github.com/lancaster-university/codal-core/compare/992c0b11a0eb2a1edca9c2f76821f89a99a3acec...509086cc8590465041b15493ab52b56e7071c110))

 - Update docstrings for LevelDetector threshold methods. (#167) (by Carlos Pereira Atencio)
 - Display scroll an empty char if the requested char is out-of-range. (#168) (by Carlos Pereira Atencio)
 - Better version support symbols for user applications (#169) (by Dr John Vidler)
 - Ensure .cpp files are `using namespace codal`. (#170) (by Carlos Pereira Atencio)
 - Add config flag to enable/disable adding codal to the global namespace. (#171) (by Carlos Pereira Atencio)

### codal-nrf52 ([0643733...8802eb4](https://github.com/lancaster-university/codal-nrf52/compare/0643733703b4f8f788af81a996f40d1f1a1527bc...8802eb49140e0389e535cb6160d9080efd951ba7))

 - NRF52Pin::wasTouched() default w/o args to use current pin touch mode state. (#53) (by Carlos Pereira Atencio)
 - Ensure all .cpp files have `using namespace codal`. (#54) (by Carlos Pereira Atencio)
 - Set neopixel functions inside the codal namespace. (by Carlos Pereira Atencio)
 - Wrap `using namespace` in headers with codal config flag. (#43) (by Carlos Pereira Atencio)

## [v0.2.66](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.65...v0.2.66)

 - Update Changelog.md (by Dr John Vidler)
 - Snapshot v0.2.66 (by Dr John Vidler)

### codal-core ([7b6f9df...992c0b1](https://github.com/lancaster-university/codal-core/compare/7b6f9df3b586f8dd9bede4bd0baa0cdc212b6ba2...992c0b11a0eb2a1edca9c2f76821f89a99a3acec))

 - Applying the patch suggested by @dpgeorge to handle large receiving buffers for pullInto() (by Dr John Vidler)

## [v0.2.65](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.63...v0.2.65)

 - Corrected Changelog.md after my tag snarfu (by Dr John Vidler)
 - Refined the changelog script to only update the head of the log under normal cases (by Dr John Vidler)
 - Set MICROBIT_RADIO_MAX_PACKET_SIZE as a configurable value. (#387) (by Carlos Pereira Atencio)
 - Change datalog CSV download mime type from text/plain to text/csv (#339) (by Martin Williams)
 - Added a new CONFIG_MIXER_DEFAULT_CHANNEL_SAMPLERATE constant for channels with no defined rate to use (by Dr John Vidler)
 - Fixed a bug where in some conditions the microphone would not auto-start (by Dr John Vidler)
 - Update MicroBitLog to support 3 header formats (#399) (by Matt Hillsdon)
 - Added MicroBitButton.h to the required includes for MicroBitCompat.h so its still included in the build when not using bluetooth (by Dr John Vidler)
 - Moved the MicroBitButton inclusion to the tail of MicroBitCompat to avoid circular dependencies or a bunch of needless forward declarations (by Dr John Vidler)
 - Bumped the default Mixer2 channel sample rate back up to 44100 (by Dr John Vidler)
 - Snapshot v0.2.65 (by Dr John Vidler)

### codal-core ([63f017f...7b6f9df](https://github.com/lancaster-university/codal-core/compare/63f017fdcd12aca769264c36fe47e6071a40e502...7b6f9df3b586f8dd9bede4bd0baa0cdc212b6ba2))

 - Updated to correctly restore mic state on returning from a deep sleep (by Dr John Vidler)
 - Typo (by Dr John Vidler)
 - Working audio resampling rates if requested by downstream components of a SplitterChannel (by Dr John Vidler)

### codal-microbit-nrf5sdk ([5714cbe...d41d5c7](https://github.com/microbit-foundation/codal-microbit-nrf5sdk/compare/5714cbe338c3544793bbb841a7ae81708a3ffae1...d41d5c7ebe53a1d01935e61d4ffa891e5112e119))

 - Revert "Quieted a warning on fallthrough, as this is intended behaviour" (by Dr John Vidler)

## [v0.2.63](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.62...v0.2.63)

 - Removed deactivateLevelSPL entirely as per codal-microbit-v2 issue #222 (by Dr John Vidler)
 - Fix for the microphone failing to wake after a sleep event (by Dr John Vidler)
 - Commented out a temporary alias for the face/logo touch button, staged for removal (by Dr John Vidler)
 - Revert for the face alias removal, for v0.2.63 release (by Dr John Vidler)
 - Snapshot v0.2.63 (by Dr John Vidler)
 - Reverted the power saving for the LED Matrix to avoid strange issues with Pins. (by Dr John Vidler)
 - Snapshot v0.2.64 (by Dr John Vidler)
 - Updated target-locked with the corrected tag version (by Dr John Vidler)

### codal-core ([b06b603...63f017f](https://github.com/lancaster-university/codal-core/compare/b06b6031df2f3e9fd9459fbe13a0db95a5cc47f0...63f017fdcd12aca769264c36fe47e6071a40e502))

 - Moved to a bool check on new buffers to prevent race conditions when ISRs fire (by Dr John Vidler)
 - Updated the recording to use 60k of RAM and guarded the debug output (by Dr John Vidler)
 - Added clickCount tracking on touch events (by Dr John Vidler)
 - Removed clickcount and dmesg from TouchButton, as this is handled in buttonActive() (by Dr John Vidler)
 - Revert of the maximum StreamRecording memory usage for v0.2.63 to be safe (by Dr John Vidler)

### codal-nrf52 ([099f15a...0643733](https://github.com/lancaster-university/codal-nrf52/compare/099f15ab7932ca902976262b989890f0edb3b1ce...0643733703b4f8f788af81a996f40d1f1a1527bc))

 - Added wasTouched support to NRF52Pins, to match the isTouched functionality (by Dr John Vidler)
 - Updated documentation for further clarity for wasTouched behaviour, added a touch mode parameter to the call (by Dr John Vidler)

### codal-microbit-nrf5sdk ([f98e1a9...5714cbe](https://github.com/microbit-foundation/codal-microbit-nrf5sdk/compare/f98e1a9325acef9de52416b3e30cc820531b6699...5714cbe338c3544793bbb841a7ae81708a3ffae1))

 - Quieted a warning on fallthrough, as this is intended behaviour (by Dr John Vidler)

## [v0.2.62](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.61...v0.2.62)

 - Snapshot v0.2.61-master.0 (by Dr John Vidler)
 - Snapshot v0.2.61-master.1 (by Dr John Vidler)
 - Snapshot v0.2.61-master.2 (by Dr John Vidler)
 - Update target-locked.json codal-core commit to fix builds. (by Carlos Pereira Atencio)
 - Snapshot v0.2.62 (by Dr John Vidler)

### codal-core ([a116354...b06b603](https://github.com/lancaster-university/codal-core/compare/a1163541d10c0a0d3dd9b2e8ac1b8906100449ad...b06b6031df2f3e9fd9459fbe13a0db95a5cc47f0))

 - Added pointer debugging to the audio recording. Fixed ref-not-value error (by Dr John Vidler)
 - Exposd a call to dump the heap state when not in full memory debug mode (by Dr John Vidler)
 - Exposd a call to dump the heap state when not in full memory debug mode (by Dr John Vidler)
 - Made the heap allocator less chatty, so we can identify anomolies (by Dr John Vidler)
 - Fix issue building without DMESG_SERIAL_DEBUG. (by Carlos Pereira Atencio)
 - Re-enabled temporarily disabled debug output (by Dr John Vidler)

### codal-nrf52 ([2d77a3c...099f15a](https://github.com/lancaster-university/codal-nrf52/compare/2d77a3c89d14c5f4344e73511ca32086a0fcc8ad...099f15ab7932ca902976262b989890f0edb3b1ce))

 - Add more docs about updating nrfx submodule (#52) (by Carlos Pereira Atencio)

## [v0.2.61](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.60...v0.2.61)

 - Shorter reset to Bluetooth mode animation (#368) (by Martin Williams)
 - Added a hook to manually deactivate the microphone when we deep sleep (by Dr John Vidler)
 - Snapshot v0.2.60-master.0 (by Dr John Vidler)
 - Snapshot v0.2.60-master.1 (by Dr John Vidler)
 - Snapshot v0.2.61 (by Dr John Vidler)

### codal-core ([c2bc323...a116354](https://github.com/lancaster-university/codal-core/compare/c2bc323334e95bd7fc7e91bf64d782b575ce98ae...a1163541d10c0a0d3dd9b2e8ac1b8906100449ad))

 - Reverted to referencing the existing buffer, rather than an explicit copy constructor (by Dr John Vidler)
 - Lowered the default max recording size to 50k, to avoid OOMs with higher level languages (MakeCode, etc.) (by Dr John Vidler)

### codal-nrf52 ([4b2d834...2d77a3c](https://github.com/lancaster-university/codal-nrf52/compare/4b2d8342ea37a4fd13de340c0fff63526ea5465d...2d77a3c89d14c5f4344e73511ca32086a0fcc8ad))

 - [Clang Compatibility] Patch to define the .heap section as allocatable. (#51) (by Johnn333)

### codal-microbit-nrf5sdk ([ef4662e...f98e1a9](https://github.com/microbit-foundation/codal-microbit-nrf5sdk/compare/ef4662e13875a7b03e7296d7ac24a2b4d231f323...f98e1a9325acef9de52416b3e30cc820531b6699))

 - [Clang compatibility] A revised Clang patch. (#17) (by Johnn333)

## [v0.2.60](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.59...v0.2.60)

 - Fix issue when missing motion sensors crashed instead of panicking. (#360) (by Carlos Pereira Atencio)
 - Minor fix of seedRandom return value. (#369) (by Carlos Pereira Atencio)
 - Minor cleanup of config flags. (#363) (by Carlos Pereira Atencio)
 - radio: Reduce power level options to match V1 DAL. (#275) (by Carlos Pereira Atencio)
 - BLE: Simplify device name string creation. (#371) (by Carlos Pereira Atencio)
 - Snapshot v0.2.60 (by Dr John Vidler)

### codal-core ([32cc87c...c2bc323](https://github.com/lancaster-university/codal-core/compare/32cc87c3298a49663c3b3b7b1881ec7aeb3d73a5...c2bc323334e95bd7fc7e91bf64d782b575ce98ae))

 - Preprocessor Directives to ignore builtin clang function (by Johnn333)
 - Make AbstractButton::isPressed() pure virtual. (by Carlos Pereira Atencio)
 - Make all methods in NVMController pure virtual. (by Carlos Pereira Atencio)
 - Make motion sensors configure() & requestUpdate() pure virtual. (by Carlos Pereira Atencio)
 - Removed unused variable in CodalFiber (by Dr John Vidler)
 - Extended CodalAssert with an explicit failure case (for unimplmented functions, for example) and forced a DMESG flush after each to ensure we get serial output (by Dr John Vidler)
 - Added a set of C function hooks to patch holes in the Newlib API to squash the GCC v12+ warnings (by Dr John Vidler)
 - Added the weak attribute to all the nosys support functions, so folks can override them later (by Dr John Vidler)
 - Added gcc version guard to the source too, just in case (by Dr John Vidler)
 - Removed direct flush calls in favour of DMESGF (by Dr John Vidler)
 - Enforce that we always create a new buffer. Slower, but constantly-time slower (by Dr John Vidler)
 - StreamRecording now keeps track of the last received sample rate, and will report this rate except when it has no buffered data, when it will report the upstream rate. (by Dr John Vidler)

### codal-nrf52 ([2dbf5aa...4b2d834](https://github.com/lancaster-university/codal-nrf52/compare/2dbf5aa214c7432c0b1b8a19e92a3f9a4fc4b9da...4b2d8342ea37a4fd13de340c0fff63526ea5465d))

 - Apply workaround for nRF52833 errata 78 for timer high power consumption. (#49) (by Carlos Pereira Atencio)
 - Explicitly set SAADC channel config mode to single ended. (#50) (by Carlos Pereira Atencio)
 - [Clang compatibility] Update nrfx version to 2.1.0 (#48) (by Johnn333)

## [v0.2.59](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.58...v0.2.59)

 - Added an on-release action to grab library builds (by Dr John Vidler)
 - Reduce power consumption of LED matrix on power off mode. (#366) (by Carlos Pereira Atencio)
 - Snapshot v0.2.59 (by Carlos Pereira Atencio)

## [v0.2.58](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.57...v0.2.58)

 - CI: Add option to size diff workflow to configure samples repo commit. (#358) (by Carlos Pereira Atencio)
 - CI: Temporarily disable pxt docker build until image becomes available. (#359) (by Carlos Pereira Atencio)
 - MicroBitBLEManager - Move MICROBIT_BLE_MAXIMUM_BONDS to MicroBitConfig (#299) (by Martin Williams)
 - Snapshot v0.2.58 (by Carlos Pereira Atencio)

### codal-core ([2658178...32cc87c](https://github.com/lancaster-university/codal-core/compare/2658178aade744af248b116c2e4277b94a242b69...32cc87c3298a49663c3b3b7b1881ec7aeb3d73a5))

 - Compass::clearCalibration() resets calibration member variable (by Martin Williams)
 - Update touch code to release/reconnect on pin reuse (codal-microbit-v2#345) (by Joe Finney)

### codal-nrf52 ([84b1aee...2dbf5aa](https://github.com/lancaster-university/codal-nrf52/compare/84b1aeec870addcfb6027a924dbfe83f1f10c6db...2dbf5aa214c7432c0b1b8a19e92a3f9a4fc4b9da))

 - Update touch code to release/reconnect on pin reuse (codal-microbit-v2#345) (by Joe Finney)

## [v0.2.57](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.56...v0.2.57)

 - Disbled the MICROBIT_BLE_UTILITY_SERVICE_PAIRING configuration by default, enabled on BETA (by Dr John Vidler)
 - Snapshot v0.2.57 (by Dr John Vidler)

### codal-core ([763a2d1...2658178](https://github.com/lancaster-university/codal-core/compare/763a2d1a916d4db3fe3f3f2b1eaa9947cc8d0a7e...2658178aade744af248b116c2e4277b94a242b69))

 - Added a "fast path" route to reduce delays on getValue calls (by Dr John Vidler)
 - Added a new arg to FiberLock to run in legacy MUTEX mode, or SEMAPHORE mode with corrected semantics. (by Dr John Vidler)
 - Added missing public as part of declaration (by Dr John Vidler)

## [v0.2.56](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.55...v0.2.56)

 - Added a little extra to the git changelog generator; it now created a tree-link list for the libraries on the latest tag (by Dr John Vidler)
 - Remove FXOS vestiges. (#302) (by Carlos Pereira Atencio)
 - Removed unused include for the old LevelDetector, updated event handling for demand activation (by Dr John Vidler)
 - MicroBitUtilityService - replaces PR 178 (#287) (by Martin Williams)
 - Fixed a whitespace-based merge conflict! (by Dr John Vidler)
 - MicroBitCompassCalibrator avoid using max(int,int) (#290) (by Martin Williams)
 - CI uPy: Don't checkout submodules recursively. (#351) (by Carlos Pereira Atencio)
 - Added isMicrophoneEnabled() to MicroBitAudio to match isSpeakerEnabled (by Dr John Vidler)
 - Fixed #if logic (by Dr John Vidler)
 - Removed spurious duplicate documentation, added support for isMicrophoneEnabled (by Dr John Vidler)
 - Snapshot v0.2.56 (by Dr John Vidler)

### codal-core ([bf65ebe...763a2d1](https://github.com/lancaster-university/codal-core/compare/bf65ebed2003049bd026e1f685bb756e189c9258...763a2d1a916d4db3fe3f3f2b1eaa9947cc8d0a7e))

 - Added a lock to the initial access/events for the level detector, to prevent spurious values and events on cold-start (by Dr John Vidler)
 - Added note that this is actually a kind of hidden deep copy (by Dr John Vidler)
 - Added a note on stripping the least significant bits as part of LPF (by Dr John Vidler)
 - setGain and getGain now return floats, as they use fractional values internally (by Dr John Vidler)
 - StreamRecording objects now use a linked list of buffers for storage (by Dr John Vidler)
 - StreamSplitter now correctly connects to its upstream (by Dr John Vidler)
 - Added virtual to isConnected on DataSource to prevent internal misrouting (by Dr John Vidler)
 - Added extended assert support, requires CODAL flags to enable (by Dr John Vidler)
 - Extended DMESG with optional Fiber and Time markers (by Dr John Vidler)
 - Updated CodalFiber to correctly behave as a counting semaphore (by Dr John Vidler)
 - Corrected locking behaviour on the LevelDetectorSPL to avoid race conditions (by Dr John Vidler)
 - Updated documentation for StreamRecording (by Dr John Vidler)
 - Removed unused assert (by Dr John Vidler)
 - Refactored assert to remove assert_true and assert_false in favour of a single unified assert call (by Dr John Vidler)
 - Set the default FiberLock count to 1, as other APIs expect that the lock starts UNLOCKED rather than LOCKED (by Dr John Vidler)

## [v0.2.55](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.54...v0.2.55)

 - Added additional workflow status badges (by Dr John Vidler)
 - Fix BLE panic 071 (#334) (by Martin Williams)
 - Snapshot v0.2.55 (by Dr John Vidler)

### codal-nrf52 ([76dbf90...84b1aee](https://github.com/lancaster-university/codal-nrf52/compare/76dbf9088a3dc2e273e3c37134dc661985aac609...84b1aeec870addcfb6027a924dbfe83f1f10c6db))

 - Fix NeoPixel/WS2812B timing. (#47) (by Jim Mussared)

## [v0.2.54](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.53...v0.2.54)

 - Update update-changelog.yml (by Dr John Vidler)
 - Snapshot v0.2.54 (by Dr John Vidler)

### codal-core ([94cbd39...bf65ebe](https://github.com/lancaster-university/codal-core/compare/94cbd3933c61924677fc1f2255eaf61bd175a0d1...bf65ebed2003049bd026e1f685bb756e189c9258))

 - Replaced an invalid schedule() call with a fiber_sleep for the synchronous record and playback calls. (by Dr John Vidler)

## [v0.2.53](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.52...v0.2.53)

 - Added an automated changelog script and github action, for testing (by Dr John Vidler)
 - Added an automated changelog script and github action, for testing (by Dr John Vidler)
 - CI MakeCode: Use the default pxt-core and pxt-common-packages from package.json (#308) (by Carlos Pereira Atencio)
 - Improve sound quality of virtual sound pin (#312) (by Joe Finney)
 - !!BREAKING CHANGE!! - The old level detector is now removed, in favour of the SPL level detector. Users who need the old one must create their own (by Dr John Vidler)
 - Snapshot v0.2.53 (by Dr John Vidler)

### codal-core ([a1e1db6...94cbd39](https://github.com/lancaster-university/codal-core/compare/a1e1db692909a260ab341c89b292e085daecd259...94cbd3933c61924677fc1f2255eaf61bd175a0d1))

 - Added functionality described in issue #326, removed duplicate documentation in cpp. (by Dr John Vidler)
 - Adjusted CODAL_STREAM_IDLE_TIMEOUT_MS down to 75ms - Can be adjusted in codal.json if required by higher-level applications (by Dr John Vidler)
 - - Added async and sync variants of play and record, for convenience - Removed unusued bool and check for timeouts in getValue (by Dr John Vidler)

## [v0.2.52](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.51...v0.2.52)

 - Update README.md with status badges (by Dr John Vidler)
 - Added volume and sample rate control for MixerChannels, added documentation to match (by Dr John Vidler)
 - Implemented new API requirements for Mixer2, Synth and virtual output pins (by Dr John Vidler)
 - Snapshot v0.2.52 (by Dr John Vidler)

### codal-core ([4b2b35e...a1e1db6](https://github.com/lancaster-university/codal-core/compare/4b2b35e3b0db84d62a4cf3d3b582da66974793fb...a1e1db692909a260ab341c89b292e085daecd259))

 - Updated streams API to include connection checks by API requirement, Fixed the level detectors to correctly handle initial connections to their upstream (by Dr John Vidler)

### codal-nrf52 ([5fa1d16...76dbf90](https://github.com/lancaster-university/codal-nrf52/compare/5fa1d16d6de6bd2848ae776e3039ce6bcad4e3e1...76dbf9088a3dc2e273e3c37134dc661985aac609))

 - Updated the ADC channels to use the new requirements for stream API classes (by Dr John Vidler)

## [v0.2.51](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.50...v0.2.51)

 - Change yotta module.json dependencies to foundation org. (by microbit-carlos)
 - Revert "Update target name to reflect micro:bit version number" (by microbit-carlos)
 - CI PXT: Restrict Yotta dependencies as workaround to pip install errors. (#288) (by Carlos Pereira Atencio)
 - CI: Ensure necessary repos are cloned with submodules. (#292) (by Carlos Pereira Atencio)
 - Add UX feature: Triple-tap-reset to enter pairing mode (by Joe Finney)
 - Generalize NOINIT region (by Joe Finney)
 - Remove target.json DEVICE_DMESG_BUFFER_SIZE to use default (#298) (by Carlos Pereira Atencio)
 - Updated MicroBitAudio and the Mixer to correctly work with the new streams API (by Dr John Vidler)
 - Reverted a change to the project name which broke the build entirely (by Dr John Vidler)
 - Snapshot v0.2.51 (by Dr John Vidler)

### codal-core ([350a794...4b2b35e](https://github.com/lancaster-university/codal-core/compare/350a794a6f43d8e91b4762f110bad675169dc908...4b2b35e3b0db84d62a4cf3d3b582da66974793fb))

 - Minor bugfixes to historic branch. (by Joe Finney)
 - Add config option for printf (by James Devine)
 - remove warnings (by James Devine)
 - Remove type punned pointer warning (by Joe Finney)
 - DMESG cleanup (by Joe Finney)
 - Make update() virtual (by Joe Finney)
 - Add variant of readRegister() method (by Joe Finney)
 - Parameterise Active Hi/Lo functionality on IRQ line. (by Joe Finney)
 - Fix typos in comments (by Joe Finney)
 - Introduce LSM303 Combined Acceleromter/Magnetometer Driver (by Joe Finney)
 - Fix Coordinate System Translations (by Joe Finney)
 - Make FXOS8700 driver compatible with 32 bit Sample3D structure (by Joe Finney)
 - Introduce FXOS8700 and LSM303 support for shared IRQ lines (by Joe Finney)
 - Introduce Compass::setAccelerometer() method (by Joe Finney)
 - Introduce ACTIVE_HI / ACTIVE_LO functionality for generic Pin (by Joe Finney)
 - Updates sensor drivers to use Pin::isActive() (by Joe Finney)
 - Upstream improved compass calibration model (by Joe Finney)
 - Introduce isDetected() method into FXOS8700 driver (by Joe Finney)
 - Ensure variables are fully initialized (by Joe Finney)
 - Add NOP definitions for DMESGN() and DMESGF() (by Joe Finney)
 - prevent timer underflow caused by very short wait periods (by Joe Finney)
 - remove enable/disable irq from printf. (by James Devine)
 - Introduce MemorySource (by Joe Finney)
 - Parameterise constructor to MemorySource (by Joe Finney)
 - All virtual classes need a virtual destructor (by Michal Moskal)
 - yotta: Add yotta compatibility (by Raphael Gault)
 - Remove legacy FXOS8700Accelerometer class (by Joe Finney)
 - Set default DEVICE_DMESG_BUFFER_SIZE based on DMESG_ENABLE. (#160) (by Carlos Pereira Atencio)
 - Updated DataStream to support decoupling the input fiber from the output to enable async stream data (by Dr John Vidler)
 - Updated StreamRecording to use the new PR/Pull metrics, removed old debug code (by Dr John Vidler)
 - Removed unused events, patched against the new PR/Pull metrics, removed spurious debug output, removed old debug code (by Dr John Vidler)
 - EffectFilters now correctly create deep copies of ManagedBuffers marked readOnly before editing their contents (by Dr John Vidler)
 - Updated FIFOStream to match the new PR/Pull metrics (by Dr John Vidler)
 - Inserted missing setBeta() call in constructor to initialise the object beta value (by Dr John Vidler)
 - Removed old debug code, updated constructor signature (internal behavioural change, no caller updates needed), component now correctly uses PR/Pull metrics (by Dr John Vidler)
 - Corrected pullRequest return values based on component loading (by Dr John Vidler)
 - Corrected pullRequest return values based on component loading (by Dr John Vidler)

## [v0.2.50](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.49...v0.2.50)

 - CI: Enable manual triggers for size diff workflow. (#282) (by Carlos Pereira Atencio)
 - Introduce sound playback "until playback complete" option (#231) (by Joe Finney)
 - Fixed the stream wiring in MicroBitAudio, and updated the related classes to align to the incoming stream API updates (by Dr John Vidler)
 - Updated the readme with a little on the tag/release policy (by Dr John Vidler)
 - Added more info, per Carlos suggestion in the issue (by Dr John Vidler)
 - Snapshot v0.2.50 (by Dr John Vidler)

### codal-core ([bf23caa...350a794](https://github.com/lancaster-university/codal-core/compare/bf23caab5e19b055bca8718f7bb52a763d077ff7...350a794a6f43d8e91b4762f110bad675169dc908))

 - Introduce Pin Disconnecting flag (by Joe Finney)
 - Prevent fibers from momopolising access to AnimatedDisplay (#253) (by Joe Finney)
 - Revert commit 434f12e52b9e551c96422ef76e6f9f3c5f6fa803 (#253) (by Joe Finney)
 - Only wipe specified region on large ManagedBuffer::shift (#157) (by C272)
 - add rms based clap detection to level detector spl (by Oliver Hulland)
 - made lower threshold once in clap to consider noise over. allows better isolation of non clap noises (longer noises for example) (by Oliver Hulland)
 - refactor, fix short taps - now register less and don't allow fast subsequent ones (by Oliver Hulland)
 - Remove DMESG and add comments (by Oliver Hulland)
 - Reworked and updated the streams API; still a fair amount of debug code in here that will need cleaning out before tag/release (by Dr John Vidler)
 - Added a Doxygen skip rule for ManagedString "+" operators, as sphinx/breathe/exhale cant deal with this structure yet (by Dr John Vidler)
 - Completed the fix for this issue: https://github.com/lancaster-university/codal-microbit-v2/issues/278 (by Dr John Vidler)
 - Added initial values for lpf_value and lpf_beta (by Dr John Vidler)

### codal-nrf52 ([14bcffe...5fa1d16](https://github.com/lancaster-university/codal-nrf52/compare/14bcffe406a044e50d89616b97bd3757d0a00e82...5fa1d16d6de6bd2848ae776e3039ce6bcad4e3e1))

 - Prevent recursive disconnet loops in NRF52Pin and accidental erasure of obj state (by Joe Finney)

## [v0.2.49](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.48...v0.2.49)

 - MicroBitRadio: fix setFrequencyBand() and do not reset tx power and frequency band in enable() (#252) (by Martin Williams)
 - docstrings: Add granularity info to MicroBit::sleep() & remove return. (#248) (by Carlos Pereira Atencio)
 - docstrings: Minor fix in documented define name. (#246) (by Carlos Pereira Atencio)
 - Simplify the BLE - Device Info - Model Number to only indicate V2. (#255) (by Carlos Pereira Atencio)
 - CI: Add build workflow using microbit-v2-samples. (#256) (by Carlos Pereira Atencio)
 - Added a small patch to confirm that all splitter channels have gone before turning the microphone off (by Dr John Vidler)
 - CI: Workflow to calculate build size diff before & after PR/commit. (#262) (by Carlos Pereira Atencio)
 - CI: Build MicroPython on each push. (#276) (by Carlos Pereira Atencio)
 - CI: Build MakeCode with and without docker on each commit. (#274) (by Carlos Pereira Atencio)
 - Added information on demand-wake for the audio pipeline and fixed the wake-up sequence (by Dr John Vidler)
 - Snapshot v0.2.49 (by Dr John Vidler)

### codal-core ([75b92a9...bf23caa](https://github.com/lancaster-university/codal-core/compare/75b92a9ef3a206cdc63f65576c03e2aa72b4a45b...bf23caab5e19b055bca8718f7bb52a763d077ff7))

 - StreamSplitters now emit finer-grained events to enable nuanced handling of pipeline state changes, and also perform channel teardown if a downstream component fails to pull for too long to prevent stalling the pipeline. (by Dr John Vidler)
 - Added missing virtual attribute for the SplitterChannel destructor (by Dr John Vidler)

## [v0.2.48](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.47...v0.2.48)

 - Swapped the ZIP_LISTS implementation for a plain CMake one, to allow builds on older CMake versions. Also only now halting the build if SOFTDEVICE_PRESENT is asserted with any value and DEVICE_BLE is not 1, and would therefore be an invalid, unbootable configuration (by Dr John Vidler)
 - Snapshot v0.2.48 (by Dr John Vidler)

### codal-core ([a45ede4...75b92a9](https://github.com/lancaster-university/codal-core/compare/a45ede432a646a0af86fbd89d9dd692689fb6fbd...75b92a9ef3a206cdc63f65576c03e2aa72b4a45b))

 - Ensure AnimatedDisplay::waitForFreeDisplay() is free of race conditions (#140) (by Martin Williams)

## [v0.2.47](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.46...v0.2.47)

 - Builds will now error if the old SOFTDEVICE_PRESENT flag is asserted by the target or user config. Please use DEVICE_BLE instead. (by Dr John Vidler)
 - Snapshot v0.2.47 (by Dr John Vidler)

## [v0.2.46](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.45...v0.2.46)

 - Introduce audio output isplaying() API (by Joe Finney)
 - Fix miscalculation in output latency (by Joe Finney)
 - Snapshot v0.2.46 (by Dr John Vidler)

## [v0.2.45](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.44...v0.2.45)

 - Improve temporal response of SoundOutputPin::setAnalogValue() (by Joe Finney)
 - Introduce convenience functions (by Joe Finney)
 - Introduce egress double buffering onto SoundEmojiSynth (by Joe Finney)
 - Moved away from defining SOFTDEVICE_PRESENT directly in the target. If you require bluetooth, please define DEVICE_BLE as 1 in your codal.json (by Dr John Vidler)
 - Re-enabled bluetooth by default, and it can be turned off via DEVICE_BLE being set to 0 in codal.json (by Dr John Vidler)
 - Updated methods to align with the sample rate changes in -core (by Dr John Vidler)
 - Snapshot v0.2.45 (by Dr John Vidler)

### codal-core ([f08eb30...a45ede4](https://github.com/lancaster-university/codal-core/compare/f08eb30d75d0a786229344431177aa0e1c67c0a3...a45ede432a646a0af86fbd89d9dd692689fb6fbd))

 - First build (by Joe Finney)
 - Introduce PinPeripheral::pinLock (by Joe Finney)
 - Remove typos/debug (by Joe Finney)
 - First build (by Joe Finney)
 - Introduce PinPeripheral::pinLock (by Joe Finney)
 - Remove typos/debug (by Joe Finney)
 - Added comment to mark ID range 80-99 as reserved (by Dr John Vidler)
 - Added a safe ID area, and a static CodalComponent method for generating safe dynamic runtime component IDs (by Dr John Vidler)
 - Added sample rate support to the stream classes (by Dr John Vidler)

### codal-nrf52 ([92341a8...14bcffe](https://github.com/lancaster-university/codal-nrf52/compare/92341a800190536a6eb9df186864a49f9e7ce2da...14bcffe406a044e50d89616b97bd3757d0a00e82))

 - First build (by Joe Finney)
 - Implement PinPeripheral::pinLock APIs for dynamically allocated NRF52 peripherals (by Joe Finney)
 - Bugfixes following lab bench testing of peripheral switching (by Joe Finney)
 - Remove typos, dead code and debug (by Joe Finney)
 - First build (by Joe Finney)
 - Implement PinPeripheral::pinLock APIs for dynamically allocated NRF52 peripherals (by Joe Finney)
 - Bugfixes following lab bench testing of peripheral switching (by Joe Finney)
 - Remove typos, dead code and debug (by Joe Finney)
 - Remove implicit assumption to use high drive mode on SPI pins (by Joe Finney)
 - Added sample rate support to the ADC (by Dr John Vidler)

## [v0.2.44](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.43...v0.2.44)

 - First build of codal-microbit-v2 without softdevice (by Joe Finney)
 - First build of codal-microbit-v2 without softdevice (by Joe Finney)
 - Updated CMake to read the SOFTDEVICE_PRESENT flag and switch linker script as required. (by Dr John Vidler)
 - Added a default for SOFTDEVICE_PRESENT to enabled, to preserve existing behaviour (by Dr John Vidler)
 - Fix typo in MicroBitConfig.h closing an #ifndef. (#239) (by Carlos Pereira Atencio)
 - Re-added old MBED based IDs as deprecated-marked integers. Values have changed, avoid using these over CODAL official IO (by Dr John Vidler)
 - Added support for SET_ prefix for configurations to enable/disable -D style gcc flags, to enable softdevice removal from the build (by Dr John Vidler)
 - Snapshot v0.2.44 (by Dr John Vidler)

## [v0.2.43](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.42...v0.2.43)

 - NRF52LEDMatrix::rotateTo (by Martin Williams)
 - Removed a small delay in the emoji synth to fix a Python issue (by John Vidler)
 - Snapshot v0.2.43 (by John Vidler)
 - Rolled back a tag, commits were accidentally included (by John Vidler)
 - Removed a small delay in the emoji synth to allow HLLs to run correctly (by John Vidler)
 - Snapshot v0.2.42-hotfix.0 (by John Vidler)
 - Snapshot v0.2.42-hotfix.1 (by John Vidler)
 - Serial ports can now be used to wake the board from deep sleep states. (by John Vidler)
 - Enabled the deep-sleep wake functionality on the USB serial port by default. (by John Vidler)
 - Removed checks for wake sources when asked to perform a deep sleep. By policy, we want to sleep anyway, and we should normally have a serial port wake source (by John Vidler)
 - Removed dead code (by Dr John Vidler)
 - Fixed typo (by Dr John Vidler)
 - Added a new configuration flag MICROBIT_USB_SERIAL_WAKE, which when set to 1 will cause the board to wake on serial data over USB (by Dr John Vidler)
 - Removed spurious DMESGF in MicroBit.cpp (by Dr John Vidler)
 - Removed unused MBED IDs (by Dr John Vidler)
 - Moved to using CodalComponent.h constants for IDs to avoid conflicts. CodalComponent.h is the source of truth for this (by Dr John Vidler)
 - Snapshot v0.2.43 (by Dr John Vidler)

### codal-core ([d376f1a...f08eb30](https://github.com/lancaster-university/codal-core/compare/d376f1a851457effaf34058c62bd6c42c5ada6a0...f08eb30d75d0a786229344431177aa0e1c67c0a3))

 - Moved IDs to CodalComponent from MicroBitCompat over in the v2 repo, this we we have a single source of true IDs to avoid overlaps. Future changes should either use this, or come up with a better system (by Dr John Vidler)

### codal-nrf52 ([f169008...92341a8](https://github.com/lancaster-university/codal-nrf52/compare/f169008a35e69171aa60761a17bd4917be9dee08...92341a800190536a6eb9df186864a49f9e7ce2da))

 - Support for toggleable deep sleep modes for UART (by Dr John Vidler)

## [v0.2.42](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.41...v0.2.42)

 - Inserted a small delay into playback to account for the last buffer leaving the synth before emitting a compelte event (by John Vidler)
 - Added a simple guard to setSampleRate to inhibit rates under 1 sample/sec (by John Vidler)
 - Added a deepSleep variant which does not respect the remaining wait time if woken prematurely, to better support Python (by John Vidler)
 - Added correct return data, and updated the documentation (by John Vidler)
 - Added missing return semantics documentation (by John Vidler)
 - Fixed off-by-one error for the panic codes (by John Vidler)
 - Updated the README to point to the microbit-v2-samples repository (by John Vidler)
 - Improve accuracy and sensitivity of microphone level detector (by Joe Finney)
 - Added an error code for DAPLink/USB Interface errors (by John Vidler)
 - Snapshot v0.2.42 (by John Vidler)

### codal-core ([5af239e...d376f1a](https://github.com/lancaster-university/codal-core/compare/5af239edccccdc6e9df1d4281101093375a70a63...d376f1a851457effaf34058c62bd6c42c5ada6a0))

 - Replaced off-by-one error in the lock that blockd one fiber more than expected when initialised with the new constructor (by John Vidler)
 - fiber fix for recent GCC (by Michal Moskal)
 - Refactor StreamFilter class (by Joe Finney)
 - Optimize LevelDetectorSPL class (by Joe Finney)

## [v0.2.41](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.40...v0.2.41)

 - Add the DAPLink version to the data logging HTM page. (by Carlos Pereira Atencio)
 - datalog html: Document, fix, update, and create new test data (#197) (by Carlos Pereira Atencio)
 - Bug/flash configuration (#205) (by Dr John Vidler)
 - datalog htm: Fix unintuitive error msg when trying to plot with empty data. (#198) (by Carlos Pereira Atencio)
 - Inserted a small delay into playback to account for the last buffer leaving the synth before emitting a compelte event (#210) (by Dr John Vidler)
 - Bug/sound emoji timing (#215) (by Dr John Vidler)
 - Added a new error class which uses PanicCode as a base for Micro:bit v2 specific errors, and implemented these in the Accel and Compass classes (by John Vidler)
 - MicroBitLog now only checks the version string preamble, to avoid bugs where old logs are not detected during an erase cycle (by John Vidler)
 - Snapshot v0.2.41 (by John Vidler)

### codal-core ([4e1d131...5af239e](https://github.com/lancaster-university/codal-core/compare/4e1d1311b2115b14e6d7e08d60e3d14f4ca0a9e5...5af239edccccdc6e9df1d4281101093375a70a63))

 - Updated the serial send() ASYNC mode to correctly follow the spec in the documentation. Updated the documentation with further information on the behaviour (by John Vidler)
 - mark system_timer_current_time_us() as real time (used in Event()) (by Michal Moskal)
 - Add USB_EP_FLAG_ASYNC and UsbEndpointIn::canWrite() (by Michal Moskal)
 - make previous change less intrusive (by Michal Moskal)
 - DMESG() on event queue overflow (by Michal Moskal)
 - Added a semaphore mode to the FiberLock, for multiple access to limited shared resources. Updated documentation to remove double-annotated functions (by John Vidler)
 - Reverted to the break behaviour to preserve the tx interrupt (by John Vidler)
 - Enforced serial RX pullup on a redirect, to match micro:bit v1.xx behaviour (by John Vidler)
 - Removed DEVICE_USB_ERROR as its never used, and repurposed it for the base error address DEVICE_PERIPHERAL_ERROR (by John Vidler)

### codal-nrf52 ([ac01d74...f169008](https://github.com/lancaster-university/codal-nrf52/compare/ac01d74dc13236beddde78a5dab23c9e41b42c23...f169008a35e69171aa60761a17bd4917be9dee08))

 - Minor cleanup (whitespace) (by John Vidler)

## [v0.2.40](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.39...v0.2.40)

 - PowerManager now includes a new getPowerData to correctly handle the I2C power data, and getPowerConsumption is now a deprecated proxy for this so we dont immediately break compatability with other existing code (by Dr John Vidler)
 - Add the DAPLink version to the data logging HTM page. (#196) (by Carlos Pereira Atencio)
 - Add the DAPLink version to the data logging HTM page. (#199) (by Dr John Vidler)
 - Add MicroBitLog::readData (#177) (by Martin Williams)
 - Snapshot v0.2.40 (by Dr John Vidler)

### codal-core ([db201e9...4e1d131](https://github.com/lancaster-university/codal-core/compare/db201e94c6a2767e881ba4457c4087eb34312603...4e1d1311b2115b14e6d7e08d60e3d14f4ca0a9e5))

 - Altered ASYNC behaviour to now deschedule the current fiber for zero ticks if the txbuffer is full to allow the hardware to drain (by Dr John Vidler)

## [v0.2.39](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.38...v0.2.39)

 - Added missing end frequency parameter to the log generator function (by Dr John Vidler)
 - Removed redundant CodalDmesg.h include (by Dr John Vidler)
 - Removed redundant CodalDmesg.h include (by Dr John Vidler)
 - Add polyphonic real-time synthesizer. (#185) (by preglow)
 - Fix MicroBitBLEManager support for MicroBitPowerManager::deepSleep  (#155) (by Martin Williams)
 - Minor fix to read back the right amout of I2C data from the interface mcu. (#159) (by Carlos Pereira Atencio)
 - Removed old driver (no longer used) and simplified the interface (now just a thin proxy), but this requires further squashing in future releases (#192) (by Dr John Vidler)
 - Zeroed out the processorTemperature to squash an uninitialized variable compiler warning (by Dr John Vidler)
 - Added support for the BUSY flag on the KL27 (#190) (by Dr John Vidler)
 - Snapshot v0.2.39 (by Dr John Vidler)

### codal-core ([befa2c5...db201e9](https://github.com/lancaster-university/codal-core/compare/befa2c51b3595ca0d44e1bc5daad0531974b77e7...db201e94c6a2767e881ba4457c4087eb34312603))

 - Removed incorrect documentation block, pending replacement (by Dr John Vidler)

## [v0.2.38](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.37...v0.2.38)

 - Increase DAPLink I2C wait for data to be ready timeout. (#166) (by Carlos Pereira Atencio)
 - Snapshot v0.2.38 (by Dr John Vidler)

## [v0.2.37](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.35...v0.2.37)

 - Changed variable sampleRate to float to avoid rounding errors when generating frequencies. (by Joshua Hill)
 - [input pipeline] Moved initialisation of mic ADC channel, stream normaliser, splitter into microbitaudio. Moved initialisation of level detector to microbitaudio. Created events to enable and disable channel, called from splitter (by Joshua Hill)
 - [input pipeline] formatting (by Joshua Hill)
 - Changes according to review (by Joshua Hill)
 - [input pipeline] Changes acording to review (by Joshua Hill)
 - [input pipeline] Remove event from function paramaters - bug fix (by Joshua Hill)
 - [input-pipeline] Added now-required param to disable connect on start (by Joshua Hill)
 - [inputpipeline] Set default SPL level to not be active on start (by Joshua Hill)
 - [input-pipeline] Changed event to capture DEVICE_ID_MICROPHONE which is used for LevelDetectorSPL in legacy code (by Joshua Hill)
 - [input-pipeline] Removed debug messages (by Joshua Hill)
 - [input-pipeline] bug fix for new level detector init in microbit audio (by Joshua Hill)
 - setting up 0.2f 8-bit pipeline ready for future work (by Joshua Hill)
 - Create README.md (by Joshua Hill)
 - change microbit audio macro to use new uBit objects (by Joshua Hill)
 - Added method to set microphone gain, changed gain to 0.01 (by Joshua Hill)
 - [input-pipeline] Altered input pipeline gain scaling for 8bit (by Joshua Hill)
 - [input-pipeline] Tweaked gain to maintain legacy functionality (by Joshua Hill)
 - Snapshot v0.2.33-input_pipeline.0 (by Dr John Vidler)
 - Added a get/set volume call for audio channels in the mixer (by Dr John Vidler)
 - Added a minimal library definition for documentation generation (by Dr John Vidler)
 - Added new disable() call, and fixed some formatting issues (by Dr John Vidler)
 - Added per-channel volume control methods (by Dr John Vidler)
 - Snapshot v0.2.37 (by Dr John Vidler)

### codal-core ([6d3b410...befa2c5](https://github.com/lancaster-university/codal-core/compare/6d3b41000a6e3437a432a8edb1aedf9f49d75e57...befa2c51b3595ca0d44e1bc5daad0531974b77e7))

 - [input pipeline] Added input pipeline files (by Joshua Hill)
 - [input-pipeline] Fixed up old projects not working with new architecture (by Joshua Hill)
 - [input-pipeline] Safer connect immediately check (by Joshua Hill)
 - Updated Level Detector to be 8bit following change to stream normaliser in Microbit Audio to be 8bit. This will have effected the level detector values. (by Joshua Hill)
 - added component ID for audio processor (by Joshua Hill)
 - added debug (by Joshua Hill)
 - added component IDS for tap detector and FFT to register events (by Joshua Hill)
 - adjusted output scaling for 8-bit input (multiplier of 8) (by Joshua Hill)
 - quick adjustment to work with 8 bit values (-30 done by eye) (by Joshua Hill)
 - Added support 8 bit input (by Joshua Hill)
 - removed comments (by Joshua Hill)
 - Remove debug (by Joshua Hill)
 - add disable method (by Joshua Hill)
 - Updated SPL Level detector to support 8 bit (by Joshua Hill)
 - proper scaling of different data types (by Joshua Hill)
 - Removed debug (by Joshua Hill)
 - Make StreamSplitter more tolerant of late joining downstreams (by Joe Finney)
 - Added a minimal library definition for documentation generation (by Dr John Vidler)
 - fix subclass/protocol for hidjoystick (by Michal Moskal)
 - Added new FIFO, Recording and Filtering (audio) objects (by Dr John Vidler)
 - Included (experimental) stream data trigger object (by Dr John Vidler)

### codal-nrf52 ([0bc79cb...ac01d74](https://github.com/lancaster-university/codal-nrf52/compare/0bc79cb0232b889d7ef63fb08db358cac5104c48...ac01d74dc13236beddde78a5dab23c9e41b42c23))

 - [input pipeline] Seperated Channel activation to allow creation of deactivated channels (by Joshua Hill)
 - [input-pipeline] Changes after review (by Joshua Hill)
 - Passive implementation of NRF52ADCChannel::getSample() (by Joe Finney)
 - Protect DMA buffer acqiuisition against race conditions (by Joe Finney)
 - Reset DMA pointer when restarting the NRF52ADC (by Joe Finney)
 - Ensure NRF52ADC restarts deterministically when new channels are added (by Joe Finney)
 - Defer increment of enabledChannels until after new channel is added (by Joe Finney)
 - Stop NRF52ADC before adding channels (by Joe Finney)
 - [input pipeline] Changes to activateChannel docs according to review (by Joshua Hill)
 - NRF52ADC - move configuration changes out of IRQ handler (by Martin Williams)
 - NRF52ADC::stopRunning() - wait for not busy (by Martin Williams)
 - NRF52833 - Anomaly 212 WIP (by Martin Williams)
 - NRF52ADC - Configure channel gain in startRunning (by Martin Williams)
 - NRF52ADCChannel::setGain - configure the SAADC (by Martin Williams)
 - NRF52I2C::waitForStop: recover from hang (by Martin Williams)
 - Remove most added DMESGs (by Martin Williams)
 - Update comment (by Martin Williams)
 - Wait longer for RESUME/STOP (by Martin Williams)
 - NRF52I2C: revert debug code; add timeout configs (by Martin Williams)
 - Added a minimal library definition for documentation generation (by Dr John Vidler)

## [v0.2.35](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.34...v0.2.35)

 - Add MicroBitPowerManager::readInterfaceRequest() (by Martin Williams)
 - Snapshot v0.2.34 (by Dr John Vidler)
 - Snapshot v0.2.35 (by Dr John Vidler)
 - Snapshot v0.2.36 (by Dr John Vidler)
 - Corrected version to v0.2.35 (by Dr John Vidler)

### codal-core ([e2efc4c...6d3b410](https://github.com/lancaster-university/codal-core/compare/e2efc4cb034b04e5db5d7f6669667194b4a49024...6d3b41000a6e3437a432a8edb1aedf9f49d75e57))

 - Remove sample endpointRequest() impl   (crashes when no out endpoint) (by Michal Moskal)
 - Don't crash when not initialized (by Michal Moskal)
 - Add REAL_TIME_FUNC attribute (by Michal Moskal)
 - Put REAL_TIME_FUNC on functions that disable IRQs or are used from ISRs (by Michal Moskal)
 - Use new FORCE_RAM_FUNC attribute (by Michal Moskal)
 - Shorten IRQ-disabled period in DMESG (by Michal Moskal)
 - Fix warnings (by Michal Moskal)
 - Mark more functions real-time (ones used from ISRs) (by Michal Moskal)
 - FORCE_RAM_FUNC now correctly uses .data.ramfuncs rather than plain .data, to work with newer gcc versions. See issue #33 at https://github.com/lancaster-university/microbit-v2-samples/issues/33 (by Dr John Vidler)

### codal-nrf52 ([e1c428a...0bc79cb](https://github.com/lancaster-university/codal-nrf52/compare/e1c428ab031bc7114e72ee50a44baabae50c113b...0bc79cb0232b889d7ef63fb08db358cac5104c48))

 - Add case for 31250 in switch(baudrate) (#39) (by CaptainCredible)

## [v0.2.34](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.33...v0.2.34)

 - Update MicroBitLog serial mirroring to emit CRLF (#136) (by Joe Finney)
 - Option for Nordic Default UART characteristics (#139) (by Paul Austin)
 - BLE - make Device Info Service Model Number String sensitive to board ID (#114) (by Martin Williams)
 - Fix typo in MicroBitLog units (by Joe Finney)
 - Increase resolution of SoundOutputPin playback (#42) (by Joe Finney)
 - Update to DAPLink I2C Protocol v2 (by Joe Finney)
 - Implement fixed size journal and metadata regions (by Joe Finney)
 - Unify available off-chip flash (by Joe Finney)
 - Introduce delay following FLASH_ERASE request (by Joe Finney)
 - Remove verbose debug (by Joe Finney)
 - Align DAPLink v1/v2 workarounds (by Joe Finney)

## [v0.2.33](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.32...v0.2.33)

 - Ensure mutual exclusion for all MicroBitLog APIs (by Joe Finney)
 - Introduce workaround for back-to-back I2C Flash transactions using KL27 DAPLink firmware (by Joe Finney)
 - Move to runtime configuration of I2C DAPLink interface (by Joe Finney)
 - Snapshot v0.2.33 (by Joe Finney)

## [v0.2.32](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.31...v0.2.32)

 - Stabilize I2C communications between NRF52 and KL27 DAPLINK chip when running on battery power (#130) (by Joe Finney)
 - Improvements and optimizations to MicroBitLog (by Joe Finney)
 - Snapshot v0.2.32 (by Joe Finney)

### codal-core ([111fc68...e2efc4c](https://github.com/lancaster-university/codal-core/compare/111fc68a06a6d0b05c3e8551890dc11f014c1dc9...e2efc4cb034b04e5db5d7f6669667194b4a49024))

 - Optimize and fix logical error in inline ManagedString != method (by Joe Finney)

### codal-nrf52 ([3bb82f2...e1c428a](https://github.com/lancaster-university/codal-nrf52/compare/3bb82f2f1692f8b08b0ee424f0cb60224028d6a8...e1c428ab031bc7114e72ee50a44baabae50c113b))

 - NRF52I2C: Introduce transaction timeout (by Joe Finney)

## [v0.2.31](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.30...v0.2.31)

 - Introduce check for out of date bootloader settings page. (by Joe Finney)
 - Snapshot v0.2.31 (by Joe Finney)

## [v0.2.30](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.29...v0.2.30)

 - Prep work for adding visual preview. (#126) (by Matt Hillsdon)
 - Update datalog microbit.org URLs. (#129) (by Matt Hillsdon)
 - Add a "Visual preview" graph on online mode. (#127) (by Matt Hillsdon)
 - Update Data Logging HTML - Add a warning for IE users. (#133) (by Matt Hillsdon)
 - Update data Logging HTML - Avoid overflowing graphWrapper when error shown. (#132) (by Matt Hillsdon)
 - Update Bluetooth bootloader (#134) (by Joe Finney)
 - Parameterize RX/TX retransmission counters for NRF52->DAPLINK communications (by Joe Finney)
 - Snapshot v0.2.30 (by Joe Finney)

## [v0.2.29](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.28...v0.2.29)

 - MicroBitUARTService::send - fix for sending >20 bytes (#106) (by Martin Williams)
 - MicroBitAudio - make private pin variable a pointer (by Martin Williams)
 - Logfs serial mirroring (#122) (by Joe Finney)
 - Deep sleep 2 (#103) (by Martin Williams)
 - MicroBitAudio: Fix reference/pointer inconsistency in doSleep() (by Joe Finney)
 - Fix reset of Copy button (#118) (by Matt Hillsdon)
 - Add microbit.org-style header to MicroBitLog HTML page (by Matt Hillsdon)
 - Fix memory leak in MicroBitAudio::setSleep() / enable() (by Martin Williams)
 - Introduce automatic MY_DATA.HTM file visibility in MicroBitLog (by Joe Finney)
 - Snapshot v0.2.29 (by Joe Finney)

### codal-core ([1076c9a...111fc68](https://github.com/lancaster-university/codal-core/compare/1076c9a4388809a4e2c262d62b0064108066ab19...111fc68a06a6d0b05c3e8551890dc11f014c1dc9))

 - Deep sleep 2 (#138) (by Martin Williams)

### codal-nrf52 ([31a06cf...3bb82f2](https://github.com/lancaster-university/codal-nrf52/compare/31a06cf34da0d5bc3e5e82c3245bfb8e320fcb09...3bb82f2f1692f8b08b0ee424f0cb60224028d6a8))

 - Deep sleep 2 (#37) (by Martin Williams)

## [v0.2.28](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.27...v0.2.28)

 - Introduce MICROBIT_LOG_EVT_LOG_FULL event (by Joe Finney)
 - Snapshot v0.2.28 (by Joe Finney)

## [v0.2.27](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.26...v0.2.27)

 - Introduce MicroBitLog HTML+JS header resources (by Joe Finney)
 - Improvements to MicroBitLog (by Joe Finney)
 - Reduce granularity of MicroBitLog journalling (by Joe Finney)
 - Update HTML+JS header to parse updated journal format (by Joe Finney)
 - Leave sentinel word at the end of MicroBitLog flash space (by Joe Finney)
 - MicroBitLogFS improvements when log is full (by Joe Finney)
 - Updates to MicroBitLog to add implicit beginRow() / endRow() where possible (by Joe Finney)
 - Improve MicroBitLog full error reporting (by Joe Finney)
 - Minor improvements to MicroBitLog (by Joe Finney)
 - Performance tuning configuration of MicroBitLog (by Joe Finney)
 - MicroBitLog: Introduce online mode, iframe-reload, styles (#111) (by Matt Hillsdon)
 - Include neopixel header file by default (by Joe Finney)
 - Snapshot v0.2.27 (by Joe Finney)

## [v0.2.26](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.25...v0.2.26)

 - Mixer2 - Add isSilent() and fire SOUND/SILENCE events (#91) (by Martin Williams)
 - Fix typo (by Joe Finney)
 - Introduce MicroBitLog (by Joe Finney)
 - Snapshot v0.2.26 (by Joe Finney)

### codal-core ([caee115...1076c9a](https://github.com/lancaster-university/codal-core/compare/caee1151f0fdfb3a6678f9c269e7b33c9163bfea...1076c9a4388809a4e2c262d62b0064108066ab19))

 - Default atomic operations for MCU without hardware support (#132) (by Joe Finney)
 - add events to CodalUSB (by James Devine)
 - ifguard uf2_info in GhostFAT implementation (by James Devine)
 - fixup GhostFAT uf2 bootloader ifdef (by James Devine)
 - add USB_EP_TIMEOUT flag (by James Devine)
 - unset EP_TIMEOUT flag on read (OUT token) (by James Devine)
 - correct incorrect bitmsk unset (by James Devine)
 - remove EP_TIMEOUT flag (by James Devine)

### codal-nrf52 ([d547526...31a06cf](https://github.com/lancaster-university/codal-nrf52/compare/d54752690753ef8377d38f0338b9fcac9f60655d...31a06cf34da0d5bc3e5e82c3245bfb8e320fcb09))

 - begin work on nrf52 usb (by James Devine)
 - almost complete working usb implementation (by James Devine)
 - NRF52Serial - Avoid overwriting pin definitions during redirection (by Martin Williams)
 - NRF52Serial::configurePins - wait for TX buffer to empty (by Martin Williams)
 - working usb stack [minus iso endpoints] (by James Devine)
 - clean up cmakelists.txt (by James Devine)
 - PR feedback: remove ifdef __cplusplus (by James Devine)
 - update copyright (by James Devine)
 - NRF52ADC: Improvements to discrete sampling (by Joe Finney)
 - WIP: USB power fix when soft reset (by James Devine)
 - Bugfixes to NRF USB (by James Devine)
 - check EPSTATUS variable before attempting a read (by James Devine)
 - Add timeout on USB write (by James Devine)
 - Fixup usb timeout (by James Devine)
 - adjust timeout (by James Devine)
 - Remove EP_TIMEOUT flag and increase timeout duration (by James Devine)
 - NRF52I2C Workaround for missed SHORT trigger (#102) (#36) (by Joe Finney)

### codal-microbit-nrf5sdk ([691869a...ef4662e](https://github.com/microbit-foundation/codal-microbit-nrf5sdk/compare/691869a8261e08b283deb0e2267fae3c00c17dae...ef4662e13875a7b03e7296d7ac24a2b4d231f323))

 - Update README.md (#14) (by microbit-pauline)

## [v0.2.25](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.24...v0.2.25)

 - Changed variable sampleRate to float to avoid rounding errors when generating frequencies. (#71) (by Joshua Hill)
 - Add Partial Flashing and Memory Map documentation (#65) (by Sam Kent)
 - Enable I2C silicon errata workaround for high speed I2C (by Joe Finney)
 - Update MicroBitButton compat constructor.  (#75) (by Sam Kent)
 - MicroBitUARTService - wake up send() on disconnection (#54) (by Martin Williams)
 - Reduce ram origin (#52) (by Martin Williams)
 - Ensure internet GPIO pins are unaffected by DEVICE_DEFAULT_PULLMODE (by Joe Finney)
 - BLE Partial Flashing for MicroPython (#64) (by Sam Kent)
 - Snapshot v0.2.25 (by Joe Finney)

### codal-core ([969813d...caee115](https://github.com/lancaster-university/codal-core/compare/969813d451c33daeccef3b41cf904f6c08e6974f...caee1151f0fdfb3a6678f9c269e7b33c9163bfea))

 - Close possible race condition in Serial receive handling (by Joe Finney)
 - Protect against buffer underflow in Serial::readUntil() (codal-microbit-v2#79) (by Joe Finney)
 - Ensure Serial buffer sizes are valid (codal-microbit-v2#74) (by Joe Finney)
 - Ensure DEVICE_BUTTON_STATE_HOLD_TRIGGERED state is cleared in Button class (codal-microbit-v2#53) (by Joe Finney)
 - Make zero initialisation of ManagedBuffers optional (by Joe Finney)
 - Update RefCounted types to use atomic operations on reference count (by Joe Finney)

### codal-nrf52 ([1a86248...d547526](https://github.com/lancaster-university/codal-nrf52/compare/1a8624801d45301e0f5dfc2c1a17a5efca8082ab...d54752690753ef8377d38f0338b9fcac9f60655d))

 - Changed variables sampleRate and periodUs and their uses in functions to float to avoid rounding errors when generating frequencies. (by Joshua Hill)
 - Wait for I2C hardware to stop following I2C error condition (#49) (by Joe Finney)
 - Introduce optional support for Nordic I2C errata 219 (by Joe Finney)
 - Introduce setDriveMode() method (by Joe Finney)
 - Explicitly configure NRF52I2C SDA and SCL pin configuration (by Joe Finney)
 - NRF52I2C compatibility improvements (by Joe Finney)
 - Fix incorrect mapping for BitMode32 (by James Devine)
 - Reset UART DMA receive buffer pointer on RX restart (codal-microbit-v2#60) (by Joe Finney)
 - Fix inaccurate UART receive bytecount (codal-microbit-v2#80) (by Joe Finney)
 - Correctly generate timestamps on pin rise/fall events (codal-microbit-v2#68) (by Joe Finney)
 - Enable NRF52Pin GPIO edge events for pins using NRF_P1 (codal-microbit-v2#50) (by Joe Finney)
 - Correctly allow NRF52Pin to move from EVENT_ON_EDGE to PULSE_ON_EDGE mode. (by Joe Finney)
 - Update NRF52Pin to honour DEVICE_DEFAULT_PULLMODE (codal-microbit-v2#17) (by Joe Finney)
 - Ensure timer is reset when the first NRF52TouchSensor pin is added (by Joe Finney)

## [v0.2.24](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.23...v0.2.24)

 - Snapshot v0.2.24 (by Joe Finney)

### codal-core ([624048d...969813d](https://github.com/lancaster-university/codal-core/compare/624048d8588a2d2ca3e5b30c9635d41e999ce0b7...969813d451c33daeccef3b41cf904f6c08e6974f))

 - Fixed typo: availiable -> available (by Steve Kemp)
 - MessageBus.cpp : Honour changes in cb_arg when recycling event listeners (by Joe Finney)
 - Optimize serial write oprerations (by Joe Finney)

### codal-nrf52 ([f050e2e...1a86248](https://github.com/lancaster-university/codal-nrf52/compare/f050e2ec3500cc39ec03e3a0c0dcf065c8ca4ac2...1a8624801d45301e0f5dfc2c1a17a5efca8082ab))

 - Further stability improvements to NRF52Serial (by Joe Finney)
 - Support zero length NRF52I2C write transactions (by Joe Finney)

## [v0.2.23](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.22...v0.2.23)

 - Fix BLE disconnection (#43) (by Martin Williams)
 - Introduce Mixer2::setSilenceLevel() API (by Joe Finney)
 - Snapshot v0.2.23 (by Joe Finney)

### codal-core ([879ec3d...624048d](https://github.com/lancaster-university/codal-core/compare/879ec3d8dac8b510c49d57421046f493a640684a...624048d8588a2d2ca3e5b30c9635d41e999ce0b7))

 - Use lazy initialisation in Pulsein (by Joe Finney)
 - Update FiberLock to perform safely when used in IRQ context (by Joe Finney)
 - Protect FiberLock::wait() with IRQ mutex (by Joe Finney)
 - Ensure atomic increment in FiberLock::wait() (by Joe Finney)
 - Ensure all fibers blocked on PulseIn are correctly awoken (by Joe Finney)

### codal-nrf52 ([16edb84...f050e2e](https://github.com/lancaster-university/codal-nrf52/compare/16edb84e43f5faeb64b93cb9cd0ee1c4260ae31b...f050e2ec3500cc39ec03e3a0c0dcf065c8ca4ac2))

 - Allow for SPI interface without SCK (used for neopixels) (by Michal Moskal)
 - Fix lost NRF52 onPulse events (by Joe Finney)
 - Avoid over-configuration of SPI (by Michal Moskal)
 - Ensure timestamp used for pulse width measurement is initailized (by Joe Finney)
 - Correctly disable ADC channels in NRF52Pin when disconnected. (by Joe Finney)
 - Ensure NRF52PWM always restarts DMA pipeline when data arrives late. (by Joe Finney)
 - Maintain IRQ state and PulseIn state when switching from GPIO input/output (by Joe Finney)

## [v0.2.22](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.21...v0.2.22)

 - Snapshot v0.2.22 (by Joe Finney)

### codal-nrf52 ([fa9f692...16edb84](https://github.com/lancaster-university/codal-nrf52/compare/fa9f692993342e849202d82be0495af0f01b9f23...16edb84e43f5faeb64b93cb9cd0ee1c4260ae31b))

 - Remove glitch from NRF52PWM on pinConnect() (by Joe Finney)
 - Increase WS2812B reset pulse to 100uS (by Joe Finney)
 - Ensure WS2812B driver flushes downsream buffers (by Joe Finney)

## [v0.2.21](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.20...v0.2.21)

 - Add matching MicroBitButton constructor matching codal::Button (by Joe Finney)
 - Match BLE powers with DAL (by martinwork)
 - CI built bootloader (by Sam Kent)
 - Remove ambiguity in MicroBitButton constructor (by Joe Finney)
 - MicroBitButton PinNumber compatibility (by Sam Kent)
 - Add MicroBitFont typedef to MicroBitCompat.h (by Sam Kent)
 - microbit_panic - no E and a delay between each character (by martinwork)
 - MicroBitIOPinService pin order (by martinwork)
 - Update MicroBitCompat.h (by Sam Kent)
 - add constructors (by Sam Kent)
 - Add additional components (by Sam Kent)
 - wrap Serial::redirect() (by Sam Kent)
 - MicroBitI2C PinName/PinNumber compat (by Sam Kent)
 - Path through return value in MicroBitSerial::redirect() (by Joe Finney)
 - Include MicroBitI2C.h (by Joe Finney)
 - Update constructor types to require NRF52Pin (by Joe Finney)
 - use MicroBitI2C type consistently in MicroBit object (by Joe Finney)
 - Protect compat layer against memory leak in MicroBitSerial::redirect() (by Joe Finney)
 - Prevent default routing of Auxiliary Audio to P0 when an event handler is registered on that pin (by Joe Finney)
 - Snapshot v0.2.21 (by Sam Kent)

### codal-core ([7132dae...879ec3d](https://github.com/lancaster-university/codal-core/compare/7132daedac411f636df46447025c1e4fcb7bc1e6...879ec3d8dac8b510c49d57421046f493a640684a))

 - remove outdated JACDAC code (by James Devine)
 - Add backward compatibility wrapper for I2C::read() and I2C::write() (by Joe Finney)
 - Add Pin destructor (by Joe Finney)

### codal-nrf52 ([85b0a6b...fa9f692](https://github.com/lancaster-university/codal-nrf52/compare/85b0a6b2075705b2398dd5b9c38482315001f9aa...fa9f692993342e849202d82be0495af0f01b9f23))

 - remove jacdac include from ZSWS.h (by James Devine)
 - Add #ifdef around NRF_P1 usage (by James Devine)
 - further #ifdef guards around usage of NRF_P1 (by James Devine)

## [v0.2.20](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.19...v0.2.20)

 - #define Timer MbedTimer (by Sam Kent)
 - Align MicroBitStorage with CODAL APIs (by Joe Finney)
 - Introduce NRF52 Internal Flash Manager (by Joe Finney)
 - Introduce new constructor paramaeter for PartialFlashingService, so that a non-volatile memory controller to use can be defined (by Joe Finney)
 - Introduce internal NRF52 Flash Storage (by Joe Finney)
 - Use correct constructor parameters when creating PartialFlashingService (by Joe Finney)
 - Make constructor call to MicroBit::bleManager() explicit (by Joe Finney)
 - Allow BLEManager to use any type of button (by Joe Finney)
 - Introduce constructor level compatibility for MicroBitButton (by Joe Finney)
 - Introduce MicroBitAudio::setPin() and MicroBitAudio::setPinEnabled() (by Joe Finney)
 - Update microbit target to use hardware neopixel driver by default (by Joe Finney)
 - Ensure MicroBitCompass::detectedCompass is always correctly initialized (by Joe Finney)
 - Enable persistent storage of compass calibration data (by Joe Finney)
 - Add smoothing to PowerManager I2C interface (by Joe Finney)
 - Snapshot v0.2.20 (by Joe Finney)

### codal-core ([33da548...7132dae](https://github.com/lancaster-university/codal-core/compare/33da548ab2181982eb8ad14cf3b85ca0efc9c89a...7132daedac411f636df46447025c1e4fcb7bc1e6))

 - Update non-volatile memory APIs to no longer assume on chip address space (by Joe Finney)
 - Correct MAGIC constant and remove legacy debug statements (by Joe Finney)
 - Update KeyValueStorage to use lazy instantiation of buffers (by Joe Finney)
 - Add missing LICENSE in MemorySource class (by Joe Finney)
 - Remove status bitfield clash between LSM303 drivers and Compass/Accelerometer base classes (by Joe Finney)

### codal-nrf52 ([3ed9e6c...85b0a6b](https://github.com/lancaster-university/codal-nrf52/compare/3ed9e6ce738f22f75418cb69effa734999195007...85b0a6b2075705b2398dd5b9c38482315001f9aa))

 - Add configuration options to define behaviour on buffer overrun (by Joe Finney)
 - Introduce hardware (PWM) based WS2812B/neopixel driver (by Joe Finney)

## [v0.2.19](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.18...v0.2.19)

 - Ensure configuration parameters for motion sensors are initialized (by Joe Finney)
 - Snapshot v0.2.19 (by Joe Finney)

## [v0.2.18](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.17...v0.2.18)

 - Upstream PulseIn.h and PulseIn.cpp (by Joe Finney)
 - Snapshot v0.2.18 (by Joe Finney)

### codal-core ([316a483...33da548](https://github.com/lancaster-university/codal-core/compare/316a4830a67fe85f0ee1916a22856fb4ad486994...33da548ab2181982eb8ad14cf3b85ca0efc9c89a))

 - Introduce Pin::getPulseUs() and helper classes (by Joe Finney)

### codal-nrf52 ([4087e0d...3ed9e6c](https://github.com/lancaster-university/codal-nrf52/compare/4087e0dc840896d4e340e5888b52565ec5792bae...3ed9e6ce738f22f75418cb69effa734999195007))

 - Introduce NRF52Pin::getPulseUs() (by Joe Finney)

## [v0.2.17](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.16...v0.2.17)

 - Increase brightness of active pixel in Compass Calibration UX (#16) (by Joe Finney)
 - Ensure valid light sense reading on first use (#14) (by Joe Finney)
 - Adjust interrupt priorities (by Joe Finney)
 - Ensure NRF52LEDMatrix column pins are correctly configured on enable (#21) (by Joe Finney)
 - Set default mode for getDigitalValue() to PullDown (by Joe Finney)
 - Set IRQ priority for pin IRQ events (by Joe Finney)
 - Introduce blocking PulseIn behaviour (#15) (by Joe Finney)
 - Snapshot v0.2.17 (by Joe Finney)

### codal-nrf52 ([dff67e0...4087e0d](https://github.com/lancaster-university/codal-nrf52/compare/dff67e066e039358001ceafe406cc4dd902363c8...4087e0dc840896d4e340e5888b52565ec5792bae))

 - Don't assume NRF52PWM should be ultra-high priority (by Joe Finney)
 - Improve stability of NRF52Serial data reception (by Joe Finney)
 - Remove unnecessary code from NRF52Pin (by Joe Finney)

## [v0.2.16](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.15...v0.2.16)

 - Cope with zero-duration effects (#11) (by Matt Hillsdon)
 - Polyfilling mbed for extensions (#13) (by Sam Kent)
 - Suppress deprecated API compat warning when compiling the compat code itself (by Joe Finney)
 - Snapshot v0.2.16 (by Joe Finney)

## [v0.2.15](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.14...v0.2.15)

 - Defer START/STOP of synthesizer in SoundOutputPin (by Joe Finney)
 - Snapshot v0.2.15 (by Joe Finney)

## [v0.2.14](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.13...v0.2.14)

 - Add support for stopping sound expressions. (#9) (by Matt Hillsdon)
 - Optimisation: Use a flag for stopping state. (#10) (by Matt Hillsdon)
 - Apply silence optimisation on virtual audio pin (by Joe Finney)
 - Snapshot v0.2.14 (by Joe Finney)

### codal-nrf52 ([406a4e9...dff67e0](https://github.com/lancaster-university/codal-nrf52/compare/406a4e9b08fb4a227b979f95629764cae7bd8a1d...dff67e066e039358001ceafe406cc4dd902363c8))

 - Remove rogue IO_STATUS_DIGITAL_OUT flag when using pin as an analogue output (by Joe Finney)

## [v0.2.13](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.12...v0.2.13)

 - Add support for 8-bit mixer output (by Joe Finney)
 - Ensure SoundSynthesizer output is normalized when gain < 1.0 (by Joe Finney)
 - Move to using SoftDevice S113 to reduce memory usage (by Martin Williams)
 - Enable on-demand activaiton of audio output pipeline (by Joe Finney)
 - Normalise virtual audio pin volume levels to be consistent with micro:bit v1 (by Joe Finney)
 - Lower interrupt priority of PCM audio interrupts (by Joe Finney)
 - Introduce optimzed silence generation on synthesizer channels (by Joe Finney)
 - Snapshot v0.2.13 (by Joe Finney)

### codal-core ([fac7b59...316a483](https://github.com/lancaster-university/codal-core/compare/fac7b59a72a90c25b41ce20c16872ea5d99d14c6...316a4830a67fe85f0ee1916a22856fb4ad486994))

 - Close potential race condition in Serial::send() (by Joe Finney)

### codal-microbit-nrf5sdk ([249aef4...691869a](https://github.com/microbit-foundation/codal-microbit-nrf5sdk/compare/249aef43dd6afd9bfaedd31414e5a866d3464c3c...691869a8261e08b283deb0e2267fae3c00c17dae))

 - Swap FDS and MicroBitStorage (by martinwork)
 - Add headers for SoftDevice S113 (by martinwork)
 - Use headers for SoftDevice S113 (by martinwork)

## [v0.2.12](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.11...v0.2.12)

 - Snapshot v0.2.12 (by Joe Finney)

### codal-nrf52 ([300ca77...406a4e9](https://github.com/lancaster-university/codal-nrf52/compare/300ca77b9fc53a8100d4af19f8ffbd9674c29b06...406a4e9b08fb4a227b979f95629764cae7bd8a1d))

 - Fix memory leak in pin events In fact, avoid allocation at all there (by Michal Moskal)
 - Fix inaccurate servo pulsewidth calculation in NRF52Pin::setServoPulseUs() (by Joe Finney)

## [v0.2.11](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.10...v0.2.11)

 - Snapshot v0.2.11 (by Michal Moskal)

## [v0.2.10](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.9...v0.2.10)

 - Snapshot v0.2.10 (by Michal Moskal)

## [v0.2.9](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.8...v0.2.9)

 - SoundExpressions::play is now sync. (#2) (by Matt Hillsdon)
 - Snapshot v0.2.9 (by Joe Finney)

### codal-nrf52 ([e9d44ac...300ca77](https://github.com/lancaster-university/codal-nrf52/compare/e9d44ac45b177a6c6d3fe48b2bdc9f219fcb41fb...300ca77b9fc53a8100d4af19f8ffbd9674c29b06))

 - Fix NRF52Pin::setAnalogPeriod() (by Joe Finney)

## [v0.2.8](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.7...v0.2.8)

 - Improve response time of back-to-back calls to SoundOutputPin (by Joe Finney)
 - Snapshot v0.2.8 (by Joe Finney)

## [v0.2.7](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.6...v0.2.7)

 - Snapshot v0.2.7 (by Joe Finney)

### codal-nrf52 ([2f35ae5...e9d44ac](https://github.com/lancaster-university/codal-nrf52/compare/2f35ae5eeecbe62b983b1dacafe8cbed97bd15ab...e9d44ac45b177a6c6d3fe48b2bdc9f219fcb41fb))

 - Ensure NRF52Pin fully overrides codal::Pin (by Joe Finney)

## [v0.2.6](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.5...v0.2.6)

 - Ensure SoundOutputPin overrides base methods from codal::Pin (by Joe Finney)
 - Snapshot v0.2.6 (by Joe Finney)

## [v0.2.5](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.4...v0.2.5)

 - Introduce MicroBitAudio output pipeline (by Matt Hillsdon)
 - Correct MIT license (by Joe Finney)
 - Introduce API for per-channel samplerates indepdent out output rate (by Joe Finney)
 - Introduce per channel sub/super sampling of MixerChannels (by Joe Finney)
 - Snapshot v0.2.5 (by Joe Finney)

### codal-core ([e0089a9...fac7b59](https://github.com/lancaster-university/codal-core/compare/e0089a9c7b5c92e44482e324d39aef0728912a07...fac7b59a72a90c25b41ce20c16872ea5d99d14c6))

 - Simplify MemorySource (by Joe Finney)

### codal-nrf52 ([3ec21bd...2f35ae5](https://github.com/lancaster-university/codal-nrf52/compare/3ec21bd3278f8743f05b9ed7ce34ae6b4fbc71b3...2f35ae5eeecbe62b983b1dacafe8cbed97bd15ab))

 - Adopt new MemorySource API (by Joe Finney)

## [v0.2.4](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.3...v0.2.4)

 - Introduce target-locked.json (by Joe Finney)
 - Snapshot v0.2.3 (by Joe Finney)
 - Snapshot v0.2.4 (by Joe Finney)

## [v0.2.3](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.2...v0.2.3)

 - Ensure I2C Flash Geometry is loaded before it is used (by Joe Finney)
 - Ensure KL27 is awake before starting any i2c communications (by Joe Finney)
 - Ensure default __wrap_atexit is defined (by Joe Finney)
 - Update references after repository migration (by Joe Finney)
 - Update target name to reflect micro:bit version number (by Joe Finney)
 - Introduce MIT License file (by Joe Finney)
 - Ensure last effect step generates enough samples. (by Matt Hillsdon)

## [v0.2.2](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.1...v0.2.2)

 - Introduce htonl/htons into MicroBitCompat (by Joe Finney)
 - Add interface to PowerManager to indicate if an I2C transaction with the KL27 is in progress (by Joe Finney)
 - Add API support for awaitPacket() (by Joe Finney)
 - Fixes for MicroBitFlashManager (by Joe Finney)
 - Add MicroBitFlashManager to uBit model (by Joe Finney)
 - Fix bug that prevents display operating in lightsensing modes (by Joe Finney)
 - Gather USB Flash geometry on demand (by Joe Finney)
 - Rename micro:bit face logo button on request (by Joe Finney)
 - Snapshot v0.2.2 (by Joe Finney)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-core', 'codal-core.git']' returned non-zero exit status 128.

### codal-core ([05c9728...b8bb974](https://github.com/microbit-foundation/codal-core/compare/05c9728ab56e43ac7bd5e9852a3177a75c112663...b8bb974e882182c2f0068f0d62ea9daab3c5b626))

 - Unable to find commits

### codal-nrf52 ([11309c0...3ec21bd](https://github.com/lancaster-university/codal-nrf52/compare/11309c03127181edfc41f145a967c1855ec7a765...3ec21bd3278f8743f05b9ed7ce34ae6b4fbc71b3))

 - Fix visibilty of NRF52Serial::setBaudrate (by Joe Finney)

## [v0.2.1](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.0...v0.2.1)

 - Update bootloader.o (by Sam Kent)
 - Introduce PowerManager (by Joe Finney)
 - SoundEmojiSynthesizer Improvements (by Joe Finney)
 - Refactor SoundEmojiSynthesizer (by Joe Finney)
 - Minor improvements to SoundEmojiSynthesizer (by Joe Finney)
 - Generalise arpeggio effects (by Joe Finney)
 - SoundEmojiSynthesizer now uses Equal Temperament tuning by default (by Joe Finney)
 - Fix inversion of JUST_SCALE CONFIG option (by Joe Finney)
 - Introduce power management improvements (by Joe Finney)
 - Introduce MicroBitUSBFlashManager (by Joe Finney)
 - Introduce NIOP workaround for Kl27 silicon errata (by Joe Finney)
 - Fix initiatisation of NRF52LedMatrix (by Joe Finney)
 - Increase sensitivty of onLoud() (by Joe Finney)
 - KL27 I2C Comms Refinements (by Joe Finney)
 - Align PIN_ID values with edge connector IDs as in micro:bit v1. (by Joe Finney)
 - Introduce tuned capacitative touch parameters (by Joe Finney)
 - Bring out Face touch sensor as first class entity (by Joe Finney)
 - Add default calibration constant for face touch (by Joe Finney)
 - Utilize and set default CONFIG option for capacitative touch sensing (by Joe Finney)
 - Add MICROBIT_UBIT_FACE_TOUCH_BUTTON feature macro (by Joe Finney)
 - Introduce API for USBFlashStorage (alpha) (by Joe Finney)
 - Snapshot v0.2.1 (by Joe Finney)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-core', 'codal-core.git']' returned non-zero exit status 128.

### codal-core ([24dc5df...05c9728](https://github.com/microbit-foundation/codal-core/compare/24dc5dfd093cd90fd93b651ce9d460943f831df6...05c9728ab56e43ac7bd5e9852a3177a75c112663))

 - Unable to find commits

### codal-nrf52 ([e76b296...11309c0](https://github.com/lancaster-university/codal-nrf52/compare/e76b2961d8267280c6c0b38bb99327e56903ad44...11309c03127181edfc41f145a967c1855ec7a765))

 - Introduce support for GPIO DETECT signals (by Joe Finney)
 - Add support to disable timer interrupts during deep sleep (by Joe Finney)
 - Introduce MIT LICENSE (by Joe Finney)
 - Enable dynamic allocation/deallocation of PWM channels. (by Joe Finney)
 - Add selective support for resistive and capacitative sensing (by Joe Finney)
 - Allow CONFIG based definition of deafult capacitative touch calibraiton (by Joe Finney)

## [v0.2.0](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.1.1...v0.2.0)

 - Update linker script, uicr.o and settings.o (by martinwork)
 - Add linker script sections for nRF5SDK (by martinwork)
 - Add SOFTDEVICE_PRESENT to definitions (by martinwork)
 - Duplicate nRF5SDK files: drv_rtc.c, nrf_pwr_mgmt.c (by martinwork)
 - nRF5SDK_mods for codal-nrf52 nrfx compatibility (by martinwork)
 - Duplicate nRF5SDK nrf_sdh_soc.c and .h (by martinwork)
 - Modify nrf_sdh_soc - bug workaround (by martinwork)
 - DMESG backend for NRF_LOG. CODAL_DEBUG sets NRF_LOG_ENABLED. (by martinwork)
 - Add some nRF5SDK source files to CMakeLists.txt (by martinwork)
 - MicroBitBLEManager, Nordic buttonless DFU service, OPEN_LINK (by martinwork)
 - seedRandom (by martinwork)
 - Put the log levels back to normal (by martinwork)
 - Fix NRF_SDH_LOG_LEVEL (by martinwork)
 - Avoid use of UNUSED_PARAMETER (by martinwork)
 - MicroBit::getSerial() (by martinwork)
 - Change NO_BLE to DEVICE_BLE (by martinwork)
 - Duplicate ble_dfu_bonded.c (by martinwork)
 - Set NRF_DFU_BLE_BUTTONLESS_SUPPORTS_BONDS (by martinwork)
 - bootloader.o with REQUIRES_BONDS and SERVICE_CHANGED (by martinwork)
 - Fixes for just works (by martinwork)
 - Update sdk_config (by martinwork)
 - BLE - Allow re-pairing (by martinwork)
 - BLE - Do not disconnect immediately after pairing (by martinwork)
 - BLE - Create buttonless DFU service with MICROBIT_BLE_SECURITY_LEVEL (by martinwork)
 - Add MICROBIT_BLE_SECURITY_MODE (by martinwork)
 - BLE - Use idleCallback instead of timer for nrf_pwr_mgmt_shutdown (by martinwork)
 - Fix MICROBIT_BLE_STATUS_DELETE_BOND (by martinwork)
 - Improve pairing logic and shutdown handling (by martinwork)
 - BLE - only show pair tick after BLE_GAP_EVT_AUTH_STATUS (by martinwork)
 - BLE - Use BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION (by martinwork)
 - Add a shutdown delay (by martinwork)
 - Duplicate nrf_sdh.c (by martinwork)
 - Revert "Duplicate nrf_sdh.c" (by martinwork)
 - Restart without disabling softdevice (by martinwork)
 - BLE services (by martinwork)
 - yotta: Add module.json and adapt CMakeLists for yotta build (by Raphael Gault)
 - yotta: Explicitly point to master branch of dependencies (by Raphael Gault)
 - Update bootloader.o (by martinwork)
 - Rename MicroBitIOPinService.unused to cpp (by martinwork)
 - MicroBitIOPinService (by martinwork)
 - Rename MicroBitEventService.unused to cpp (by martinwork)
 - MicroBitEventService (by martinwork)
 - Rename MicroBitUARTService.unused as cpp (by martinwork)
 - MicroBitUARTService (by martinwork)
 - Rename MicroBitMagnetometerService.unused as cpp (by martinwork)
 - MicroBitMagnetometerService (by martinwork)
 - Rename MicroBitPartialFlashingService.unused as cpp (by martinwork)
 - MicroBitPartialFlashingService (by martinwork)
 - MicroBitBLEManager - fix advertising interval and timeout (by martinwork)
 - Rename MicroBitEddystone.unused as cpp (by martinwork)
 - MicroBitEddystone (by martinwork)
 - MICROBIT_DMESG_LEVEL (by martinwork)
 - Adjust GATT table size and RAM region to support all services (by martinwork)
 - Fix Eddystone (by martinwork)
 - use codal-microbit-nrf5sdk (by Sam Kent)
 - Merge updates from current DAL master (by martinwork)
 - MicroBitFlash, MicroBitStorage, MicroBitFile, MicroBitFileSystem (by martinwork)
 - Use MicroBitStorage and MicroBitFlash (by martinwork)
 - Removed SOFTDEVICE_PRESENT from target.json (by martinwork)
 - MicroBitPartialFlashingService - update settings pages (by martinwork)
 - Delete nRF5SDK_mods and sdk_config folders (by martinwork)
 - Avoid sign comparison warning (by martinwork)
 - Call app_timer_init() (by martinwork)
 - Better DMESG for peer manager failure events (by martinwork)
 - Remove ../codal-nrf52 includes (by martinwork)
 - Update module.json (by Sam Kent)
 - microbit_panic and target_panic (by martinwork)
 - target.json defines SOFTDEVICE_PRESENT and DEVICE_BLE (by martinwork)
 - Add V2.0 to DIS Model Number String (by martinwork)
 - Create the partial flashing service (by martinwork)
 - Avoid multiple calls to pm_local_database_has_changed() (by martinwork)
 - Remove auto bond deletion (by martinwork)
 - Fix softdevice state change naming (by martinwork)
 - Fix partial flashing (by martinwork)
 - Set all NRF_GPIOTE->CONFIG to zero (by martinwork)
 - Weak target_panic (by martinwork)
 - Adopt NRF52ADC driver (by Joe Finney)
 - Set ADC configuration for audio quality (by Joe Finney)
 - Add TouchSensor support (by Joe Finney)
 - Use NRF52TouchSensor for implementation of Pin::isTouched() (by Joe Finney)
 - Update to use NRF52LEDMatrix driver (by Joe Finney)
 - Adopt simplifed FXOS8700 driver (by Joe Finney)
 - Enable Compass Calibration (by Joe Finney)
 - Introduce NRF52LedMatrix (by Joe Finney)
 - Handle 180 degree rotation of FXOS8700 sensor (by Joe Finney)
 - Fix partial flashing page size (by martinwork)
 - Review MICROBIT_DMESG_LEVELs (by martinwork)
 - Fix capitalisation typos of NRF52LedMatrix.h (by Joe Finney)
 - Wire in NRF52LEDMatrix light sensor and align to microbit-dal API (by Joe Finney)
 - Align MicroBitIO Pin ordering with edge connector (fix #78) (by Joe Finney)
 - Ensure random number generator is seeded consistently (Fix #52) (by Joe Finney)
 - Set Peripheral IRQ priorities that are compatible with Nordic SoftDevice (by Joe Finney)
 - Merge MicroBit.cpp/h from upstream master (by martinwork)
 - Use codal::Compass/Accelerometer to match changed uBit (by martinwork)
 - Cleanup compiler warnings (by Joe Finney)
 - Update MIC_INIT / MIC_ENABLE macros to use NRF52ADC (by Joe Finney)
 - Snapshot v0.2.0 (by Joe Finney)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-core', 'codal-core.git']' returned non-zero exit status 128.

### codal-core ([1b9bd3a...24dc5df](https://github.com/microbit-foundation/codal-core/compare/1b9bd3aea55524fbc2f615668ee5b0e0c7c74264...24dc5dfd093cd90fd93b651ce9d460943f831df6))

 - Unable to find commits

### codal-nrf52 ([b6d25e6...e76b296](https://github.com/lancaster-university/codal-nrf52/compare/b6d25e6a40d826db16ae229db11c0780cefbb0ca...e76b2961d8267280c6c0b38bb99327e56903ad44))

 - Introduce unified ADC driver (by Joe Finney)
 - Remove DMESG instrumentation in NRF52ADC (by Joe Finney)
 - Enable support for 16MHz prescaler (by Joe Finney)
 - Account for 14 bit samples from ADC driver (by Joe Finney)
 - Add support for software oversampling in NRF52ADC (by Joe Finney)
 - Introduce NRF52TouchSensor (by Joe Finney)
 - Increase TouchSensor frequency (by Joe Finney)
 - Use NRF52TouchSensor as implementation of Pin::isTouched() (by Joe Finney)
 - Accurately define maximum sensor value (by Joe Finney)
 - NRF52TouchSensor updates to PPI/GPIOTE constants (by Joe Finney)
 - Enable NRF52PWM Harwdare Double Buffering (by Joe Finney)
 - Update NRF52Pin to utilise typed MemorySource (by Joe Finney)
 - Fix out of date API comment block (by Joe Finney)
 - Update NRF52I2C to fully reset I2C bus on initialisation (by Joe Finney)
 - Fix capitalisation typo in #include (by Joe Finney)

## [v0.1.1](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.1.0...v0.1.1)

 - i2c: Add second instance of i2c (by Raphael Gault)
 - Add default initialization code for microphone (by Michal Moskal)
 - Disable DMESG on serial by default (by Michal Moskal)
 - Snapshot v0.1.1 (by Michal Moskal)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-core', 'codal-core.git']' returned non-zero exit status 128.

### codal-core ([1b9bd3a...1b9bd3a](https://github.com/microbit-foundation/codal-core/compare/1b9bd3aea55524fbc2f615668ee5b0e0c7c74264...1b9bd3aea55524fbc2f615668ee5b0e0c7c74264))

 - Unable to find commits

### codal-nrf52 ([06f9b85...b6d25e6](https://github.com/lancaster-university/codal-nrf52/compare/06f9b855717db46c0ab2af2ad820d60858d2a2e6...b6d25e6a40d826db16ae229db11c0780cefbb0ca))

 - Correct polarity of PWM (by Michal Moskal)

## [v0.1.0](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.16...v0.1.0)

 - Rename project to match repo name (by Michal Moskal)
 - Snapshot v0.1.0 (by Michal Moskal)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-core', 'codal-core.git']' returned non-zero exit status 128.

### codal-core ([1b9bd3a...1b9bd3a](https://github.com/microbit-foundation/codal-core/compare/1b9bd3aea55524fbc2f615668ee5b0e0c7c74264...1b9bd3aea55524fbc2f615668ee5b0e0c7c74264))

 - Unable to find commits

## [v0.0.16](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.15...v0.0.16)

 - Disable BLE by default as it doesn't compile (by Michal Moskal)
 - Add blinking microbit_panic() (by Michal Moskal)
 - Disable IRQs and set drive in panic (by Michal Moskal)
 - Rename output binary to MICROBIT (by Michal Moskal)
 - Snapshot v0.0.16 (by Michal Moskal)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-core', 'codal-core.git']' returned non-zero exit status 128.

### codal-core ([1b9bd3a...1b9bd3a](https://github.com/microbit-foundation/codal-core/compare/1b9bd3aea55524fbc2f615668ee5b0e0c7c74264...1b9bd3aea55524fbc2f615668ee5b0e0c7c74264))

 - Unable to find commits

## [v0.0.15](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.14...v0.0.15)

 - MicroBitDevice: Add microbit_seed_random() implementation without BLE (by Raphael Gault)
 - Motion Sensor: Add a delay at accelerometer/compass detection (by Raphael Gault)
 - MicroBitDevice: Define the microbit_friendly_name function (by Raphael Gault)
 - fix FLASH_PROGRAM_END calculation (by martinwork)
 - Add Softdevice and Bootloader (#31) (by Sam Kent)
 - Add configuration defines for sneaky pxt compilation (by Michal Moskal)
 - Remove incorrect define (by Michal Moskal)
 - Add shim readLightLevel() for pxt (by Michal Moskal)
 - Rename defines as per discussion with @jaustin (by Michal Moskal)
 - Make SPI work in microbit-compat (by Michal Moskal)
 - Snapshot v0.0.15 (by Michal Moskal)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-core', 'codal-core.git']' returned non-zero exit status 128.

### codal-core ([99ac7fc...1b9bd3a](https://github.com/microbit-foundation/codal-core/compare/99ac7fc173290287689f484ce0e750466960de60...1b9bd3aea55524fbc2f615668ee5b0e0c7c74264))

 - Unable to find commits

### codal-nrf52 ([d262242...06f9b85](https://github.com/lancaster-university/codal-nrf52/compare/d2622428aa337155d589037ac07f4197f3179459...06f9b855717db46c0ab2af2ad820d60858d2a2e6))

 - Added .project file for giteye. (by Kei)
 - Fixed typo in NRF52Serial::setBaudrate(). & Added newline at EOF. (by Kei)
 - Added UARTE in sync_serial. (by Kei)
 - Fixed NRF52Serial for UARTE(DMA). (by Kei)
 - Added comment for NRF52Serial.write() function. (by Kei)
 - Updated configurePins() & Added isConfigured() for checking init. (by Kei)
 - Implement Pin::conditionalSetDigitalValue (by Michal Moskal)
 - Fix compilation problem (by Michal Moskal)
 - Fix Pin::eventOn(DEVICE_PIN_INTERRUPT_ON_EDGE) (by Michal Moskal)
 - conditionalSetDigitalValue -> getAndSetDigitalValue (by Michal Moskal)
 - Nesting-safe target_disable_irq() (by Michal Moskal)
 - Recognize BRK as end of transmission (by Michal Moskal)
 - Add comments (by Michal Moskal)
 - Use target_disable_irq (by Michal Moskal)
 - Use *SET/CLR registers properly (by Michal Moskal)
 - Avoid jitter on setDigitalValue() (by Michal Moskal)
 - Avoid race in target_disable_irq() (by Michal Moskal)
 - Make sure getAndSet is as quick as possible (by Michal Moskal)
 - Added peripheral_alloc (by modifying sync_serial) (by Kei)
 - Applied peripheral_alloc to I2C, SPI, Serial. (by Kei)
 - Moved position of constructor in NRF52Serial file. (by Kei)
 - Modified NRF52Serial using hal library. (Added function for DMA) (by Kei)
 - Add free_alloc_peri(device) to free allocated peri. (by Kei)
 - Apply free_alloc_peri(), Replace is_configured() with target_panic(). (by Kei)
 - Delete .project file. (by Kei)
 - Adopt SWS to peripheral alloc Side effect: allow for more than one SWS (by Michal Moskal)
 - Clean up after bootloader (by Michal Moskal)
 - Correct various LATCH/GPIOTE assumptions (by Michal Moskal)
 - Set high drive on SPI pins (by Michal Moskal)
 - Add mbed-compatible aliases in SPI (by Michal Moskal)

## [v0.0.14](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.13...v0.0.14)

 - Migrate codal_target_hal to upstream codal-nrf52 (by Joe Finney)
 - Bring speaker up in HiDrive mode by default (by Joe Finney)
 - Snapshot v0.0.14 (by James Devine)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-core', 'codal-core.git']' returned non-zero exit status 128.

### codal-core ([330de9b...99ac7fc](https://github.com/microbit-foundation/codal-core/compare/330de9b5e86f69b6e75b15109ada953b1a71a1ca...99ac7fc173290287689f484ce0e750466960de60))

 - Unable to find commits

### codal-nrf52 ([0e20dc8...d262242](https://github.com/lancaster-university/codal-nrf52/compare/0e20dc813aa7dca4d8c4e7c5b1f631205c92bdfc...d2622428aa337155d589037ac07f4197f3179459))

 - Introduce additional features for NRF52PWM (by Joe Finney)
 - Introduce setPeriodUs() function (by Joe Finney)
 - Fix NRF52PWM period calculation (by Joe Finney)
 - Introduce PWM support (by Joe Finney)

## [v0.0.13](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.12...v0.0.13)

 - Use nrf.h not nrf52.h (by Michal Moskal)
 - Snapshot v0.0.13 (by Michal Moskal)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-core', 'codal-core.git']' returned non-zero exit status 128.

### codal-core ([330de9b...330de9b](https://github.com/microbit-foundation/codal-core/compare/330de9b5e86f69b6e75b15109ada953b1a71a1ca...330de9b5e86f69b6e75b15109ada953b1a71a1ca))

 - Unable to find commits

### codal-nrf52 ([9de3e86...0e20dc8](https://github.com/lancaster-university/codal-nrf52/compare/9de3e862c160c02c0536066252840607185e2959...0e20dc813aa7dca4d8c4e7c5b1f631205c92bdfc))

 - Disable PRS (conflicts with our I2C/SPI drivers) (by Michal Moskal)
 - Default to UARTE0 (by Michal Moskal)
 - Move base functions from target_hal here to avoid duplication (by Michal Moskal)
 - Remove mbed references (by Michal Moskal)
 - Format (by Michal Moskal)
 - Add vector table relocation (by Michal Moskal)
 - Further VTOR relocation fixes (by Michal Moskal)
 - Basic double-buffering in PWM (by Michal Moskal)
 - Fix PDM buffer size (by Michal Moskal)

## [v0.0.12](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.11...v0.0.12)

 - Snapshot v0.0.12 (by Michal Moskal)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-core', 'codal-core.git']' returned non-zero exit status 128.

### codal-core ([330de9b...330de9b](https://github.com/microbit-foundation/codal-core/compare/330de9b5e86f69b6e75b15109ada953b1a71a1ca...330de9b5e86f69b6e75b15109ada953b1a71a1ca))

 - Unable to find commits

### codal-nrf52 ([86b21b5...9de3e86](https://github.com/lancaster-university/codal-nrf52/compare/86b21b5c096a8be28586c097f5f02a7235889628...9de3e862c160c02c0536066252840607185e2959))

 - Dynamic allocation of TWIM/SPIM instances (by Michal Moskal)
 - Start work on I2S neopixel (by Michal Moskal)
 - Use generic NRF52 ifdef (by Michal Moskal)
 - Add 52833 (by Michal Moskal)
 - Add disclaimer (by Michal Moskal)
 - Simplify GPIOTE_IRQHandler and fix LATCH clearing (by Michal Moskal)
 - Fix compilation error in serial (by Michal Moskal)
 - use same packet size as microbit to match CRC (by Peli)
 - Disable neopixel (by Michal Moskal)
 - Remove unfinished neopixel (by Michal Moskal)
 - Fixed typo in NRF52Serial::setBaudrate(). & Added newline at EOF. (by Kei)
 - Add bit-bang neopixel implementation (by Michal Moskal)

## [v0.0.11](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.10...v0.0.11)

 - Introduce LED Matrix brightness constants (by Joe Finney)
 - Add Temperature Sensing (by Joe Finney)
 - Remove mbed dependencies (by Joe Finney)
 - add Serial to model, and define DMESG SERIAL DEBUG (by James Devine)
 - Snapshot v0.0.11 (by James Devine)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-core', 'codal-core.git']' returned non-zero exit status 128.

### codal-core ([fce43db...330de9b](https://github.com/microbit-foundation/codal-core/compare/fce43db64bee7ebd0c48f244c296d4e72d1a6b42...330de9b5e86f69b6e75b15109ada953b1a71a1ca))

 - Unable to find commits
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-nrf52', 'codal-nrf52.git']' returned non-zero exit status 128.

### codal-nrf52 ([ec82c60...86b21b5](https://github.com/microbit-foundation/codal-nrf52/compare/ec82c60b36608e9f70cac4a7378f17751fb0931a...86b21b5c096a8be28586c097f5f02a7235889628))

 - Unable to find commits

## [v0.0.10](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.9...v0.0.10)

 - update dev json to point to master (by James Devine)
 - Remove unused function warning (by Joe Finney)
 - Update Microbit uBit model to explicitly define ACTIVE_LOW IRQ line for accelerometer and magnetometer sensors (by Joe Finney)
 - Migrate LSM303 driver to codal-core (by Joe Finney)
 - Update construction and lazy instantiation for accelerometer and magnetometer (by Joe Finney)
 - Introduce improved compass calibration algorithm (by Joe Finney)
 - Introduce autoconfiguration of Accelerometer and Compass sensors (by Joe Finney)
 - Configuration Update (by Joe Finney)
 - Remove unecessary debug (by Joe Finney)
 - Snapshot v0.0.10 (by Joe Finney)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-core', 'codal-core.git']' returned non-zero exit status 128.

### codal-core ([951cad8...fce43db](https://github.com/microbit-foundation/codal-core/compare/951cad8b012382e1343919e8185d0c01ef90260e...fce43db64bee7ebd0c48f244c296d4e72d1a6b42))

 - Unable to find commits
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-mbedos', 'codal-mbedos.git']' returned non-zero exit status 128.

### codal-mbedos ([4db2eb7...e696071](https://github.com/microbit-foundation/codal-mbedos/compare/4db2eb7a0d92d54aa391cb3d14b242a21cc11474...e6960710702649ed28597e2451d3438045dcdb6e))

 - Unable to find commits
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-nrf52', 'codal-nrf52.git']' returned non-zero exit status 128.

### codal-nrf52 ([e000650...ec82c60](https://github.com/microbit-foundation/codal-nrf52/compare/e0006509416cb6567dda7229d4814043c70f2497...ec82c60b36608e9f70cac4a7378f17751fb0931a))

 - Unable to find commits

## [v0.0.9](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.8...v0.0.9)

 - Set target to provide printf (by James Devine)
 - Snapshot v0.0.9 (by James Devine)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-core', 'codal-core.git']' returned non-zero exit status 128.

### codal-core ([951cad8...951cad8](https://github.com/microbit-foundation/codal-core/compare/951cad8b012382e1343919e8185d0c01ef90260e...951cad8b012382e1343919e8185d0c01ef90260e))

 - Unable to find commits
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-mbedos', 'codal-mbedos.git']' returned non-zero exit status 128.

### codal-mbedos ([afc818c...4db2eb7](https://github.com/microbit-foundation/codal-mbedos/compare/afc818c289f85f292a3c3101612e0df36aabba8e...4db2eb7a0d92d54aa391cb3d14b242a21cc11474))

 - Unable to find commits
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-nrf52', 'codal-nrf52.git']' returned non-zero exit status 128.

### codal-nrf52 ([e000650...e000650](https://github.com/microbit-foundation/codal-nrf52/compare/e0006509416cb6567dda7229d4814043c70f2497...e0006509416cb6567dda7229d4814043c70f2497))

 - Unable to find commits

## [v0.0.8](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.6...v0.0.8)

 - update model, sync with core changes (by James Devine)
 - restore yotta_cfg_mappings (by James Devine)
 - Snapshot v0.0.6-nrf52833-mbedos.0 (by James Devine)
 - manually include MESEvents for now (by James Devine)
 - Snapshot v0.0.6-nrf52833-mbedos.1 (by James Devine)
 - add missing define, uncomment thermometer and radio in this branch (by James Devine)
 - Snapshot v0.0.6-core-fastforward.0 (by James Devine)
 - Snapshot v0.0.7 (by James Devine)
 - Snapshot v0.0.8 (by James Devine)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-core', 'codal-core.git']' returned non-zero exit status 128.

### codal-core ([e9247e8...951cad8](https://github.com/microbit-foundation/codal-core/compare/e9247e840c340e302528c42d550a06dcfaa8897b...951cad8b012382e1343919e8185d0c01ef90260e))

 - Unable to find commits
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-mbedos', 'codal-mbedos.git']' returned non-zero exit status 128.

### codal-mbedos ([43c63ed...afc818c](https://github.com/microbit-foundation/codal-mbedos/compare/43c63ed54bf5d4802d48d81862714df80b3239e1...afc818c289f85f292a3c3101612e0df36aabba8e))

 - Unable to find commits
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-nrf52', 'codal-nrf52.git']' returned non-zero exit status 128.

### codal-nrf52 ([ea1b9ea...e000650](https://github.com/microbit-foundation/codal-nrf52/compare/ea1b9ea71f8693b76dbb46178e3a8d8c0a2ac0d8...e0006509416cb6567dda7229d4814043c70f2497))

 - Unable to find commits

## [v0.0.6](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.5...v0.0.6)

 - Re-enable radio (by Jonathan Austin)
 - Snapshot v0.0.6 (by James Devine)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-core', 'codal-core.git']' returned non-zero exit status 128.

### codal-core ([e9247e8...e9247e8](https://github.com/microbit-foundation/codal-core/compare/e9247e840c340e302528c42d550a06dcfaa8897b...e9247e840c340e302528c42d550a06dcfaa8897b))

 - Unable to find commits
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-mbedos', 'codal-mbedos.git']' returned non-zero exit status 128.

### codal-mbedos ([ed0c700...43c63ed](https://github.com/microbit-foundation/codal-mbedos/compare/ed0c700604fd63e2f2baae8a83c69e8f9fc5c5f8...43c63ed54bf5d4802d48d81862714df80b3239e1))

 - Unable to find commits
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-nrf52', 'codal-nrf52.git']' returned non-zero exit status 128.

### codal-nrf52 ([ea1b9ea...ea1b9ea](https://github.com/microbit-foundation/codal-nrf52/compare/ea1b9ea71f8693b76dbb46178e3a8d8c0a2ac0d8...ea1b9ea71f8693b76dbb46178e3a8d8c0a2ac0d8))

 - Unable to find commits

## [v0.0.5](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.4...v0.0.5)

 - Snapshot v0.0.5 (by Joe Finney)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-core', 'codal-core.git']' returned non-zero exit status 128.

### codal-core ([e9247e8...e9247e8](https://github.com/microbit-foundation/codal-core/compare/e9247e840c340e302528c42d550a06dcfaa8897b...e9247e840c340e302528c42d550a06dcfaa8897b))

 - Unable to find commits
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-mbedos', 'codal-mbedos.git']' returned non-zero exit status 128.

### codal-mbedos ([7f8b960...ed0c700](https://github.com/microbit-foundation/codal-mbedos/compare/7f8b96087a38addb7f6cea83e97eacbed6f04800...ed0c700604fd63e2f2baae8a83c69e8f9fc5c5f8))

 - Unable to find commits
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-nrf52', 'codal-nrf52.git']' returned non-zero exit status 128.

### codal-nrf52 ([ea1b9ea...ea1b9ea](https://github.com/microbit-foundation/codal-nrf52/compare/ea1b9ea71f8693b76dbb46178e3a8d8c0a2ac0d8...ea1b9ea71f8693b76dbb46178e3a8d8c0a2ac0d8))

 - Unable to find commits

## [v0.0.4](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.3...v0.0.4)

 - Update to mbedos-5.1.3 TARGET_NRF52_MICROBIT_v1_43 (by Joe Finney)
 - Update to v1.43 pinmapping (by Joe Finney)
 - Updates for v1.43 (by Joe Finney)
 - Updates for v1.43 (by Joe Finney)
 - Update locations of dependencies (by Joe Finney)
 - Snapshot v0.0.4 (by Joe Finney)

### codal-core ([886c58d...e9247e8](https://github.com/lancaster-university/codal-core/compare/886c58d9ddcdf77381b733f0abd7af44e1066d9b...e9247e840c340e302528c42d550a06dcfaa8897b))

 - Minor bugfixes to historic branch. (by Joe Finney)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-mbedos', 'codal-mbedos.git']' returned non-zero exit status 128.

### codal-mbedos ([17a43f4...7f8b960](https://github.com/microbit-foundation/codal-mbedos/compare/17a43f4637e2bbbe4f868dd856ef7a1517b6c6dc...7f8b96087a38addb7f6cea83e97eacbed6f04800))

 - Unable to find commits
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/microbit-foundation/codal-nrf52', 'codal-nrf52.git']' returned non-zero exit status 128.

### codal-nrf52 ([9aac236...ea1b9ea](https://github.com/microbit-foundation/codal-nrf52/compare/9aac23686a640372dd09074e796f9d4b0eb6e8fd...ea1b9ea71f8693b76dbb46178e3a8d8c0a2ac0d8))

 - Unable to find commits

## [v0.0.3](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.2...v0.0.3)

 - Update target json (by James Devine)
 - ADded dmesg guards (by James Devine)
 - changes to target.json (by James Devine)
 - Remove unused codepath (by Joe Finney)
 - Move to using native NRF52Timer (by Joe Finney)
 - Update target specification to reference new repo locations (by Joe Finney)
 - Add "HelloWorld" sample application (by Joe Finney)
 - Snapshot v0.0.3 (by Joe Finney)

### codal-core ([d7270e9...886c58d](https://github.com/lancaster-university/codal-core/compare/d7270e90d0b910f803c8d87024d5696bca9ffd41...886c58d9ddcdf77381b733f0abd7af44e1066d9b))

 - Add support for I2C repeated start transactions (by Joe Finney)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/finneyj/codal-mbedos', 'codal-mbedos.git']' returned non-zero exit status 128.

### codal-mbedos ([cbe9742...17a43f4](https://github.com/finneyj/codal-mbedos/compare/cbe97420f2481c9ba29d3ec5127d09db228f9dcb...17a43f4637e2bbbe4f868dd856ef7a1517b6c6dc))

 - Unable to find commits
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/jamesadevine/codal-nrf52', 'codal-nrf52.git']' returned non-zero exit status 128.

### codal-nrf52 ([1253824...9aac236](https://github.com/jamesadevine/codal-nrf52/compare/125382439498768f963e96d626397f51db6b068e...9aac23686a640372dd09074e796f9d4b0eb6e8fd))

 - Unable to find commits

## [v0.0.2](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.1...v0.0.2)

 - Snapshot v0.0.2 (by James Devine)
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/finneyj/codal-mbedos', 'codal-mbedos.git']' returned non-zero exit status 128.

### codal-mbedos ([cbe9742...cbe9742](https://github.com/finneyj/codal-mbedos/compare/cbe97420f2481c9ba29d3ec5127d09db228f9dcb...cbe97420f2481c9ba29d3ec5127d09db228f9dcb))

 - Unable to find commits
Cannot access repo: Command '['git', 'clone', '--bare', '--filter=blob:none', '--quiet', 'https://github.com/jamesadevine/codal-nrf52', 'codal-nrf52.git']' returned non-zero exit status 128.

### codal-nrf52 ([1253824...1253824](https://github.com/jamesadevine/codal-nrf52/compare/125382439498768f963e96d626397f51db6b068e...125382439498768f963e96d626397f51db6b068e))

 - Unable to find commits

# Changelog
*The head of this file is autogenerated and will be overwritten on the next tag.*

For official release notes, please see Releases.md

The current tag uses the following library versions:
 - codal-core = https://github.com/lancaster-university/codal-core/tree/1bc6d4d10c47f4fe34e2a54004530d5071e892d4
 - codal-nrf52 = https://github.com/lancaster-university/codal-nrf52/tree/1fbb7240290fe36a55c61378f5cdeb7640f3ec4a
 - codal-microbit-nrf5sdk = https://github.com/microbit-foundation/codal-microbit-nrf5sdk/tree/4b8abc690f6c9fca6132e6db5ee13a795a263f88

## [v0.3.4](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.3.3...v0.3.4)

 - CI: Update Changelog script to skip listing changelog edit commits. (by Carlos Pereira Atencio)
 - CI: Updates due to latest arm compiler not supporting Intel Mac. (by Carlos Pereira Atencio)
 - Snapshot v0.3.4 (by Carlos Pereira Atencio)

## [v0.3.3](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.3.2...v0.3.3)

 - Add additional validation checks around SoundEmjiSynthesizer, and guarantee mutual exclusion when IRQ triggers (by Joe Finney)
 - Bump to head of codal-core (by Joe Finney)

### codal-core ([8c8366f...1bc6d4d](https://github.com/lancaster-university/codal-core/compare/8c8366f9a1e92da69f90fe816456c4b9c42ffd13...1bc6d4d10c47f4fe34e2a54004530d5071e892d4))

 - ST7735 driver fix for screen hanging. (#186) (by KierPalin)

## [v0.3.2](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.3.1...v0.3.2)

 - CI: Update Windows runner from deprecated 2019 to oldest available 2022. (by Carlos Pereira Atencio)
 - Make writes to mManagedBuffer safe (by Joe Finney)
 - Snapshot v0.3.2 (by Carlos Pereira Atencio)

### codal-core ([a2f5051...8c8366f](https://github.com/lancaster-university/codal-core/compare/a2f5051dc60f89276546517f1077e75659ae6741...8c8366f9a1e92da69f90fe816456c4b9c42ffd13))

 - Prevent microphone LED from being onn all the time (bug #499) (by Joe Finney)
 - Smooth values in the 35..56dB range (by Joe Finney)

## [v0.3.1](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.3.0...v0.3.1)

 - CI: Now that all audio-refactor branches are merged, revert CI temp changes. (by Carlos Pereira Atencio)
 - Snapshot v0.3.1 (by Carlos Pereira Atencio)

### codal-core ([24f1284...a2f5051](https://github.com/lancaster-university/codal-core/compare/24f1284018657fd01a276ef554d16f9550d65d7f...a2f5051dc60f89276546517f1077e75659ae6741))

 - Ensure LevelDetectorSPL::getValue() waits for valid data before returning (#486) (by Joe Finney)
 - Fix bug related to quiet received buffer (by Joe Finney)
 - Fix bug #487 (by Joe Finney)

## [v0.3.0](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.71...v0.3.0)

 - Remove legacy code relater to runtime reconfiguration of audio pipeline (by Joe Finney)
 - Chnages ito support audio-refactor. (by Joe Finney)
 - Bugfix minor errors in pipeline (by Joe Finney)
 - bugfix (by Joe Finney)
 - Set default sample rate back to 11000 (by Joe Finney)
 - Reduce clicking noise on microphone acivation (by Joe Finney)
 - Increase start up delay to 3 frames, to avoid occasional clicks. (by Joe Finney)
 - Reduce sensitivity of microphone to permit detection of 35db (by Joe Finney)
 - Introduce multitrack audio playback capability (by Joe Finney)
 - Expose address that marks the usable top of flash memory (by Joe Finney)
 - Update target-locked to allow for a fake tagged-released for micromusic testing (by Joe Finney)
 - Use 32 bit addressing for limit macros (by Joe Finney)
 - Update target-locked hashes (by Joe Finney)
 - Add static definitions of TOP_OF_FLASH (by Joe Finney)
 - Update target-locked hashes. (by Carlos Pereira Atencio)
 - ci: Run CI workslflows with projects using the latest audio refactor. (by Carlos Pereira Atencio)
 - Add message bus listener removal handler for LevelDetectorSPL instance (by Joe Finney)
 - Update target-locked hashes. (by Carlos Pereira Atencio)
 - Return levelDetectorSPL settings to previous values (bug #472) (by Joe Finney)
 - Default Mixerchannel rate to upstream default on creation. (by Joe Finney)
 - Set default microphone samplerate by a CONFIG option (by Joe Finney)
 - Update target-locked to latest revision (by Joe Finney)
 - Filter audio data that can arrive when hardware is disabled (#478) (by Joe Finney)
 - bump version (by Joe Finney)
 - Snapshot v0.3.0 (by Carlos Pereira Atencio)

### codal-core ([238e938...24f1284](https://github.com/lancaster-university/codal-core/compare/238e93846b434b730d4d0bfde99b43ccfde29cef...24f1284018657fd01a276ef554d16f9550d65d7f))

 - Remove runtime configuration of bitrate for StreamSplitter (by Joe Finney)
 - Remove legacy code. (by Joe Finney)
 - Introduce DataSourceSink base class (by Joe Finney)
 - More refactoring. (by Joe Finney)
 - Bugfix minor errors (by Joe Finney)
 - Add tri-state for connectedness of audio filters (by Joe Finney)
 - Refactored stream recording class (by Joe Finney)
 - Bugfix for smaller packets being received from the microphone (by Joe Finney)
 - Enable back to back recordAsync / playbackAsync (by Joe Finney)
 - Fix typo in test expression. classic! (by Joe Finney)
 - Introduce (optional) StreamAnalyzer class (by Joe Finney)
 - Update LevelDetectorSPL to always wake sleeping fibers. (by Joe Finney)
 - Fix bug relating to reset of LevelDetectorSPL state (by Joe Finney)
 - Add support for detecting removal of MessageBus listeners (by Joe Finney)
 - Fix synchronous playback bug via SerialStreamer API (bug #477) (by Joe Finney)
 - Introduce SerialStreamer class into codal-core (by Joe Finney)
 - Register SerialStreamer component as always wantingData (by Joe Finney)
 - Remove stale data stored in DataStream (by Joe Finney)
 - Add noise gate to level detector SPL (by Joe Finney)
 - Add optional support to filter unwanted packets at source (by Joe Finney)
 - Create tailored dataWanted() behaviour for SerialStremaer (#485) (by Joe Finney)

### codal-nrf52 ([8802eb4...1fbb724](https://github.com/lancaster-university/codal-nrf52/compare/8802eb49140e0389e535cb6160d9080efd951ba7...1fbb7240290fe36a55c61378f5cdeb7640f3ec4a))

 - Updated documentation for NRF52I2C and NRF52Pin (both .h & .cpp) (#55) (by KierPalin)
 - Eliminate compiler warning about derived method signature (#57) (by Kevin Walsh)
 - Minor change to support debugging (by Joe Finney)
 - Changes to support audio-refactor. (by Joe Finney)
 - Bugfix minor errors (by Joe Finney)
 - Bugfix (by Joe Finney)
 - Add configurable startup delay to ADC channels (by Joe Finney)
 - Fix audio channel release (bug #476) (by Joe Finney)
 - Add interface to allow sample rate of microphone to be defined (#474) (by Joe Finney)
 - Add basic validation for defining microphone ADC samplerate (by Joe Finney)

### codal-microbit-nrf5sdk ([d41d5c7...4b8abc6](https://github.com/microbit-foundation/codal-microbit-nrf5sdk/compare/d41d5c7ebe53a1d01935e61d4ffa891e5112e119...4b8abc690f6c9fca6132e6db5ee13a795a263f88))

 - Suppress all warnings from this target as it's 3rd party code. (#18) (by Carlos Pereira Atencio)

## [v0.2.71](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.70...v0.2.71)

 - Fix first use of io.logo.isTouched() (#480) (by Martin Williams)
 - Save a bit of memory from MicroBitIO. (#466) (by Carlos Pereira Atencio)
 - Snapshot v0.2.71 (by Carlos Pereira Atencio)

## [v0.2.70](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.69...v0.2.70)

 - CI: Update mac runner version to macOS 13, as 12 is deprecated. (by Carlos Pereira Atencio)
 - Fix Pi value typo in SoundSynthesizerEffects. (#456) (by Carlos Pereira Atencio)
 - CI: Reduce the min GCC version used to match what MakeCode (PXT) uses. (#461) (by Carlos Pereira Atencio)
 - Update MicroBitAudio.isPlaying() to take into account initial silence. (#452) (by Carlos Pereira Atencio)
 - Add 'show_size' option to target.json file to print build size info. (#372) (by Carlos Pereira Atencio)
 - ci: Also build MakeCode & MicroPython on PRs. (#473) (by Carlos Pereira Atencio)
 - Eliminate noreturn compiler warnings (#467) (by Kevin Walsh)
 - CI: Update deprecated GH Actions runner image ubuntu-20.4 to 22.04. (by Carlos Pereira Atencio)
 - Ensure float operations in Sound Expression/Synth & Compass cal. (#454) (by Carlos Pereira Atencio)
 - Overwrite default CODAL `uBit.audio.levelSPL` 8-bit conversion range. (#481) (by Carlos Pereira Atencio)
 - Snapshot v0.2.70 (by Carlos Pereira Atencio)

### codal-core ([c43b899...238e938](https://github.com/lancaster-university/codal-core/compare/c43b89936ed72c022f2649f5540a9a74ef48fd04...238e93846b434b730d4d0bfde99b43ccfde29cef))

 - Ensure float literals and float versions of math functions are used. (#176) (by Carlos Pereira Atencio)
 - Fix MemorySource::playAsync for ManagedBuffer (#180) (by Martin Williams)
 - Fix TouchSensor::removeTouchButton (#179) (by Martin Williams)
 - Fix SerialStreamer::duration() (#172) (by Martin Williams)
 - Add compiler specific attributes for FORCE_RAM_FUNC. (#166) (by Carlos Pereira Atencio)
 - Fix Compass::init() warning and array delete bug (#181) (by Kevin Walsh)
 - LevelDetectorSPL: Cast `LEVEL_DETECTOR_SPL_8BIT_X_POINT` to floats. (#182) (by Carlos Pereira Atencio)

## [v0.2.69](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.68...v0.2.69)

 - Fix CODAL_VERSION config value defined in target.json (by Carlos Pereira Atencio)
 - CI: Changelog script to also include libs when created from scratch. (by Carlos Pereira Atencio)
 - CI: Update Changelog script to clone libs in tmp dir to get history. (by Carlos Pereira Atencio)
 - Snapshot v0.2.69 (by Carlos Pereira Atencio)

### codal-core ([509086c...c43b899](https://github.com/lancaster-university/codal-core/compare/509086cc8590465041b15493ab52b56e7071c110...c43b89936ed72c022f2649f5540a9a74ef48fd04))

 - StreamRecording - calculate memory overhead of buffers (#175) (by Martin Williams)

## [v0.2.68](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.67...v0.2.68)

 - Allow log full FUL to be written to last byte (#438) (by Martin Williams)
 - Temporarily set CODAL_VERSION back to "unknown". (#445) (by Carlos Pereira Atencio)
 - Snapshot v0.2.68 (by Carlos Pereira Atencio)

## [v0.2.67](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.66...v0.2.67)

 - Update Changelog.md (by Dr John Vidler)
 - CI: Restore building MakeCode with Docker. (#407) (by Carlos Pereira Atencio)
 - CI: Update versions of actions to remove warnings about node 16. (#408) (by Carlos Pereira Atencio)
 - Remove uBit.io.face as it was a temp additional that is no longer needed. (#410) (by Carlos Pereira Atencio)
 - Added definitions to match the Makecode effects parameters (#417) (by Dr John Vidler)
 - CI:Update Changelog script to include changes in lib dependencies. (#420) (by Carlos Pereira Atencio)
 - Set uBit.io.logo to capacitive touch mode by default. (#418) (by Carlos Pereira Atencio)
 - CI: PXT build workaround due to deprecated PXT docker image type. (#435) (by Carlos Pereira Atencio)
 - CI: Update macOS runner version as 11 is deprecated. (#436) (by Carlos Pereira Atencio)
 - Populate MICROBIT_DAL_VERSION and add microbit_dal_version(). (#434) (by Carlos Pereira Atencio)
 - Addition of getRows(n, k) and getNumberOfRows(n) to the datalogger.  (#431) (by KierPalin)
 - Ensure header files and .cpp files use the codal namespace appropriately. (#437) (by Carlos Pereira Atencio)
 - Snapshot v0.2.67 (by Carlos Pereira Atencio)

### codal-core ([992c0b1...509086c](https://github.com/lancaster-university/codal-core/compare/992c0b11a0eb2a1edca9c2f76821f89a99a3acec...509086cc8590465041b15493ab52b56e7071c110))

 - Update docstrings for LevelDetector threshold methods. (#167) (by Carlos Pereira Atencio)
 - Display scroll an empty char if the requested char is out-of-range. (#168) (by Carlos Pereira Atencio)
 - Better version support symbols for user applications (#169) (by Dr John Vidler)
 - Ensure .cpp files are `using namespace codal`. (#170) (by Carlos Pereira Atencio)
 - Add config flag to enable/disable adding codal to the global namespace. (#171) (by Carlos Pereira Atencio)

### codal-nrf52 ([0643733...8802eb4](https://github.com/lancaster-university/codal-nrf52/compare/0643733703b4f8f788af81a996f40d1f1a1527bc...8802eb49140e0389e535cb6160d9080efd951ba7))

 - NRF52Pin::wasTouched() default w/o args to use current pin touch mode state. (#53) (by Carlos Pereira Atencio)
 - Ensure all .cpp files have `using namespace codal`. (#54) (by Carlos Pereira Atencio)
 - Set neopixel functions inside the codal namespace. (by Carlos Pereira Atencio)
 - Wrap `using namespace` in headers with codal config flag. (#43) (by Carlos Pereira Atencio)

## [v0.2.66](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.65...v0.2.66)

 - Update Changelog.md (by Dr John Vidler)
 - Snapshot v0.2.66 (by Dr John Vidler)

### codal-core ([7b6f9df...992c0b1](https://github.com/lancaster-university/codal-core/compare/7b6f9df3b586f8dd9bede4bd0baa0cdc212b6ba2...992c0b11a0eb2a1edca9c2f76821f89a99a3acec))

 - Applying the patch suggested by @dpgeorge to handle large receiving buffers for pullInto() (by Dr John Vidler)

## [v0.2.65](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.63...v0.2.65)

 - Corrected Changelog.md after my tag snarfu (by Dr John Vidler)
 - Refined the changelog script to only update the head of the log under normal cases (by Dr John Vidler)
 - Set MICROBIT_RADIO_MAX_PACKET_SIZE as a configurable value. (#387) (by Carlos Pereira Atencio)
 - Change datalog CSV download mime type from text/plain to text/csv (#339) (by Martin Williams)
 - Added a new CONFIG_MIXER_DEFAULT_CHANNEL_SAMPLERATE constant for channels with no defined rate to use (by Dr John Vidler)
 - Fixed a bug where in some conditions the microphone would not auto-start (by Dr John Vidler)
 - Update MicroBitLog to support 3 header formats (#399) (by Matt Hillsdon)
 - Added MicroBitButton.h to the required includes for MicroBitCompat.h so its still included in the build when not using bluetooth (by Dr John Vidler)
 - Moved the MicroBitButton inclusion to the tail of MicroBitCompat to avoid circular dependencies or a bunch of needless forward declarations (by Dr John Vidler)
 - Bumped the default Mixer2 channel sample rate back up to 44100 (by Dr John Vidler)
 - Snapshot v0.2.65 (by Dr John Vidler)

### codal-core ([63f017f...7b6f9df](https://github.com/lancaster-university/codal-core/compare/63f017fdcd12aca769264c36fe47e6071a40e502...7b6f9df3b586f8dd9bede4bd0baa0cdc212b6ba2))

 - Updated to correctly restore mic state on returning from a deep sleep (by Dr John Vidler)
 - Typo (by Dr John Vidler)
 - Working audio resampling rates if requested by downstream components of a SplitterChannel (by Dr John Vidler)

### codal-microbit-nrf5sdk ([5714cbe...d41d5c7](https://github.com/microbit-foundation/codal-microbit-nrf5sdk/compare/5714cbe338c3544793bbb841a7ae81708a3ffae1...d41d5c7ebe53a1d01935e61d4ffa891e5112e119))

 - Revert "Quieted a warning on fallthrough, as this is intended behaviour" (by Dr John Vidler)

## [v0.2.63](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.62...v0.2.63)

 - Removed deactivateLevelSPL entirely as per codal-microbit-v2 issue #222 (by Dr John Vidler)
 - Fix for the microphone failing to wake after a sleep event (by Dr John Vidler)
 - Commented out a temporary alias for the face/logo touch button, staged for removal (by Dr John Vidler)
 - Revert for the face alias removal, for v0.2.63 release (by Dr John Vidler)
 - Snapshot v0.2.63 (by Dr John Vidler)
 - Reverted the power saving for the LED Matrix to avoid strange issues with Pins. (by Dr John Vidler)
 - Snapshot v0.2.64 (by Dr John Vidler)
 - Updated target-locked with the corrected tag version (by Dr John Vidler)

### codal-core ([b06b603...63f017f](https://github.com/lancaster-university/codal-core/compare/b06b6031df2f3e9fd9459fbe13a0db95a5cc47f0...63f017fdcd12aca769264c36fe47e6071a40e502))

 - Moved to a bool check on new buffers to prevent race conditions when ISRs fire (by Dr John Vidler)
 - Updated the recording to use 60k of RAM and guarded the debug output (by Dr John Vidler)
 - Added clickCount tracking on touch events (by Dr John Vidler)
 - Removed clickcount and dmesg from TouchButton, as this is handled in buttonActive() (by Dr John Vidler)
 - Revert of the maximum StreamRecording memory usage for v0.2.63 to be safe (by Dr John Vidler)

### codal-nrf52 ([099f15a...0643733](https://github.com/lancaster-university/codal-nrf52/compare/099f15ab7932ca902976262b989890f0edb3b1ce...0643733703b4f8f788af81a996f40d1f1a1527bc))

 - Added wasTouched support to NRF52Pins, to match the isTouched functionality (by Dr John Vidler)
 - Updated documentation for further clarity for wasTouched behaviour, added a touch mode parameter to the call (by Dr John Vidler)

### codal-microbit-nrf5sdk ([f98e1a9...5714cbe](https://github.com/microbit-foundation/codal-microbit-nrf5sdk/compare/f98e1a9325acef9de52416b3e30cc820531b6699...5714cbe338c3544793bbb841a7ae81708a3ffae1))

 - Quieted a warning on fallthrough, as this is intended behaviour (by Dr John Vidler)

## [v0.2.62](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.61...v0.2.62)

 - Snapshot v0.2.61-master.0 (by Dr John Vidler)
 - Snapshot v0.2.61-master.1 (by Dr John Vidler)
 - Snapshot v0.2.61-master.2 (by Dr John Vidler)
 - Update target-locked.json codal-core commit to fix builds. (by Carlos Pereira Atencio)
 - Snapshot v0.2.62 (by Dr John Vidler)

### codal-core ([a116354...b06b603](https://github.com/lancaster-university/codal-core/compare/a1163541d10c0a0d3dd9b2e8ac1b8906100449ad...b06b6031df2f3e9fd9459fbe13a0db95a5cc47f0))

 - Added pointer debugging to the audio recording. Fixed ref-not-value error (by Dr John Vidler)
 - Exposd a call to dump the heap state when not in full memory debug mode (by Dr John Vidler)
 - Exposd a call to dump the heap state when not in full memory debug mode (by Dr John Vidler)
 - Made the heap allocator less chatty, so we can identify anomolies (by Dr John Vidler)
 - Fix issue building without DMESG_SERIAL_DEBUG. (by Carlos Pereira Atencio)
 - Re-enabled temporarily disabled debug output (by Dr John Vidler)

### codal-nrf52 ([2d77a3c...099f15a](https://github.com/lancaster-university/codal-nrf52/compare/2d77a3c89d14c5f4344e73511ca32086a0fcc8ad...099f15ab7932ca902976262b989890f0edb3b1ce))

 - Add more docs about updating nrfx submodule (#52) (by Carlos Pereira Atencio)

## [v0.2.61](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.60...v0.2.61)

 - Shorter reset to Bluetooth mode animation (#368) (by Martin Williams)
 - Added a hook to manually deactivate the microphone when we deep sleep (by Dr John Vidler)
 - Snapshot v0.2.60-master.0 (by Dr John Vidler)
 - Snapshot v0.2.60-master.1 (by Dr John Vidler)
 - Snapshot v0.2.61 (by Dr John Vidler)

### codal-core ([c2bc323...a116354](https://github.com/lancaster-university/codal-core/compare/c2bc323334e95bd7fc7e91bf64d782b575ce98ae...a1163541d10c0a0d3dd9b2e8ac1b8906100449ad))

 - Reverted to referencing the existing buffer, rather than an explicit copy constructor (by Dr John Vidler)
 - Lowered the default max recording size to 50k, to avoid OOMs with higher level languages (MakeCode, etc.) (by Dr John Vidler)

### codal-nrf52 ([4b2d834...2d77a3c](https://github.com/lancaster-university/codal-nrf52/compare/4b2d8342ea37a4fd13de340c0fff63526ea5465d...2d77a3c89d14c5f4344e73511ca32086a0fcc8ad))

 - [Clang Compatibility] Patch to define the .heap section as allocatable. (#51) (by Johnn333)

### codal-microbit-nrf5sdk ([ef4662e...f98e1a9](https://github.com/microbit-foundation/codal-microbit-nrf5sdk/compare/ef4662e13875a7b03e7296d7ac24a2b4d231f323...f98e1a9325acef9de52416b3e30cc820531b6699))

 - [Clang compatibility] A revised Clang patch. (#17) (by Johnn333)

## [v0.2.60](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.59...v0.2.60)

 - Fix issue when missing motion sensors crashed instead of panicking. (#360) (by Carlos Pereira Atencio)
 - Minor fix of seedRandom return value. (#369) (by Carlos Pereira Atencio)
 - Minor cleanup of config flags. (#363) (by Carlos Pereira Atencio)
 - radio: Reduce power level options to match V1 DAL. (#275) (by Carlos Pereira Atencio)
 - BLE: Simplify device name string creation. (#371) (by Carlos Pereira Atencio)
 - Snapshot v0.2.60 (by Dr John Vidler)

### codal-core ([32cc87c...c2bc323](https://github.com/lancaster-university/codal-core/compare/32cc87c3298a49663c3b3b7b1881ec7aeb3d73a5...c2bc323334e95bd7fc7e91bf64d782b575ce98ae))

 - Preprocessor Directives to ignore builtin clang function (by Johnn333)
 - Make AbstractButton::isPressed() pure virtual. (by Carlos Pereira Atencio)
 - Make all methods in NVMController pure virtual. (by Carlos Pereira Atencio)
 - Make motion sensors configure() & requestUpdate() pure virtual. (by Carlos Pereira Atencio)
 - Removed unused variable in CodalFiber (by Dr John Vidler)
 - Extended CodalAssert with an explicit failure case (for unimplmented functions, for example) and forced a DMESG flush after each to ensure we get serial output (by Dr John Vidler)
 - Added a set of C function hooks to patch holes in the Newlib API to squash the GCC v12+ warnings (by Dr John Vidler)
 - Added the weak attribute to all the nosys support functions, so folks can override them later (by Dr John Vidler)
 - Added gcc version guard to the source too, just in case (by Dr John Vidler)
 - Removed direct flush calls in favour of DMESGF (by Dr John Vidler)
 - Enforce that we always create a new buffer. Slower, but constantly-time slower (by Dr John Vidler)
 - StreamRecording now keeps track of the last received sample rate, and will report this rate except when it has no buffered data, when it will report the upstream rate. (by Dr John Vidler)

### codal-nrf52 ([2dbf5aa...4b2d834](https://github.com/lancaster-university/codal-nrf52/compare/2dbf5aa214c7432c0b1b8a19e92a3f9a4fc4b9da...4b2d8342ea37a4fd13de340c0fff63526ea5465d))

 - Apply workaround for nRF52833 errata 78 for timer high power consumption. (#49) (by Carlos Pereira Atencio)
 - Explicitly set SAADC channel config mode to single ended. (#50) (by Carlos Pereira Atencio)
 - [Clang compatibility] Update nrfx version to 2.1.0 (#48) (by Johnn333)

## [v0.2.59](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.58...v0.2.59)

 - Added an on-release action to grab library builds (by Dr John Vidler)
 - Reduce power consumption of LED matrix on power off mode. (#366) (by Carlos Pereira Atencio)
 - Snapshot v0.2.59 (by Carlos Pereira Atencio)

## [v0.2.58](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.57...v0.2.58)

 - CI: Add option to size diff workflow to configure samples repo commit. (#358) (by Carlos Pereira Atencio)
 - CI: Temporarily disable pxt docker build until image becomes available. (#359) (by Carlos Pereira Atencio)
 - MicroBitBLEManager - Move MICROBIT_BLE_MAXIMUM_BONDS to MicroBitConfig (#299) (by Martin Williams)
 - Snapshot v0.2.58 (by Carlos Pereira Atencio)

### codal-core ([2658178...32cc87c](https://github.com/lancaster-university/codal-core/compare/2658178aade744af248b116c2e4277b94a242b69...32cc87c3298a49663c3b3b7b1881ec7aeb3d73a5))

 - Compass::clearCalibration() resets calibration member variable (by Martin Williams)
 - Update touch code to release/reconnect on pin reuse (codal-microbit-v2#345) (by Joe Finney)

### codal-nrf52 ([84b1aee...2dbf5aa](https://github.com/lancaster-university/codal-nrf52/compare/84b1aeec870addcfb6027a924dbfe83f1f10c6db...2dbf5aa214c7432c0b1b8a19e92a3f9a4fc4b9da))

 - Update touch code to release/reconnect on pin reuse (codal-microbit-v2#345) (by Joe Finney)

## [v0.2.57](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.56...v0.2.57)

 - Disbled the MICROBIT_BLE_UTILITY_SERVICE_PAIRING configuration by default, enabled on BETA (by Dr John Vidler)
 - Snapshot v0.2.57 (by Dr John Vidler)

### codal-core ([763a2d1...2658178](https://github.com/lancaster-university/codal-core/compare/763a2d1a916d4db3fe3f3f2b1eaa9947cc8d0a7e...2658178aade744af248b116c2e4277b94a242b69))

 - Added a "fast path" route to reduce delays on getValue calls (by Dr John Vidler)
 - Added a new arg to FiberLock to run in legacy MUTEX mode, or SEMAPHORE mode with corrected semantics. (by Dr John Vidler)
 - Added missing public as part of declaration (by Dr John Vidler)

## [v0.2.56](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.55...v0.2.56)

 - Added a little extra to the git changelog generator; it now created a tree-link list for the libraries on the latest tag (by Dr John Vidler)
 - Remove FXOS vestiges. (#302) (by Carlos Pereira Atencio)
 - Removed unused include for the old LevelDetector, updated event handling for demand activation (by Dr John Vidler)
 - MicroBitUtilityService - replaces PR 178 (#287) (by Martin Williams)
 - Fixed a whitespace-based merge conflict! (by Dr John Vidler)
 - MicroBitCompassCalibrator avoid using max(int,int) (#290) (by Martin Williams)
 - CI uPy: Don't checkout submodules recursively. (#351) (by Carlos Pereira Atencio)
 - Added isMicrophoneEnabled() to MicroBitAudio to match isSpeakerEnabled (by Dr John Vidler)
 - Fixed #if logic (by Dr John Vidler)
 - Removed spurious duplicate documentation, added support for isMicrophoneEnabled (by Dr John Vidler)
 - Snapshot v0.2.56 (by Dr John Vidler)

### codal-core ([bf65ebe...763a2d1](https://github.com/lancaster-university/codal-core/compare/bf65ebed2003049bd026e1f685bb756e189c9258...763a2d1a916d4db3fe3f3f2b1eaa9947cc8d0a7e))

 - Added a lock to the initial access/events for the level detector, to prevent spurious values and events on cold-start (by Dr John Vidler)
 - Added note that this is actually a kind of hidden deep copy (by Dr John Vidler)
 - Added a note on stripping the least significant bits as part of LPF (by Dr John Vidler)
 - setGain and getGain now return floats, as they use fractional values internally (by Dr John Vidler)
 - StreamRecording objects now use a linked list of buffers for storage (by Dr John Vidler)
 - StreamSplitter now correctly connects to its upstream (by Dr John Vidler)
 - Added virtual to isConnected on DataSource to prevent internal misrouting (by Dr John Vidler)
 - Added extended assert support, requires CODAL flags to enable (by Dr John Vidler)
 - Extended DMESG with optional Fiber and Time markers (by Dr John Vidler)
 - Updated CodalFiber to correctly behave as a counting semaphore (by Dr John Vidler)
 - Corrected locking behaviour on the LevelDetectorSPL to avoid race conditions (by Dr John Vidler)
 - Updated documentation for StreamRecording (by Dr John Vidler)
 - Removed unused assert (by Dr John Vidler)
 - Refactored assert to remove assert_true and assert_false in favour of a single unified assert call (by Dr John Vidler)
 - Set the default FiberLock count to 1, as other APIs expect that the lock starts UNLOCKED rather than LOCKED (by Dr John Vidler)

## [v0.2.55](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.54...v0.2.55)

 - Added additional workflow status badges (by Dr John Vidler)
 - Fix BLE panic 071 (#334) (by Martin Williams)
 - Snapshot v0.2.55 (by Dr John Vidler)

### codal-nrf52 ([76dbf90...84b1aee](https://github.com/lancaster-university/codal-nrf52/compare/76dbf9088a3dc2e273e3c37134dc661985aac609...84b1aeec870addcfb6027a924dbfe83f1f10c6db))

 - Fix NeoPixel/WS2812B timing. (#47) (by Jim Mussared)

## [v0.2.54](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.53...v0.2.54)

 - Update update-changelog.yml (by Dr John Vidler)
 - Snapshot v0.2.54 (by Dr John Vidler)

### codal-core ([94cbd39...bf65ebe](https://github.com/lancaster-university/codal-core/compare/94cbd3933c61924677fc1f2255eaf61bd175a0d1...bf65ebed2003049bd026e1f685bb756e189c9258))

 - Replaced an invalid schedule() call with a fiber_sleep for the synchronous record and playback calls. (by Dr John Vidler)

## [v0.2.53](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.52...v0.2.53)

 - Added an automated changelog script and github action, for testing (by Dr John Vidler)
 - Added an automated changelog script and github action, for testing (by Dr John Vidler)
 - CI MakeCode: Use the default pxt-core and pxt-common-packages from package.json (#308) (by Carlos Pereira Atencio)
 - Improve sound quality of virtual sound pin (#312) (by Joe Finney)
 - !!BREAKING CHANGE!! - The old level detector is now removed, in favour of the SPL level detector. Users who need the old one must create their own (by Dr John Vidler)
 - Snapshot v0.2.53 (by Dr John Vidler)

### codal-core ([a1e1db6...94cbd39](https://github.com/lancaster-university/codal-core/compare/a1e1db692909a260ab341c89b292e085daecd259...94cbd3933c61924677fc1f2255eaf61bd175a0d1))

 - Added functionality described in issue #326, removed duplicate documentation in cpp. (by Dr John Vidler)
 - Adjusted CODAL_STREAM_IDLE_TIMEOUT_MS down to 75ms - Can be adjusted in codal.json if required by higher-level applications (by Dr John Vidler)
 - - Added async and sync variants of play and record, for convenience - Removed unusued bool and check for timeouts in getValue (by Dr John Vidler)

## [v0.2.52](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.51...v0.2.52)

 - Update README.md with status badges (by Dr John Vidler)
 - Added volume and sample rate control for MixerChannels, added documentation to match (by Dr John Vidler)
 - Implemented new API requirements for Mixer2, Synth and virtual output pins (by Dr John Vidler)
 - Snapshot v0.2.52 (by Dr John Vidler)

### codal-core ([4b2b35e...a1e1db6](https://github.com/lancaster-university/codal-core/compare/4b2b35e3b0db84d62a4cf3d3b582da66974793fb...a1e1db692909a260ab341c89b292e085daecd259))

 - Updated streams API to include connection checks by API requirement, Fixed the level detectors to correctly handle initial connections to their upstream (by Dr John Vidler)

### codal-nrf52 ([5fa1d16...76dbf90](https://github.com/lancaster-university/codal-nrf52/compare/5fa1d16d6de6bd2848ae776e3039ce6bcad4e3e1...76dbf9088a3dc2e273e3c37134dc661985aac609))

 - Updated the ADC channels to use the new requirements for stream API classes (by Dr John Vidler)

## [v0.2.51](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.50...v0.2.51)

 - Change yotta module.json dependencies to foundation org. (by microbit-carlos)
 - Revert "Update target name to reflect micro:bit version number" (by microbit-carlos)
 - CI PXT: Restrict Yotta dependencies as workaround to pip install errors. (#288) (by Carlos Pereira Atencio)
 - CI: Ensure necessary repos are cloned with submodules. (#292) (by Carlos Pereira Atencio)
 - Add UX feature: Triple-tap-reset to enter pairing mode (by Joe Finney)
 - Generalize NOINIT region (by Joe Finney)
 - Remove target.json DEVICE_DMESG_BUFFER_SIZE to use default (#298) (by Carlos Pereira Atencio)
 - Updated MicroBitAudio and the Mixer to correctly work with the new streams API (by Dr John Vidler)
 - Reverted a change to the project name which broke the build entirely (by Dr John Vidler)
 - Snapshot v0.2.51 (by Dr John Vidler)

### codal-core ([350a794...4b2b35e](https://github.com/lancaster-university/codal-core/compare/350a794a6f43d8e91b4762f110bad675169dc908...4b2b35e3b0db84d62a4cf3d3b582da66974793fb))

 - Minor bugfixes to historic branch. (by Joe Finney)
 - Add config option for printf (by James Devine)
 - remove warnings (by James Devine)
 - Remove type punned pointer warning (by Joe Finney)
 - DMESG cleanup (by Joe Finney)
 - Make update() virtual (by Joe Finney)
 - Add variant of readRegister() method (by Joe Finney)
 - Parameterise Active Hi/Lo functionality on IRQ line. (by Joe Finney)
 - Fix typos in comments (by Joe Finney)
 - Introduce LSM303 Combined Acceleromter/Magnetometer Driver (by Joe Finney)
 - Fix Coordinate System Translations (by Joe Finney)
 - Make FXOS8700 driver compatible with 32 bit Sample3D structure (by Joe Finney)
 - Introduce FXOS8700 and LSM303 support for shared IRQ lines (by Joe Finney)
 - Introduce Compass::setAccelerometer() method (by Joe Finney)
 - Introduce ACTIVE_HI / ACTIVE_LO functionality for generic Pin (by Joe Finney)
 - Updates sensor drivers to use Pin::isActive() (by Joe Finney)
 - Upstream improved compass calibration model (by Joe Finney)
 - Introduce isDetected() method into FXOS8700 driver (by Joe Finney)
 - Ensure variables are fully initialized (by Joe Finney)
 - Add NOP definitions for DMESGN() and DMESGF() (by Joe Finney)
 - prevent timer underflow caused by very short wait periods (by Joe Finney)
 - remove enable/disable irq from printf. (by James Devine)
 - Introduce MemorySource (by Joe Finney)
 - Parameterise constructor to MemorySource (by Joe Finney)
 - All virtual classes need a virtual destructor (by Michal Moskal)
 - yotta: Add yotta compatibility (by Raphael Gault)
 - Remove legacy FXOS8700Accelerometer class (by Joe Finney)
 - Set default DEVICE_DMESG_BUFFER_SIZE based on DMESG_ENABLE. (#160) (by Carlos Pereira Atencio)
 - Updated DataStream to support decoupling the input fiber from the output to enable async stream data (by Dr John Vidler)
 - Updated StreamRecording to use the new PR/Pull metrics, removed old debug code (by Dr John Vidler)
 - Removed unused events, patched against the new PR/Pull metrics, removed spurious debug output, removed old debug code (by Dr John Vidler)
 - EffectFilters now correctly create deep copies of ManagedBuffers marked readOnly before editing their contents (by Dr John Vidler)
 - Updated FIFOStream to match the new PR/Pull metrics (by Dr John Vidler)
 - Inserted missing setBeta() call in constructor to initialise the object beta value (by Dr John Vidler)
 - Removed old debug code, updated constructor signature (internal behavioural change, no caller updates needed), component now correctly uses PR/Pull metrics (by Dr John Vidler)
 - Corrected pullRequest return values based on component loading (by Dr John Vidler)
 - Corrected pullRequest return values based on component loading (by Dr John Vidler)

## [v0.2.50](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.49...v0.2.50)

 - CI: Enable manual triggers for size diff workflow. (#282) (by Carlos Pereira Atencio)
 - Introduce sound playback "until playback complete" option (#231) (by Joe Finney)
 - Fixed the stream wiring in MicroBitAudio, and updated the related classes to align to the incoming stream API updates (by Dr John Vidler)
 - Updated the readme with a little on the tag/release policy (by Dr John Vidler)
 - Added more info, per Carlos suggestion in the issue (by Dr John Vidler)
 - Snapshot v0.2.50 (by Dr John Vidler)

### codal-core ([bf23caa...350a794](https://github.com/lancaster-university/codal-core/compare/bf23caab5e19b055bca8718f7bb52a763d077ff7...350a794a6f43d8e91b4762f110bad675169dc908))

 - Introduce Pin Disconnecting flag (by Joe Finney)
 - Prevent fibers from momopolising access to AnimatedDisplay (#253) (by Joe Finney)
 - Revert commit 434f12e52b9e551c96422ef76e6f9f3c5f6fa803 (#253) (by Joe Finney)
 - Only wipe specified region on large ManagedBuffer::shift (#157) (by C272)
 - add rms based clap detection to level detector spl (by Oliver Hulland)
 - made lower threshold once in clap to consider noise over. allows better isolation of non clap noises (longer noises for example) (by Oliver Hulland)
 - refactor, fix short taps - now register less and don't allow fast subsequent ones (by Oliver Hulland)
 - Remove DMESG and add comments (by Oliver Hulland)
 - Reworked and updated the streams API; still a fair amount of debug code in here that will need cleaning out before tag/release (by Dr John Vidler)
 - Added a Doxygen skip rule for ManagedString "+" operators, as sphinx/breathe/exhale cant deal with this structure yet (by Dr John Vidler)
 - Completed the fix for this issue: https://github.com/lancaster-university/codal-microbit-v2/issues/278 (by Dr John Vidler)
 - Added initial values for lpf_value and lpf_beta (by Dr John Vidler)

### codal-nrf52 ([14bcffe...5fa1d16](https://github.com/lancaster-university/codal-nrf52/compare/14bcffe406a044e50d89616b97bd3757d0a00e82...5fa1d16d6de6bd2848ae776e3039ce6bcad4e3e1))

 - Prevent recursive disconnet loops in NRF52Pin and accidental erasure of obj state (by Joe Finney)

## [v0.2.49](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.48...v0.2.49)

 - MicroBitRadio: fix setFrequencyBand() and do not reset tx power and frequency band in enable() (#252) (by Martin Williams)
 - docstrings: Add granularity info to MicroBit::sleep() & remove return. (#248) (by Carlos Pereira Atencio)
 - docstrings: Minor fix in documented define name. (#246) (by Carlos Pereira Atencio)
 - Simplify the BLE - Device Info - Model Number to only indicate V2. (#255) (by Carlos Pereira Atencio)
 - CI: Add build workflow using microbit-v2-samples. (#256) (by Carlos Pereira Atencio)
 - Added a small patch to confirm that all splitter channels have gone before turning the microphone off (by Dr John Vidler)
 - CI: Workflow to calculate build size diff before & after PR/commit. (#262) (by Carlos Pereira Atencio)
 - CI: Build MicroPython on each push. (#276) (by Carlos Pereira Atencio)
 - CI: Build MakeCode with and without docker on each commit. (#274) (by Carlos Pereira Atencio)
 - Added information on demand-wake for the audio pipeline and fixed the wake-up sequence (by Dr John Vidler)
 - Snapshot v0.2.49 (by Dr John Vidler)

### codal-core ([75b92a9...bf23caa](https://github.com/lancaster-university/codal-core/compare/75b92a9ef3a206cdc63f65576c03e2aa72b4a45b...bf23caab5e19b055bca8718f7bb52a763d077ff7))

 - StreamSplitters now emit finer-grained events to enable nuanced handling of pipeline state changes, and also perform channel teardown if a downstream component fails to pull for too long to prevent stalling the pipeline. (by Dr John Vidler)
 - Added missing virtual attribute for the SplitterChannel destructor (by Dr John Vidler)

## [v0.2.48](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.47...v0.2.48)

 - Swapped the ZIP_LISTS implementation for a plain CMake one, to allow builds on older CMake versions. Also only now halting the build if SOFTDEVICE_PRESENT is asserted with any value and DEVICE_BLE is not 1, and would therefore be an invalid, unbootable configuration (by Dr John Vidler)
 - Snapshot v0.2.48 (by Dr John Vidler)

### codal-core ([a45ede4...75b92a9](https://github.com/lancaster-university/codal-core/compare/a45ede432a646a0af86fbd89d9dd692689fb6fbd...75b92a9ef3a206cdc63f65576c03e2aa72b4a45b))

 - Ensure AnimatedDisplay::waitForFreeDisplay() is free of race conditions (#140) (by Martin Williams)

## [v0.2.47](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.46...v0.2.47)

 - Builds will now error if the old SOFTDEVICE_PRESENT flag is asserted by the target or user config. Please use DEVICE_BLE instead. (by Dr John Vidler)
 - Snapshot v0.2.47 (by Dr John Vidler)

## [v0.2.46](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.45...v0.2.46)

 - Introduce audio output isplaying() API (by Joe Finney)
 - Fix miscalculation in output latency (by Joe Finney)
 - Snapshot v0.2.46 (by Dr John Vidler)

## [v0.2.45](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.44...v0.2.45)

 - Improve temporal response of SoundOutputPin::setAnalogValue() (by Joe Finney)
 - Introduce convenience functions (by Joe Finney)
 - Introduce egress double buffering onto SoundEmojiSynth (by Joe Finney)
 - Moved away from defining SOFTDEVICE_PRESENT directly in the target. If you require bluetooth, please define DEVICE_BLE as 1 in your codal.json (by Dr John Vidler)
 - Re-enabled bluetooth by default, and it can be turned off via DEVICE_BLE being set to 0 in codal.json (by Dr John Vidler)
 - Updated methods to align with the sample rate changes in -core (by Dr John Vidler)
 - Snapshot v0.2.45 (by Dr John Vidler)

### codal-core ([f08eb30...a45ede4](https://github.com/lancaster-university/codal-core/compare/f08eb30d75d0a786229344431177aa0e1c67c0a3...a45ede432a646a0af86fbd89d9dd692689fb6fbd))

 - First build (by Joe Finney)
 - Introduce PinPeripheral::pinLock (by Joe Finney)
 - Remove typos/debug (by Joe Finney)
 - First build (by Joe Finney)
 - Introduce PinPeripheral::pinLock (by Joe Finney)
 - Remove typos/debug (by Joe Finney)
 - Added comment to mark ID range 80-99 as reserved (by Dr John Vidler)
 - Added a safe ID area, and a static CodalComponent method for generating safe dynamic runtime component IDs (by Dr John Vidler)
 - Added sample rate support to the stream classes (by Dr John Vidler)

### codal-nrf52 ([92341a8...14bcffe](https://github.com/lancaster-university/codal-nrf52/compare/92341a800190536a6eb9df186864a49f9e7ce2da...14bcffe406a044e50d89616b97bd3757d0a00e82))

 - First build (by Joe Finney)
 - Implement PinPeripheral::pinLock APIs for dynamically allocated NRF52 peripherals (by Joe Finney)
 - Bugfixes following lab bench testing of peripheral switching (by Joe Finney)
 - Remove typos, dead code and debug (by Joe Finney)
 - First build (by Joe Finney)
 - Implement PinPeripheral::pinLock APIs for dynamically allocated NRF52 peripherals (by Joe Finney)
 - Bugfixes following lab bench testing of peripheral switching (by Joe Finney)
 - Remove typos, dead code and debug (by Joe Finney)
 - Remove implicit assumption to use high drive mode on SPI pins (by Joe Finney)
 - Added sample rate support to the ADC (by Dr John Vidler)

## [v0.2.44](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.43...v0.2.44)

 - First build of codal-microbit-v2 without softdevice (by Joe Finney)
 - First build of codal-microbit-v2 without softdevice (by Joe Finney)
 - Updated CMake to read the SOFTDEVICE_PRESENT flag and switch linker script as required. (by Dr John Vidler)
 - Added a default for SOFTDEVICE_PRESENT to enabled, to preserve existing behaviour (by Dr John Vidler)
 - Fix typo in MicroBitConfig.h closing an #ifndef. (#239) (by Carlos Pereira Atencio)
 - Re-added old MBED based IDs as deprecated-marked integers. Values have changed, avoid using these over CODAL official IO (by Dr John Vidler)
 - Added support for SET_ prefix for configurations to enable/disable -D style gcc flags, to enable softdevice removal from the build (by Dr John Vidler)
 - Snapshot v0.2.44 (by Dr John Vidler)

## [v0.2.43](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.42...v0.2.43)

 - NRF52LEDMatrix::rotateTo (by Martin Williams)
 - Removed a small delay in the emoji synth to fix a Python issue (by John Vidler)
 - Snapshot v0.2.43 (by John Vidler)
 - Rolled back a tag, commits were accidentally included (by John Vidler)
 - Removed a small delay in the emoji synth to allow HLLs to run correctly (by John Vidler)
 - Snapshot v0.2.42-hotfix.0 (by John Vidler)
 - Snapshot v0.2.42-hotfix.1 (by John Vidler)
 - Serial ports can now be used to wake the board from deep sleep states. (by John Vidler)
 - Enabled the deep-sleep wake functionality on the USB serial port by default. (by John Vidler)
 - Removed checks for wake sources when asked to perform a deep sleep. By policy, we want to sleep anyway, and we should normally have a serial port wake source (by John Vidler)
 - Removed dead code (by Dr John Vidler)
 - Fixed typo (by Dr John Vidler)
 - Added a new configuration flag MICROBIT_USB_SERIAL_WAKE, which when set to 1 will cause the board to wake on serial data over USB (by Dr John Vidler)
 - Removed spurious DMESGF in MicroBit.cpp (by Dr John Vidler)
 - Removed unused MBED IDs (by Dr John Vidler)
 - Moved to using CodalComponent.h constants for IDs to avoid conflicts. CodalComponent.h is the source of truth for this (by Dr John Vidler)
 - Snapshot v0.2.43 (by Dr John Vidler)

### codal-core ([d376f1a...f08eb30](https://github.com/lancaster-university/codal-core/compare/d376f1a851457effaf34058c62bd6c42c5ada6a0...f08eb30d75d0a786229344431177aa0e1c67c0a3))

 - Moved IDs to CodalComponent from MicroBitCompat over in the v2 repo, this we we have a single source of true IDs to avoid overlaps. Future changes should either use this, or come up with a better system (by Dr John Vidler)

### codal-nrf52 ([f169008...92341a8](https://github.com/lancaster-university/codal-nrf52/compare/f169008a35e69171aa60761a17bd4917be9dee08...92341a800190536a6eb9df186864a49f9e7ce2da))

 - Support for toggleable deep sleep modes for UART (by Dr John Vidler)

## [v0.2.42](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.41...v0.2.42)

 - Inserted a small delay into playback to account for the last buffer leaving the synth before emitting a compelte event (by John Vidler)
 - Added a simple guard to setSampleRate to inhibit rates under 1 sample/sec (by John Vidler)
 - Added a deepSleep variant which does not respect the remaining wait time if woken prematurely, to better support Python (by John Vidler)
 - Added correct return data, and updated the documentation (by John Vidler)
 - Added missing return semantics documentation (by John Vidler)
 - Fixed off-by-one error for the panic codes (by John Vidler)
 - Updated the README to point to the microbit-v2-samples repository (by John Vidler)
 - Improve accuracy and sensitivity of microphone level detector (by Joe Finney)
 - Added an error code for DAPLink/USB Interface errors (by John Vidler)
 - Snapshot v0.2.42 (by John Vidler)

### codal-core ([5af239e...d376f1a](https://github.com/lancaster-university/codal-core/compare/5af239edccccdc6e9df1d4281101093375a70a63...d376f1a851457effaf34058c62bd6c42c5ada6a0))

 - Replaced off-by-one error in the lock that blockd one fiber more than expected when initialised with the new constructor (by John Vidler)
 - fiber fix for recent GCC (by Michal Moskal)
 - Refactor StreamFilter class (by Joe Finney)
 - Optimize LevelDetectorSPL class (by Joe Finney)

## [v0.2.41](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.40...v0.2.41)

 - Add the DAPLink version to the data logging HTM page. (by Carlos Pereira Atencio)
 - datalog html: Document, fix, update, and create new test data (#197) (by Carlos Pereira Atencio)
 - Bug/flash configuration (#205) (by Dr John Vidler)
 - datalog htm: Fix unintuitive error msg when trying to plot with empty data. (#198) (by Carlos Pereira Atencio)
 - Inserted a small delay into playback to account for the last buffer leaving the synth before emitting a compelte event (#210) (by Dr John Vidler)
 - Bug/sound emoji timing (#215) (by Dr John Vidler)
 - Added a new error class which uses PanicCode as a base for Micro:bit v2 specific errors, and implemented these in the Accel and Compass classes (by John Vidler)
 - MicroBitLog now only checks the version string preamble, to avoid bugs where old logs are not detected during an erase cycle (by John Vidler)
 - Snapshot v0.2.41 (by John Vidler)

### codal-core ([4e1d131...5af239e](https://github.com/lancaster-university/codal-core/compare/4e1d1311b2115b14e6d7e08d60e3d14f4ca0a9e5...5af239edccccdc6e9df1d4281101093375a70a63))

 - Updated the serial send() ASYNC mode to correctly follow the spec in the documentation. Updated the documentation with further information on the behaviour (by John Vidler)
 - mark system_timer_current_time_us() as real time (used in Event()) (by Michal Moskal)
 - Add USB_EP_FLAG_ASYNC and UsbEndpointIn::canWrite() (by Michal Moskal)
 - make previous change less intrusive (by Michal Moskal)
 - DMESG() on event queue overflow (by Michal Moskal)
 - Added a semaphore mode to the FiberLock, for multiple access to limited shared resources. Updated documentation to remove double-annotated functions (by John Vidler)
 - Reverted to the break behaviour to preserve the tx interrupt (by John Vidler)
 - Enforced serial RX pullup on a redirect, to match micro:bit v1.xx behaviour (by John Vidler)
 - Removed DEVICE_USB_ERROR as its never used, and repurposed it for the base error address DEVICE_PERIPHERAL_ERROR (by John Vidler)

### codal-nrf52 ([ac01d74...f169008](https://github.com/lancaster-university/codal-nrf52/compare/ac01d74dc13236beddde78a5dab23c9e41b42c23...f169008a35e69171aa60761a17bd4917be9dee08))

 - Minor cleanup (whitespace) (by John Vidler)

## [v0.2.40](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.39...v0.2.40)

 - PowerManager now includes a new getPowerData to correctly handle the I2C power data, and getPowerConsumption is now a deprecated proxy for this so we dont immediately break compatability with other existing code (by Dr John Vidler)
 - Add the DAPLink version to the data logging HTM page. (#196) (by Carlos Pereira Atencio)
 - Add the DAPLink version to the data logging HTM page. (#199) (by Dr John Vidler)
 - Add MicroBitLog::readData (#177) (by Martin Williams)
 - Snapshot v0.2.40 (by Dr John Vidler)

### codal-core ([db201e9...4e1d131](https://github.com/lancaster-university/codal-core/compare/db201e94c6a2767e881ba4457c4087eb34312603...4e1d1311b2115b14e6d7e08d60e3d14f4ca0a9e5))

 - Altered ASYNC behaviour to now deschedule the current fiber for zero ticks if the txbuffer is full to allow the hardware to drain (by Dr John Vidler)

## [v0.2.39](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.38...v0.2.39)

 - Added missing end frequency parameter to the log generator function (by Dr John Vidler)
 - Removed redundant CodalDmesg.h include (by Dr John Vidler)
 - Removed redundant CodalDmesg.h include (by Dr John Vidler)
 - Add polyphonic real-time synthesizer. (#185) (by preglow)
 - Fix MicroBitBLEManager support for MicroBitPowerManager::deepSleep  (#155) (by Martin Williams)
 - Minor fix to read back the right amout of I2C data from the interface mcu. (#159) (by Carlos Pereira Atencio)
 - Removed old driver (no longer used) and simplified the interface (now just a thin proxy), but this requires further squashing in future releases (#192) (by Dr John Vidler)
 - Zeroed out the processorTemperature to squash an uninitialized variable compiler warning (by Dr John Vidler)
 - Added support for the BUSY flag on the KL27 (#190) (by Dr John Vidler)
 - Snapshot v0.2.39 (by Dr John Vidler)

### codal-core ([befa2c5...db201e9](https://github.com/lancaster-university/codal-core/compare/befa2c51b3595ca0d44e1bc5daad0531974b77e7...db201e94c6a2767e881ba4457c4087eb34312603))

 - Removed incorrect documentation block, pending replacement (by Dr John Vidler)

## [v0.2.38](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.37...v0.2.38)

 - Increase DAPLink I2C wait for data to be ready timeout. (#166) (by Carlos Pereira Atencio)
 - Snapshot v0.2.38 (by Dr John Vidler)

## [v0.2.37](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.35...v0.2.37)

 - Changed variable sampleRate to float to avoid rounding errors when generating frequencies. (by Joshua Hill)
 - [input pipeline] Moved initialisation of mic ADC channel, stream normaliser, splitter into microbitaudio. Moved initialisation of level detector to microbitaudio. Created events to enable and disable channel, called from splitter (by Joshua Hill)
 - [input pipeline] formatting (by Joshua Hill)
 - Changes according to review (by Joshua Hill)
 - [input pipeline] Changes acording to review (by Joshua Hill)
 - [input pipeline] Remove event from function paramaters - bug fix (by Joshua Hill)
 - [input-pipeline] Added now-required param to disable connect on start (by Joshua Hill)
 - [inputpipeline] Set default SPL level to not be active on start (by Joshua Hill)
 - [input-pipeline] Changed event to capture DEVICE_ID_MICROPHONE which is used for LevelDetectorSPL in legacy code (by Joshua Hill)
 - [input-pipeline] Removed debug messages (by Joshua Hill)
 - [input-pipeline] bug fix for new level detector init in microbit audio (by Joshua Hill)
 - setting up 0.2f 8-bit pipeline ready for future work (by Joshua Hill)
 - Create README.md (by Joshua Hill)
 - change microbit audio macro to use new uBit objects (by Joshua Hill)
 - Added method to set microphone gain, changed gain to 0.01 (by Joshua Hill)
 - [input-pipeline] Altered input pipeline gain scaling for 8bit (by Joshua Hill)
 - [input-pipeline] Tweaked gain to maintain legacy functionality (by Joshua Hill)
 - Snapshot v0.2.33-input_pipeline.0 (by Dr John Vidler)
 - Added a get/set volume call for audio channels in the mixer (by Dr John Vidler)
 - Added a minimal library definition for documentation generation (by Dr John Vidler)
 - Added new disable() call, and fixed some formatting issues (by Dr John Vidler)
 - Added per-channel volume control methods (by Dr John Vidler)
 - Snapshot v0.2.37 (by Dr John Vidler)

### codal-core ([6d3b410...befa2c5](https://github.com/lancaster-university/codal-core/compare/6d3b41000a6e3437a432a8edb1aedf9f49d75e57...befa2c51b3595ca0d44e1bc5daad0531974b77e7))

 - [input pipeline] Added input pipeline files (by Joshua Hill)
 - [input-pipeline] Fixed up old projects not working with new architecture (by Joshua Hill)
 - [input-pipeline] Safer connect immediately check (by Joshua Hill)
 - Updated Level Detector to be 8bit following change to stream normaliser in Microbit Audio to be 8bit. This will have effected the level detector values. (by Joshua Hill)
 - added component ID for audio processor (by Joshua Hill)
 - added debug (by Joshua Hill)
 - added component IDS for tap detector and FFT to register events (by Joshua Hill)
 - adjusted output scaling for 8-bit input (multiplier of 8) (by Joshua Hill)
 - quick adjustment to work with 8 bit values (-30 done by eye) (by Joshua Hill)
 - Added support 8 bit input (by Joshua Hill)
 - removed comments (by Joshua Hill)
 - Remove debug (by Joshua Hill)
 - add disable method (by Joshua Hill)
 - Updated SPL Level detector to support 8 bit (by Joshua Hill)
 - proper scaling of different data types (by Joshua Hill)
 - Removed debug (by Joshua Hill)
 - Make StreamSplitter more tolerant of late joining downstreams (by Joe Finney)
 - Added a minimal library definition for documentation generation (by Dr John Vidler)
 - fix subclass/protocol for hidjoystick (by Michal Moskal)
 - Added new FIFO, Recording and Filtering (audio) objects (by Dr John Vidler)
 - Included (experimental) stream data trigger object (by Dr John Vidler)

### codal-nrf52 ([0bc79cb...ac01d74](https://github.com/lancaster-university/codal-nrf52/compare/0bc79cb0232b889d7ef63fb08db358cac5104c48...ac01d74dc13236beddde78a5dab23c9e41b42c23))

 - [input pipeline] Seperated Channel activation to allow creation of deactivated channels (by Joshua Hill)
 - [input-pipeline] Changes after review (by Joshua Hill)
 - Passive implementation of NRF52ADCChannel::getSample() (by Joe Finney)
 - Protect DMA buffer acqiuisition against race conditions (by Joe Finney)
 - Reset DMA pointer when restarting the NRF52ADC (by Joe Finney)
 - Ensure NRF52ADC restarts deterministically when new channels are added (by Joe Finney)
 - Defer increment of enabledChannels until after new channel is added (by Joe Finney)
 - Stop NRF52ADC before adding channels (by Joe Finney)
 - [input pipeline] Changes to activateChannel docs according to review (by Joshua Hill)
 - NRF52ADC - move configuration changes out of IRQ handler (by Martin Williams)
 - NRF52ADC::stopRunning() - wait for not busy (by Martin Williams)
 - NRF52833 - Anomaly 212 WIP (by Martin Williams)
 - NRF52ADC - Configure channel gain in startRunning (by Martin Williams)
 - NRF52ADCChannel::setGain - configure the SAADC (by Martin Williams)
 - NRF52I2C::waitForStop: recover from hang (by Martin Williams)
 - Remove most added DMESGs (by Martin Williams)
 - Update comment (by Martin Williams)
 - Wait longer for RESUME/STOP (by Martin Williams)
 - NRF52I2C: revert debug code; add timeout configs (by Martin Williams)
 - Added a minimal library definition for documentation generation (by Dr John Vidler)

## [v0.2.35](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.34...v0.2.35)

 - Add MicroBitPowerManager::readInterfaceRequest() (by Martin Williams)
 - Snapshot v0.2.34 (by Dr John Vidler)
 - Snapshot v0.2.35 (by Dr John Vidler)
 - Snapshot v0.2.36 (by Dr John Vidler)
 - Corrected version to v0.2.35 (by Dr John Vidler)

### codal-core ([e2efc4c...6d3b410](https://github.com/lancaster-university/codal-core/compare/e2efc4cb034b04e5db5d7f6669667194b4a49024...6d3b41000a6e3437a432a8edb1aedf9f49d75e57))

 - Remove sample endpointRequest() impl   (crashes when no out endpoint) (by Michal Moskal)
 - Don't crash when not initialized (by Michal Moskal)
 - Add REAL_TIME_FUNC attribute (by Michal Moskal)
 - Put REAL_TIME_FUNC on functions that disable IRQs or are used from ISRs (by Michal Moskal)
 - Use new FORCE_RAM_FUNC attribute (by Michal Moskal)
 - Shorten IRQ-disabled period in DMESG (by Michal Moskal)
 - Fix warnings (by Michal Moskal)
 - Mark more functions real-time (ones used from ISRs) (by Michal Moskal)
 - FORCE_RAM_FUNC now correctly uses .data.ramfuncs rather than plain .data, to work with newer gcc versions. See issue #33 at https://github.com/lancaster-university/microbit-v2-samples/issues/33 (by Dr John Vidler)

### codal-nrf52 ([e1c428a...0bc79cb](https://github.com/lancaster-university/codal-nrf52/compare/e1c428ab031bc7114e72ee50a44baabae50c113b...0bc79cb0232b889d7ef63fb08db358cac5104c48))

 - Add case for 31250 in switch(baudrate) (#39) (by CaptainCredible)

## [v0.2.34](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.33...v0.2.34)

 - Update MicroBitLog serial mirroring to emit CRLF (#136) (by Joe Finney)
 - Option for Nordic Default UART characteristics (#139) (by Paul Austin)
 - BLE - make Device Info Service Model Number String sensitive to board ID (#114) (by Martin Williams)
 - Fix typo in MicroBitLog units (by Joe Finney)
 - Increase resolution of SoundOutputPin playback (#42) (by Joe Finney)
 - Update to DAPLink I2C Protocol v2 (by Joe Finney)
 - Implement fixed size journal and metadata regions (by Joe Finney)
 - Unify available off-chip flash (by Joe Finney)
 - Introduce delay following FLASH_ERASE request (by Joe Finney)
 - Remove verbose debug (by Joe Finney)
 - Align DAPLink v1/v2 workarounds (by Joe Finney)

## [v0.2.33](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.32...v0.2.33)

 - Ensure mutual exclusion for all MicroBitLog APIs (by Joe Finney)
 - Introduce workaround for back-to-back I2C Flash transactions using KL27 DAPLink firmware (by Joe Finney)
 - Move to runtime configuration of I2C DAPLink interface (by Joe Finney)
 - Snapshot v0.2.33 (by Joe Finney)

## [v0.2.32](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.31...v0.2.32)

 - Stabilize I2C communications between NRF52 and KL27 DAPLINK chip when running on battery power (#130) (by Joe Finney)
 - Improvements and optimizations to MicroBitLog (by Joe Finney)
 - Snapshot v0.2.32 (by Joe Finney)

### codal-core ([111fc68...e2efc4c](https://github.com/lancaster-university/codal-core/compare/111fc68a06a6d0b05c3e8551890dc11f014c1dc9...e2efc4cb034b04e5db5d7f6669667194b4a49024))

 - Optimize and fix logical error in inline ManagedString != method (by Joe Finney)

### codal-nrf52 ([3bb82f2...e1c428a](https://github.com/lancaster-university/codal-nrf52/compare/3bb82f2f1692f8b08b0ee424f0cb60224028d6a8...e1c428ab031bc7114e72ee50a44baabae50c113b))

 - NRF52I2C: Introduce transaction timeout (by Joe Finney)

## [v0.2.31](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.30...v0.2.31)

 - Introduce check for out of date bootloader settings page. (by Joe Finney)
 - Snapshot v0.2.31 (by Joe Finney)

## [v0.2.30](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.29...v0.2.30)

 - Prep work for adding visual preview. (#126) (by Matt Hillsdon)
 - Update datalog microbit.org URLs. (#129) (by Matt Hillsdon)
 - Add a "Visual preview" graph on online mode. (#127) (by Matt Hillsdon)
 - Update Data Logging HTML - Add a warning for IE users. (#133) (by Matt Hillsdon)
 - Update data Logging HTML - Avoid overflowing graphWrapper when error shown. (#132) (by Matt Hillsdon)
 - Update Bluetooth bootloader (#134) (by Joe Finney)
 - Parameterize RX/TX retransmission counters for NRF52->DAPLINK communications (by Joe Finney)
 - Snapshot v0.2.30 (by Joe Finney)

## [v0.2.29](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.28...v0.2.29)

 - MicroBitUARTService::send - fix for sending >20 bytes (#106) (by Martin Williams)
 - MicroBitAudio - make private pin variable a pointer (by Martin Williams)
 - Logfs serial mirroring (#122) (by Joe Finney)
 - Deep sleep 2 (#103) (by Martin Williams)
 - MicroBitAudio: Fix reference/pointer inconsistency in doSleep() (by Joe Finney)
 - Fix reset of Copy button (#118) (by Matt Hillsdon)
 - Add microbit.org-style header to MicroBitLog HTML page (by Matt Hillsdon)
 - Fix memory leak in MicroBitAudio::setSleep() / enable() (by Martin Williams)
 - Introduce automatic MY_DATA.HTM file visibility in MicroBitLog (by Joe Finney)
 - Snapshot v0.2.29 (by Joe Finney)

### codal-core ([1076c9a...111fc68](https://github.com/lancaster-university/codal-core/compare/1076c9a4388809a4e2c262d62b0064108066ab19...111fc68a06a6d0b05c3e8551890dc11f014c1dc9))

 - Deep sleep 2 (#138) (by Martin Williams)

### codal-nrf52 ([31a06cf...3bb82f2](https://github.com/lancaster-university/codal-nrf52/compare/31a06cf34da0d5bc3e5e82c3245bfb8e320fcb09...3bb82f2f1692f8b08b0ee424f0cb60224028d6a8))

 - Deep sleep 2 (#37) (by Martin Williams)

## [v0.2.28](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.27...v0.2.28)

 - Introduce MICROBIT_LOG_EVT_LOG_FULL event (by Joe Finney)
 - Snapshot v0.2.28 (by Joe Finney)

## [v0.2.27](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.26...v0.2.27)

 - Introduce MicroBitLog HTML+JS header resources (by Joe Finney)
 - Improvements to MicroBitLog (by Joe Finney)
 - Reduce granularity of MicroBitLog journalling (by Joe Finney)
 - Update HTML+JS header to parse updated journal format (by Joe Finney)
 - Leave sentinel word at the end of MicroBitLog flash space (by Joe Finney)
 - MicroBitLogFS improvements when log is full (by Joe Finney)
 - Updates to MicroBitLog to add implicit beginRow() / endRow() where possible (by Joe Finney)
 - Improve MicroBitLog full error reporting (by Joe Finney)
 - Minor improvements to MicroBitLog (by Joe Finney)
 - Performance tuning configuration of MicroBitLog (by Joe Finney)
 - MicroBitLog: Introduce online mode, iframe-reload, styles (#111) (by Matt Hillsdon)
 - Include neopixel header file by default (by Joe Finney)
 - Snapshot v0.2.27 (by Joe Finney)

## [v0.2.26](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.25...v0.2.26)

 - Mixer2 - Add isSilent() and fire SOUND/SILENCE events (#91) (by Martin Williams)
 - Fix typo (by Joe Finney)
 - Introduce MicroBitLog (by Joe Finney)
 - Snapshot v0.2.26 (by Joe Finney)

### codal-core ([caee115...1076c9a](https://github.com/lancaster-university/codal-core/compare/caee1151f0fdfb3a6678f9c269e7b33c9163bfea...1076c9a4388809a4e2c262d62b0064108066ab19))

 - Default atomic operations for MCU without hardware support (#132) (by Joe Finney)
 - add events to CodalUSB (by James Devine)
 - ifguard uf2_info in GhostFAT implementation (by James Devine)
 - fixup GhostFAT uf2 bootloader ifdef (by James Devine)
 - add USB_EP_TIMEOUT flag (by James Devine)
 - unset EP_TIMEOUT flag on read (OUT token) (by James Devine)
 - correct incorrect bitmsk unset (by James Devine)
 - remove EP_TIMEOUT flag (by James Devine)

### codal-nrf52 ([d547526...31a06cf](https://github.com/lancaster-university/codal-nrf52/compare/d54752690753ef8377d38f0338b9fcac9f60655d...31a06cf34da0d5bc3e5e82c3245bfb8e320fcb09))

 - begin work on nrf52 usb (by James Devine)
 - almost complete working usb implementation (by James Devine)
 - NRF52Serial - Avoid overwriting pin definitions during redirection (by Martin Williams)
 - NRF52Serial::configurePins - wait for TX buffer to empty (by Martin Williams)
 - working usb stack [minus iso endpoints] (by James Devine)
 - clean up cmakelists.txt (by James Devine)
 - PR feedback: remove ifdef __cplusplus (by James Devine)
 - update copyright (by James Devine)
 - NRF52ADC: Improvements to discrete sampling (by Joe Finney)
 - WIP: USB power fix when soft reset (by James Devine)
 - Bugfixes to NRF USB (by James Devine)
 - check EPSTATUS variable before attempting a read (by James Devine)
 - Add timeout on USB write (by James Devine)
 - Fixup usb timeout (by James Devine)
 - adjust timeout (by James Devine)
 - Remove EP_TIMEOUT flag and increase timeout duration (by James Devine)
 - NRF52I2C Workaround for missed SHORT trigger (#102) (#36) (by Joe Finney)

### codal-microbit-nrf5sdk ([691869a...ef4662e](https://github.com/microbit-foundation/codal-microbit-nrf5sdk/compare/691869a8261e08b283deb0e2267fae3c00c17dae...ef4662e13875a7b03e7296d7ac24a2b4d231f323))

 - Update README.md (#14) (by microbit-pauline)

## [v0.2.25](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.24...v0.2.25)

 - Changed variable sampleRate to float to avoid rounding errors when generating frequencies. (#71) (by Joshua Hill)
 - Add Partial Flashing and Memory Map documentation (#65) (by Sam Kent)
 - Enable I2C silicon errata workaround for high speed I2C (by Joe Finney)
 - Update MicroBitButton compat constructor.  (#75) (by Sam Kent)
 - MicroBitUARTService - wake up send() on disconnection (#54) (by Martin Williams)
 - Reduce ram origin (#52) (by Martin Williams)
 - Ensure internet GPIO pins are unaffected by DEVICE_DEFAULT_PULLMODE (by Joe Finney)
 - BLE Partial Flashing for MicroPython (#64) (by Sam Kent)
 - Snapshot v0.2.25 (by Joe Finney)

### codal-core ([969813d...caee115](https://github.com/lancaster-university/codal-core/compare/969813d451c33daeccef3b41cf904f6c08e6974f...caee1151f0fdfb3a6678f9c269e7b33c9163bfea))

 - Close possible race condition in Serial receive handling (by Joe Finney)
 - Protect against buffer underflow in Serial::readUntil() (codal-microbit-v2#79) (by Joe Finney)
 - Ensure Serial buffer sizes are valid (codal-microbit-v2#74) (by Joe Finney)
 - Ensure DEVICE_BUTTON_STATE_HOLD_TRIGGERED state is cleared in Button class (codal-microbit-v2#53) (by Joe Finney)
 - Make zero initialisation of ManagedBuffers optional (by Joe Finney)
 - Update RefCounted types to use atomic operations on reference count (by Joe Finney)

### codal-nrf52 ([1a86248...d547526](https://github.com/lancaster-university/codal-nrf52/compare/1a8624801d45301e0f5dfc2c1a17a5efca8082ab...d54752690753ef8377d38f0338b9fcac9f60655d))

 - Changed variables sampleRate and periodUs and their uses in functions to float to avoid rounding errors when generating frequencies. (by Joshua Hill)
 - Wait for I2C hardware to stop following I2C error condition (#49) (by Joe Finney)
 - Introduce optional support for Nordic I2C errata 219 (by Joe Finney)
 - Introduce setDriveMode() method (by Joe Finney)
 - Explicitly configure NRF52I2C SDA and SCL pin configuration (by Joe Finney)
 - NRF52I2C compatibility improvements (by Joe Finney)
 - Fix incorrect mapping for BitMode32 (by James Devine)
 - Reset UART DMA receive buffer pointer on RX restart (codal-microbit-v2#60) (by Joe Finney)
 - Fix inaccurate UART receive bytecount (codal-microbit-v2#80) (by Joe Finney)
 - Correctly generate timestamps on pin rise/fall events (codal-microbit-v2#68) (by Joe Finney)
 - Enable NRF52Pin GPIO edge events for pins using NRF_P1 (codal-microbit-v2#50) (by Joe Finney)
 - Correctly allow NRF52Pin to move from EVENT_ON_EDGE to PULSE_ON_EDGE mode. (by Joe Finney)
 - Update NRF52Pin to honour DEVICE_DEFAULT_PULLMODE (codal-microbit-v2#17) (by Joe Finney)
 - Ensure timer is reset when the first NRF52TouchSensor pin is added (by Joe Finney)

## [v0.2.24](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.23...v0.2.24)

 - Snapshot v0.2.24 (by Joe Finney)

### codal-core ([624048d...969813d](https://github.com/lancaster-university/codal-core/compare/624048d8588a2d2ca3e5b30c9635d41e999ce0b7...969813d451c33daeccef3b41cf904f6c08e6974f))

 - Fixed typo: availiable -> available (by Steve Kemp)
 - MessageBus.cpp : Honour changes in cb_arg when recycling event listeners (by Joe Finney)
 - Optimize serial write oprerations (by Joe Finney)

### codal-nrf52 ([f050e2e...1a86248](https://github.com/lancaster-university/codal-nrf52/compare/f050e2ec3500cc39ec03e3a0c0dcf065c8ca4ac2...1a8624801d45301e0f5dfc2c1a17a5efca8082ab))

 - Further stability improvements to NRF52Serial (by Joe Finney)
 - Support zero length NRF52I2C write transactions (by Joe Finney)

## [v0.2.23](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.22...v0.2.23)

 - Fix BLE disconnection (#43) (by Martin Williams)
 - Introduce Mixer2::setSilenceLevel() API (by Joe Finney)
 - Snapshot v0.2.23 (by Joe Finney)

### codal-core ([879ec3d...624048d](https://github.com/lancaster-university/codal-core/compare/879ec3d8dac8b510c49d57421046f493a640684a...624048d8588a2d2ca3e5b30c9635d41e999ce0b7))

 - Use lazy initialisation in Pulsein (by Joe Finney)
 - Update FiberLock to perform safely when used in IRQ context (by Joe Finney)
 - Protect FiberLock::wait() with IRQ mutex (by Joe Finney)
 - Ensure atomic increment in FiberLock::wait() (by Joe Finney)
 - Ensure all fibers blocked on PulseIn are correctly awoken (by Joe Finney)

### codal-nrf52 ([16edb84...f050e2e](https://github.com/lancaster-university/codal-nrf52/compare/16edb84e43f5faeb64b93cb9cd0ee1c4260ae31b...f050e2ec3500cc39ec03e3a0c0dcf065c8ca4ac2))

 - Allow for SPI interface without SCK (used for neopixels) (by Michal Moskal)
 - Fix lost NRF52 onPulse events (by Joe Finney)
 - Avoid over-configuration of SPI (by Michal Moskal)
 - Ensure timestamp used for pulse width measurement is initailized (by Joe Finney)
 - Correctly disable ADC channels in NRF52Pin when disconnected. (by Joe Finney)
 - Ensure NRF52PWM always restarts DMA pipeline when data arrives late. (by Joe Finney)
 - Maintain IRQ state and PulseIn state when switching from GPIO input/output (by Joe Finney)

## [v0.2.22](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.21...v0.2.22)

 - Snapshot v0.2.22 (by Joe Finney)

### codal-nrf52 ([fa9f692...16edb84](https://github.com/lancaster-university/codal-nrf52/compare/fa9f692993342e849202d82be0495af0f01b9f23...16edb84e43f5faeb64b93cb9cd0ee1c4260ae31b))

 - Remove glitch from NRF52PWM on pinConnect() (by Joe Finney)
 - Increase WS2812B reset pulse to 100uS (by Joe Finney)
 - Ensure WS2812B driver flushes downsream buffers (by Joe Finney)

## [v0.2.21](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.20...v0.2.21)

 - Add matching MicroBitButton constructor matching codal::Button (by Joe Finney)
 - Match BLE powers with DAL (by martinwork)
 - CI built bootloader (by Sam Kent)
 - Remove ambiguity in MicroBitButton constructor (by Joe Finney)
 - MicroBitButton PinNumber compatibility (by Sam Kent)
 - Add MicroBitFont typedef to MicroBitCompat.h (by Sam Kent)
 - microbit_panic - no E and a delay between each character (by martinwork)
 - MicroBitIOPinService pin order (by martinwork)
 - Update MicroBitCompat.h (by Sam Kent)
 - add constructors (by Sam Kent)
 - Add additional components (by Sam Kent)
 - wrap Serial::redirect() (by Sam Kent)
 - MicroBitI2C PinName/PinNumber compat (by Sam Kent)
 - Path through return value in MicroBitSerial::redirect() (by Joe Finney)
 - Include MicroBitI2C.h (by Joe Finney)
 - Update constructor types to require NRF52Pin (by Joe Finney)
 - use MicroBitI2C type consistently in MicroBit object (by Joe Finney)
 - Protect compat layer against memory leak in MicroBitSerial::redirect() (by Joe Finney)
 - Prevent default routing of Auxiliary Audio to P0 when an event handler is registered on that pin (by Joe Finney)
 - Snapshot v0.2.21 (by Sam Kent)

### codal-core ([7132dae...879ec3d](https://github.com/lancaster-university/codal-core/compare/7132daedac411f636df46447025c1e4fcb7bc1e6...879ec3d8dac8b510c49d57421046f493a640684a))

 - remove outdated JACDAC code (by James Devine)
 - Add backward compatibility wrapper for I2C::read() and I2C::write() (by Joe Finney)
 - Add Pin destructor (by Joe Finney)

### codal-nrf52 ([85b0a6b...fa9f692](https://github.com/lancaster-university/codal-nrf52/compare/85b0a6b2075705b2398dd5b9c38482315001f9aa...fa9f692993342e849202d82be0495af0f01b9f23))

 - remove jacdac include from ZSWS.h (by James Devine)
 - Add #ifdef around NRF_P1 usage (by James Devine)
 - further #ifdef guards around usage of NRF_P1 (by James Devine)

## [v0.2.20](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.19...v0.2.20)

 - #define Timer MbedTimer (by Sam Kent)
 - Align MicroBitStorage with CODAL APIs (by Joe Finney)
 - Introduce NRF52 Internal Flash Manager (by Joe Finney)
 - Introduce new constructor paramaeter for PartialFlashingService, so that a non-volatile memory controller to use can be defined (by Joe Finney)
 - Introduce internal NRF52 Flash Storage (by Joe Finney)
 - Use correct constructor parameters when creating PartialFlashingService (by Joe Finney)
 - Make constructor call to MicroBit::bleManager() explicit (by Joe Finney)
 - Allow BLEManager to use any type of button (by Joe Finney)
 - Introduce constructor level compatibility for MicroBitButton (by Joe Finney)
 - Introduce MicroBitAudio::setPin() and MicroBitAudio::setPinEnabled() (by Joe Finney)
 - Update microbit target to use hardware neopixel driver by default (by Joe Finney)
 - Ensure MicroBitCompass::detectedCompass is always correctly initialized (by Joe Finney)
 - Enable persistent storage of compass calibration data (by Joe Finney)
 - Add smoothing to PowerManager I2C interface (by Joe Finney)
 - Snapshot v0.2.20 (by Joe Finney)

### codal-core ([33da548...7132dae](https://github.com/lancaster-university/codal-core/compare/33da548ab2181982eb8ad14cf3b85ca0efc9c89a...7132daedac411f636df46447025c1e4fcb7bc1e6))

 - Update non-volatile memory APIs to no longer assume on chip address space (by Joe Finney)
 - Correct MAGIC constant and remove legacy debug statements (by Joe Finney)
 - Update KeyValueStorage to use lazy instantiation of buffers (by Joe Finney)
 - Add missing LICENSE in MemorySource class (by Joe Finney)
 - Remove status bitfield clash between LSM303 drivers and Compass/Accelerometer base classes (by Joe Finney)

### codal-nrf52 ([3ed9e6c...85b0a6b](https://github.com/lancaster-university/codal-nrf52/compare/3ed9e6ce738f22f75418cb69effa734999195007...85b0a6b2075705b2398dd5b9c38482315001f9aa))

 - Add configuration options to define behaviour on buffer overrun (by Joe Finney)
 - Introduce hardware (PWM) based WS2812B/neopixel driver (by Joe Finney)

## [v0.2.19](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.18...v0.2.19)

 - Ensure configuration parameters for motion sensors are initialized (by Joe Finney)
 - Snapshot v0.2.19 (by Joe Finney)

## [v0.2.18](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.17...v0.2.18)

 - Upstream PulseIn.h and PulseIn.cpp (by Joe Finney)
 - Snapshot v0.2.18 (by Joe Finney)

### codal-core ([316a483...33da548](https://github.com/lancaster-university/codal-core/compare/316a4830a67fe85f0ee1916a22856fb4ad486994...33da548ab2181982eb8ad14cf3b85ca0efc9c89a))

 - Introduce Pin::getPulseUs() and helper classes (by Joe Finney)

### codal-nrf52 ([4087e0d...3ed9e6c](https://github.com/lancaster-university/codal-nrf52/compare/4087e0dc840896d4e340e5888b52565ec5792bae...3ed9e6ce738f22f75418cb69effa734999195007))

 - Introduce NRF52Pin::getPulseUs() (by Joe Finney)

## [v0.2.17](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.16...v0.2.17)

 - Increase brightness of active pixel in Compass Calibration UX (#16) (by Joe Finney)
 - Ensure valid light sense reading on first use (#14) (by Joe Finney)
 - Adjust interrupt priorities (by Joe Finney)
 - Ensure NRF52LEDMatrix column pins are correctly configured on enable (#21) (by Joe Finney)
 - Set default mode for getDigitalValue() to PullDown (by Joe Finney)
 - Set IRQ priority for pin IRQ events (by Joe Finney)
 - Introduce blocking PulseIn behaviour (#15) (by Joe Finney)
 - Snapshot v0.2.17 (by Joe Finney)

### codal-nrf52 ([dff67e0...4087e0d](https://github.com/lancaster-university/codal-nrf52/compare/dff67e066e039358001ceafe406cc4dd902363c8...4087e0dc840896d4e340e5888b52565ec5792bae))

 - Don't assume NRF52PWM should be ultra-high priority (by Joe Finney)
 - Improve stability of NRF52Serial data reception (by Joe Finney)
 - Remove unnecessary code from NRF52Pin (by Joe Finney)

## [v0.2.16](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.15...v0.2.16)

 - Cope with zero-duration effects (#11) (by Matt Hillsdon)
 - Polyfilling mbed for extensions (#13) (by Sam Kent)
 - Suppress deprecated API compat warning when compiling the compat code itself (by Joe Finney)
 - Snapshot v0.2.16 (by Joe Finney)

## [v0.2.15](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.14...v0.2.15)

 - Defer START/STOP of synthesizer in SoundOutputPin (by Joe Finney)
 - Snapshot v0.2.15 (by Joe Finney)

## [v0.2.14](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.13...v0.2.14)

 - Add support for stopping sound expressions. (#9) (by Matt Hillsdon)
 - Optimisation: Use a flag for stopping state. (#10) (by Matt Hillsdon)
 - Apply silence optimisation on virtual audio pin (by Joe Finney)
 - Snapshot v0.2.14 (by Joe Finney)

### codal-nrf52 ([406a4e9...dff67e0](https://github.com/lancaster-university/codal-nrf52/compare/406a4e9b08fb4a227b979f95629764cae7bd8a1d...dff67e066e039358001ceafe406cc4dd902363c8))

 - Remove rogue IO_STATUS_DIGITAL_OUT flag when using pin as an analogue output (by Joe Finney)

## [v0.2.13](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.12...v0.2.13)

 - Add support for 8-bit mixer output (by Joe Finney)
 - Ensure SoundSynthesizer output is normalized when gain < 1.0 (by Joe Finney)
 - Move to using SoftDevice S113 to reduce memory usage (by Martin Williams)
 - Enable on-demand activaiton of audio output pipeline (by Joe Finney)
 - Normalise virtual audio pin volume levels to be consistent with micro:bit v1 (by Joe Finney)
 - Lower interrupt priority of PCM audio interrupts (by Joe Finney)
 - Introduce optimzed silence generation on synthesizer channels (by Joe Finney)
 - Snapshot v0.2.13 (by Joe Finney)

### codal-core ([fac7b59...316a483](https://github.com/lancaster-university/codal-core/compare/fac7b59a72a90c25b41ce20c16872ea5d99d14c6...316a4830a67fe85f0ee1916a22856fb4ad486994))

 - Close potential race condition in Serial::send() (by Joe Finney)

### codal-microbit-nrf5sdk ([249aef4...691869a](https://github.com/microbit-foundation/codal-microbit-nrf5sdk/compare/249aef43dd6afd9bfaedd31414e5a866d3464c3c...691869a8261e08b283deb0e2267fae3c00c17dae))

 - Swap FDS and MicroBitStorage (by martinwork)
 - Add headers for SoftDevice S113 (by martinwork)
 - Use headers for SoftDevice S113 (by martinwork)

## [v0.2.12](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.11...v0.2.12)

 - Snapshot v0.2.12 (by Joe Finney)

### codal-nrf52 ([300ca77...406a4e9](https://github.com/lancaster-university/codal-nrf52/compare/300ca77b9fc53a8100d4af19f8ffbd9674c29b06...406a4e9b08fb4a227b979f95629764cae7bd8a1d))

 - Fix memory leak in pin events In fact, avoid allocation at all there (by Michal Moskal)
 - Fix inaccurate servo pulsewidth calculation in NRF52Pin::setServoPulseUs() (by Joe Finney)

## [v0.2.11](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.10...v0.2.11)

 - Snapshot v0.2.11 (by Michal Moskal)

## [v0.2.10](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.9...v0.2.10)

 - Snapshot v0.2.10 (by Michal Moskal)

## [v0.2.9](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.8...v0.2.9)

 - SoundExpressions::play is now sync. (#2) (by Matt Hillsdon)
 - Snapshot v0.2.9 (by Joe Finney)

### codal-nrf52 ([e9d44ac...300ca77](https://github.com/lancaster-university/codal-nrf52/compare/e9d44ac45b177a6c6d3fe48b2bdc9f219fcb41fb...300ca77b9fc53a8100d4af19f8ffbd9674c29b06))

 - Fix NRF52Pin::setAnalogPeriod() (by Joe Finney)

## [v0.2.8](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.7...v0.2.8)

 - Improve response time of back-to-back calls to SoundOutputPin (by Joe Finney)
 - Snapshot v0.2.8 (by Joe Finney)

## [v0.2.7](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.6...v0.2.7)

 - Snapshot v0.2.7 (by Joe Finney)

### codal-nrf52 ([2f35ae5...e9d44ac](https://github.com/lancaster-university/codal-nrf52/compare/2f35ae5eeecbe62b983b1dacafe8cbed97bd15ab...e9d44ac45b177a6c6d3fe48b2bdc9f219fcb41fb))

 - Ensure NRF52Pin fully overrides codal::Pin (by Joe Finney)

## [v0.2.6](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.5...v0.2.6)

 - Ensure SoundOutputPin overrides base methods from codal::Pin (by Joe Finney)
 - Snapshot v0.2.6 (by Joe Finney)

## [v0.2.5](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.4...v0.2.5)

 - Introduce MicroBitAudio output pipeline (by Matt Hillsdon)
 - Correct MIT license (by Joe Finney)
 - Introduce API for per-channel samplerates indepdent out output rate (by Joe Finney)
 - Introduce per channel sub/super sampling of MixerChannels (by Joe Finney)
 - Snapshot v0.2.5 (by Joe Finney)

### codal-core ([e0089a9...fac7b59](https://github.com/lancaster-university/codal-core/compare/e0089a9c7b5c92e44482e324d39aef0728912a07...fac7b59a72a90c25b41ce20c16872ea5d99d14c6))

 - Simplify MemorySource (by Joe Finney)

### codal-nrf52 ([3ec21bd...2f35ae5](https://github.com/lancaster-university/codal-nrf52/compare/3ec21bd3278f8743f05b9ed7ce34ae6b4fbc71b3...2f35ae5eeecbe62b983b1dacafe8cbed97bd15ab))

 - Adopt new MemorySource API (by Joe Finney)

## [v0.2.4](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.3...v0.2.4)

 - Introduce target-locked.json (by Joe Finney)
 - Snapshot v0.2.3 (by Joe Finney)
 - Snapshot v0.2.4 (by Joe Finney)

## [v0.2.3](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.2...v0.2.3)

 - Ensure I2C Flash Geometry is loaded before it is used (by Joe Finney)
 - Ensure KL27 is awake before starting any i2c communications (by Joe Finney)
 - Ensure default __wrap_atexit is defined (by Joe Finney)
 - Update references after repository migration (by Joe Finney)
 - Update target name to reflect micro:bit version number (by Joe Finney)
 - Introduce MIT License file (by Joe Finney)
 - Ensure last effect step generates enough samples. (by Matt Hillsdon)

## [v0.2.2](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.1...v0.2.2)

 - Introduce htonl/htons into MicroBitCompat (by Joe Finney)
 - Add interface to PowerManager to indicate if an I2C transaction with the KL27 is in progress (by Joe Finney)
 - Add API support for awaitPacket() (by Joe Finney)
 - Fixes for MicroBitFlashManager (by Joe Finney)
 - Add MicroBitFlashManager to uBit model (by Joe Finney)
 - Fix bug that prevents display operating in lightsensing modes (by Joe Finney)
 - Gather USB Flash geometry on demand (by Joe Finney)
 - Rename micro:bit face logo button on request (by Joe Finney)
 - Snapshot v0.2.2 (by Joe Finney)

### codal-core ([05c9728...b8bb974](https://github.com/microbit-foundation/codal-core/compare/05c9728ab56e43ac7bd5e9852a3177a75c112663...b8bb974e882182c2f0068f0d62ea9daab3c5b626))

 - Unable to find commits

### codal-nrf52 ([11309c0...3ec21bd](https://github.com/lancaster-university/codal-nrf52/compare/11309c03127181edfc41f145a967c1855ec7a765...3ec21bd3278f8743f05b9ed7ce34ae6b4fbc71b3))

 - Fix visibilty of NRF52Serial::setBaudrate (by Joe Finney)

## [v0.2.1](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.2.0...v0.2.1)

 - Update bootloader.o (by Sam Kent)
 - Introduce PowerManager (by Joe Finney)
 - SoundEmojiSynthesizer Improvements (by Joe Finney)
 - Refactor SoundEmojiSynthesizer (by Joe Finney)
 - Minor improvements to SoundEmojiSynthesizer (by Joe Finney)
 - Generalise arpeggio effects (by Joe Finney)
 - SoundEmojiSynthesizer now uses Equal Temperament tuning by default (by Joe Finney)
 - Fix inversion of JUST_SCALE CONFIG option (by Joe Finney)
 - Introduce power management improvements (by Joe Finney)
 - Introduce MicroBitUSBFlashManager (by Joe Finney)
 - Introduce NIOP workaround for Kl27 silicon errata (by Joe Finney)
 - Fix initiatisation of NRF52LedMatrix (by Joe Finney)
 - Increase sensitivty of onLoud() (by Joe Finney)
 - KL27 I2C Comms Refinements (by Joe Finney)
 - Align PIN_ID values with edge connector IDs as in micro:bit v1. (by Joe Finney)
 - Introduce tuned capacitative touch parameters (by Joe Finney)
 - Bring out Face touch sensor as first class entity (by Joe Finney)
 - Add default calibration constant for face touch (by Joe Finney)
 - Utilize and set default CONFIG option for capacitative touch sensing (by Joe Finney)
 - Add MICROBIT_UBIT_FACE_TOUCH_BUTTON feature macro (by Joe Finney)
 - Introduce API for USBFlashStorage (alpha) (by Joe Finney)
 - Snapshot v0.2.1 (by Joe Finney)

### codal-core ([24dc5df...05c9728](https://github.com/microbit-foundation/codal-core/compare/24dc5dfd093cd90fd93b651ce9d460943f831df6...05c9728ab56e43ac7bd5e9852a3177a75c112663))

 - Unable to find commits

### codal-nrf52 ([e76b296...11309c0](https://github.com/lancaster-university/codal-nrf52/compare/e76b2961d8267280c6c0b38bb99327e56903ad44...11309c03127181edfc41f145a967c1855ec7a765))

 - Introduce support for GPIO DETECT signals (by Joe Finney)
 - Add support to disable timer interrupts during deep sleep (by Joe Finney)
 - Introduce MIT LICENSE (by Joe Finney)
 - Enable dynamic allocation/deallocation of PWM channels. (by Joe Finney)
 - Add selective support for resistive and capacitative sensing (by Joe Finney)
 - Allow CONFIG based definition of deafult capacitative touch calibraiton (by Joe Finney)

## [v0.2.0](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.1.1...v0.2.0)

 - Update linker script, uicr.o and settings.o (by martinwork)
 - Add linker script sections for nRF5SDK (by martinwork)
 - Add SOFTDEVICE_PRESENT to definitions (by martinwork)
 - Duplicate nRF5SDK files: drv_rtc.c, nrf_pwr_mgmt.c (by martinwork)
 - nRF5SDK_mods for codal-nrf52 nrfx compatibility (by martinwork)
 - Duplicate nRF5SDK nrf_sdh_soc.c and .h (by martinwork)
 - Modify nrf_sdh_soc - bug workaround (by martinwork)
 - DMESG backend for NRF_LOG. CODAL_DEBUG sets NRF_LOG_ENABLED. (by martinwork)
 - Add some nRF5SDK source files to CMakeLists.txt (by martinwork)
 - MicroBitBLEManager, Nordic buttonless DFU service, OPEN_LINK (by martinwork)
 - seedRandom (by martinwork)
 - Put the log levels back to normal (by martinwork)
 - Fix NRF_SDH_LOG_LEVEL (by martinwork)
 - Avoid use of UNUSED_PARAMETER (by martinwork)
 - MicroBit::getSerial() (by martinwork)
 - Change NO_BLE to DEVICE_BLE (by martinwork)
 - Duplicate ble_dfu_bonded.c (by martinwork)
 - Set NRF_DFU_BLE_BUTTONLESS_SUPPORTS_BONDS (by martinwork)
 - bootloader.o with REQUIRES_BONDS and SERVICE_CHANGED (by martinwork)
 - Fixes for just works (by martinwork)
 - Update sdk_config (by martinwork)
 - BLE - Allow re-pairing (by martinwork)
 - BLE - Do not disconnect immediately after pairing (by martinwork)
 - BLE - Create buttonless DFU service with MICROBIT_BLE_SECURITY_LEVEL (by martinwork)
 - Add MICROBIT_BLE_SECURITY_MODE (by martinwork)
 - BLE - Use idleCallback instead of timer for nrf_pwr_mgmt_shutdown (by martinwork)
 - Fix MICROBIT_BLE_STATUS_DELETE_BOND (by martinwork)
 - Improve pairing logic and shutdown handling (by martinwork)
 - BLE - only show pair tick after BLE_GAP_EVT_AUTH_STATUS (by martinwork)
 - BLE - Use BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION (by martinwork)
 - Add a shutdown delay (by martinwork)
 - Duplicate nrf_sdh.c (by martinwork)
 - Revert "Duplicate nrf_sdh.c" (by martinwork)
 - Restart without disabling softdevice (by martinwork)
 - BLE services (by martinwork)
 - yotta: Add module.json and adapt CMakeLists for yotta build (by Raphael Gault)
 - yotta: Explicitly point to master branch of dependencies (by Raphael Gault)
 - Update bootloader.o (by martinwork)
 - Rename MicroBitIOPinService.unused to cpp (by martinwork)
 - MicroBitIOPinService (by martinwork)
 - Rename MicroBitEventService.unused to cpp (by martinwork)
 - MicroBitEventService (by martinwork)
 - Rename MicroBitUARTService.unused as cpp (by martinwork)
 - MicroBitUARTService (by martinwork)
 - Rename MicroBitMagnetometerService.unused as cpp (by martinwork)
 - MicroBitMagnetometerService (by martinwork)
 - Rename MicroBitPartialFlashingService.unused as cpp (by martinwork)
 - MicroBitPartialFlashingService (by martinwork)
 - MicroBitBLEManager - fix advertising interval and timeout (by martinwork)
 - Rename MicroBitEddystone.unused as cpp (by martinwork)
 - MicroBitEddystone (by martinwork)
 - MICROBIT_DMESG_LEVEL (by martinwork)
 - Adjust GATT table size and RAM region to support all services (by martinwork)
 - Fix Eddystone (by martinwork)
 - use codal-microbit-nrf5sdk (by Sam Kent)
 - Merge updates from current DAL master (by martinwork)
 - MicroBitFlash, MicroBitStorage, MicroBitFile, MicroBitFileSystem (by martinwork)
 - Use MicroBitStorage and MicroBitFlash (by martinwork)
 - Removed SOFTDEVICE_PRESENT from target.json (by martinwork)
 - MicroBitPartialFlashingService - update settings pages (by martinwork)
 - Delete nRF5SDK_mods and sdk_config folders (by martinwork)
 - Avoid sign comparison warning (by martinwork)
 - Call app_timer_init() (by martinwork)
 - Better DMESG for peer manager failure events (by martinwork)
 - Remove ../codal-nrf52 includes (by martinwork)
 - Update module.json (by Sam Kent)
 - microbit_panic and target_panic (by martinwork)
 - target.json defines SOFTDEVICE_PRESENT and DEVICE_BLE (by martinwork)
 - Add V2.0 to DIS Model Number String (by martinwork)
 - Create the partial flashing service (by martinwork)
 - Avoid multiple calls to pm_local_database_has_changed() (by martinwork)
 - Remove auto bond deletion (by martinwork)
 - Fix softdevice state change naming (by martinwork)
 - Fix partial flashing (by martinwork)
 - Set all NRF_GPIOTE->CONFIG to zero (by martinwork)
 - Weak target_panic (by martinwork)
 - Adopt NRF52ADC driver (by Joe Finney)
 - Set ADC configuration for audio quality (by Joe Finney)
 - Add TouchSensor support (by Joe Finney)
 - Use NRF52TouchSensor for implementation of Pin::isTouched() (by Joe Finney)
 - Update to use NRF52LEDMatrix driver (by Joe Finney)
 - Adopt simplifed FXOS8700 driver (by Joe Finney)
 - Enable Compass Calibration (by Joe Finney)
 - Introduce NRF52LedMatrix (by Joe Finney)
 - Handle 180 degree rotation of FXOS8700 sensor (by Joe Finney)
 - Fix partial flashing page size (by martinwork)
 - Review MICROBIT_DMESG_LEVELs (by martinwork)
 - Fix capitalisation typos of NRF52LedMatrix.h (by Joe Finney)
 - Wire in NRF52LEDMatrix light sensor and align to microbit-dal API (by Joe Finney)
 - Align MicroBitIO Pin ordering with edge connector (fix #78) (by Joe Finney)
 - Ensure random number generator is seeded consistently (Fix #52) (by Joe Finney)
 - Set Peripheral IRQ priorities that are compatible with Nordic SoftDevice (by Joe Finney)
 - Merge MicroBit.cpp/h from upstream master (by martinwork)
 - Use codal::Compass/Accelerometer to match changed uBit (by martinwork)
 - Cleanup compiler warnings (by Joe Finney)
 - Update MIC_INIT / MIC_ENABLE macros to use NRF52ADC (by Joe Finney)
 - Snapshot v0.2.0 (by Joe Finney)

### codal-core ([1b9bd3a...24dc5df](https://github.com/microbit-foundation/codal-core/compare/1b9bd3aea55524fbc2f615668ee5b0e0c7c74264...24dc5dfd093cd90fd93b651ce9d460943f831df6))

 - Unable to find commits

### codal-nrf52 ([b6d25e6...e76b296](https://github.com/lancaster-university/codal-nrf52/compare/b6d25e6a40d826db16ae229db11c0780cefbb0ca...e76b2961d8267280c6c0b38bb99327e56903ad44))

 - Introduce unified ADC driver (by Joe Finney)
 - Remove DMESG instrumentation in NRF52ADC (by Joe Finney)
 - Enable support for 16MHz prescaler (by Joe Finney)
 - Account for 14 bit samples from ADC driver (by Joe Finney)
 - Add support for software oversampling in NRF52ADC (by Joe Finney)
 - Introduce NRF52TouchSensor (by Joe Finney)
 - Increase TouchSensor frequency (by Joe Finney)
 - Use NRF52TouchSensor as implementation of Pin::isTouched() (by Joe Finney)
 - Accurately define maximum sensor value (by Joe Finney)
 - NRF52TouchSensor updates to PPI/GPIOTE constants (by Joe Finney)
 - Enable NRF52PWM Harwdare Double Buffering (by Joe Finney)
 - Update NRF52Pin to utilise typed MemorySource (by Joe Finney)
 - Fix out of date API comment block (by Joe Finney)
 - Update NRF52I2C to fully reset I2C bus on initialisation (by Joe Finney)
 - Fix capitalisation typo in #include (by Joe Finney)

## [v0.1.1](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.1.0...v0.1.1)

 - i2c: Add second instance of i2c (by Raphael Gault)
 - Add default initialization code for microphone (by Michal Moskal)
 - Disable DMESG on serial by default (by Michal Moskal)
 - Snapshot v0.1.1 (by Michal Moskal)

### codal-core ([1b9bd3a...1b9bd3a](https://github.com/microbit-foundation/codal-core/compare/1b9bd3aea55524fbc2f615668ee5b0e0c7c74264...1b9bd3aea55524fbc2f615668ee5b0e0c7c74264))

 - Unable to find commits

### codal-nrf52 ([06f9b85...b6d25e6](https://github.com/lancaster-university/codal-nrf52/compare/06f9b855717db46c0ab2af2ad820d60858d2a2e6...b6d25e6a40d826db16ae229db11c0780cefbb0ca))

 - Correct polarity of PWM (by Michal Moskal)

## [v0.1.0](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.16...v0.1.0)

 - Rename project to match repo name (by Michal Moskal)
 - Snapshot v0.1.0 (by Michal Moskal)

### codal-core ([1b9bd3a...1b9bd3a](https://github.com/microbit-foundation/codal-core/compare/1b9bd3aea55524fbc2f615668ee5b0e0c7c74264...1b9bd3aea55524fbc2f615668ee5b0e0c7c74264))

 - Unable to find commits

## [v0.0.16](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.15...v0.0.16)

 - Disable BLE by default as it doesn't compile (by Michal Moskal)
 - Add blinking microbit_panic() (by Michal Moskal)
 - Disable IRQs and set drive in panic (by Michal Moskal)
 - Rename output binary to MICROBIT (by Michal Moskal)
 - Snapshot v0.0.16 (by Michal Moskal)

### codal-core ([1b9bd3a...1b9bd3a](https://github.com/microbit-foundation/codal-core/compare/1b9bd3aea55524fbc2f615668ee5b0e0c7c74264...1b9bd3aea55524fbc2f615668ee5b0e0c7c74264))

 - Unable to find commits

## [v0.0.15](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.14...v0.0.15)

 - MicroBitDevice: Add microbit_seed_random() implementation without BLE (by Raphael Gault)
 - Motion Sensor: Add a delay at accelerometer/compass detection (by Raphael Gault)
 - MicroBitDevice: Define the microbit_friendly_name function (by Raphael Gault)
 - fix FLASH_PROGRAM_END calculation (by martinwork)
 - Add Softdevice and Bootloader (#31) (by Sam Kent)
 - Add configuration defines for sneaky pxt compilation (by Michal Moskal)
 - Remove incorrect define (by Michal Moskal)
 - Add shim readLightLevel() for pxt (by Michal Moskal)
 - Rename defines as per discussion with @jaustin (by Michal Moskal)
 - Make SPI work in microbit-compat (by Michal Moskal)
 - Snapshot v0.0.15 (by Michal Moskal)

### codal-core ([99ac7fc...1b9bd3a](https://github.com/microbit-foundation/codal-core/compare/99ac7fc173290287689f484ce0e750466960de60...1b9bd3aea55524fbc2f615668ee5b0e0c7c74264))

 - Unable to find commits

### codal-nrf52 ([d262242...06f9b85](https://github.com/lancaster-university/codal-nrf52/compare/d2622428aa337155d589037ac07f4197f3179459...06f9b855717db46c0ab2af2ad820d60858d2a2e6))

 - Added .project file for giteye. (by Kei)
 - Fixed typo in NRF52Serial::setBaudrate(). & Added newline at EOF. (by Kei)
 - Added UARTE in sync_serial. (by Kei)
 - Fixed NRF52Serial for UARTE(DMA). (by Kei)
 - Added comment for NRF52Serial.write() function. (by Kei)
 - Updated configurePins() & Added isConfigured() for checking init. (by Kei)
 - Implement Pin::conditionalSetDigitalValue (by Michal Moskal)
 - Fix compilation problem (by Michal Moskal)
 - Fix Pin::eventOn(DEVICE_PIN_INTERRUPT_ON_EDGE) (by Michal Moskal)
 - conditionalSetDigitalValue -> getAndSetDigitalValue (by Michal Moskal)
 - Nesting-safe target_disable_irq() (by Michal Moskal)
 - Recognize BRK as end of transmission (by Michal Moskal)
 - Add comments (by Michal Moskal)
 - Use target_disable_irq (by Michal Moskal)
 - Use *SET/CLR registers properly (by Michal Moskal)
 - Avoid jitter on setDigitalValue() (by Michal Moskal)
 - Avoid race in target_disable_irq() (by Michal Moskal)
 - Make sure getAndSet is as quick as possible (by Michal Moskal)
 - Added peripheral_alloc (by modifying sync_serial) (by Kei)
 - Applied peripheral_alloc to I2C, SPI, Serial. (by Kei)
 - Moved position of constructor in NRF52Serial file. (by Kei)
 - Modified NRF52Serial using hal library. (Added function for DMA) (by Kei)
 - Add free_alloc_peri(device) to free allocated peri. (by Kei)
 - Apply free_alloc_peri(), Replace is_configured() with target_panic(). (by Kei)
 - Delete .project file. (by Kei)
 - Adopt SWS to peripheral alloc Side effect: allow for more than one SWS (by Michal Moskal)
 - Clean up after bootloader (by Michal Moskal)
 - Correct various LATCH/GPIOTE assumptions (by Michal Moskal)
 - Set high drive on SPI pins (by Michal Moskal)
 - Add mbed-compatible aliases in SPI (by Michal Moskal)

## [v0.0.14](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.13...v0.0.14)

 - Migrate codal_target_hal to upstream codal-nrf52 (by Joe Finney)
 - Bring speaker up in HiDrive mode by default (by Joe Finney)
 - Snapshot v0.0.14 (by James Devine)

### codal-core ([330de9b...99ac7fc](https://github.com/microbit-foundation/codal-core/compare/330de9b5e86f69b6e75b15109ada953b1a71a1ca...99ac7fc173290287689f484ce0e750466960de60))

 - Unable to find commits

### codal-nrf52 ([0e20dc8...d262242](https://github.com/lancaster-university/codal-nrf52/compare/0e20dc813aa7dca4d8c4e7c5b1f631205c92bdfc...d2622428aa337155d589037ac07f4197f3179459))

 - Introduce additional features for NRF52PWM (by Joe Finney)
 - Introduce setPeriodUs() function (by Joe Finney)
 - Fix NRF52PWM period calculation (by Joe Finney)
 - Introduce PWM support (by Joe Finney)

## [v0.0.13](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.12...v0.0.13)

 - Use nrf.h not nrf52.h (by Michal Moskal)
 - Snapshot v0.0.13 (by Michal Moskal)

### codal-core ([330de9b...330de9b](https://github.com/microbit-foundation/codal-core/compare/330de9b5e86f69b6e75b15109ada953b1a71a1ca...330de9b5e86f69b6e75b15109ada953b1a71a1ca))

 - Unable to find commits

### codal-nrf52 ([9de3e86...0e20dc8](https://github.com/lancaster-university/codal-nrf52/compare/9de3e862c160c02c0536066252840607185e2959...0e20dc813aa7dca4d8c4e7c5b1f631205c92bdfc))

 - Disable PRS (conflicts with our I2C/SPI drivers) (by Michal Moskal)
 - Default to UARTE0 (by Michal Moskal)
 - Move base functions from target_hal here to avoid duplication (by Michal Moskal)
 - Remove mbed references (by Michal Moskal)
 - Format (by Michal Moskal)
 - Add vector table relocation (by Michal Moskal)
 - Further VTOR relocation fixes (by Michal Moskal)
 - Basic double-buffering in PWM (by Michal Moskal)
 - Fix PDM buffer size (by Michal Moskal)

## [v0.0.12](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.11...v0.0.12)

 - Snapshot v0.0.12 (by Michal Moskal)

### codal-core ([330de9b...330de9b](https://github.com/microbit-foundation/codal-core/compare/330de9b5e86f69b6e75b15109ada953b1a71a1ca...330de9b5e86f69b6e75b15109ada953b1a71a1ca))

 - Unable to find commits

### codal-nrf52 ([86b21b5...9de3e86](https://github.com/lancaster-university/codal-nrf52/compare/86b21b5c096a8be28586c097f5f02a7235889628...9de3e862c160c02c0536066252840607185e2959))

 - Dynamic allocation of TWIM/SPIM instances (by Michal Moskal)
 - Start work on I2S neopixel (by Michal Moskal)
 - Use generic NRF52 ifdef (by Michal Moskal)
 - Add 52833 (by Michal Moskal)
 - Add disclaimer (by Michal Moskal)
 - Simplify GPIOTE_IRQHandler and fix LATCH clearing (by Michal Moskal)
 - Fix compilation error in serial (by Michal Moskal)
 - use same packet size as microbit to match CRC (by Peli)
 - Disable neopixel (by Michal Moskal)
 - Remove unfinished neopixel (by Michal Moskal)
 - Fixed typo in NRF52Serial::setBaudrate(). & Added newline at EOF. (by Kei)
 - Add bit-bang neopixel implementation (by Michal Moskal)

## [v0.0.11](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.10...v0.0.11)

 - Introduce LED Matrix brightness constants (by Joe Finney)
 - Add Temperature Sensing (by Joe Finney)
 - Remove mbed dependencies (by Joe Finney)
 - add Serial to model, and define DMESG SERIAL DEBUG (by James Devine)
 - Snapshot v0.0.11 (by James Devine)

### codal-core ([fce43db...330de9b](https://github.com/microbit-foundation/codal-core/compare/fce43db64bee7ebd0c48f244c296d4e72d1a6b42...330de9b5e86f69b6e75b15109ada953b1a71a1ca))

 - Unable to find commits

### codal-nrf52 ([ec82c60...86b21b5](https://github.com/microbit-foundation/codal-nrf52/compare/ec82c60b36608e9f70cac4a7378f17751fb0931a...86b21b5c096a8be28586c097f5f02a7235889628))

 - Unable to find commits

## [v0.0.10](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.9...v0.0.10)

 - update dev json to point to master (by James Devine)
 - Remove unused function warning (by Joe Finney)
 - Update Microbit uBit model to explicitly define ACTIVE_LOW IRQ line for accelerometer and magnetometer sensors (by Joe Finney)
 - Migrate LSM303 driver to codal-core (by Joe Finney)
 - Update construction and lazy instantiation for accelerometer and magnetometer (by Joe Finney)
 - Introduce improved compass calibration algorithm (by Joe Finney)
 - Introduce autoconfiguration of Accelerometer and Compass sensors (by Joe Finney)
 - Configuration Update (by Joe Finney)
 - Remove unecessary debug (by Joe Finney)
 - Snapshot v0.0.10 (by Joe Finney)

### codal-core ([951cad8...fce43db](https://github.com/microbit-foundation/codal-core/compare/951cad8b012382e1343919e8185d0c01ef90260e...fce43db64bee7ebd0c48f244c296d4e72d1a6b42))

 - Unable to find commits

### codal-mbedos ([4db2eb7...e696071](https://github.com/microbit-foundation/codal-mbedos/compare/4db2eb7a0d92d54aa391cb3d14b242a21cc11474...e6960710702649ed28597e2451d3438045dcdb6e))

 - Unable to find commits

### codal-nrf52 ([e000650...ec82c60](https://github.com/microbit-foundation/codal-nrf52/compare/e0006509416cb6567dda7229d4814043c70f2497...ec82c60b36608e9f70cac4a7378f17751fb0931a))

 - Unable to find commits

## [v0.0.9](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.8...v0.0.9)

 - Set target to provide printf (by James Devine)
 - Snapshot v0.0.9 (by James Devine)

### codal-core ([951cad8...951cad8](https://github.com/microbit-foundation/codal-core/compare/951cad8b012382e1343919e8185d0c01ef90260e...951cad8b012382e1343919e8185d0c01ef90260e))

 - Unable to find commits

### codal-mbedos ([afc818c...4db2eb7](https://github.com/microbit-foundation/codal-mbedos/compare/afc818c289f85f292a3c3101612e0df36aabba8e...4db2eb7a0d92d54aa391cb3d14b242a21cc11474))

 - Unable to find commits

### codal-nrf52 ([e000650...e000650](https://github.com/microbit-foundation/codal-nrf52/compare/e0006509416cb6567dda7229d4814043c70f2497...e0006509416cb6567dda7229d4814043c70f2497))

 - Unable to find commits

## [v0.0.8](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.6...v0.0.8)

 - update model, sync with core changes (by James Devine)
 - restore yotta_cfg_mappings (by James Devine)
 - Snapshot v0.0.6-nrf52833-mbedos.0 (by James Devine)
 - manually include MESEvents for now (by James Devine)
 - Snapshot v0.0.6-nrf52833-mbedos.1 (by James Devine)
 - add missing define, uncomment thermometer and radio in this branch (by James Devine)
 - Snapshot v0.0.6-core-fastforward.0 (by James Devine)
 - Snapshot v0.0.7 (by James Devine)
 - Snapshot v0.0.8 (by James Devine)

### codal-core ([e9247e8...951cad8](https://github.com/microbit-foundation/codal-core/compare/e9247e840c340e302528c42d550a06dcfaa8897b...951cad8b012382e1343919e8185d0c01ef90260e))

 - Unable to find commits

### codal-mbedos ([43c63ed...afc818c](https://github.com/microbit-foundation/codal-mbedos/compare/43c63ed54bf5d4802d48d81862714df80b3239e1...afc818c289f85f292a3c3101612e0df36aabba8e))

 - Unable to find commits

### codal-nrf52 ([ea1b9ea...e000650](https://github.com/microbit-foundation/codal-nrf52/compare/ea1b9ea71f8693b76dbb46178e3a8d8c0a2ac0d8...e0006509416cb6567dda7229d4814043c70f2497))

 - Unable to find commits

## [v0.0.6](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.5...v0.0.6)

 - Re-enable radio (by Jonathan Austin)
 - Snapshot v0.0.6 (by James Devine)

### codal-core ([e9247e8...e9247e8](https://github.com/microbit-foundation/codal-core/compare/e9247e840c340e302528c42d550a06dcfaa8897b...e9247e840c340e302528c42d550a06dcfaa8897b))

 - Unable to find commits

### codal-mbedos ([ed0c700...43c63ed](https://github.com/microbit-foundation/codal-mbedos/compare/ed0c700604fd63e2f2baae8a83c69e8f9fc5c5f8...43c63ed54bf5d4802d48d81862714df80b3239e1))

 - Unable to find commits

### codal-nrf52 ([ea1b9ea...ea1b9ea](https://github.com/microbit-foundation/codal-nrf52/compare/ea1b9ea71f8693b76dbb46178e3a8d8c0a2ac0d8...ea1b9ea71f8693b76dbb46178e3a8d8c0a2ac0d8))

 - Unable to find commits

## [v0.0.5](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.4...v0.0.5)

 - Snapshot v0.0.5 (by Joe Finney)

### codal-core ([e9247e8...e9247e8](https://github.com/microbit-foundation/codal-core/compare/e9247e840c340e302528c42d550a06dcfaa8897b...e9247e840c340e302528c42d550a06dcfaa8897b))

 - Unable to find commits

### codal-mbedos ([7f8b960...ed0c700](https://github.com/microbit-foundation/codal-mbedos/compare/7f8b96087a38addb7f6cea83e97eacbed6f04800...ed0c700604fd63e2f2baae8a83c69e8f9fc5c5f8))

 - Unable to find commits

### codal-nrf52 ([ea1b9ea...ea1b9ea](https://github.com/microbit-foundation/codal-nrf52/compare/ea1b9ea71f8693b76dbb46178e3a8d8c0a2ac0d8...ea1b9ea71f8693b76dbb46178e3a8d8c0a2ac0d8))

 - Unable to find commits

## [v0.0.4](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.3...v0.0.4)

 - Update to mbedos-5.1.3 TARGET_NRF52_MICROBIT_v1_43 (by Joe Finney)
 - Update to v1.43 pinmapping (by Joe Finney)
 - Updates for v1.43 (by Joe Finney)
 - Updates for v1.43 (by Joe Finney)
 - Update locations of dependencies (by Joe Finney)
 - Snapshot v0.0.4 (by Joe Finney)

### codal-core ([886c58d...e9247e8](https://github.com/lancaster-university/codal-core/compare/886c58d9ddcdf77381b733f0abd7af44e1066d9b...e9247e840c340e302528c42d550a06dcfaa8897b))

 - Minor bugfixes to historic branch. (by Joe Finney)

### codal-mbedos ([17a43f4...7f8b960](https://github.com/microbit-foundation/codal-mbedos/compare/17a43f4637e2bbbe4f868dd856ef7a1517b6c6dc...7f8b96087a38addb7f6cea83e97eacbed6f04800))

 - Unable to find commits

### codal-nrf52 ([9aac236...ea1b9ea](https://github.com/microbit-foundation/codal-nrf52/compare/9aac23686a640372dd09074e796f9d4b0eb6e8fd...ea1b9ea71f8693b76dbb46178e3a8d8c0a2ac0d8))

 - Unable to find commits

## [v0.0.3](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.2...v0.0.3)

 - Update target json (by James Devine)
 - ADded dmesg guards (by James Devine)
 - changes to target.json (by James Devine)
 - Remove unused codepath (by Joe Finney)
 - Move to using native NRF52Timer (by Joe Finney)
 - Update target specification to reference new repo locations (by Joe Finney)
 - Add "HelloWorld" sample application (by Joe Finney)
 - Snapshot v0.0.3 (by Joe Finney)

### codal-core ([d7270e9...886c58d](https://github.com/lancaster-university/codal-core/compare/d7270e90d0b910f803c8d87024d5696bca9ffd41...886c58d9ddcdf77381b733f0abd7af44e1066d9b))

 - Add support for I2C repeated start transactions (by Joe Finney)

### codal-mbedos ([cbe9742...17a43f4](https://github.com/finneyj/codal-mbedos/compare/cbe97420f2481c9ba29d3ec5127d09db228f9dcb...17a43f4637e2bbbe4f868dd856ef7a1517b6c6dc))

 - Unable to find commits

### codal-nrf52 ([1253824...9aac236](https://github.com/jamesadevine/codal-nrf52/compare/125382439498768f963e96d626397f51db6b068e...9aac23686a640372dd09074e796f9d4b0eb6e8fd))

 - Unable to find commits

## [v0.0.2](https://github.com/lancaster-university/codal-microbit-v2/compare/v0.0.1...v0.0.2)

 - Snapshot v0.0.2 (by James Devine)

### codal-mbedos ([cbe9742...cbe9742](https://github.com/finneyj/codal-mbedos/compare/cbe97420f2481c9ba29d3ec5127d09db228f9dcb...cbe97420f2481c9ba29d3ec5127d09db228f9dcb))

 - Unable to find commits

### codal-nrf52 ([1253824...1253824](https://github.com/jamesadevine/codal-nrf52/compare/125382439498768f963e96d626397f51db6b068e...125382439498768f963e96d626397f51db6b068e))

 - Unable to find commits

