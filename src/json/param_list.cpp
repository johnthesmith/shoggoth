#include <iostream>
#include <cstring>

#include "../lib/math.h"


#include "param.h"
#include "param_list.h"
#include "param_string.h"
#include "param_bool.h"
#include "param_int.h"
#include "param_double.h"
#include "param_object.h"



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
    Return paramer by name
*/
Param* ParamList::getByName
(
    string a
)
{
    return ( Param* )getByIndex( getIndexByName( a ));
}



/*
    Return parameter by list of name path
*/
Param* ParamList::getByName
(
    vector <string> aName   /* Names of parameter */
)
{
    Param* result = NULL;
    Param* param = NULL;
    int c = aName.size();
    int i = 0;

    if( c > 0 )
    {
        do
        {
            ParamList* iParamList = param == NULL ? this : param -> getObject();
            param = iParamList -> getByName( aName[ i ] );
            if( i == c - 1 )
            {
                result = param;
            }
            i++;
        } while ( i < c && param != NULL && param -> getType() == KT_OBJECT );
    }
    return result;
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


/******************************************************************************
    get
*/


/*
    Get string value
*/
string ParamList::getString
(
    string aName,   /* Name of parameter */
    string aDefault /* Value */
)
{
    return getString( getIndexByName( aName ), aDefault );
}




/*
    Get string value
*/
string ParamList::getString
(
    int aIndex,   /* Name of parameter */
    string aDefault /* Value */
)
{
    auto p = getByIndex( aIndex );
    return
    p == NULL || p -> getType() == KT_UNKNOWN
    ? aDefault
    : p -> getString();
}



/*
    Get string value by full path
*/
string ParamList::getString
(
    vector <string> aName,  /* Name of parameter */
    string aDefault         /* default value */
)
{
    Param* result = getByName( aName );
    return result == NULL ? aDefault : result -> getString();
}



/*
    get boolean value by index
*/
bool ParamList::getBool
(
    int aIndex,     /* Name of parameter */
    bool aDefault   /* Value */
)
{
    auto p = getByIndex( aIndex );
    return
    p == NULL || p -> getType() == KT_UNKNOWN
    ? aDefault
    : p -> getBool();
}



/*
    Set boolean value by name
*/
bool ParamList::getBool
(
    string aName,   /* Name of parameter */
    bool aDefault   /* Value */
)
{
    return getBool( getIndexByName( aName ), aDefault );
}




/*
    Get boolean value by path
*/
bool ParamList::getBool
(
    vector <string> aName,  /* Name of parameter */
    bool aDefault           /* default value */
)
{
    Param* result = getByName( aName );
    return result == NULL ? aDefault : result -> getBool();
}




/*
    Get integer value by name
*/
long long int ParamList::getInt
(
    string aName,   /* Name of parameter */
    long long int aDefault    /* Value */
)
{
    return getInt( getIndexByName( aName ), aDefault );
}




/*
    get integer value by index
*/
long long int ParamList::getInt
(
    int aIndex,             /* Index of parameter */
    long long int aDefault  /* Value */
)
{
    auto p = getByIndex( aIndex );
    return
    p == NULL || p -> getType() == KT_UNKNOWN
    ? aDefault
    : p -> getInt();
}



/*
    Get integer value by path
*/
long long int ParamList::getInt
(
    vector <string> aName,  /* Name of parameter */
    long long int aDefault  /* default value */
)
{
    Param* result = getByName( aName );
    return result == NULL ? aDefault : result -> getInt();
}





/*
    Get double value by name
*/
double ParamList::getDouble
(
    string aName,   /* Name of parameter */
    double aDefault    /* Value */
)
{
    return getDouble( getIndexByName( aName ), aDefault );
}



/*
    Get double value by index
*/
double ParamList::getDouble
(
    int aIndex,         /* Name of parameter */
    double aDefault     /* Value */
)
{
    auto p = getByIndex( aIndex );
    return
    p == NULL || p -> getType() == KT_UNKNOWN
    ? aDefault
    : p -> getDouble();
}



/*
    Get double value by path
*/
double ParamList::getDouble
(
    vector <string> aName,  /* Name of parameter */
    double aDefault         /* default value */
)
{
    Param* result = getByName( aName );
    return result == NULL ? aDefault : result -> getDouble();
}



/*
    Get object value by name
*/
ParamList* ParamList::getObject
(
    string aName,   /* Name of parameter */
    ParamList* aDefault    /* Value */
)
{
    return getObject( getIndexByName( aName ), aDefault );
}



/*
    Get object value by index
*/
ParamList* ParamList::getObject
(
    int aIndex,             /* Name of parameter */
    ParamList* aDefault     /* Value */
)
{
    auto p = getByIndex( aIndex );
    return
    p == NULL || p -> getType() == KT_UNKNOWN
    ? aDefault
    : p -> getObject();
}



/*
    Get object value by path
*/
ParamList* ParamList::getObject
(
    vector <string> aName,  /* Name of parameter */
    ParamList* aDefault         /* default value */
)
{
    Param* result = getByName( aName );
    return result == NULL ? aDefault : result -> getObject();
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
    Param* p = NULL;

    if( i < 0 )
    {
        /* Create new boolean param */
        p = ParamBool::create() -> setName( aName );
        push( p );
    }
    else
    {
        p = getByIndex( i );
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
    Push string value
*/
ParamList* ParamList::pushString
(
    string aValue   /* Default value */
)
{
    /* Create new string param */
    push( ParamString::create() -> setValue( aValue ));
    return this;
}



/*
    Push string value
*/
ParamList* ParamList::pushInt
(
    long long int aValue   /* Default value */
)
{
    /* Create new string param */
    push( ParamInt::create() -> setValue( aValue ));
    return this;
}



/*
    Push bool value
*/
ParamList* ParamList::pushBool
(
    bool aValue   /* Default value */
)
{
    /* Create new string param */
    push( ParamBool::create() -> setValue( aValue ));
    return this;
}



/*
    Push double value
*/
ParamList* ParamList::pushDouble
(
    double aValue   /* Default value */
)
{
    /* Create new string param */
    push( ParamDouble::create() -> setValue( aValue ));
    return this;
}



/*
    Push object value
*/
ParamList* ParamList::pushObject
(
    ParamList* aValue   /* Default value */
)
{
    /* Create new string param */
    push( ParamObject::create() -> setValue( aValue ));
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
    Param* p = NULL;
    if( i < 0 )
    {
        /* Create new string param */
        p = ParamString::create() -> setName( aName );
        push( p );
    }
    else
    {
        p = getByIndex( i );
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
    Param* p = NULL;

    if( i < 0 )
    {
        /* Create new string param */
        p = ParamInt::create() -> setName( aName );
        push( p );
    }
    else
    {
        p = getByIndex( i );
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
    Param* p = NULL;

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
            p = getByIndex( i );
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



/*
    Set param list
*/
ParamList* ParamList::setObject
(
    string aName,   /* Name of parameter */
    ParamList* aValue   /* Value */
)
{
    auto i = getIndexByName( aName );
    Param* p = NULL;

    if( i < 0 )
    {
        /* Create new object */
        p = ParamObject::create() -> setName( aName );
        push( p );
    }
    else
    {
        /* Parameter exists */
        p = getByIndex( i );
        /* And it is not a string */
        p -> destroy();
        p = ParamObject::create() -> setName( aName );
        setByIndex( i, p );
    }

    /* Set value */
    ((ParamObject*) p ) -> setValue( aValue );

    return this;
}




ParamList* ParamList::dump
(
    int depth
)
{
    int c = getCount();
    for( int i = 0; i < c; i++ )
    {
        auto p = getByIndex( i );
        string tab = "";
        tab.resize( 4 * depth, ' ' );
        cout << tab;
        switch( p -> getType() )
        {
            case KT_OBJECT:
                cout
                << "["
                << p -> getNameOfType()
                << "] "
                << p -> getName()
                << "\n";

                ((ParamObject*) p) -> getValue() -> dump( depth + 1 );
            break;
            default:
                cout
                << "["
                << p -> getNameOfType()
                << "] "
                << p -> getName()
                << "="
                << getString( i )
                << "\n";
            break;
        }
    };
    return this;
}



/*
    Loop with lyambda
*/
ParamList* ParamList::loop
(
    function <bool ( Param* )> callback
)
{
    bool stop = false;
    for( int i = 0; i < count && !stop; i++ )
    {
        stop = callback(( Param*) items[ i ] );
    }
    return this;
}
