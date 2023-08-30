#include "param_list_file.h"
#include "json.h"

using namespace std;



ParamListFile* ParamListFile::create()
{
    return new ParamListFile();
}



void ParamListFile::destroy()
{
    delete this;
}



/*
    Load structure from json file
*/
ParamListFile* ParamListFile::fromJsonFile
(
    string aFileName
)
{
    if( isOk() )
    {
        Json::create()
        -> fromFile( aFileName )
        -> copyTo( this )
        -> resultTo( this )
        -> destroy();
    }
    return this;
}



ParamListFile* ParamListFile::clear()
{
    ParamList::clear();
    return this;
}
