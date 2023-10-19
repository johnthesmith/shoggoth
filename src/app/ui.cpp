#include <iostream>

/* File read */
#include <streambuf>
#include <sstream>
#include <fstream>

/* for config update control */
#include <sys/types.h>
#include <sys/stat.h>


/* Local libraries */
#include "../graph/scene_payload.h"
#include "../graph/scene.h"
#include "../graph/rgba.h"
#include "../graph/log_points.h"
#include "../graph/draw_mode.h"
#include "../graph/chart.h"
#include "../graph/rect2d.h"

#include "../shoggoth/neuron.h"
#include "../shoggoth/func.h"

#include "../lib/rnd.h"
#include "../lib/hid.h"
#include "../lib/moment.h"
#include "../lib/utils.h"
#include "../json/json.h"

#include "../math.h"

/* User libraries */
#include "ui.h"



using namespace std;




/*
    Constructor
*/
Ui::Ui
(
    ShoggothApplication* a,
    NetGraph* aNet
)
: ScenePayload( a ) /* Call parent constructor */
{
    camera = Camera::create();
    net = aNet;

    getLog()
    -> trace( "Config source" )
    -> prm( "file", getApplication()
    -> getConfigFileName() );
}



/*
    Destructor
*/
Ui::~Ui()
{
    camera -> destroy();
}



/*
    Creator
*/
Ui* Ui::create
(
    ShoggothApplication* a,
    NetGraph* aNet
)
{
    return new Ui( a, aNet );
}



/*
    Destructor
*/
void Ui::destroy()
{
    delete this;
}



ShoggothApplication* Ui::getApplication()
{
    return (ShoggothApplication*) Payload::getApplication();
}



Ui* Ui::help()
{
    getLog()
    -> info( "----" )
    -> info( "Help" )
    -> info( "----" )
    -> info( "h - help" )
    -> info( "v - draw value of neurons" )
    -> info( "e - draw error of neurons" )
    -> info( "b - switch show/hide binds" )
    -> info( "c - switch show/hide layer cover" )
    -> info( "l - switch on/off learning mode" )
    -> info( "r - set random values for selected neurons" )
    -> lineEnd()
    ;
    return this;
}



/******************************************************************************
    Events
*/



/*
    Activate payload
*/
void Ui::onActivate
(
    Scene& aScene   /* Scene object */
)
{
    aScene.setFar( 1000.0 );
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_TEXTURE_2D );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
}



/*
    Main calc method
*/
void Ui::onCalc
(
    Scene* aScene   /* Scene object */
)
{
    /* Read config */
    auto nowMoment = now();
    if( lastConfigCheck + MILLISECOND * 1000 < nowMoment )
    {
        net -> readNet();
        /* Set last moment */
        lastConfigCheck = nowMoment;
    }
}



/*
    Main draw method
*/
void Ui::onDraw
(
    Scene* aScene   /* Scene object */
)
{
    aScene -> clearColor();


    applyScreenToScene( aScene );

    net -> drawSelectedNeurons( aScene );

    applyCameraToScene( camera, aScene );

    /* Draw Layers */
    net
    -> drawLayers( aScene, camera -> getChanged() )
    -> drawNerves( aScene );

    /* Reset camera changed */
    camera -> setChanged( false );

    /*
        Switch to flat screen
    */
    applyScreenToScene( aScene );


    /* Draw Learning mode */
    net -> drawLearningMode( aScene );

    aScene -> drawAxisIdentity();

    /* Draw view matrix */
    aScene
    -> color( interfaceColor )
    -> setTextSize( 20 )
    -> drawMatrix( Point3d( 0, aScene -> getViewport().height, 0 ) );

    /*
        Draw neuron chart
    */
    Neuron* neuron = net -> getSelectedFirst();
    if( neuron != NULL )
    {

        aScene
        -> color( interfaceColor )
        -> setLineWidth( 3 )
        -> begin( LOOP )
        -> sendRect
        (
            Rect2d().setCenterSize
            (
                neuron -> getScreen(),
                Point2d( 10.0, 10.0 )
            )
        )
        -> end();

        net -> drawNeuronChart( aScene, net -> getSelectedNeurons() );
    }


    net -> drawSelectedNeurons( aScene );

    if( selectTopLeft != selectBottomRight )
    {
        /* Draw selection rect */
        aScene
        -> color( interfaceColorDark )
        -> setLineWidth( 2 )
        -> begin( QUAD )
        -> sendRect( selectTopLeft, selectBottomRight )
        -> end()
        -> color( interfaceColor )
        -> setLineWidth( 1 )
        -> begin( LOOP )
        -> sendRect( selectTopLeft, selectBottomRight )
        -> end()
        ;

        /* Draw selected neurons */
        NeuronList* list = NeuronList::create();
        net -> getNeuronsByScreenRect( list, selectTopLeft, selectBottomRight );

        aScene
        -> color( interfaceColor )
        -> setPointSize( 4 )
        -> begin( POINT );
        list -> loop
        (
            [ &aScene ]( Neuron* neuron ) -> bool
            {
                aScene -> vertex( neuron -> getScreen() );
                return false;
            }
        );
        aScene -> end();

        list -> destroy();
    }

    /* Draw screen cursor radius */
    aScene
    -> color( interfaceColor )
    -> setLineWidth( 2 )
    -> begin( LOOP )
    -> sendRect
    (
        Rect2d().setCenterSize
        (
            aScene -> getMouseCurrentScreen(),
            Point2d
            (
                net -> getCursorRadius(),
                net -> getCursorRadius()
            )
        )
    )
    -> end();
}



