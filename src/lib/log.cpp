#include <string>
#include <stack>
#include <iostream>

#include "log.h"
#include "str.h"
#include "utils.h"



using namespace std; // для того что бы std:: не приходилось каждый раз писать


Log::Log()
{
    beginStack = new stack <long long>;
}


/*
    Деструктор
*/
Log::~Log()
{
    lineEnd();

    if( isOpen() )
    {
        close();
    }

    delete beginStack;
}



/*
    Construct
*/
Log* Log::create()
{
    return new Log();
}



/*
    Destroy the log
*/
void Log::destroy()
{
    delete this;
}



/*
    проверка открыт ли Лог файл
*/
bool Log::isOpen()
{
    return fileHandle != NULL;
}



/*
    Открыть логфайл
*/
Log* Log::open()
{
    if( !isOpen() && fileName != "" )
    {
        fileHandle = fopen( fileName.c_str(), "a" );
        if( !isOpen() )
        {
            cout << "File open error for " << fileName << "\n";
        }
    }
    return this;
}



/*
    Закрытие лога
*/
Log* Log::close()
{
    if( isOpen() )
    {
        fclose( fileHandle );
        fileHandle = NULL;
    }
    return this;
}



/*
    Получить статус активности вывода в логфайл
*/
bool Log::getEnabled()
{
    return enabled;
}



/*
    Установить активность вывода в логфайл
*/
Log* Log::setEnable( bool a )
{
    enabled = a;
    return this;
}



Log* Log::write
(
    string a  /* String for log */
)
{
    if( enabled )
    {
        buffer.append( a );
    }
    return this;
}



/*
    Вывод символа завершения стоки
*/
Log* Log::eol()
{
    write( string( 1, cEOL ));
    return this;
}



/*
    Вывод пробела
*/
Log* Log::space()
{
    write(" ");
    return this;
}



/*
    Вывод текста в лог
*/
Log* Log::text
(
    string a    /* Значение */
)
{
    if( width > 0 )
    {
        a = strAlign( a, align, width );
    }

    write( a );

    return this;
}



/*
    Установка ширины вывода текста
*/
Log* Log::setWidth
(
    int a
)
{
    width = a;
    return this;
}



/*
    Вывод значений
*/

/*
    Вывод логического значения в лог
*/
Log* Log::value
(
    bool a
)
{
    if( a )
    {
        text( trueValue );
    }
    else
    {
        text( falseValue );
    }
    return this;
}



/*
    Вывод строки в лог
*/
Log* Log::value
(
    char* a /* Выводимое значение */
)
{
    text( string( a ) );
    return this;
}



/*
    Вывод целочисленного занчения в лог
*/
Log* Log::value
(
    int a
)
{
    text( to_string( a ) );
    return this;
}




/*
    Вывод целочисленного занчения в лог
*/
Log* Log::value
(
    unsigned int a
)
{
    text( to_string( a ) );
    return this;
}



Log* Log::value
(
    long long a
)
{
    text( to_string( a ) );
    return this;
}



Log* Log::value
(
    float a
)
{
    text( to_string( a ) );
    return this;
}



Log* Log::value
(
    double a
)
{
    pushColor();
    if( a > 0 )
    {
        setColor( colorValuePos );
    }
    else if( a < 0 )
    {
        setColor( colorValueNeg );
    }
    else
    {
        setColor( colorValueZero );
    }
    text( to_string( a ) );
    popColor();
    return this;
}



Log* Log::value
(
    string a
)
{
    text( a );
    return this;
}



/*
    Вывод целочисленного занчения в лог формате [ int title:value ]
*/
Log* Log::prm
(
    string aTitle,  /* Title for parameter */
    string aValue   /* Value */
)
{
    pushColor();
    setColor( colorLabel );
    text( "[" );
    text( "string " );
    setColor( colorInfo );
    text( aTitle );
    space();
    setColor( colorValue );
    value ( aValue );
    setColor( colorLabel );
    text( "]" );
    popColor();

    return this;
}



