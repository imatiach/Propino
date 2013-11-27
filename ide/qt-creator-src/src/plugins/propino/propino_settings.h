/***************************************************************************//**
* @file
* Propino Settings Header File
*
* @version @n 1.0
* @date @n 10/17/2013
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2013 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 10/17/2013
*******************************************************************************/

#ifndef PROPINO_SETTINGS_H
#define PROPINO_SETTINGS_H

#define PROPINO_KEY_GROUP "Propino"

// Propino Plugin //////////////////////////////////////////////////////////////

#define PROPINO_PLUGIN "Plugin"

#define PROPINO_PLUGIN_KEY_GROUP (PROPINO_KEY_GROUP "/" PROPINO_PLUGIN)

#define PROPINO_PLUGIN_KEY_WDIR "workspaceDir"
#define PROPINO_PLUGIN_VAL_WDIR "/Propino"

#define PROPINO_PLUGIN_KEY_SBSN "selectedBoardSerialNumber"

// Propino Ftdi ////////////////////////////////////////////////////////////////

#define PROPINO_FTDI "QFtdi"

#define PROPINO_FTDI_KEY_GROUP (PROPINO_KEY_GROUP "/" PROPINO_FTDI)

#define PROPINO_FTDI_KEY_NAME "name"

#define PROPINO_FTDI_KEY_MAKE "make"
#define PROPINO_FTDI_VAL_MAKE "default.cmake"

// Propino Fftw ////////////////////////////////////////////////////////////////

#define PROPINO_FFTW "QFftw"

#define PROPINO_FFTW_KEY_GROUP (PROPINO_KEY_GROUP "/" PROPINO_FFTW)

#define PROPINO_FFTW_KEY_WISDOM "wisdom"

// Program Builder /////////////////////////////////////////////////////////////

#define PROGRAM_BUILDER "ProgramBuilder"

#define PROGRAM_BUILDER_KEY_GROUP (PROPINO_KEY_GROUP "/" PROGRAM_BUILDER)

#define PROGRAM_BUILDER_KEY_USER_CMAKE "userCMakeSearchPath"
#define PROGRAM_BUILDER_KEY_SYSTEM_CMAKE "systemCMakeSearchPath"

#define PROGRAM_BUILDER_KEY_USER_BIN "userBinarySearchPath"
#define PROGRAM_BUILDER_KEY_SYSTEM_BIN "systemBinarySearchPath"

#define PROGRAM_BUILDER_KEY_USER_S1BIN "userS1BinarySearchPath"
#define PROGRAM_BUILDER_KEY_SYSTEM_S1BIN "systemS1BinarySearchPath"

#define PROGRAM_BUILDER_KEY_USER_S2BIN "userS2BinarySearchPath"
#define PROGRAM_BUILDER_KEY_SYSTEM_S2BIN "systemS2BinarySearchPath"

#define PROGRAM_BUILDER_VAL_USER_CMAKE \
"/share/propino/cmake"

#define PROGRAM_BUILDER_VAL_SYSTEM_CMAKE \
"/../share/propino/cmake"

#define PROGRAM_BUILDER_VAL_USER_BIN \
"/share/propino/binaries/program"

#define PROGRAM_BUILDER_VAL_SYSTEM_BIN \
"/../share/propino/binaries/program"

#define PROGRAM_BUILDER_VAL_USER_S1BIN \
"/share/propino/binaries/loader/stage1"

#define PROGRAM_BUILDER_VAL_SYSTEM_S1BIN \
"/../share/propino/binaries/loader/stage1"

#define PROGRAM_BUILDER_VAL_USER_S2BIN \
"/share/propino/binaries/loader/stage2"

#define PROGRAM_BUILDER_VAL_SYSTEM_S2BIN \
"/../share/propino/binaries/loader/stage2"

#define PROGRAM_BUILDER_KEY_LP "loaderProtocol"
#define PROGRAM_BUILDER_KEY_BF "binaryFile"
#define PROGRAM_BUILDER_KEY_S1BF "stage1BinaryFile"
#define PROGRAM_BUILDER_KEY_S2BF "stage2BinaryFile"

// Binary Editor ///////////////////////////////////////////////////////////////

