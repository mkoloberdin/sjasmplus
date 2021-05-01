/* 

  SjASMPlus Z80 Cross Compiler

  This is modified source of SjASM by Aprisobal - aprisobal@tut.by

  Copyright (c) 2005 Sjoerd Mastijn

  This software is provided 'as-is', without any express or implied warranty.
  In no event will the authors be held liable for any damages arising from the
  use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it freely,
  subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not claim
	 that you wrote the original software. If you use this software in a product,
	 an acknowledgment in the product documentation would be appreciated but is
	 not required.

  2. Altered source versions must be plainly marked as such, and must not be
	 misrepresented as being the original software.

  3. This notice may not be removed or altered from any source distribution.

*/

#ifndef SJASMPLUS_DIRECTIVES_H
#define SJASMPLUS_DIRECTIVES_H

#include "tables.h"
#include "io_trd.h"

extern FunctionTable DirectivesTable;

void InsertDirectives();

void checkRepeatStackAtEOF();

optional<std::string> doSAVETRD(const fs::path &FileName, const zx::trd::HobetaFilename &HobetaFileName,
                                int Start, int Length, int Autostart);

optional<std::string> doSAVESNA(const fs::path &FileName, uint16_t Start);

bool LuaSetPage(aint n);

bool LuaSetSlot(aint n);

#endif // SJASMPLUS_DIRECTIVES_H
