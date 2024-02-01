    /*
    Dimension Layer

    it's 3d sized.

    This class must be extended other classes.
    Don't use it directly.
*/


#pragma once

#include "layer.h"
#include "../../../../lib/json/param_list.h"    /* For read size from params */
#include "../../../../lib/graph/point3i.h"      /* Size of layer */



using namespace std;


class LayerDim: public Layer
{
    private:

        /*
            States
        */

        /* Dimention size */
        Point3i size    = POINT_3I_0;

    public:



        /*
            Constructor
        */
        LayerDim
        (
            Limb*,          /* Limb object */
            string = ""     /* id */
        );



        /*
            Set dimentions size
        */
        LayerDim* setSize
        (
            const Point3i& = POINT_3I_0
        );



        /*
            Set dimentions size from params
        */
        LayerDim* setSize
        (
            ParamList*
        );



        /*
            Return 3d layer size at neurons for each axis
        */
        Point3i getSize();



        /*
            Return index by point 3i at layer
        */
        int indexByPos
        (
            const Point3i&
        );

};
