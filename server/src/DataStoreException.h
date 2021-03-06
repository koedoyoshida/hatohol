/*
 * Copyright (C) 2013 Project Hatohol
 *
 * This file is part of Hatohol.
 *
 * Hatohol is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Hatohol is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hatohol. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DataStoreException_h
#define DataStoreException_h

#include "HatoholException.h"

class DataStoreException : public HatoholException
{
public:
	explicit DataStoreException(const string &brief,
	                            const char *sourceFileName = "",
	                            int lineNumber = UNKNOWN_LINE_NUMBER);
	virtual ~DataStoreException() _HATOHOL_NOEXCEPT;
private:
};

#define THROW_DATA_STORE_EXCEPTION(FMT, ...) \
do { \
	string msg = StringUtils::sprintf(FMT, ##__VA_ARGS__); \
	throw DataStoreException(msg, __FILE__, __LINE__); \
} while (0)

#endif // DataStoreException_h
