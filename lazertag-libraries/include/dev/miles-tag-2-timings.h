/*
 * miles-tag-2-timings.h
 *
 *  Created on: 06 дек. 2014 г.
 *      Author: alexey
 */

#ifndef LAZERTAG_RIFLE_INCLUDE_MILES_TAG_2_TIMINGS_H_
#define LAZERTAG_RIFLE_INCLUDE_MILES_TAG_2_TIMINGS_H_

// Transmitter constants
#define HADER_PERIOD            2400
#define BIT_ONE_PERIOD          1200
#define BIT_ZERO_PERIOD         600
#define BIT_WAIT_PERIOD         600

// Receiver constants
#define HEADER_PERIOD_MIN       2300
#define HEADER_PERIOD_MAX       2600

#define BIT_ONE_PERIOD_MIN      1100
#define BIT_ONE_PERIOD_MAX      1500

#define BIT_ZERO_PERIOD_MIN     500
#define BIT_ZERO_PERIOD_MAX     750

#define BIT_WAIT_PERIOD_MIN     500
#define BIT_WAIT_PERIOD_MAX     900



#endif /* LAZERTAG_RIFLE_INCLUDE_MILES_TAG_2_TIMINGS_H_ */