Log* Log::prm
(
    string aTitle,  /* Title for parameter */
    int aValue      /* Value */
)
{
    pushColor();
    setColor( colorLabel );
    text( "[" );
    text( "int " );
    setColor( colorInfo );
    text( aTitle );
    space();
    setColor( colorValue );
    value ( aValue );
    setColor( colorLabel );
    text( "]" );
    popColor();

    return this;
}



Log* Log::prm
(
    string aTitle,  /* Title for parameter */
    unsigned int aValue      /* Value */
)
{
    pushColor();
    setColor( colorLabel );
    text( "[" );
    text( "int " );
    setColor( colorInfo );
    text( aTitle );
    space();
    setColor( colorValue );
    value ( aValue );
    setColor( colorLabel );
    text( "]" );
    popColor();

    return this;
}



Log* Log::prm
(
    string aTitle,  /* Title for parameter */
    long long aValue      /* Value */
)
{
    pushColor();
    setColor( colorLabel );
    text( "[" );
    text( "long long " );
    setColor( colorInfo );
    text( aTitle );
    space();
    setColor( colorValue );
    value ( aValue );
    setColor( colorLabel );
    text( "]" );
    popColor();

    return this;
}



Log* Log::prm
(
    string aTitle,  /* Title for parameter */
    double aValue      /* Value */
)
{
    pushColor();
    setColor( colorLabel );
    text( "[" );
    text( "double " );
    setColor( colorInfo );
    text( aTitle );
    space();
    setColor( colorValue );
    value ( aValue );
    setColor( colorLabel );
    text( "]" );
    popColor();

    return this;
}



Log* Log::print
(
    string a    /* Message */
)
{
    begin();
    lineEnd();
    pushColor();
    setColor( colorInfo );
    write( a );
    popColor();
    end();
    return this;
}



/******************************************************************************
    Line contrl
*/

Log* Log::lineBegin
(
    LogRecord type
)
{
    lineEnd();

    /* Collect the line begin moment */
    auto momentCurrent = now();
    long long delta = momentLineBegin > 0 ?  momentCurrent - momentLineBegin : 0;
    momentLineBegin = momentCurrent;

    string Tab = "";
    Tab.resize( depth * tabSize, tabChar );

    typeLine = type;
    string charLine;

    switch( type )
    {
        case lrBegin:
            colorLine = colorJob;
            charLine = ">";
        break;
        case lrEnd:
            colorLine = colorJob;
            charLine = "<";
        break;
        case lrWarning:
            colorLine = colorWarning;
            charLine = "!";
        break;
        case lrTrace:
            colorLine = colorTrace;
            charLine = "~";
        break;
        case lrDebug:
            colorLine = colorDebug;
            charLine = "#";
        break;
        case lrInfo:
            colorLine = colorInfo;
            charLine = "i";
        break;
        case lrError:
            colorLine = colorError;
            charLine = "X";
        break;
    }

    setAlign( ALIGN_RIGHT );
    setWidth( 8 );
    setColor( colorLabel );
    value( delta );
    space();
    setAlign();
    setWidth( 0 );
    setColor( colorLine );
    write( charLine );
    space();
    setColor( tabColor );
    write( Tab );
    setColor( colorLine );

    return this;
}



/*
    Безусловено завершение линии
    Если линия была открыта то буфер выгрушается в направление
*/
Log* Log::lineEnd()
{
    /* Вывод строки */
    if( enabled && typeLine != lrNone )
    {
        /* Close current line */
        typeLine = lrNone;

        /* Draw enter */
        eol();

        if( fileName == "" )
        {
            /* Write to console */
            cout << buffer;
        }
        else
        {
            /* Write to file */
            if( !isOpen() ) open();
            fwrite( buffer.c_str(), buffer.length(), 1, fileHandle );
            close();
        }
        buffer = "";
    }
    return this;
}



/******************************************************************************
    Log lines
*/

Log* Log::begin()
{
    return begin( "" );
}



Log* Log::begin
(
    string message
)
{
    lineBegin( lrBegin );

    beginStack -> push( momentLineBegin );

    write( message );

    depth++;

    return this;
}



