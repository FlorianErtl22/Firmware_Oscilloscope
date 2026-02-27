# Professional STM32 Project Structure & Makefile Guide

When transitioning from a managed IDE (like STM32CubeIDE) to a custom text editor (like VS Code), managing your project structure and `Makefile` correctly is critical. If you don't separate your code from the ST-generated code, STM32CubeMX will overwrite your work every time you regenerate the project.

## 1. The Professional Folder Tree

Always isolate your custom application code from the auto-generated hardware initialization code. Create a dedicated `App/` (or `Application/`) folder at the root of your project.

```text
My_STM32_Project/
├── Core/                  <-- ST Generated (main.c, interrupts, hardware init)
│   ├── Inc/
│   └── Src/
├── Drivers/               <-- ST Generated (HAL, CMSIS)
├── Middlewares/           <-- ST Generated (FreeRTOS, LwIP, etc.)
├── Makefile               <-- ST Generated (You will modify this)
│
└── App/                   <-- YOUR CUSTOM CODE (Safe from CubeMX)
    ├── Hardware/          
    │   ├── Inc/           (e.g., adc_ctrl.h, uart_parser.h)
    │   └── Src/           (e.g., adc_ctrl.c, uart_parser.c)
    ├── DSP/               
    │   ├── Inc/           (e.g., fft_processor.h)
    │   └── Src/           (e.g., fft_processor.c)
    └── Network/           
        ├── Inc/           (e.g., udp_stream.h)
        └── Src/           (e.g., udp_stream.c)