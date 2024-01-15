/*
    List of Params
*/

#pragma once

#include <vector>
#include <functional>   /* for lyambda */

#include "../lib/heap.h"
#include "param.h"



using namespace std;



typedef vector <string> Path;



struct ParamList : public Heap
{
    private:

        ParamList* parent = NULL;

    public:


        ParamList();



        ~ParamList();



        static ParamList* create();



        void destroy() override;


        /*
            Add Params from argument list to this list
        */
        ParamList* add
        (
            ParamList*
        );




        /*
            Return index by Param
        */
        int indexBy
        (
            Param*
        );




        /*
            Return paramer or null by name
        */
        Param* getByName
        (
            string
        );



        /*
            Return parameter by list of name path
        */
        Param* getByName
        (
            Path /* Names of parameter */
        );



        /*
            Return Param by index
        */
        Param* getByIndex
        (
            int
        );



        /*
            Return Param by index
        */
        ParamList* setByIndex
        (
            int,
            Param*
        );



        /*
            Resize
        */
        ParamList* resize
        (
            int
        );




        /*
            Delete Param by index
        */
        ParamList* deleteParam
        (
            int /* Index of the Param */
        );



        /*
            Clear list
            All parameters will be destroy and list will be resize
        */
        ParamList* clear();



        /*
            Return Param by name
        */
        int getIndexByName
        (
            string  /* Name of parameter */
        );



        string getString
        (
            int,            /* Index of parameter */
            string  = ""    /* Dafault value */
        );



        string getString
        (
            string,         /* Name of parameter */
            string  = ""    /* Dafault value */
        );



        ParamList* loadString
        (
            string,         /* Name of parameter */
            string&,        /* Result */
            string  = ""    /* Dafault value */
        );



        string getString
        (
            Path,/* Name of parameter */
            string  = ""    /* Dafault value */
        );




        bool getBool
        (
            int,            /* Index of parameter */
            bool = false    /* Dafault value */
        );



        bool getBool
        (
            string,         /* Name of parameter */
            bool = false    /* Dafault value */
        );



        /*
            Get boolean value by path
        */
        bool getBool
        (
            Path,           /* Name path of parameter */
            bool = false    /* default value */
        );



        /*
            Load bool value from param to var
        */
        ParamList* loadBool
        (
            string,         /* Name of parameter */
            bool&,          /* Place for result */
            bool = false    /* Default value */
        );



        /*
            Load bool value from param to var
        */
        ParamList* loadBool
        (
            Path,           /* Path of parameters */
            bool&,          /* Place for result */
            bool = false    /* Default value */
        );



        long long int getInt
        (
            int,                /* Name of parameter */
            long long int = 0   /* Dafault value */
        );



        long long int getInt
        (
            string,             /* Name of parameter */
            long long int = 0   /* Dafault value */
        );



        /*
            Get integer value by path
        */
        long long int getInt
        (
            Path,    /* Name of parameter */
            long long int = 0   /* default value */
        );



        ParamList* loadInt
        (
            string,             /* Name of parameter */
            int&,               /* Result */
            int = 0             /* Dafault value */
        );



        ParamList* loadInt
        (
            string,             /* Name of parameter */
            unsigned int&,      /* Result */
            unsigned int = 0    /* Dafault value */
        );



        ParamList* loadInt
        (
            string,             /* Name of parameter */
            long long int&,
            long long int = 0   /* Dafault value */
        );



        double getDouble
        (
            int,            /* Name of parameter */
            double = 0.0    /* Dafault value */
        );



        double getDouble
        (
            string,         /* Name of parameter */
            double = 0.0    /* Dafault value */
        );



        /*
            Get double value by path
        */
        double getDouble
        (
            Path,  /* Name of parameter */
            double = 0        /* default value */
        );





        ParamList* getData
        (
            int,     /* Name of parameter */
            char*&,
            size_t&
        );



        ParamList* getData
        (
            string,         /* Name of parameter */
            char*&,
            size_t&
        );



        /*
            Get double value by path
        */
        ParamList* getData
        (
            Path,  /* Name of parameter */
            char*&,
            size_t&
        );



        /*
            Get object value by name
        */
        ParamList* getObject
        (
            string,             /* Name of parameter */
            ParamList* = NULL   /* Value */
        );



        /*
            Get object value by index
        */
        ParamList* getObject
        (
            int aIndex,         /* Name of parameter */
            ParamList* = NULL   /* Value */
        );