#define BINARY_EDITOR "BinaryEditor"

#define BINARY_EDITOR_KEY_GROUP (PROPINO_KEY_GROUP "/" BINARY_EDITOR)

#define BINARY_EDITOR_KEY_STATE "state"
#define BINARY_EDITOR_KEY_GEOMETRY "geometry"

#define BINARY_EDITOR_KEY_SAVE_FILE "saveFile"
#define BINARY_EDITOR_KEY_OPEN_FILE "openFile"
#define BINARY_EDITOR_KEY_EX_A_FILE "expAFile"
#define BINARY_EDITOR_KEY_EX_C_FILE "expCFile"
#define BINARY_EDITOR_KEY_DIFF_FILE "diffFile"

#define BINARY_EDITOR_KEY_WARNING_MESSAGE "warningMessage"

#define BINARY_EDITOR_KEY_FINDER_F_STRINGS "finderFStrings"
#define BINARY_EDITOR_KEY_FINDER_R_STRINGS "finderRStrings"

// Program Memory //////////////////////////////////////////////////////////////

#define PROGRAM_MEMORY "ProgramMemoryApp"

#define PROGRAM_MEMORY_KEY_GROUP (PROPINO_KEY_GROUP "/" PROGRAM_MEMORY)

#define PROGRAM_MEMORY_KEY_UBSP PROGRAM_BUILDER_KEY_USER_BIN
#define PROGRAM_MEMORY_KEY_SBSP PROGRAM_BUILDER_KEY_SYSTEM_BIN

#define PROGRAM_MEMORY_KEY_US1BSP PROGRAM_BUILDER_KEY_USER_S1BIN
#define PROGRAM_MEMORY_KEY_SS1BSP PROGRAM_BUILDER_KEY_SYSTEM_S1BIN

#define PROGRAM_MEMORY_KEY_US2BSP PROGRAM_BUILDER_KEY_USER_S2BIN
#define PROGRAM_MEMORY_KEY_SS2BSP PROGRAM_BUILDER_KEY_SYSTEM_S2BIN

#define PROGRAM_MEMORY_VAL_UBSP \
"/share/propino/binaries/memory/program"

#define PROGRAM_MEMORY_VAL_SBSP \
"/../share/propino/binaries/memory/program"

#define PROGRAM_MEMORY_VAL_US1BSP PROGRAM_BUILDER_VAL_USER_S1BIN
#define PROGRAM_MEMORY_VAL_SS1BSP PROGRAM_BUILDER_VAL_SYSTEM_S1BIN

#define PROGRAM_MEMORY_VAL_US2BSP PROGRAM_BUILDER_VAL_USER_S2BIN
#define PROGRAM_MEMORY_VAL_SS2BSP PROGRAM_BUILDER_VAL_SYSTEM_S2BIN

#define PROGRAM_MEMORY_KEY_LP PROGRAM_BUILDER_KEY_LP
#define PROGRAM_MEMORY_KEY_BF PROGRAM_BUILDER_KEY_BF
#define PROGRAM_MEMORY_KEY_S1BF PROGRAM_BUILDER_KEY_S1BF
#define PROGRAM_MEMORY_KEY_S2BF PROGRAM_BUILDER_KEY_S2BF

#define PROGRAM_MEMORY_KEY_STATE BINARY_EDITOR_KEY_STATE
#define PROGRAM_MEMORY_KEY_GEOMETRY BINARY_EDITOR_KEY_GEOMETRY

#define PROGRAM_MEMORY_KEY_SAVE_FILE BINARY_EDITOR_KEY_SAVE_FILE
#define PROGRAM_MEMORY_KEY_OPEN_FILE BINARY_EDITOR_KEY_OPEN_FILE
#define PROGRAM_MEMORY_KEY_EX_A_FILE BINARY_EDITOR_KEY_EX_A_FILE
#define PROGRAM_MEMORY_KEY_EX_C_FILE BINARY_EDITOR_KEY_EX_C_FILE
#define PROGRAM_MEMORY_KEY_DIFF_FILE BINARY_EDITOR_KEY_DIFF_FILE

