/*
 * periph-mapping-helper.h
 *
 *  Created on: 04 янв. 2015 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_HW_PERIPH_MAPPING_HELPER_H_
#define LAZERTAG_RIFLE_INCLUDE_HW_PERIPH_MAPPING_HELPER_H_

#define GPIOx(_N)                 ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define PIN_MASK(_N)              (1 << (_N))
#define RCC_MASKx(_N)             (RCC_APB2Periph_GPIOA << (_N))

#endif /* LAZERTAG_RIFLE_INCLUDE_HW_PERIPH_MAPPING_HELPER_H_ */
