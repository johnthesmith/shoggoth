#pragma once

#include <string>
#include <regex>

#include "text_align.h"



using namespace std;



/*
    https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
*/
template<typename ... Args>
std::string strFormat
(
    const std::string& format,  /* format template */
    Args ... args               /* list of arguments */
)
{
    string result;
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1;
    if( size >= 0 )
    {
        std::unique_ptr<char[]> buf( new char[ size ] );
        snprintf( buf.get(), size, format.c_str(), args ... );
        result = std::string( buf.get(), buf.get() + size - 1 );
    }
    else
    {
        result = "";
    }
    return result;
}




/*
    строку AValue выровненную до длилны ALength символами AChar по правилу ATextAlign
*/

string strAlign
(
    string aValue,
    TextAlign aAlign,
    int aLength
)
{
    int delta = aValue.size() - aLength;
    if( delta > 0 )
    {
        int b;
        int e = aLength;
        switch (aAlign)
        {
            case ALIGN_LEFT     : b = 0; break;
            case ALIGN_RIGHT    : b = delta; break;
            case ALIGN_CENTER   : b = delta / 2; break;
        }
        aValue = aValue.substr( b, e );
    }
    else
    {
        string s = aValue;
        switch ( aAlign )
        {
            case ALIGN_LEFT:
                aValue.resize( aLength, ' ');
            break;
            case ALIGN_RIGHT:
                aValue.assign( -delta, ' ').append( s );
            break;
            case ALIGN_CENTER:
                aValue.resize( -delta/2, ' ' );
                aValue.append( s ).resize( aLength, ' ' );
            break;
        }
    }
    return aValue;
}



/*
    Получение строки из вектора строк
*/
string strFromVector
(
    vector<string> list,    /* Вектор строк */
    const string delimiter        /* Разделитель */
)
{
    string result;
//    switch( list.size() )
//    {
//    	case 0: result = "";   break;
//    	case 1: result = list[0]; break;
//    	default:
//    		ostringstream os;
//    		copy( list.begin(),list.end() - 1, ostream_iterator<string>(os, delimiter));
//    		os << *list.rbegin();
//    		result = os.str();
//	    break;
//    }
    return result;
}
