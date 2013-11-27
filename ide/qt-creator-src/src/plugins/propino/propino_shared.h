/***************************************************************************//**
* @file
* Propino Shared Header File
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

#ifndef PROPINO_SHARED_H
#define PROPINO_SHARED_H

/*******************************************************************************
* Serial Protocol (Little Endian):
*
* Byte 0 = 27 ('\e')
* Byte 1 = Serial Function [0:255] (See Below)
*
* if(SerialFunction != EXTENDED_COMMAND)
* {
*   Nibble 5 = Channel Handle [0:15] (Window Handle = 0)
*   Nibble 6 = Serial Data Type [0:9] (See Below)
*   Byte 3 = Argument Length (([0:255] + 1) * sizeof(SerialDataType))
*
*   Bytes [4:259 or 4:515 or 4:1027 or 4:2051] = Argument Data
* }
* else
* {
*   Byte 2 = Serial Data Type [0:9] (See Below)
*   Byte 3 = Argument Length (([0:255] + 1) * sizeof(SerialDataType))
*   Long 2 = Serial Function [-2,147,483,648:2,147,483,647] (See Below)
*   Long 3 = Window Handle [0:4,294,967,295]
*   Long 4 = Channel Handle [0:4,294,967,295]
*
*   Bytes [16:271 or 16:527 or 16:1039 or 16:2063] = Argument Data
* }
*
* Argument Data can be an array.
*******************************************************************************/

enum SerialFunction
{
    NULL_COMMAND = 0,

    SET_BAUDRATE,

    NEW_BAR_CHART,
    NEW_CURVE,
    NEW_GRAPH,
    NEW_BOX_CHART,

    SET_LINE_COLOR,
    SET_FILL_COLOR,
    SET_LINE_STYLE,
    SET_FILL_STYLE,

    SET_CURVE_DELTA_MODE,
    SET_GRAPH_DELTA_MODE,

    SET_CURVE_SIZE,
    SET_GRAPH_SIZE,

    SET_CURVE_SAMPLE_RATE,
    SET_GRAPH_SAMPLE_RATE,

    SET_CURVE_SCALER,
    SET_GRAPH_SCALER,

    ADD_BAR_DATA,

    SET_BOX_DATA,

    ADD_CURVE_DATA_0,
    ADD_CURVE_DATA_1,

    ADD_GRAPH_DATA_0,
    ADD_GRAPH_DATA_1,

    SET_BAR_ABOVE,
    SET_BAR_BELOW,

    SET_CURVE_DRAWING_STYLE,
    SET_CURVE_SCATTER_STYLE,
    SET_CURVE_FILL_CURVE,
    SET_CURVE_FFT,

    SET_GRAPH_DRAWING_STYLE,
    SET_GRAPH_SCATTER_STYLE,
    SET_GRAPH_FILL_GRAPH,
    SET_GRAPH_FFT,

    CLEAR_DATA,
    CLEAR_PLOT,
    CLEAR_ALL_DATUM,
    CLEAR_ALL_PLOTS,

    SET_X_AXIS_SCALE_TYPE,
    SET_X_AXIS_SCALE_LOG_BASE,
    SET_X_AXIS_RANGE_REVERSED,
    SET_X_AXIS_LABEL,

    SET_Y_AXIS_SCALE_TYPE,
    SET_Y_AXIS_SCALE_LOG_BASE,
    SET_Y_AXIS_RANGE_REVERSED,
    SET_Y_AXIS_LABEL,

    SET_FFT_X_AXIS_SCALE_TYPE,
    SET_FFT_X_AXIS_SCALE_LOG_BASE,
    SET_FFT_X_AXIS_RANGE_REVERSED,
    SET_FFT_X_AXIS_LABEL,

    SET_FFT_Y_AXIS_SCALE_TYPE,
    SET_FFT_Y_AXIS_SCALE_LOG_BASE,
    SET_FFT_Y_AXIS_RANGE_REVERSED,
    SET_FFT_Y_AXIS_LABEL,

    EXTENDED_COMMAND = 255,

    SET_PLOT_BACKGROUND_COLOR,

    TERMINAL_PRINT,

    NEW_OSCILLOSCOPE_WINDOW,
    CLOSE_OSCILLOSCOPE_WINDOW,
    CLOSE_ALL_OSCILLOSCOPES,

    NEW_TERMINAL_WINDOW,
    CLOSE_TERMINAL_WINDOW,
    CLOSE_ALL_TERMINALS
};

enum SerialDataType
{
    U_8_BIT,
    S_8_BIT,
    U_16_BIT,
    S_16_BIT,
    U_32_BIT,
    S_32_BIT,
    U_64_BIT,
    S_64_BIT,

    F_32_BIT, // float
    F_64_BIT // double
};

enum SerialTerminalControlCharacter
{
    CC_NULL = 0,

    CC_HOME_CURSOR,
    CC_END_CURSOR,
    CC_MOVE_CURSOR_LEFT,
    CC_MOVE_CURSOR_RIGHT,
    CC_MOVE_CURSOR_UP,
    CC_MOVE_CURSOR_DOWN,

    CC_BEEP_SPEAKER,
    CC_BACKSPACE,
    CC_TAB,
    CC_LINE_FEED,
    CC_CLEAR_TO_END_OF_LINE,
    CC_CLEAR_LINES_BELOW,
    CC_CARRIAGE_RETURN,
    CC_CLEAR_TO_START_OF_LINE,
    CC_CLEAR_LINES_ABOVE,

    CC_CLEAR_SCREEN,

    CC_XON,
    CC_SET_FOREGROUND_TO_GRAY,
    CC_XOFF,
    CC_SET_FOREGROUND_TO_BLACK,
    CC_SET_FOREGROUND_TO_BLUE,
    CC_SET_FOREGROUND_TO_RED,
    CC_SET_FOREGROUND_TO_GREEN,
    CC_SET_FOREGROUND_TO_MAGENTA,
    CC_SET_FOREGROUND_TO_CYAN,
    CC_SET_FOREGROUND_TO_YELLOW,

    CC_ESCAPE,

    CC_MOVE_TO_START_OF_LINE,
    CC_MOVE_TO_END_OF_LINE,
    CC_MOVE_TO_PREVIOUS_START_OF_LINE,
    CC_MOVE_TO_NEXT_START_OF_LINE,

    CC_DELETE = 127
};

#endif // PROPINO_SHARED_H

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
