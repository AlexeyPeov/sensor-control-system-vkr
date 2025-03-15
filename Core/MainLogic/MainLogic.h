/*
 * MainLogic.h
 *
 *  Created on: Mar 15, 2025
 *      Author: alex
 */

#ifndef MAINLOGIC_MAINLOGIC_H_
#define MAINLOGIC_MAINLOGIC_H_

#include <stdint.h>

#include "print/print.h"
#include "Screen/Screen.h"

class MainLogic {
public:
	MainLogic();
	~MainLogic();

	void update(uint32_t dtInMs);
};

#endif /* MAINLOGIC_MAINLOGIC_H_ */
