## Low CPU Load 5x5 LED Matrix Driver

This document describes the implementation of a low-CPU-load display driver for the micro:bit's 5x5 LED matrix, and how to enable it.

### Overview

The default driver for the LED matrix uses interrupts to refresh the display. This approach is simple but has a significant performance impact, as it requires the CPU to constantly switch context to handle the display refresh.

A new, DMA-based driver has been implemented to offload the display refresh process from the CPU. This driver uses the nRF52's powerful peripherals (Timers, PPI, GPIOTE, and DMA) to create an autonomous refresh mechanism that requires almost no CPU intervention.

### Performance Benefits

The DMA-based driver provides a significant reduction in CPU load, freeing up the processor for other tasks. This is especially beneficial when using computationally expensive features like data logging or radio communication.

### How to Enable the DMA-Based Driver

A compile-time option is available to switch between the original interrupt-driven driver and the new DMA-based driver. To enable the DMA driver, set the `USE_DMA_DISPLAY_DRIVER` flag to `1` in your `codal.json` or `target.json` configuration file:

```json
{
    "config": {
        "USE_DMA_DISPLAY_DRIVER": 1
    }
}
```

By default, this option is set to `0`, which means the original interrupt-driven driver will be used. This ensures backward compatibility with existing projects.

### Functional Differences

The DMA-based driver is highly optimized for performance, but it has one functional limitation: the `readLightLevel()` method is not supported. This is because the hardware peripherals used for the display refresh are not compatible with the analog input required for light sensing. If your application requires light sensing, you must use the original interrupt-driven driver.

---

## Low-Risk Refactoring Plan to a Low CPU Load Driver

This plan outlines a safe, step-by-step process to introduce a new, high-performance DMA-based display driver alongside the existing one, allowing for testing and validation without breaking existing functionality.

### Step 1: Scaffolding and Build System Integration
- [x] Create new files `source/NRF52DmaLedMatrix.h` and `source/NRF52DmaLedMatrix.cpp`.
- [x] In the new header, define a `NRF52DmaLEDMatrix` class that inherits from `Display` and mirrors the public API of the existing `NRF52LEDMatrix` class.
- [x] Since the project's `CMakeLists.txt` automatically discovers new source files, no manual changes are needed to add the new driver to the build. This can be verified by triggering a build.

### Step 2: Create a Compile-Time Driver Selection Mechanism
- [x] In `target.json`, add a new configuration option `USE_DMA_DISPLAY_DRIVER` and set its default value to `0`.
- [x] In `model/MicroBit.h`, use a preprocessor macro to allow selecting the display driver at compile time. This is crucial for A/B testing and debugging.
  ```cpp
  #if CONFIG_ENABLED(USE_DMA_DISPLAY_DRIVER)
  #include "NRF52DmaLedMatrix.h"
  #else
  #include "MicroBitDisplay.h" // Or NRF52LedMatrix.h
  #endif

  class MicroBit {
      // ...
  #if CONFIG_ENABLED(USE_DMA_DISPLAY_DRIVER)
      NRF52DmaLEDMatrix           display;
  #else
      MicroBitDisplay             display;
  #endif
      // ...
  };
  ```
- [x] Apply the same conditional logic to the constructor in `model/MicroBit.cpp` to ensure the correct display object is instantiated.

### Step 3: Implement the Autonomous Hardware Refresh Cycle
- [x] In `NRF52DmaLedMatrix.cpp`, inside the `enable()` method, configure the hardware peripherals.
- [x] **Timers:** Configure one timer for PWM brightness control, and another to trigger the start of each display frame (e.g., at 100Hz).
- [x] **GPIOTE:** Configure GPIOTE channels in "Task" mode to control the 5 row pins.
- [x] **DMA:** Configure 5 DMA channels. Each channel will be responsible for writing a pre-calculated bitmask for one row's column data to the GPIO OUTCLR register.
- [x] **PPI:** Configure PPI channels to link the timer events to the GPIOTE and DMA tasks. This creates the "domino effect" where one hardware event triggers the next, forming the core of the CPU-less refresh cycle.

### Step 4: Implement the Display Buffer Update Logic
- [x] In `NRF52DmaLedMatrix.cpp`, implement the `systemTick()` method. This function will be called periodically by the main scheduler.
- [x] Inside `systemTick()`, read the shared `image` buffer (the `MicroBitImage`) and translate its contents into the bitmasks required by the DMA channels, handling any display rotation. This is the only part of the refresh that requires the CPU, and only when the image changes.

### Step 5: Implement the Core `Display` API
- [x] **`setBrightness()`:** Implement this to update the PWM timer's compare register, which controls the duty cycle of the column pins.
- [x] **`disable()`:** Implement this to stop all timers and disable the PPI channels, effectively shutting down the display hardware.
- [x] **`setSleep()`:** Implement this to simply call `enable()` or `disable()` based on the input.

### Step 6: Address Functional Regressions
- [x] **`readLightLevel()`:** This feature is incompatible with the DMA driver's architecture, as it requires reconfiguring the display pins for analog input. The safest implementation is to return `DEVICE_NOT_SUPPORTED` to make it clear to developers that this is a trade-off for the performance gain.

### Step 7: Final Testing and Validation
- [ ] Build the firmware with the `USE_DMA_DISPLAY_DRIVER` config flag enabled.
- [ ] Flash a test program (like `samples/main.cpp`) to a physical micro:bit and verify that all animations and scrolling text appear correctly.
- [ ] Use a debugger (e.g., PyOCD) to monitor CPU usage and confirm a significant reduction in load compared to the original interrupt-driven driver.
- [ ] (Optional) Use a logic analyzer to inspect the GPIO pins and verify that the refresh rate and signal timings are correct.