#define PROGRAM_MEMORY_KEY_WARNING_MESSAGE BINARY_EDITOR_KEY_WARNING_MESSAGE

#define PROGRAM_MEMORY_KEY_FINDER_F_STRINGS BINARY_EDITOR_KEY_FINDER_F_STRINGS
#define PROGRAM_MEMORY_KEY_FINDER_R_STRINGS BINARY_EDITOR_KEY_FINDER_R_STRINGS

// Storage Memory //////////////////////////////////////////////////////////////

#define STORAGE_MEMORY "StorageMemoryApp"

#define STORAGE_MEMORY_KEY_GROUP (PROPINO_KEY_GROUP "/" STORAGE_MEMORY)

#define STORAGE_MEMORY_KEY_UBSP PROGRAM_BUILDER_KEY_USER_BIN
#define STORAGE_MEMORY_KEY_SBSP PROGRAM_BUILDER_KEY_SYSTEM_BIN

#define STORAGE_MEMORY_KEY_US1BSP PROGRAM_BUILDER_KEY_USER_S1BIN
#define STORAGE_MEMORY_KEY_SS1BSP PROGRAM_BUILDER_KEY_SYSTEM_S1BIN

#define STORAGE_MEMORY_KEY_US2BSP PROGRAM_BUILDER_KEY_USER_S2BIN
#define STORAGE_MEMORY_KEY_SS2BSP PROGRAM_BUILDER_KEY_SYSTEM_S2BIN

#define STORAGE_MEMORY_VAL_UBSP \
"/share/propino/binaries/memory/storage"

#define STORAGE_MEMORY_VAL_SBSP \
"/../share/propino/binaries/memory/storage"

#define STORAGE_MEMORY_VAL_US1BSP PROGRAM_BUILDER_VAL_USER_S1BIN
#define STORAGE_MEMORY_VAL_SS1BSP PROGRAM_BUILDER_VAL_SYSTEM_S1BIN

#define STORAGE_MEMORY_VAL_US2BSP PROGRAM_BUILDER_VAL_USER_S2BIN
#define STORAGE_MEMORY_VAL_SS2BSP PROGRAM_BUILDER_VAL_SYSTEM_S2BIN

#define STORAGE_MEMORY_KEY_LP PROGRAM_BUILDER_KEY_LP
#define STORAGE_MEMORY_KEY_BF PROGRAM_BUILDER_KEY_BF
#define STORAGE_MEMORY_KEY_S1BF PROGRAM_BUILDER_KEY_S1BF
#define STORAGE_MEMORY_KEY_S2BF PROGRAM_BUILDER_KEY_S2BF

#define STORAGE_MEMORY_KEY_STATE BINARY_EDITOR_KEY_STATE
#define STORAGE_MEMORY_KEY_GEOMETRY BINARY_EDITOR_KEY_GEOMETRY

#define STORAGE_MEMORY_KEY_SAVE_FILE BINARY_EDITOR_KEY_SAVE_FILE
#define STORAGE_MEMORY_KEY_OPEN_FILE BINARY_EDITOR_KEY_OPEN_FILE
#define STORAGE_MEMORY_KEY_EX_A_FILE BINARY_EDITOR_KEY_EX_A_FILE
#define STORAGE_MEMORY_KEY_EX_C_FILE BINARY_EDITOR_KEY_EX_C_FILE
#define STORAGE_MEMORY_KEY_DIFF_FILE BINARY_EDITOR_KEY_DIFF_FILE

#define STORAGE_MEMORY_KEY_WARNING_MESSAGE BINARY_EDITOR_KEY_WARNING_MESSAGE

#define STORAGE_MEMORY_KEY_FINDER_F_STRINGS BINARY_EDITOR_KEY_FINDER_F_STRINGS
#define STORAGE_MEMORY_KEY_FINDER_R_STRINGS BINARY_EDITOR_KEY_FINDER_R_STRINGS

// Board Settings //////////////////////////////////////////////////////////////

#define BOARD_SETTINGS "BoardSettingsDialog"

#define BOARD_SETTINGS_KEY_GROUP (PROPINO_KEY_GROUP "/" BOARD_SETTINGS)