Log* Log::end()
{
    return end( "" );
}



Log* Log::end
(
    string aMessage
)
{
    depth--;

    if( depth < 0 )
    {
        depth=0;
    }

    lineBegin( lrEnd );
    write( aMessage );

    long long momentBegin = beginStack -> top();
    beginStack -> pop();
    long long delta = momentLineBegin - momentBegin;
    pushColor();
    setColor( colorLabel );
    value( delta );
    popColor();

    return this;
}



Log* Log::debug()
{
    return debug( "" );
}



Log* Log::debug
(
    string a    /* Message */
)
{
    lineBegin( lrDebug );
    write( a );
    return this;
}



Log* Log::trace()
{
    return trace( "" );
}



Log* Log::trace
(
    string a    /* Message */
)
{
    lineBegin( lrTrace );
    write( a );
    return this;
}



Log* Log::info()
{
    return info( "" );
}


Log* Log::info
(
    string a    /* Message */
)
{
    lineBegin( lrInfo );
    write( a );
    return this;
}



Log* Log::warning()
{
    return warning( "" );
}

Log* Log::warning
(
    string a    /* Message */
)
{
    lineBegin( lrWarning );
    write( a );
    return this;
}



Log* Log::error()
{
    return error( "" );
}

Log* Log::error
(
    string a    /* Message */
)
{
    lineBegin( lrError );
    write( a );
    return this;
}


/******************************************************************************
*/


// TLog &Param(string AName, int AValue)
// {
//  Write(AName+"="+to_string(AValue));
//  return **this;
// }
//
//
//
// TLog &Param(string AName, unsigned AValue)
// {
//  Write(AName+"="+to_string(AValue));
//  return **this;
// }
//
//
//
// TLog &Param(string AName, long long AValue)
// {
//  Write(AName+"="+to_string(AValue));
//  return **this;
// }
//
//
// TLog &Param(string AName, unsigned long long AValue)
// {
//  Write(AName+"="+to_string(AValue));
//  return **this;
// }
//
//
//
// TLog &Param(string AName, double AValue)
// {
//  Write(AName+"="+to_string(AValue));
//  return **this;
// }
//
//
//
// // Вывод параметра в формате AName=AValue
// TLog &Param(string AName, string AValue)
// {
//  Write(AName+"="+AValue);
//  return **this;
// }



/******************************************************************************
    Service
*/


/*
    Установка умолчального выравнивания по левому краю
*/
Log* Log::setAlign()
{
    return setAlign( ALIGN_LEFT );
}



/*
    Установка умолчального выравнивания по левому краю
*/
Log* Log::setAlign( TextAlign a )
{
    align = a;
    return this;
}



/*
    Возвращает значение выравнивания
*/
TextAlign Log::getAlign()
{
    return align;
}



/*
    Установка умолчального цвета
*/
Log* Log::setColor()
{
    setColor( INK_DEFAULT );
    return this;
}



/*
    Установка специфического цвета
*/
Log* Log::setColor
(
    string a    /* Цветовая константа */
)
{
    color = a;  /* Сохраняем цвет */
    if( getColored() )
    {
        write( a );         /* Изменяем цвет на консоли */
    }
    return this;
}



bool Log::getColored()
{
    return colored;
}



Log* Log::setColored( bool value )
{
    colored = value;
    return this;
}



Log* Log::pushColor()
{
    colorStack.push( color );
    return this;
}



Log* Log::popColor()
{
    setColor( colorStack.top());
    colorStack.pop();
    return this;
}



Log* Log::pushAlign()
{
    alignStack.push( color );
    return this;
}



Log* Log::popAlign()
{
    setColor( colorStack.top());
    alignStack.pop();
    return this;
}



/*
    Set log file name
*/
Log* Log::setFileName
(
    string aFileName
)
{
    fileName = aFileName;
    if( isOpen() )
    {
        close();
    }
    open();
    return this;
}



/*
    Return log file name
*/
string Log::getFileName()
{
    return fileName;
}