/*
    Keyboard up event
*/
void Ui::onKeyUp
(
    Scene&      aScene, /* Scene object */
    const int   aKey,   /* Key */
    const int   aCode,  /* Scan code*/
    const int   aMode   /* Mode */
)
{
    switch( aKey )
    {
        case KEY_ESCAPE:
            aScene.setTerminate( true );
        break;
        case KEY_LEFT_CONTROL:
            camera -> setEyeLock( false );
        break;
        case KEY_LEFT_SHIFT:
            camera -> setTargetLock( false );
        break;
    }
}




/*
    Keyboard up event
*/
void Ui::onKeyDown
(
    Scene&      aScene, /* Scene object */
    const int   aKey,   /* Key */
    const int   aCode,  /* Scan code*/
    const int   aMode   /* Mode */
)
{
    switch( aKey )
    {
        case KEY_ESCAPE:
            aScene . closeWindow();
        break;
        case KEY_1:
        case KEY_2:
        case KEY_3:
        case KEY_4:
        case KEY_5:
        case KEY_6:
        case KEY_7:
        case KEY_8:
        case KEY_9:
//            fillScreen( aKey );
        break;

        case KEY_LEFT_BRACKET:
        {
            auto index = net -> getLayers() -> getIndexById( selectedLayerId );
            index = ( index <= 0 ? net -> getLayers() -> getCount() : index ) - 1;
            auto layer = net -> getLayers() -> getByIndex( index );

            if( layer != NULL )
            {
                camera
                -> setTop( POINT_3D_Y )
                -> setEye( layer -> getEye() + POINT_3D_I * 2 )
                -> setTarget( layer -> getEye() );
                selectedLayerId = layer -> getId();
            }
        }
        break;

        case KEY_RIGHT_BRACKET:
        {
            auto index = net -> getLayers() -> getIndexById( selectedLayerId );
            index = index >= net -> getLayers() -> getCount() - 1 ? 0 : ( index + 1 );
            auto layer = net -> getLayers() -> getByIndex( index );

            if( layer != NULL )
            {
                camera
                -> setTop( POINT_3D_Y )
                -> setEye( layer -> getEye() + POINT_3D_I * 2 )
                -> setTarget( layer -> getEye() );
                selectedLayerId = layer -> getId();
            }
        }
        break;

        case KEY_H:
            help();
        break;

        case KEY_R:
            net -> getSelectedNeurons() -> loop
            (
                []( Neuron* neuron )
                {
                    neuron -> setValue( Rnd::get( 0.0, 1.0 ) );
                    return false;
                }
            );
        break;
        case KEY_V:
            net -> setNeuronDrawMode( NDM_VALUE );
        break;
        case KEY_E:
            net -> setNeuronDrawMode( NDM_ERROR );
        break;
        case KEY_B:
            net -> switchShowBinds();
        break;
        case KEY_C:
            net -> switchShowLayer();
        break;
        case KEY_L:
            net -> switchLearningMode();
        break;
        case KEY_LEFT_CONTROL:
            camera -> setEyeLock( true );
        break;
        case KEY_LEFT_SHIFT:
            camera -> setTargetLock( true );
        break;
    }
}




void Ui::onMouseMove
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
    if
    (
        !aScene.isMouseLeftDrag() &&
        aScene.isMouseButton( MB_LEFT )
    )
    {
        if( aScene.isKey( KEY_LEFT_CONTROL ))
        {
            net -> addSelectedByCursor( &aScene );
        }

        if( aScene.isKey( KEY_LEFT_ALT ))
        {
            net -> removeSelectedByCursor( &aScene );
        }
    }
}



/*
    Mouse left drag begin
*/
bool Ui::onLeftDragBegin
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
//    if( aScene.isKey( KEY_LEFT_SHIFT ))
//    {
//        selectTopLeft = aScene.getMouseCurrentScreen();
//        selectBottomRight = selectTopLeft;
//    }
    return false;
}



/*
    Mouse left drag begin
*/
void Ui::onLeftDragEnd
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
//    if( aScene.isKey( KEY_LEFT_SHIFT ))
//    {
//        net -> selectNeuronsByRect( selectTopLeft, selectBottomRight );
//        selectTopLeft = POINT_3D_0;
//        selectBottomRight = POINT_3D_0;
//    }
}



/*
    Mouse left drag
*/
void Ui::onLeftDrag
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
    selectBottomRight = aScene.getMouseCurrentScreen();
}



/*
    Mouse rigth drag
*/
void Ui::onRightDrag
(
    Scene& aScene,      /* Scene object */
    const Point3d& aPoint
)
{
    double k = camera -> getGaze().magn() / aScene.getNear();
    Point3d p =
    (
        aScene.getMouseLastWorld() - aScene.getMouseCurrentWorld()
    ) * k;
    camera -> shift( p );
}



