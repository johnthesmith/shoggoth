#include "./net_config.h"
#include "../../../../lib/core/math.h"




/*
    Creator
*/
NetConfig* NetConfig::create()
{
    return new NetConfig();
}



/*
    Return layer Params by id
*/
ParamList*  NetConfig::getLayerById
(
    /* Layer id */
    string aLayerId
)
{
    return getObject( Path{ "layers", aLayerId });
}



/*
    Return layer type by layer id
*/
string NetConfig::getLayerTypeById
(
    /* Layer id */
    string aLayerId
)
{
    auto layer = getLayerById( aLayerId );
    return layer == NULL ? "" : layer -> getString( Path{ "type" });
}



/*
    Get nerve by mutation conditions
*/
ParamList* NetConfig::getNerveByMutation
(
    /* Mutation */
    ParamList* aMutation,
    /* Rnd */
    Rnd* aRnd
)
{
    /* Create menu paramlist for choise */
    vector <ParamList*> menu;

    /* Collect menu */
    nerveLoop
    (
        [ &aMutation, &menu, this ]
        ( ParamList* item )
        {
            auto typeFrom = getLayerTypeById( item -> getString( Path{ "idFrom" }));
            auto typeTo = getLayerTypeById( item -> getString( Path{ "idTo" }));
            if
            (
                aMutation -> valueExists( Path{ "parent" }, typeFrom ) &&
                aMutation -> valueExists( Path{ "child" }, typeTo )
            )
            {
                    menu.push_back( item );
            }
            return false;
        }
    );

    /* Choise from menu by random */
    return
    menu.size() > 0
    ? menu[ aRnd -> get( 0, menu.size() - 1 )]
    : NULL;
}



/*
    Create new layer between two other layers
*/
ParamList* NetConfig::createLayer
(
    /* New layer id*/
    string newLayerId,
    /* Parent layer id*/
    string parentLayreId,
    /* Child layer id */
    string childLayerId,
    /* Type pointer */
    ParamList* aType,
    /* Interpolation 0.0..1.0 */
    double aItp
)
{
    /* Get parent structure */
    auto parentLayer = getLayerById( parentLayreId );
    /* Get child structure */
    auto childLayer = getLayerById( childLayerId );
    /* Define new layer as result */
    ParamList* newLayer = NULL;

    if( parentLayer != NULL && childLayer != NULL )
    {
        /* Create new layer */
        auto newLayer = ParamList::create();
        /* Put layer in to tree */
        setObject( Path{ "layers", newLayerId }, newLayer );
        /* Set layer type */
        newLayer -> copyObject( Path{ "*" }, aType );

        /* Set size */
        setInt
        (
            Path{ "layers", newLayerId, "size", "0" },
            itpLin
            (
                parentLayer -> getInt( Path{ "size", "0" }),
                childLayer -> getInt( Path{ "size", "0" }),
                aItp
            )
        );
        setInt
        (
            Path{ "layers", newLayerId, "size", "1" },
            itpLin
            (
                parentLayer -> getInt( Path{ "size", "1" }),
                childLayer -> getInt( Path{ "size", "1" }),
                aItp
            )
        );
        setInt
        (
            Path{ "layers", newLayerId, "size", "2" },
            itpLin
            (
                parentLayer -> getInt( Path{ "size", "2" }),
                childLayer -> getInt( Path{ "size", "2" }),
                aItp
            )
        );

        /* Set position */
        setDouble
        (
            Path{ "layers", newLayerId, "position", "0" },
            itpLin
            (
                parentLayer -> getDouble( Path{ "position", "0" }),
                childLayer -> getDouble( Path{ "position", "0" }),
                aItp
            )
        );
        setDouble
        (
            Path{ "layers", newLayerId, "position", "1" },
            itpLin
            (
                parentLayer -> getDouble( Path{ "postion", "1" }),
                childLayer -> getDouble( Path{ "position", "1" }),
                aItp
            )
        );
        setDouble
        (
            Path{ "layers", newLayerId, "position", "2" },
            itpLin
            (
                parentLayer -> getDouble( Path{ "position", "2" }),
                childLayer -> getDouble( Path{ "position", "2" }),
                aItp
            )
        );
    }

    return newLayer;
}



