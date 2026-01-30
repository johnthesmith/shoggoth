#include <iostream>
#include "shoggoth_db.h"
#include "../../../../lib/json/param_list.h"


/*
    Constructor
*/
ShoggothDb::ShoggothDb
(
    LogManager* aLogManager,
    const string aFileName
)
:
logManager( aLogManager ),
db( nullptr )
{
    open( aFileName );
}



/*
    Деструктор
*/
ShoggothDb::~ShoggothDb()
{
    close();
}



/*
    Создать объект (статический метод)
*/
ShoggothDb* ShoggothDb::create
(
    LogManager* aLogManager,
    const string aFileName
)
{
    return new ShoggothDb( aLogManager, aFileName );
}



/*
    Удалить объект
*/
void ShoggothDb::destroy()
{
    delete this;
}



/*
    Открыть базу
*/
ShoggothDb* ShoggothDb::open
(
    /* File name */
    const string aFileName
)
{
    if( !isOpened() )
    {
        if( sqlite3_open( aFileName.c_str(), &db ) == SQLITE_OK )
        {
            getLog()
            -> trace( "Db opened" )
            -> prm( "file name", aFileName );
            /* Init db */
            init();
        }
        else
        {
            getLog()
            -> warning( "Error db open" )
            -> prm( "file name", aFileName )
            -> prm( "message", sqlite3_errmsg( db ) );
            setResult( "db-open-error" );
        }
    }
    return this;
}



/*
    Закрыть базу
*/
ShoggothDb* ShoggothDb::close()
{
    if( isOpened() )
    {
        sqlite3_close(db);
        db = nullptr;
    }
    return this;
}



/*
    Return true if db was opened
*/
bool ShoggothDb::isOpened()
{
    return db != nullptr;
}



/*
    Execute sql code
*/
ShoggothDb* ShoggothDb::exec
(
    string aSource
)
{
    if( isOpened() )
    {
        char* err = nullptr;
        if
        (
            sqlite3_exec
            (
                db,
                aSource.c_str(),
                nullptr,
                nullptr,
                &err
            ) != SQLITE_OK
        )
        {
            getLog()
            -> warning( "Init error" )
            -> prm( "message", err );

            sqlite3_free( err );
            setResult( "db-init-error" );
        }
    }
    return this;
}



/*
    Execute sql code
*/
ShoggothDb* ShoggothDb::exec
(
    const string aSource,
    ParamList* aParamList
)
{
    getLog() -> begin( "SQL execute" );

    sqlite3_stmt* stmt = nullptr;

    if
    (
        sqlite3_prepare_v2
        (
            db,
            aSource.c_str(),
            -1,
            &stmt,
            nullptr
        ) != SQLITE_OK
    )
    {
        getLog()
        -> warning( "Prepare failed" )
        -> prm( "message", sqlite3_errmsg(db) );
        setResult( "db-write-prepare-error" );
    }
    else
    {
        int index = 1;
        aParamList -> loop
        (
            [ &index, &stmt ]
            ( void* item )
            {
                auto param = ( Param* ) item;
                switch( param -> getType() )
                {
                    case KT_UNKNOWN:
                    case KT_DATA:
                    case KT_OBJECT:
                    case KT_ARRAY:
                    case KT_NULL:
                        sqlite3_bind_null
                        (
                            stmt,
                            index
                        );
                	break;
                	case KT_BOOL:
                		sqlite3_bind_int
                		(
                		    stmt,
                		    index,
                		    param -> getBool() ? 1 : 0
            		    );
                    break;
                    case KT_STRING:
                    {
                        sqlite3_bind_text
                        (
                            stmt,
                            index,
                            param -> getString().c_str(),
                            -1,
                            SQLITE_TRANSIENT
                        );
                    }
                    break;
                    case KT_DOUBLE:
                        sqlite3_bind_double
                        (
                            stmt,
                            index,
                            param -> getDouble()
                        );
                    break;
                    case KT_INT:
                        sqlite3_bind_int64
                        (
                            stmt,
                            index,
                            param -> getInt()
                        );
                    break;
                }
                index ++;
                return false;
            }
        );

        if( sqlite3_step( stmt ) != SQLITE_DONE )
        {
            getLog()
            -> warning( "Execute failed" )
            -> prm( "message", sqlite3_errmsg(db) )
            -> prm( "sql", aSource )
            -> dump( aParamList )
            ;
            setResult( "db-write-insert-error" );
        }
        sqlite3_finalize( stmt );
    }

    getLog() -> end();

    return this;
}


