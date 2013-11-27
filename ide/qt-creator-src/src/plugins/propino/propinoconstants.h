/***************************************************************************//**
* @file
* Propino Constants Header File
*
* @version @n 1.0
* @date @n 10/18/2013
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2013 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 10/18/2013
*******************************************************************************/

#ifndef PROPINOCONSTANTS_H
#define PROPINOCONSTANTS_H

#define STRINGIFY_INTERNAL(x) #x
#define STRINGIFY(x) STRINGIFY_INTERNAL(x)

#define PROPINO_VERSION_STR STRINGIFY(PROPINO_VERSION)
#define PROPINO_COMPAT_VERSION_STR STRINGIFY(PROPINO_COMPAT_VERSION)

namespace Propino {
namespace Constants {

// Icons ///////////////////////////////////////////////////////////////////////

const char PROPINO_LOGO_PATH[] = ":/ICONS/propino_logo.png";
const char PROPINO_SPLASH_PATH[] = ":/ICONS/propino_splash.png";

// Board Menu //////////////////////////////////////////////////////////////////

const char BOARD_MENU_ID[] = "Propino.Board.Menu";
const char EDIT_BOARD_SETTINGS_ID[] = "Propino.Edit.Board.Settings.Action";
const char FREE_SELECTED_BOARD_ID[] = "Propino.Free.Selected.Board.Action";
const char CHANGE_SELECTED_BOARD_ID[] = "Propino.Change.Selected.Board.Menu";
const char OPEN_SERIAL_TERMINAL_ID[] = "Propino.Open.Serial.Terminal.Action";
const char EDIT_PROGRAM_MEMORY_ID[] = "Propino.Edit.Program.Memory.Action";
const char EDIT_STORAGE_MEMORY_ID[] = "Propino.Edit.Storage.Memory.Action";
const char BINARY_LOADER_ID[] = "Propino.Binary.Loader.Action";

// Widgets Menu ////////////////////////////////////////////////////////////////

const char WIDGETS_MENU_ID[] = "Propino.Widgets.Menu";
const char REMOVE_WIDGET_MENU_ID[] = "Propino.Remove.Widget.Menu";
const char REMOVE_ALL_WIDGETS_ID[] = "Propino.Remove.All.Widgets.Action";

// Help Menu ///////////////////////////////////////////////////////////////////

const char GENERAL_HELP_ID[] = "Propino.General.Help.Action";
const char EDITOR_HELP_ID[] = "Propino.Editor.Help.Action";

const char ABOUT_PROPINO_ID[] = "Propino.About.Propino.Action";
const char ABOUT_QT_ID[] = "Propino.About.Qt.Action";

// Progress ////////////////////////////////////////////////////////////////////

const char GET_DEVICES_ID[] = "Propino.Get.Devices";

////////////////////////////////////////////////////////////////////////////////

} // namespace Propino
} // namespace Constants

#endif // PROPINOCONSTANTS_H

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
