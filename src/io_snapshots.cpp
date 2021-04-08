/*

  SjASMPlus Z80 Cross Assembler

  Copyright (c) 2004-2008 Aprisobal

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

#include <cstring>

#include "defines.h"
#include "errors.h"
#include "zxspectrum.h"

#include "io_snapshots.h"

namespace zx {

bool saveSNA(MemModel &M, const fs::path &fname, uint16_t start) {
    unsigned char snbuf[31];

    std::ofstream ofs;
    ofs.open(fname, std::ios_base::binary);
    if (ofs.fail()) {
        Fatal("Error opening file"s, fname.string());
    }

    zx::initBasicVars(M);
    zx::initScreenAttrs(M);

    memset(snbuf, 0, sizeof(snbuf));

    snbuf[1] = 0x58; //hl'
    snbuf[2] = 0x27; //hl'
    snbuf[15] = 0x3a; //iy
    snbuf[16] = 0x5c; //iy
    // Set BC=PC to match ZX Basic's USR behavior
    snbuf[13] = (uint8_t) (start & 0xff); //bc
    snbuf[14] = (uint8_t) (start >> 8); //bc
    if (!M.isPagedMemory()) {
        snbuf[0] = 0x3F; //i
        snbuf[3] = 0x9B; //de'
        snbuf[4] = 0x36; //de'
        snbuf[5] = 0x00; //bc'
        snbuf[6] = 0x00; //bc'
        snbuf[7] = 0x44; //af'
        snbuf[8] = 0x00; //af'
        snbuf[9] = 0x2B; //hl
        snbuf[10] = 0x2D; //hl
        snbuf[11] = 0xDC; //de
        snbuf[12] = 0x5C; //de
        snbuf[17] = 0x3C; //ix
        snbuf[18] = 0xFF; //ix
        snbuf[21] = 0x54; //af
        snbuf[22] = 0x00; //af

        uint16_t stack;

        if (zx::initDefaultBasicStack(M) &&
                M.readByte(0xFF2D) == (uint8_t) 0xb1 &&
                M.readByte(0xFF2E) == (uint8_t) 0x33 &&
                M.readByte(0xFF2F) == (uint8_t) 0xe0 &&
                M.readByte(0xFF30) == (uint8_t) 0x5c) {

            snbuf[23] = 0x2D;// + 16; //sp
            snbuf[24] = 0xFF; //sp

            M.writeByte(0xFF2D + 16, (uint8_t) (start & 0x00FF), true, false);  // pc
            M.writeByte(0xFF2E + 16, (uint8_t) (start >> 8), true, false);      // pc

        } else if ((stack = zx::initMinimalBasicStack(M, 8, start)) != 0) {
            snbuf[23] = (uint8_t) (stack & 0xff); //sp
            snbuf[24] = (uint8_t) (stack >> 8); //sp

        } else {
            Warning("[SAVESNA] RAM <0x4000-0x4001> will be overridden due to 48k snapshot imperfect format."s,
                    LASTPASS);

            snbuf[23] = 0x00; //sp
            snbuf[24] = 0x40; //sp

            M.writeByte(0x4000, (uint8_t) (start & 0x00FF), true, false);  // pc
            M.writeByte(0x4001, (uint8_t) (start >> 8), true, false);      // pc
        }
    } else {
        uint16_t stack = zx::initMinimalBasicStack(M);
        snbuf[23] = (uint8_t) (stack & 0xff); //sp
        snbuf[24] = (uint8_t) (stack >> 8); //sp

        // snbuf[23] = 0X00; //sp
        // snbuf[24] = 0x60; //sp
    }
    snbuf[25] = 1; //im 1
    snbuf[26] = 7; //border 7

    ofs.write((const char *) snbuf, sizeof(snbuf) - 4);
    if (ofs.fail()) {
        Error("Error writing to "s + fname.string(), strerror(errno), CATCHALL);
        ofs.close();
        M.clearEphemerals();
        return false;
    }

    if (!M.isPagedMemory()) {
        // 48K
        ofs.write((const char *) M.getPtrToMem() + 0x4000, 0xC000);
    } else {
        // 128K
        ofs.write((const char *) M.getPtrToPage(5), 0x4000);
        ofs.write((const char *) M.getPtrToPage(2), 0x4000);
        ofs.write((const char *) M.getPtrToPageInSlot(3), 0x4000);
    }

    if (!M.isPagedMemory()) {

    } else { // 128K
        snbuf[27] = (uint8_t) (start & 0x00FF); //pc
        snbuf[28] = (uint8_t) (start >> 8); //pc
        snbuf[29] = 0x10 + M.getPageNumInSlot(3); //7ffd
        snbuf[30] = 0; //tr-dos
        ofs.write((const char *) snbuf + 27, 4);
    }

    //if (DeviceID) {
    if (!M.isPagedMemory()) {
        /*for (int i = 0; i < 5; i++) {
            if (fwrite(Device->GetPage(0)->RAM, 1, Device->GetPage(0)->Size, ff) != Device->GetPage(0)->Size) {
                Error("Write error (disk full?)", fname, CATCHALL);
                fclose(ff);
                return 0;
            }
        }*/
    } else { // 128K
        for (int i = 0; i < 8; i++) {
            if (i != M.getPageNumInSlot(3) && i != 2 && i != 5) {
                ofs.write((const char *) M.getPtrToPage(i), 0x4000);
            }
        }
    }

    M.clearEphemerals();
    if (ofs.fail()) {
        Error("Error writing to "s + fname.string(), strerror(errno), CATCHALL);
        ofs.close();
        return false;
    }

    //}
    /* else {
        char *buf = (char*) calloc(0x14000, sizeof(char));
        if (buf == NULL) {
            Error("No enough memory", 0, FATAL);
        }
        memset(buf, 0, 0x14000);
        if (fwrite(buf, 1, 0x14000, ff) != 0x14000) {
            Error("Write error (disk full?)", fname, CATCHALL);
            fclose(ff);
            return 0;
        }
    }*/

    if (M.isPagedMemory() &&
            M.getName() != "ZXSPECTRUM128"s) {
        Warning("Only 128kb will be written to snapshot"s, fname.string());
    }

    ofs.close();
    return true;
}

} // namespace zx

//eof io_snapshots.cpp