        /*
            Get object value by path
        */
        ParamList* getObject
        (
            Path,               /* Name of parameter */
            ParamList* = NULL   /* default value */
        );


        /*
            Get object value by path and attribute value
        */
        ParamList* getObject
        (
            Path,               /* Path for list of keys */
            string,             /* Key attribute name */
            string              /* Value for key attribute */
        );



        /*
            Return object by path or root prm list
        */
        ParamList* selectObject
        (
            Path
        );



        /*
            Copy object to key
        */
        ParamList* copyFrom
        (
            string,       /* Parameter name */
            ParamList*    /* Source parameters */
        );



        /*
            Push string value
        */
        ParamList* pushString
        (
            string = ""     /* Value */
        );


        /*
            Push bool value
        */
        ParamList* pushBool
        (
            bool = false     /* Value */
        );


        /*
            Push integer value
        */
        ParamList* pushInt
        (
            long long int = 0     /* Value */
        );


        /*
            Push double value
        */
        ParamList* pushDouble
        (
            double = 0.0     /* Value */
        );



        /*
            Push object value
        */
        ParamList* pushObject
        (
            ParamList*    /* Value */
        );



        /*
            Set string value
        */
        ParamList* setValue
        (
            string,         /* Name of parameter */
            string = ""     /* Value */
        );



        /*
            Set string value
        */
        ParamList* setString
        (
            string,         /* Name of parameter */
            string = ""     /* Value */
        );




        /*
            Set bool value
        */
        ParamList* setBool
        (
            string,         /* Name of parameter */
            bool = false    /* Value */
        );



        /*
            Set int value
        */
        ParamList* setInt
        (
            string,             /* Name of parameter */
            long long int = 0   /* Value */
        );



        /*
            Set int value
        */
        ParamList* setInt
        (
            int,                /* Index of parameter */
            long long int = 0   /* Value */
        );



        /*
            Set double value
        */
        ParamList* setDouble
        (
            string,         /* Name of parameter */
            double = 0.0    /* Value */
        );



        /*
            Set double value
        */
        ParamList* setObject
        (
            string,         /* Name of parameter */
            ParamList*      /* Value */
        );



        /*
            Set double value
        */
        ParamList* setObject
        (
            int,            /* Index of parameter */
            ParamList*      /* Value */
        );



        /*
            Set data buffer value
        */
        ParamList* setData
        (
            string, /* Name of parameter */
            char*,  /* Value */
            size_t  /* Size of value */
        );



        /*
            Set data buffer value
        */
        ParamList* setData
        (
            int,    /* Name of parameter */
            char*,  /* Value */
            size_t  /* Size of value */
        );



        ParamList* dump();



        ParamList* toStringInternal
        (
            stringstream&,
            int = 0 /* depth */
        );



        string toString();



        /*
            Loop with lyambda
        */
        ParamList* loop
        (
            function <bool ( Param* )>
        );




        ParamList* setParam
        (
            Param*
        );



        /*
            Copy param list from other param list
        */
        ParamList* copyTo
        (
            ParamList*
        );



        /*
            Copy param list from other param list
        */
        ParamList* copyFrom
        (
            ParamList*
        );



        bool contains
        (
            string
        );



        ParamList* toBuffer
        (
            void*&,    /* buffer */
            size_t&    /* size of buffer */
        );



        ParamList* fromBuffer
        (
            void*&,         /* buffer */
            const size_t    /* size of buffer */
        );



        ParamList* calcSize
        (
            size_t&
        );



        ParamList* fillBuffer
        (
            void*&, /* Buffer */
            size_t& /* Position */
        );



        ParamList* fillElementInBuffer
        (
            void*&,      /* Buffer */
            size_t&,     /* Position in buffer */
            Param*      aParam,
            void*,       /* Source value pointer */
            size_t       /* Size of source */
        );


        /*
            Create path from vector
        */
        ParamList* setPath
        (
            Path /* Names of parameter */
        );



        ParamList* setParent
        (
            ParamList*  /* Parent for this object */
        );



        ParamList* getParent();



        ParamList* getRoot();


        /*
            Return random param if exists
        */
        Param* getRnd();



        /*
            Return intersections with argiments
        */
        ParamList* intersect
        (
            ParamList*, /* Argument */
            ParamList*  /* Result */
        );



        /*
            Return true if intersections with argiment exists
        */
        bool isIntersect
        (
            ParamList* /* Argument */
        );



        /*
            Fill files from path to the param list
        */
        ParamList* filesFromPath
        (
            string
        );
};

