/*
 * device.h
 *
 * Device/project description
 *
 */

#ifndef DEVICE_DEVICE_H_
#define DEVICE_DEVICE_H_
#include "stm32f10x_gpio.h"

//GPIO Defines
#define PIN_LED GPIO_Pin_13 //PC13	BluePill Green LED

/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   7
#define IP_ADDR3   1

/*DHCP Client addresses: IP_ADDR0.IP_ADDR1.IP_ADDR2.DHCP_ADDR_x*/
#define DHCP_ADDR_1     2
#define DHCP_ADDR_2     3

/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   0
#define GW_ADDR1   0
#define GW_ADDR2   0
#define GW_ADDR3   0

/* MAC ADDRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
#define MAC_ADDR0   0x20
#define MAC_ADDR1   0x89
#define MAC_ADDR2   0x84
#define MAC_ADDR3   0x6A
#define MAC_ADDR4   0x96
#define MAC_ADDR5   0xAB

#endif /* DEVICE_DEVICE_H_ */
