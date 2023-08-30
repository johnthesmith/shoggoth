#pragma once

#include <stack>
#include "console.h"
#include "text_align.h"

using namespace std; /* для того что бы std:: не приходилось каждый раз писать */



/*
    Правило выравниявания текста
*/
enum LogRecord
{
    lrNone,
    lrBegin,
    lrEnd,
    lrTrace,
    lrDebug,
    lrInfo,
    lrError,
    lrWarning,
    lrDump
};



class Log
{
    private:
        /* Параметры журнала */

        bool                enabled         = true;         /* активность лога влияет на команду Write */
        int                 depth           = 0;            /* глубина конструкций begin end */
        string              fileName        = "";           /* имя файла */
        LogRecord           typeLine        = lrNone;
        int                 tabSize         = 2;            /* Размер табулятора */
        char                tabChar         = ' ';          /* Символ табулятора */

        /* Состояние журнала */
        FILE                *fileHandle     = NULL;         /* указатель на объект файла для записи */
        long long           momentLineBegin = 0;
        string              colorLine       = INK_DEFAULT;

        /* Состояния текстового процессора */
        string              buffer          = "";
        stack <string>      colorStack;
        stack <string>      alignStack;
        stack <long long>*  beginStack;

        /* Настройки текстового процессора */
        bool                colored         = true;         /* Цветной вывод */
        string              color           = "";           /* Текущий цвет */
        int                 width           = 0;            /* ширина выводимого текста */
        TextAlign           align           = ALIGN_LEFT;   /* ширина выводимого текста */
        string              trueValue       = "true";       /* строковое значение для приведения true */
        string              falseValue      = "false";      /* строковое значение для приведения false */

    public:

        string              colorError      = INK_RED;
        string              colorWarning    = INK_YELLOW;
        string              colorInfo       = INK_SILVER;
        string              colorTrace      = INK_SKY;
        string              colorDebug      = INK_AQUA;

        string              tabColor        = INK_SILVER;
        string              colorJob        = INK_GOLD;
        string              colorValue      = INK_MAGENTA;
        string              colorValueZero  = INK_CYAN;
        string              colorValuePos   = INK_GREEN;
        string              colorValueNeg   = INK_MAGENTA;
        string              colorLabel      = INK_GREY;

        /*
            Maintaince
        */



        /*
            Constructor
        */
        Log();



        /*
            Destructor
        */
        ~Log();



        /*
            Construct
        */
        static Log* create();



        /*
            Destroy the log
        */
        void destroy();



        /*
            проверка открыт ли Лог файл
        */
        bool isOpen();



        /*
            Открыть логфайл
        */
        Log* open();



        /*
            Закрытие лога
        */
        Log* close();



        /* Получить статус активности вывода в логфайл */
        bool getEnabled();

        /* Установить активность вывода в логфайл */
        Log* setEnable
        (
            bool
        );

        Log* write
        (
            string aValue  /* String for log */
        );

        /* Вывод символа завершения стоки */
        Log* eol();

        /* Вывод пробела */
        Log* space();

        /* Вывод текста в лог */
        Log* text
        (
            string aValue    /* Значение */
        );

        /* Установка умолчального выравнивания по левому краю */
        Log* setAlign();

        /* Установка умолчального выравнивания по левому краю */
        Log* setAlign( TextAlign );

        /* Возвращает значение выравнивания */
        TextAlign getAlign();

        /* Установка умолчального цвета */
        Log* setColor();

        /* Установка специфического цвета */
        Log* setColor
        (
            string /* Цветовая константа */
        );

        /* Установка ширины вывода текста */
        Log* setWidth
        (
            int /* Value */
        );

        /* Вывод значений */

        /* Вывод логического значения в лог */
        Log* value
        (
            bool /* Выводимое значение */
        );

        /* Вывод строки в лог */
        Log* value
        (
            char* /* Выводимое значение */
        );

        /* Вывод целочисленного занчения в лог */
        Log* value
        (
            int /* Выводимое значение */
        );

        /* Вывод целочисленного занчения в лог */
        Log* value
        (
            unsigned int /* Выводимое значение */
        );

        /* Вывод целочисленного длинного занчения в лог */
        Log* value
        (
            long long /* Выводимое значение */
        );

        /* Вывод float pанчения в лог */
        Log* value
        (
            float /* Выводимое значение */
        );

        /* Вывод double pанчения в лог */
        Log* value
        (
            double /* Выводимое значение */
        );

        /* Вывод double pанчения в лог */
        Log* value
        (
            string /* Выводимое значение */
        );

        /* Вывод целочисленного занчения в лог формате [ int title:value ] */
        Log* prm
        (
            string, /* Коментарий к значению */
            int     /* Выводимое значение */
        );

        /* Вывод целочисленного занчения в лог формате [ int title:value ] */
        Log* prm
        (
            string, /* Коментарий к значению */
            unsigned int     /* Выводимое значение */
        );

        /* Вывод целочисленного занчения в лог формате [ int title:value ] */
        Log* prm
        (
            string, /* Коментарий к значению */
            long long /* Выводимое значение */
        );

        /* Вывод двойного занчения c плавающей точкой в лог формате [ int title:value ] */
        Log* prm
        (
            string, /* Коментарий к значению */
            double     /* Выводимое значение */
        );



        /* Вывод целочисленного занчения в лог формате [ string title:value ] */
        Log* prm
        (
            string, /* Коментарий к значению */
            string  /* Выводимое значение */
        );



        /*
            Управление линиями
        */



        /*
            Начало линии
        */
        Log* lineBegin
        (
            LogRecord
        );

        /* Завершение линии */
        Log* lineEnd();


        /****************************************************************************
            Функционал работы с секциями
        */

        /*
            Начало секции
        */
        Log* begin();

        /* Начало секции с коментарием */
        Log* begin
        (
            string
        );

        /* Завершение секции */
        Log* end();

        /* Завершение секции с коментарием */
        Log* end
        (
            string
        );

        Log* debug();


        Log* debug
        (
            string  /* Сообщение */
        );


        Log* trace();



        Log* trace
        (
            string  /* Сообщение */
        );



        Log* info();



        Log* info
        (
            string  /* Сообщение */
        );



        Log* warning();



        Log* warning
        (
            string  /* Сообщение */
        );



        Log* error();



        Log* error
        (
            string  /* Сообщение */
        );



        Log* print
        (
            string  /* Message */
        );



        bool getColored();
        Log* setColored( bool value );
        Log* pushColor();
        Log* popColor();
        Log* pushAlign();
        Log* popAlign();



        /*
            Set log file name
        */
        Log* setFileName
        (
            string
        );



        /*
            Return log file name
        */
        string getFileName();

};
