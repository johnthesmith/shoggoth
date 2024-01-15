#include "param_list_log.h"



using namespace std;



/*
    Dump param list to log
*/
void ParamListLog::dump
(
    Log* aLog,
    ParamList* aParamList,
    string  aSection
)
{
    dumpInternal
    (
        aLog,
        aParamList,
        aSection,
        0
    );
}




void ParamListLog::dumpInternal
(
    Log*        aLog,
    ParamList*  aParamList,
    string      aSection,
    int         depth
)
{
    int c = aParamList -> getCount();
    aLog -> begin( aSection );
    for( int i = 0; i < c; i++ )
    {
        auto p = aParamList -> getByIndex( i );

        switch( p -> getType() )
        {
            case KT_OBJECT:
                if( p -> getObject() != NULL )
                {
                    dumpInternal
                    (
                        aLog,
                        p -> getObject(),
                        p -> getName( to_string( i ) ),
                        depth + 1
                    );
                }
            break;
            case KT_DATA:
            {
                auto b  = p -> getData();
                auto s  = p -> getSize();
                aLog
                -> trace()
                -> pushColor()
                -> setColor( aLog -> colorLabel )
                -> text( p -> getNameOfType() )
                -> setColor( aLog -> colorInfo )
                -> space()
                -> text( p -> getName() )
                -> setColor( aLog -> colorLabel )
                -> text( " size " )
                -> setColor( aLog -> colorValue )
                -> value( ( long long int ) s )
                -> popColor()
                ;
            }
            break;
            default:
                aLog
                -> trace()
                -> pushColor()
                -> setColor( aLog -> colorLabel )
                -> text( p -> getNameOfType() )
                -> setColor( aLog -> colorInfo )
                -> space()
                -> text( p -> getName() )
                -> setColor( aLog -> colorLabel )
                -> text( " = " )
                -> setColor( aLog -> colorValue )
                -> value( aParamList -> getString( i ) )
                -> popColor()
                ;
            break;
        }
    };
    aLog -> end();
}
