#ifndef __GPIO_H__
#define __GPIO_H__

//////////////////////////
/// Includes
/////////////////////////
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

////////////////////////
/// Defines
///////////////////////

#define IN  	( 0 )	///< GPIO direction as input
#define OUT 	( 1 )	///< GPIO direction as output
 
#define LOW 	( 0 )	///< GPIO value Low
#define HIGH 	( 1 )	///< GPIO value High

#define BUFFER_MAX 		( 4 )
#define DIRECTION_MAX 	( 35 )

#define VALUE_MAX 		( 30 )

//////////////////////////////
/// Function Prototypes
/////////////////////////////

/// Export GPIO pin
/// \param[in] pin : gpio pin number
/// \return 0 if success 
///         -1 otherwise
int32_t GPIOExport( int32_t pin_ );

/// Set GPIO direction
/// \param[in] pin : gpio pin number
/// \param[in] dir : pin direction (IN or OUT)
/// \return 0 if success 
///         -1 otherwise
int32_t GPIODirection( int32_t pin_, int32_t dir_ );

/// Write value on GPIO pin
/// \param[in] pin : gpio pin number
/// \param[in] value : gpio pin value (HIGH or LOW)
/// \return 0 if success 
///         -1 otherwise
int32_t GPIOWrite( int32_t pin_, int32_t value_ );

/// Read value on GPIO pin
/// \param[in] pin : gpio pin number
/// \return 0 if LOW 
///         1 if HIGH
int32_t GPIORead( int32_t pin_ );

#endif /// End of GPIO_H