/*
    Инициализация (создать таблицы)
*/
ShoggothDb* ShoggothDb::init()
{
    /* Create layer stats table on start */
    exec
    (
        R"(
            CREATE TABLE IF NOT EXISTS
            stats
            (
                netVersion TEXT,
                nerveId TEXT,
                sumWeight REAL,
                countWeights INTEGER
            );

            CREATE TABLE IF NOT EXISTS
            net_commit
            (
                netVersion TEXT,
                result TEXT,
                tick INTEGER,
                testCount INTEGER,
                parentSurvivalErrorAvg REAL,
                currentSurvivalErrorAvg REAL
            );

            CREATE TABLE IF NOT EXISTS
            net_start
            (
                netVersion TEXT,
                mutation TEXT
            );

            CREATE TABLE IF NOT EXISTS
            test
            (
                netVersion TEXT,
                tick INTEGER,
                testResult TEXT
            );
        )"
    );

    return this;
}


/******************************************************************************
    Setters and getters
*/

/*
    Return log object
*/
Log* ShoggothDb::getLog()
{
    return logManager -> getLog();
}



/******************************************************************************
    Application methods
*/


/*
    Создание сети
*/
ShoggothDb* ShoggothDb::netStart
(
    const string netVersion,
    const string mutation
)
{
    exec
    (
        R"(
            INSERT INTO
            net_start
            (
                netVersion,
                mutation
            )
            VALUES (?, ?)
        )",
        ParamList::shared().get()
        -> pushString( netVersion )
        -> pushString( mutation )
    );
    return this;
}



/*
    Коммит сети
*/
ShoggothDb* ShoggothDb::netCommit
(
    const string netVersion,
    const string result,
    const int tick,
    const int testCount,
    const real parentSurvivalErrorAvg,
    const real currentSurvivalErrorAvg
)
{
    exec
    (
        R"(
            INSERT INTO
            net_commit
            (
                netVersion,
                result,
                tick,
                testCount,
                parentSurvivalErrorAvg,
                currentSurvivalErrorAvg
            )
            VALUES (?, ?, ?, ?, ?, ?)
        )",
        ParamList::shared().get()
        -> pushString( netVersion )
        -> pushString( result )
        -> pushInt( tick )
        -> pushInt( testCount )
        -> pushDouble( parentSurvivalErrorAvg )
        -> pushDouble( currentSurvivalErrorAvg )
    );
    return this;
}




/*
    Записать статистику
*/
ShoggothDb* ShoggothDb::writeStartWeights
(
    const string netVersion,
    const string nerveId,
    real sumWeight,
    int countWeights
)
{
    exec
    (
        R"(
            INSERT INTO
            stats
            (
                netVersion,
                nerveId,
                sumWeight,
                countWeights
            )
            VALUES (?, ?, ?, ?)
        )",
        ParamList::shared().get()
        -> pushString( netVersion )
        -> pushString( nerveId )
        -> pushDouble( sumWeight )
        -> pushInt( countWeights )
    );
    return this;
}




/*
    Запись результатов тестирования
*/
ShoggothDb* ShoggothDb::testResult
(
    /* Версия сети */
    const string netVersion,
    /* Тик */
    const int tick,
    /* Строка значений слоя test */
    const string testResult
)
{
    exec
    (
        R"(
            INSERT INTO
            test
            (
                netVersion,
                tick,
                testResult
            )
            VALUES (?, ?, ?, ?)
        )",
        ParamList::shared().get()
        -> pushString( netVersion )
        -> pushInt( tick )
        -> pushString( testResult )
    );

    return this;
}