/*
    Mouse wheel event
*/
void Ui::onMouseWheel
(
    Scene& aScene,          /* Scene object */
    const Point3d& aDelta
)
{
    auto operation = false;

    if( aScene.isKey( KEY_W ))
    {
        int total = 0;
        double sum = 0;

        net -> nerveWeightLoop
        (
            net -> getSelectedNeurons(),
            [
                &total, &sum
            ]
            (
                int aWeightIndex,
                Neuron* aNeuronFrom,
                Neuron* aNeuronTo,
                Nerve* aNerve
            )
            {
                total ++;
                sum += aNerve -> getWeight( aWeightIndex );
                return false;
            }
        );

        if( total != 0 )
        {
            auto avgWeight = (double)sum / total;

            net -> nerveWeightLoop
            (
                net -> getSelectedNeurons(),
                [
                    &avgWeight, &aDelta
                ]
                (
                    int aWeightIndex,
                    Neuron* aNeuronFrom,
                    Neuron* aNeuronTo,
                    Nerve* aNerve
                )
                {
                    aNerve -> setWeight( aWeightIndex, avgWeight + aDelta.y * 0.1 );
                    return false;
                }
            );
        }

        operation = true;
    }


    if( aScene.isKey( KEY_P ))
    {
        net -> getSelectedNeurons() -> loop
        (
            []
            ( Neuron* aNeuron )
            {
                aNeuron -> getLayer() -> childrenLoop
                (
                    aNeuron -> getIndex(),
                    [
                        &aNeuron
                    ]
                    (
                        Layer* aParentLayer,
                        int aParentIndex,
                        Nerve* aNerve,
                        double aWeight,
                        int aIndexWeight
                    )
                    {
                        aParentLayer -> setNeuronValue( aParentIndex, aNeuron -> getValue() );
                        aNerve -> setWeight( aIndexWeight, 1 );
                        return false;
                    }
                );
                return false;
            }
        );
        operation = true;
    }


    if( aScene.isKey( KEY_S ))
    {
        net -> setCursorRadius
        (
            net -> getCursorRadius() * ( aDelta.y > 0.0 ? 1.1 : 0.9 )
        );
        operation = true;
    }

    if( aScene.isKey( KEY_V ))
    {
        if( net -> getSelectedNeurons() -> getCount() > 0 )
        {
            auto avgValue = net -> getSelectedNeurons() -> calcAvgValue();
            net -> getSelectedNeurons() -> loop
            (
                [ &aScene, &aDelta, &avgValue ]
                ( Neuron* aNeuron )
                {
                    /* Neuron value Control*/
                    aNeuron -> setValue
                    (
                        avgValue + aDelta.y *
                        ( aScene.isKey( KEY_LEFT_SHIFT ) ? 0.01 : 0.1 )
                    );
                    return false;
                }
            );
        }
        operation = true;
    }

    if( aScene.isKey( KEY_E ))
    {
        if( net -> getSelectedNeurons() -> getCount() > 0 )
        {
            auto avgError = net -> getSelectedNeurons() -> calcAvgError();
            net -> getSelectedNeurons() -> loop
            (
                [ &aScene, &aDelta, &avgError ]
                ( Neuron* aNeuron )
                {
                    /* Neuron value Control*/
                    aNeuron -> setError
                    (
                        avgError + aDelta.y *
                        ( aScene.isKey( KEY_LEFT_SHIFT ) ? 0.01 : 0.1 )
                    );
                    return false;
                }
            );
        }
        operation = true;
    }

    if( !operation )
    {
        /* Camera control*/
        bool rotation = false;

        if( aScene.isKey( KEY_LEFT_CONTROL ))
        {
            camera -> rotateEye
            (
                camera -> getRight(),
                aDelta.y * 0.1
            );
            rotation = true;
        }

        if( aScene.isKey( KEY_LEFT_SHIFT ))
        {
            camera -> rotateEye( camera -> getTop(), aDelta.y * 0.1 );
            rotation = true;
        }

        if( aScene.isKey( KEY_LEFT_ALT ))
        {
            camera -> rotateTop( camera -> getFront(), aDelta.y * 0.1 );
            rotation = true;
        }

        if( !rotation )
        {
            camera -> zoom( aDelta.y > 0 ? 0.9 : 1.1 );
        }
    }
}



/*
    On mouse left click event
*/
void Ui::onLeftClick
(
    Scene& aScene,          /* Scene object */
    const Point3d& aMouse,  /* Mouse position */
    const int aMode         /* Key mode */
)
{
}



/*
    On mouse left click event
*/
void Ui::onLeftDblClick
(
    Scene& aScene,          /* Scene object */
    const Point3d& aMouse,  /* Mouse position */
    const int aMode         /* Key mode */
)
{
    if
    (
        net
        -> setSelected( &aScene )
        -> getSelectedFirst() != NULL
    )
    {
        camera -> moveTarget
        (
            net
            -> getSelectedFirst()
            -> getWorld()
        );
    }
}
