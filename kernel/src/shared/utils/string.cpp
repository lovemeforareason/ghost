/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *  Ghost, a micro-kernel based operating system for the x86 architecture    *
 *  Copyright (C) 2015, Max Schlüssel <lokoxe@gmail.com>                     *
 *                                                                           *
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation, either version 3 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "shared/utils/string.hpp"
#include "shared/memory/memory.hpp"

void stringConcat(const char* a, const char* b, char* out)
{
	int len_a = stringLength(a);
	int len_b = stringLength(b);
	memoryCopy(out, a, len_a);
	memoryCopy(&out[len_a], b, len_b);
	out[len_a + len_b] = 0;
}

void stringCopy(char* target, const char* source)
{
	int len = stringLength(source);
	memoryCopy(target, source, len);
	target[len] = 0;
}

int stringLength(const char* str)
{
	int size = 0;
	while (*str++)
	{
		++size;
	}
	return size;
}

int stringIndexOf(const char* str, char c)
{
	int pos = 0;
	while (*str)
	{
		if (*str == c)
		{
			return pos;
		}
		++pos;
		++str;
	}
	return -1;
}

bool stringEquals(const char* stra, const char* strb)
{

	if (stra == strb)
		return true;

	int alen = stringLength(stra);
	int blen = stringLength(strb);

	if (alen != blen)
	{
		return false;
	}

	while (alen-- > 0)
	{
		if (stra[alen] != strb[alen])
		{
			return false;
		}
	}
	return true;
}

void stringReplace(char* str, char character, char replacement)
{
	for (uint32_t i = 0;; i++)
	{
		if (str[i] == 0)
		{
			break;
		}

		if (str[i] == character)
		{
			str[i] = replacement;
		}
	}
}

