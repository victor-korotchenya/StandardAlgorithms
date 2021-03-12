#include <cstdlib>
#include "ConsoleColor.h"

//namespace
//{
//#ifdef _WIN32z
//    HANDLE hConsole;
//#endif
//
//    enum Color : unsigned short
//    {
//        DarkBlue = 1,
//        DarkGreen,
//        DarkTeal,
//        DarkRed,
//        DarkPinK,
//        DarkYellow,
//        Gray,
//        DarkGray,
//        Blue,
//        Green,
//        Teal,
//        Red,
//        PinK,
//        Yellow,
//        White
//    };
//
//    void SetColor(Color)
//    {
//    //todo: p1.use color.
//        SetLastError(0);
//    if (nullptr == hConsole)
//    {
//        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
//        SetLastError(0);
//    }
//
//    SetConsoleTextAttribute(hConsole, c);
//    SetLastError(0);
//    }
//
//    void SetColorError()
//    {
//        SetColor(Color::Red);
//        system("Color 02");
//    }
//
//    void SetColorSuccess()
//    {
//        SetColor(Color::Green);
//    }
//
//    void SetColorDefault()
//    {
//        SetColor(Color::Gray);
//    }
//}

namespace Privet::Algorithms::Colors
{
    ErrorColor::ErrorColor()
    {
        //SetColorError();
        system("Color 04");//red front on black background.
    }

    ErrorColor::~ErrorColor()
    {
        //SetColorDefault();
    }

    SuccessColor::SuccessColor()
    {
        //SetColorSuccess();
        system("Color 02");//green front on black background.
    }

    SuccessColor::~SuccessColor()
    {
        //SetColorDefault();
    }
}