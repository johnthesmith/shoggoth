/*
    Shoggoth constants and conversion methods
*/
#pragma once



#include <sqlite3.h>
#include <string>
#include <vector>

#include "../../../../lib/core/real.cpp"
#include "../../../../lib/core/log_manager.h"



using namespace std;



class ShoggothDb : public Result
{
    private:
        /* The log manger object */
        LogManager* logManager      = NULL;

        /* Db handle */
        sqlite3* db;

        /* Открыть базу */
        ShoggothDb* open
        (
            const string aFileName
        );

        /* Закрыть базу */
        ShoggothDb* close();

        /* Инициализировать (создать таблицы и т.п.) */
        ShoggothDb* init();

    public:

        /*
            Constructor
        */
        ShoggothDb
        (
            /* Log manager object */
            LogManager*,
            /* Db file name */
            const string
        );



        /*
            Destructor
        */
        ~ShoggothDb();



        /*
            Create and return new db object
            Optionaly create db file with init
        */
        static ShoggothDb* create
        (
            /* Log manager object */
            LogManager*,
            /* Db file name */
            const string aFileName
        );




        /*
            Destroy db object
        */
        void destroy();



        /*
            Return true if db was opened
        */
        bool isOpened();



        /*
            Execute sql code
        */
        ShoggothDb* exec
        (
            /* Sql source code */
            string
        );



        /*
            Execute sql code
        */
        ShoggothDb* exec
        (
            const string aSource,
            ParamList* aParamList
        );


        /******************************************************************************
            Setters and getters
        */

        /*
            Return log object
        */
        Log* getLog();



        /******************************************************************************
            Application methods
        */

        /*
            Записать статистику
        */
        ShoggothDb* writeStartWeights
        (
            /* Net wersion */
            string  netVersion,
            /* Nerve id */
            string  nerveId,
            /* Sum of weights */
            real    sumWeight,
            /* Count weights */
            int     countWeights
        );


        /*
            Коммит сети
        */
        ShoggothDb* netCommit
        (
            const string netVersion,
            const string result,
            const int tick,
            const int test_count,
            const real parent_survival_error_avg,
            const real current_survival_error_avg
        );



        /*
            Создание сети
        */
        ShoggothDb* netStart
        (
            /* net version */
            const string,
            /* mutation information */
            const string
        );



        /*
            Запись результатов тестирования
        */
        ShoggothDb* testResult
        (
            /* Версия сети */
            const string,
            /* Тик */
            const int,
            /* Строка результат теста */
            const string
        );
};

