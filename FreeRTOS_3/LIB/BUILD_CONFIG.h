#ifndef BUILD_CONFIG_H_
#define BUILD_CONFIG_H_

#define MCU_1_MASTER 1
#define MCU_2_SLAVE 2

#define ACTIVE_MCU MCU_2_SLAVE

#ifndef SPI_MODE /* respect any forced override  */
#if ACTIVE_MCU == MCU_1_MASTER
#define SPI_MODE 1U /* SPI master                  */
#else
#define SPI_MODE 0U /* SPI slave                   */
#endif
#endif

#endif /* BUILD_CONFIG_H_ */
