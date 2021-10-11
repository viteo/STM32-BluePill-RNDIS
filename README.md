STM32 BluePill Template project
===
STM32 [BluePill](https://stm32-base.org/boards/STM32F103C8T6-Blue-Pill.html) template project for [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) with Standard Peripherial Library ([SPL](https://www.st.com/en/embedded-software/stm32-standard-peripheral-libraries.html)) v3.6.1.

How to use
---
Install STM32CubeIDE

    File -> Open Project from File System... -> Directory -> Finish

The project will appear in your Project Explorer. Rename it.

    Right Click -> Rename... -> Update references ✓ -> OK

The .ioc file
---
The .ioc file added for reference where you might describe pinout, peripherials, timings, interrupts, etc. But just for reference. Do not use 'Generate Code' button. If you want to hide 'Code-generation needed' notification, delete ```isbadioc=true``` string from file or change it to false.
