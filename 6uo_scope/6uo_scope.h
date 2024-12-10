#ifndef __6UO_SCOPE_H_
#define __6UO_SCOPE_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <Windows.h>
#include <signal.h>
#include <time.h> 
#include <visa.h>
#include "inih.h"

/**
 * Debug log.
 */

#ifdef _MSC_VER // Windows
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else // Linux
#error Linux is not supported
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif // ?_MSC_VER

#ifndef _6UO_DEBUG

 /** Debug printf macro NOP */
#define _6uo_dbg_printf(...) do {} while (0)
/** Debug buffer hex dump macro NOP */
#define _6uo_dbg_dump(len, buf) do {} while (0)

#else // _6UO_DEBUG

 /** Enable verbose logging anyway */
#ifndef _6UO_VERBOSE
#define _6UO_VERBOSE
#endif // !_6UO_VERBOSE

/** Define debug printf macro */
#define _6uo_dbg_printf(fmt, ...) do { printf("[DBG]  %16s:%-5d %24s():  " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__); } while (0)

#ifndef _6UO_DEBUG_NO_HEX
/** Debug buffer hex dump macro */
#define _6uo_dbg_dump(len, buf) \
		do { \
			for (int __i = 0; __i < len; __i++) printf("%02X ", (unsigned char)(buf[__i])); \
			printf("\n"); \
		} while (0)
#else
	/** Debug buffer hex dump macro NOP */
#define _6uo_dbg_dump(len, buf) do {} while (0)
#endif // !_6UO_DEBUG_NO_HEX

#endif // _6UO_DEBUG


/**
 * Verbose log.
 */
#ifndef _6UO_VERBOSE
 /** Verbose printf macro NOP */
#define _6uo_log_printf(...) do {} while (0)
#else
 /** Define verbose printf macro */
#define _6uo_log_printf(fmt, ...) do { printf("[LOG]  %16s:%-5d %24s():  " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__); } while (0)
#endif // !_6UO_VERBOSE


/**
 * Define error printf macro.
 */
#define _6uo_print_error(fmt, ...) do { fprintf(stderr, "[ERROR]%16s:%-5d %24s():  " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__); } while (0)

#endif /** __6UO_SCOPE_H_ */
