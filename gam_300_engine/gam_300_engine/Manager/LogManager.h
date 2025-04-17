/**
 * @file LogManager.h
 * @brief Declaration of the Log Manager for the game engine.
 * @details Handles writing to log files for debugging and error reporting.
 * @author
 * @date
 * Copyright (C) 2025 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */
#pragma once
#ifndef __LOG_MANAGER_H__
#define __LOG_MANAGER_H__

 // System includes.
#include <stdio.h>
#include <map>
#include <stdarg.h>   // Moved from LogManager.cpp
#include <time.h>     // Moved from LogManager.cpp
#include <string.h>   // Moved from LogManager.cpp

// Engine includes.
#include "Manager.h"

// Two-letter acronym for easier access to manager.
#define LM gam300::LogManager::getInstance()

namespace gam300 {

	const std::string LOGFILE_DEFAULT = "gam300.log";

	class LogManager : public Manager {

	private:
		LogManager();                     // Private since a singleton.
		LogManager(LogManager const&);    // Don't allow copy.
		void operator=(LogManager const&);// Don't allow assignment.
		bool m_do_flush;                  // True if flush to disk after write.
		FILE* m_p_f;                      // Pointer to main logfile.

	public:
		// If logfile is open, close it.
		~LogManager();

		// Get the one and only instance of the LogManager.
		static LogManager& getInstance();

		// Start up LogManager (open main logfile, usually "dragonfly.log").
		// Return 0 if ok, else -1.
		int startUp();

		// Shut down LogManager (close all logfiles).
		void shutDown();

		// Write to logfile.
		// Supports printf() formatting of strings.
		// Return number of bytes written (excluding prepends), -1 if error.
		int writeLog(const char* fmt, ...) const;

		// Set flush of logfile after each write.
		void setFlush(bool new_do_flush = true);
	};

} // end of namespace gam300
#endif // __LOG_MANAGER_H__