#define BOARD_SETTINGS_KEY_OPEN_USER_FILE "openUserFile"
#define BOARD_SETTINGS_KEY_OPEN_SYSTEM_FILE "openSystemFile"

#define BOARD_SETTINGS_VAL_OPEN_USER_FILE PROGRAM_BUILDER_VAL_USER_CMAKE
#define BOARD_SETTINGS_VAL_OPEN_SYSTEM_FILE PROGRAM_BUILDER_VAL_SYSTEM_CMAKE

// Program Loader //////////////////////////////////////////////////////////////

#define PROGRAM_LOADER "ProgramLoaderDialog"

#define PROGRAM_LOADER_KEY_GROUP (PROPINO_KEY_GROUP "/" PROGRAM_LOADER)

#define PROGRAM_LOADER_KEY_OPEN_USER_F "openUserFile"
#define PROGRAM_LOADER_KEY_OPEN_SYSTEM_F "openSystemFile"

#define PROGRAM_LOADER_KEY_OPEN_USER_S1F "openUserStage1File"
#define PROGRAM_LOADER_KEY_OPEN_SYSTEM_S1F "openSystemStage1File"

#define PROGRAM_LOADER_KEY_OPEN_USER_S2F "openUserStage2File"
#define PROGRAM_LOADER_KEY_OPEN_SYSTEM_S2F "openSystemStage2File"

#define PROGRAM_LOADER_VAL_OPEN_USER_F PROGRAM_BUILDER_VAL_USER_BIN
#define PROGRAM_LOADER_VAL_OPEN_SYSTEM_F PROGRAM_BUILDER_VAL_SYSTEM_BIN

#define PROGRAM_LOADER_VAL_OPEN_USER_S1F PROGRAM_BUILDER_VAL_USER_S1BIN
#define PROGRAM_LOADER_VAL_OPEN_SYSTEM_S1F PROGRAM_BUILDER_VAL_SYSTEM_S1BIN

#define PROGRAM_LOADER_VAL_OPEN_USER_S2F PROGRAM_BUILDER_VAL_USER_S2BIN
#define PROGRAM_LOADER_VAL_OPEN_SYSTEM_S2F PROGRAM_BUILDER_VAL_SYSTEM_S2BIN

// Serial Terminal /////////////////////////////////////////////////////////////

#define SERIAL_TERMINAL "SerialTerminal"

#define SERIAL_TERMINAL_KEY_GROUP \
(PROPINO_KEY_GROUP "/" SERIAL_TERMINAL)

#define SERIAL_TERMINAL_KEY_STATE "state"
#define SERIAL_TERMINAL_KEY_GEOMETRY "geometry"

#define SERIAL_TERMINAL_KEY_SAVE_FILE "saveFile"
#define SERIAL_TERMINAL_KEY_OPEN_FILE "openFile"

// Serial Oscilloscope /////////////////////////////////////////////////////////

#define SERIAL_OSCILLOSCOPE "SerialOscilloscope"

#define SERIAL_OSCILLOSCOPE_KEY_GROUP \
(PROPINO_KEY_GROUP "/" SERIAL_OSCILLOSCOPE)

#define SERIAL_OSCILLOSCOPE_KEY_STATE "state"
#define SERIAL_OSCILLOSCOPE_KEY_GEOMETRY "geometry"

#define SERIAL_OSCILLOSCOPE_KEY_SAVE_BMP_FILE "saveBmpFile"
#define SERIAL_OSCILLOSCOPE_KEY_SAVE_JPG_FILE "saveJpgFile"
#define SERIAL_OSCILLOSCOPE_KEY_SAVE_PNG_FILE "savePngFile"
#define SERIAL_OSCILLOSCOPE_KEY_SAVE_PDF_FILE "savePdfFile"

#define SERIAL_OSCILLOSCOPE_KEY_EXPORT_S_FILE "exportStateFile"
#define SERIAL_OSCILLOSCOPE_KEY_IMPORT_S_FILE "importStateFile"

#endif // PROPINO_SETTINGS_H

/***************************************************************************//**
* @file
* @par MIT License - TERMS OF USE:
* @n Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* @n
* @n The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* @n
* @n THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*******************************************************************************/
