# Project Context: STM32H7 Smart Oscilloscope

## 1. System Overview

I am building a high-speed digital oscilloscope using an **STM32H7** microcontroller. 
Currently, the system is running on bare-metal C code using the STM32 HAL library. It captures a 1024-sample waveform and transmits it to a Python GUI. 

**My immediate next goal is to migrate this bare-metal architecture to FreeRTOS and replace the UART communication with High-Speed Ethernet (LwIP).**

## 2. Current Hardware Architecture (Perfected)

We have successfully engineered a completely jitter-free, zero-CPU-polling hardware trigger system.

* **The Metronome (TIM3):** Triggers the ADC conversions.
* **The ADC & DMA:** ADC1 runs continuously in circular DMA mode, dumping 16-bit samples into a 1024-element array (`tx_buf`) located in `.RAM_D2`.
* **The Hardware Trigger (Analog Watchdog):** We use AWD1 and AWD2 to detect threshold crossings without stopping the ADC. It supports Level High, Level Low, and Dual-Watchdog Schmitt triggers (Rising/Falling edges).
* **The Hardware Delay (TIM4):** TIM4 is cascaded as a Slave to TIM3 (ITR2). It is set to One-Pulse Mode (OPM). When the AWD detects a trigger, the ISR starts TIM4. TIM4 physically counts the exact number of `post_trigger_samples` (ADC conversions) and fires its own interrupt to halt the DMA and TIM3.

## 3. Current Software Architecture

Right now, the system uses a blocking Super-Loop (`while(1)`) in `app_main.c`.

* It waits for a command byte over UART (`huart3`).
* Command `0x01`: Updates struct parameters.
* Command `0x03`: Re-initializes hardware with new thresholds/prescalers.
* Command `0x02`: Requests a waveform. This calls `wait_and_send_trigger_data()`, which turns on the AWD interrupt, waits for a global `data_ready` flag, flushes the D-Cache, mathematically stitches the circular buffer into a linear array, and transmits it via a blocking `HAL_UART_Transmit`.

## 4. Known Gotchas & Solved Bugs (Do Not Reintroduce These)

* **The L1 Cache:** DMA writes to `.RAM_D2` silently. We must manage cache invalidation (`SCB_InvalidateDCache`) before reading the array, or disable D-Cache entirely.
* **Stale Interrupt Flags:** The ADC is free-running. Before turning the AWD interrupts back on, we strictly clear the hardware flags (`__HAL_ADC_CLEAR_FLAG`) so we don't trigger on past events.
* **TIM4 State Lock:** Because TIM4 is in One-Pulse Mode, the hardware stops itself, but the HAL software state gets stuck in `BUSY`. We must call `HAL_TIM_Base_Stop_IT(&htim4)` in the callback to unlock the HAL state machine.

## 5. The Goal for This Chat: FreeRTOS + Ethernet (LwIP)

I want to completely rip out the blocking UART and bare-metal loop. We are moving to:

1. **FreeRTOS:** To decouple waveform capturing from network transmission using Tasks, Queues, and Semaphores (Deferred Interrupt Processing).
2. **LwIP (UDP/TCP):** To blast the 1024-sample arrays over Ethernet for massive real-time frame rates. TCP for receiving Python commands, UDP for streaming the waveform data.
3. **Double Buffering:** "Ping-Pong" buffers so the DMA can fill Buffer B while the FreeRTOS network task transmits Buffer A.

## 6. Task for the AI

Please review this context. To start, please provide a proposed **FreeRTOS Task Architecture** (listing the tasks, their priorities, and the Semaphores/Queues connecting them to the ADC/TIM4 hardware interrupts) that will allow me to capture and stream data continuously without the CPU blocking.