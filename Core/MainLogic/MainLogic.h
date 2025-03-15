/*
 * MainLogic.h
 *
 *  Created on: Mar 15, 2025
 *      Author: alex
 */

#ifndef MAINLOGIC_MAINLOGIC_H_
#define MAINLOGIC_MAINLOGIC_H_

#include <cstdint>

#include "print/print.h"
#include "Screen/Screen.h"
#include "Network/Network.h"

class MainLogic {
public:
	MainLogic();
	~MainLogic();

	void update(int dtInMs);

private:

	int16_t readTemperature() const;

	void setDesiredTemperature(int16_t temp);
	
	void updateDisplay();

private:

	std::string m_displayLine1;
	std::string m_displayLine2;

	int16_t m_currentTemperature = 0;
	int16_t m_desiredTemperature = 0;
};

#endif /* MAINLOGIC_MAINLOGIC_H_ */
