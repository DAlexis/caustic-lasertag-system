/*
 * rifle-display.hpp
 *
 *  Created on: 06 янв. 2015 г.
 *      Author: alexey
 */

#ifndef HAL_RIFLE_DISPLAY_HPP_
#define HAL_RIFLE_DISPLAY_HPP_

class IRifleDisplay
{
public:
	void setHealth(uint32_t health);
	void setBullets(uint32_t bullets);
	void setMagazines(uint32_t magazines);
	void update();
};

class IRifleDisplayBuilder
{
public:
	IRifleDisplay* getDisplay(const char* type);
};

extern IRifleDisplayBuilder *displayBuilder;

#endif /* HAL_RIFLE_DISPLAY_HPP_ */
