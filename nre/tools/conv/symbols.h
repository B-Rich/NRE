/**
 * $Id: symbols.h 1073 2011-09-19 16:25:53Z nasmussen $
 * Copyright (C) 2008 - 2011 Nils Asmussen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef SYMBOLS_H_
#define SYMBOLS_H_

#define MAX_FUNC_LEN    255

void sym_init(void);
void sym_addFile(const char *file);
const char *sym_resolve(unsigned long addr);
void specialChars(const char *src, char *dst, size_t dstSize);

#endif /* SYMBOLS_H_ */
