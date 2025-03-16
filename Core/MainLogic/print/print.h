/*
 * print.h
 *
 *  Created on: Mar 15, 2025
 *      Author: alex
 */

#ifndef MAINLOGIC_PRINT_PRINT_H_
#define MAINLOGIC_PRINT_PRINT_H_

#include "../Network/Network.h"

#define DEBUG_PRINT

#ifdef DEBUG_PRINT

#define debug(format, ...)                                                     \
    Network::sendMessage(Network::MsgTypeSend::LOG_DEBUG, format, ##__VA_ARGS__)
#else
#define debug(format, ...)                                                     \
    do                                                                         \
    {                                                                          \
    } while (0)
#endif

#define error(format, ...)                                                     \
    Network::sendMessage(Network::MsgTypeSend::LOG_ERROR, format, ##__VA_ARGS__)

#endif /* MAINLOGIC_PRINT_PRINT_H_ */
