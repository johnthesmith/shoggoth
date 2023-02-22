#include <iostream>
#include <cstring>

#include "param_list.h"
#include "param.h"
#include "param_string.h"



using namespace std;



ParamList::ParamList()
{
}



ParamList* ParamList::create()
{
    return new ParamList();
}



/*
    Add Params from argument list to this list
*/
ParamList* ParamList::add
(
    ParamList* a
)
{
    Heap::add(( Heap* ) a );

    return this;
}



/*
    Return index by Param
*/
int ParamList::indexBy
(
    Param* a
)
{
    return Heap::indexBy(( void* ) a );
}




/*
    Return index by Param
*/
Param* ParamList::getByIndex
(
    int a
)
{
    return ( Param* )Heap::getByIndex( a );
}




/*
    Set value by index
*/
ParamList* ParamList::setByIndex
(
    int aIndex,
    Param* aParam
)
{
    Heap::setByIndex(aIndex, ( void* ) aParam );
    return this;
}



/*
    Resize
*/
ParamList* ParamList::resize
(
    int a
)
{
    Heap::resize( a );
    return this;
}




/*
    Return Param index by Name
*/
int ParamList::getIndexByName
(
    string a    /* Name */
)
{
    int result = -1;
    int c = getCount();
    for( int i = 0; i < c && result == -1; i++ )
    {
        if( getByIndex( i ) -> getName() == a )
        {
            result = i;
        }
    }
    return result;
}



/*
    Set string value
*/
string ParamList::getString
(
    string aName,   /* Name of parameter */
    string aDefault /* Value */
)
{
    string r = aDefault;
    auto i = getIndexByName( aName );
    if( i >= 0 )
    {
        auto p = getByIndex( i );
        switch( p -> getType() )
        {
            case KT_UNKNOWN:
                r = aDefault;
            break;
            case KT_STRING:
                r = ((ParamString*) p) -> getValue();
            break;
            case KT_INT:
            break;
            case KT_BOOL:
            break;
            case KT_DOUBLE:
            break;
        }
    }
    return r;
}



/*
    Set string value
*/
ParamList* ParamList::setString
(
    string aName,   /* Name of parameter */
    string aValue   /* Default value */
)
{
    auto i = getIndexByName( aName );
    auto p = getByIndex( i );

    if( i < 0 )
    {
        /* Create new string param */
        p = ParamString::create();
        push( p );
    }
    else
    {
        /* Parameter exists */
        if( p -> getType() !=  KT_STRING )
        {
            /* And it is not a string */
            p -> destroy();
            p = ParamString::create();
            setByIndex( i, p );
        }
    }

    ((ParamString*) p ) -> setName( aName ) -> setValue( aValue );

    return this;
}



/*
    Set double value
*/
ParamList* ParamList::setDouble
(
    string aName,   /* Name of parameter */
    double aValue   /* Value */
)
{
    return this;
}



/*
    Set int value
*/
ParamList* ParamList::setInt
(
    string aName,           /* Name of parameter */
    long long int aValue    /* Value */
)
{
    return this;
}




/*
    Set bool value
*/
ParamList* ParamList::setBool
(
    string aName,   /* Name of parameter */
    bool aValue     /* Value */
)
{
    return this;
}
