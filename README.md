STM32 BluePill (F103) RNDIS device with LwIP
===
BluePill connected via USB to the host machine discovers as RNDIS device and allows to control it's parameters via browser. Based on LwIP stack, includes DHCP and DNS server.

Usage
---
Addresses defined in `device/device.h` file. By default it is `192.168.7.1`. The DHCP server should automatically assign connection. Type in your browser [http://192.168.7.1](http://192.168.7.1) to get access to the BluePill's `index.html`.

The DNS server also gives you option to get access to BluePill with readable address: [http://run.stm](http://run.stm) or just [http://run](http://run)

On the index page use `LED PC13` checkbox to switch the LED on BluePill board.

![BluePill](/fs/img/BluePill.svg)

References
---
* [LwIP](https://savannah.nongnu.org/projects/lwip/) included as [submodule](https://git.savannah.nongnu.org/git/lwip.git)
* [lrndis](https://github.com/fetisov/lrndis) by Fetisov Sergey