ParamList* NetConfig::createNerve
(
    /* Parent layer id */
    string aParentLayerId,
    /* Children layer id */
    string aChildLayerId,
    /* Pattern */
    ParamList* aType
)
{
    /* Get parent structure */
    auto parentLayer = getObject( Path{ "layers", aParentLayerId });

    /* Get child structure */
    auto childLayer = getObject( Path{ "layers", aChildLayerId });

    /* Define new layer as result */
    ParamList* newNerve = NULL;

    if( parentLayer != NULL && childLayer != NULL )
    {
        /* Create newrve section */
        newNerve = ParamList::create();

        newNerve -> setString( Path{ "idFrom" }, aParentLayerId );
        newNerve -> setString( Path{ "idTo" }, aChildLayerId );
        newNerve -> copyObject( Path{ "*" }, aType );

        /* Push */
        pushObject( Path{ "nerves" }, newNerve );

    }

    return newNerve;
}



NetConfig* NetConfig::deleteNerve
(
    /* Parent layer id */
    string aParentId,
    /* Children layer id */
    string aChildId
)
{
    return this;
}




/*
    before:
        a - b - c

    after:
        a - b - d - c
*/

NetConfig* NetConfig::mutateInsertLayer
(
    /*
    Mutation object
    mutation
    {
        //  Posibility parents types
        "parent"        :[ "cortex", "retina" ],
        //  Posibility child types
        "child"         :[ "cortex", "result" ],
        // Extended parents
        "extParentsId"  :[ "bias" ],
        // Extended chhildrens layers
        "extChildrenId" :[],
        // Type of new layer
        "layerType"     :[ "types", "layers", "cortex", "const" ],
        // Type of new nerve
        "nerveType"     :[ "types", "nerves", "cortex_nerve", "const" ]
    },
    */
    ParamList*  mutation,
    Rnd*        aRnd
)
{
    /*
        Find nerve by mutation
    */
    auto targetNerve = getNerveByMutation( mutation, aRnd );

    if( targetNerve != NULL )
    {
        string layerParentId = targetNerve -> getString( Path{ "idFrom" } );
        string layerChildId = targetNerve -> getString( Path{ "idTo" } );

        /*
            создаяем новый слой как коэффициент интерполяции где 0 это парент а 1 это ребенок.
        */
        string layerCurrentId = aRnd -> getUuid();
        createLayer
        (
            layerCurrentId,
            layerParentId,
            layerChildId,
            mutation -> getObject( Path{ "layerType" }),
            0.5
        );

        /* Delete nerve */
        deleteNerve
        (
            layerParentId,
            layerChildId
        );

        /* define list of nerves link */
        vector<pair<string, string>> links;

        /* Add main layers pair */
        links.push_back( make_pair( layerParentId, layerCurrentId ));
        links.push_back( make_pair( layerCurrentId, layerChildId ));
        /* Add additional nerves links from parent to current */
        mutation -> loop
        (
            { "extParentsId" },
            [ &links, &layerCurrentId ]
            ( Param* param )
            {
                links.push_back( make_pair( param -> getString(), layerCurrentId ));
                return false;
            }
        );
        /* Add additional nerves links from current to children */
        mutation -> loop
        (
            { "extChildrenId" },
            [ &links, &layerCurrentId ]
            ( Param* param )
            {
                links.push_back( make_pair( layerCurrentId, param -> getString() ));
                return false;
            }
        );

        for( auto &link : links )
        {
            /* Create nerve from parent layer and new layer */
            createNerve
            (
                link.first,
                link.second,
                mutation -> getObject( Path{ "nerveType" })
            );
        }
    }
    return this;
}
