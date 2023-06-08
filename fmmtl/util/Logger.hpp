#pragma once

#include <iostream>
#include <iomanip>

#include <map>
#include <string>

#include <atomic>

#include "fmmtl/util/Clock.hpp"

#include "fmmtl/config.hpp"

/** A class for accumulating the time of segments of code:
 * Usage:
 *
 * Timer timer;
 * { auto ts = timer.time_scope();
 *   // code to time
 * }
 * std::cout << timer << std::endl;
 */
 


/** @class Logger
 * @brief Logging class to keep the hit count and total time of sections of code
 *
 * Usage:
 * Logger logger;
 *
 * { Logger::timer timer = logger.log("identifier");
 *  // code to track
 * }
 *
 * // Print all string-identified events' hit counts and timings
 * std::cout << log << std::endl;
 */
 

#if defined(FMMTL_LOGGING)

//! Global static logger rather than a singleton for efficiency/consistency
 
#  define FMMTL_LOG(STRING)  
#  define FMMTL_PRINT_LOG(OUT)  
#  define FMMTL_LOG_CLEAR  

#else

#  define FMMTL_LOG(STRING)
#  define FMMTL_PRINT_LOG(OUT)
#  define FMMTL_LOG_CLEAR

#endif
