#include <iostream>
#include <cstring>

#include "../lib/math.h"

#include "param.h"
#include "param_list.h"
#include "param_string.h"
#include "param_bool.h"
#include "param_int.h"
#include "param_double.h"



using namespace std;



ParamList::ParamList()
{
}




ParamList::~ParamList()
{
    clear();
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
    Clear
*/
ParamList* ParamList::clear()
{
    int c = getCount();
    for( int i = 0; i < c; i ++ )
    {
        auto p = getByIndex( i );
        p -> destroy();
    }
    Heap::resize( 0 );
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
                r = to_string( ((ParamInt*) p) -> getValue() );
            break;
            case KT_BOOL:
                r = (( ParamBool* ) p) -> getValue() ? "true" : "false";
            break;
            case KT_DOUBLE:
                r = to_string((( ParamDouble* ) p) -> getValue());
            break;
        }
    }
    return r;
}



/*
    Set boolean value
*/
bool ParamList::getBool
(
    string aName,   /* Name of parameter */
    bool aDefault   /* Value */
)
{
    bool r = aDefault;
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
                r = ((ParamString*) p) -> getValue() == "true";
            break;
            case KT_INT:
                r = ((ParamInt*) p) -> getValue() == 0 ? false : true;
            break;
            case KT_BOOL:
                r = ((ParamBool*) p) -> getValue();
            break;
            case KT_DOUBLE:
                r = ((ParamDouble*) p) -> getValue() == 0 ? false : true;
            break;
        }
    }
    return r;
}



/*
    Set integer value
*/
long long int ParamList::getInt
(
    string aName,   /* Name of parameter */
    long long int aDefault    /* Value */
)
{
    long long int r = aDefault;
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
                r = atoi( ((ParamString*) p) -> getValue().c_str() );
            break;
            case KT_INT:
                r = ((ParamInt*) p) -> getValue();
            break;
            case KT_BOOL:
                r = (( ParamBool* ) p) -> getValue() ? 1 : 0;
            break;
            case KT_DOUBLE:
                r = (long long int) ((ParamDouble*) p) -> getValue();
            break;
        }
    }
    return r;
}



/*
    Set double value
*/
double ParamList::getDouble
(
    string aName,   /* Name of parameter */
    double aDefault    /* Value */
)
{
    double r = aDefault;
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
                r = atoi( ((ParamString*) p) -> getValue().c_str() );
            break;
            case KT_INT:
                r = ((ParamInt*) p) -> getValue();
            break;
            case KT_BOOL:
                r = (( ParamBool* ) p) -> getValue() ? 1.0 : 0.0;
            break;
            case KT_DOUBLE:
                r = (( ParamDouble* ) p) -> getValue();
            break;
        }
    }
    return r;
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
    auto i = getIndexByName( aName );
    auto p = getByIndex( i );

    if( i < 0 )
    {
        /* Create new boolean param */
        p = ParamBool::create() -> setName( aName );
        push( p );
    }
    else
    {
        /* Parameter exists */
        if( p -> getType() !=  KT_BOOL )
        {
            /* And it is not a bool */
            p -> destroy();
            p = ParamBool::create() -> setName( aName );
            setByIndex( i, p );
        }
    }

    /* Set value */
    (( ParamBool* ) p ) -> setValue( aValue );

    return this;
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
        p = ParamString::create() -> setName( aName );
        push( p );
    }
    else
    {
        /* Parameter exists */
        if( p -> getType() !=  KT_STRING )
        {
            /* And it is not a string */
            p -> destroy();
            p = ParamString::create() -> setName( aName );
            setByIndex( i, p );
        }
    }

    /* Set value */
    ((ParamString*) p ) -> setValue( aValue );

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
    auto i = getIndexByName( aName );
    auto p = getByIndex( i );

    if( i < 0 )
    {
        /* Create new string param */
        p = ParamInt::create() -> setName( aName );
        push( p );
    }
    else
    {
        /* Parameter exists */
        if( p -> getType() !=  KT_INT )
        {
            /* And it is not a string */
            p -> destroy();
            p = ParamInt::create() -> setName( aName );
            setByIndex( i, p );
        }
    }

    /* Set value */
    ((ParamInt*) p ) -> setValue( aValue );

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
    auto i = getIndexByName( aName );
    auto p = getByIndex( i );

    if( i < 0 )
    {
        /* Create new string param */
        p = ParamDouble::create() -> setName( aName );
        push( p );
    }
    else
    {
        /* Parameter exists */
        if( p -> getType() !=  KT_DOUBLE )
        {
            /* And it is not a string */
            p -> destroy();
            p = ParamDouble::create() -> setName( aName );
            setByIndex( i, p );
        }
    }

    /* Set value */
    ((ParamDouble*) p ) -> setValue( aValue );

    return this;
}
