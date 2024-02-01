#include <iostream>

/* File read */
#include <streambuf>
#include <sstream>
#include <fstream>

/* for config update control */
#include <sys/types.h>
#include <sys/stat.h>


/* Local libraries */
#include "../math.h"

#include "../../../../lib/graph/scene_payload.h"
#include "../../../../lib/graph/scene.h"
#include "../../../../lib/graph/rgba.h"
#include "../../../../lib/graph/log_points.h"
#include "../../../../lib/graph/draw_mode.h"
#include "../../../../lib/graph/chart.h"
#include "../../../../lib/graph/rect2d.h"
#include "../../../../lib/core/rnd.h"
#include "../../../../lib/core/hid.h"
#include "../../../../lib/core/moment.h"
#include "../../../../lib/core/utils.h"
#include "../../../../lib/json/json.h"

#include "../shoggoth/limb/layer_ui.h"
#include "../shoggoth/limb/neuron_ui.h"
#include "../shoggoth/limb/neuron_list_ui.h"
#include "../shoggoth/func.h"

/* User libraries */
#include "ui.h"



using namespace std;




/*
    Constructor
*/
Ui::Ui
(
    Net* aNet
)
: ScenePayload( aNet -> getApplication() ) /* Call parent constructor */
{
    limb = LimbUi::create( aNet );
    camera = Camera::create();
    setScene( Scene::create( getLog() ));

    getScene()
    -> getFont()
    -> setFontName
    (
        getApplication()
        -> getConfig()
        -> getString( Path{ "tasks", taskToString( TASK_UI ), "fontName" } )
    )
    -> setGliphSize
    (
        getApplication()
        -> getConfig()
        -> getInt( Path{ "tasks", taskToString( TASK_UI ), "gliphSize" }, 16 )
    )
    -> setCharSet
    (
        getApplication()
        -> getConfig()
        -> getString( Path{ "tasks", taskToString( TASK_UI ), "charSet" })
    );

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
    getScene() -> finit() -> destroy();
    camera -> destroy();
    limb -> destroy();
}



/*
    Creator
*/
Ui* Ui::create
(
    Net* aNet
)
{
    return new Ui( aNet );
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
    /* Prepare Limb */
    limb -> getNet()
    -> syncToLimb( limb )
    -> swapValuesAndErrors
    (
        Actions{ READ_VALUES }, /* Action */
        TASK_UI,                /* Role */
        limb                    /* Participant object */
    );
}


/*
    Main draw method
*/
void Ui::onDraw
(
    Scene* aScene   /* Scene object */
)
{
    /* Switch scene to the flat view */
    aScene -> clearColor();
    applyScreenToScene( aScene );

    limb -> drawSelectedNeurons( aScene );


    /* Switch scene to 3d view */
    applyCameraToScene( camera, aScene );


    /* Draw Layers */
    limb
    -> drawLayers( aScene, camera -> getChanged() )
//    -> drawNerves( aScene )
    ;

    /* Reset camera changed */
    camera -> setChanged( false );

    /*
        Switch to flat screen
    */
    applyScreenToScene( aScene );

    aScene -> drawAxisIdentity();

    /* Draw view matrix */
    aScene
    -> color( interfaceColor )
    -> setTextSize( 20 )
    -> drawMatrix( Point3d( 0, aScene -> getViewport().height, 0 ) );

    /*
        Draw neuron chart
    */
    auto neuron = limb -> getSelectedFirst();
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

        limb -> drawNeuronChart( aScene, limb -> getSelectedNeurons() );
    }

    limb -> drawSelectedNeurons( aScene );

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
        auto list = NeuronListUi::create();
        limb -> getNeuronsByScreenRect( list, selectTopLeft, selectBottomRight );

        aScene
        -> color( interfaceColor )
        -> setPointSize( 4 )
        -> begin( POINT );
        list -> loop
        (
            [ &aScene ]
            ( NeuronUi* neuron ) -> bool
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
                limb -> getCursorRadius(),
                limb -> getCursorRadius()
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
        case KEY_T:
 //           limb -> setCalcTick();
        break;
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
            auto index = limb -> getLayerList() -> getIndexById( selectedLayerId );
            index = ( index <= 0 ? limb -> getLayerList() -> getCount() : index ) - 1;
            auto layer = ( LayerUi* ) (limb -> getLayerList() -> getByIndex( index ));

            if( layer != NULL )
            {
                camera
                -> setTop( POINT_3D_Y )
                -> setEye( layer -> getObject() -> getEye() + POINT_3D_I * 2 )
                -> setTarget( layer -> getObject() -> getEye() );
                selectedLayerId = layer -> getId();
            }
        }
        break;

        case KEY_RIGHT_BRACKET:
        {
            auto index = limb -> getLayerList() -> getIndexById( selectedLayerId );
            index = index >= limb -> getLayerList() -> getCount() - 1 ? 0 : ( index + 1 );
            auto layer = (LayerUi*) ( limb -> getLayerList() -> getByIndex( index ));

            if( layer != NULL )
            {
                camera
                -> setTop( POINT_3D_Y )
                -> setEye( layer -> getObject() -> getEye() + POINT_3D_I * 2 )
                -> setTarget( layer -> getObject() -> getEye() );
                selectedLayerId = layer -> getId();
            }
        }
        break;

        case KEY_H:
            help();
        break;

        case KEY_R:
            limb -> getSelectedNeurons() -> loop
            (
                []
                ( NeuronUi* neuron )
                {
                    neuron -> setValue( Rnd::get( 0.0, 1.0 ) );
                    return false;
                }
            );
        break;
        case KEY_V:
            limb -> setNeuronDrawMode( NDM_VALUE );
        break;
        case KEY_E:
            limb -> setNeuronDrawMode( NDM_ERROR );
        break;
        case KEY_B:
            limb -> switchShowBinds();
        break;
        case KEY_C:
            limb -> switchShowLayer();
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
            limb -> addSelectedByCursor( &aScene );
        }

        if( aScene.isKey( KEY_LEFT_ALT ))
        {
            limb -> removeSelectedByCursor( &aScene );
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
//        limb -> selectNeuronsByRect( selectTopLeft, selectBottomRight );
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

//    if( aScene.isKey( KEY_W ))
//    {
//        int total = 0;
//        double sum = 0;
//
//        limb -> nerveWeightLoop
//        (
//            limb -> getSelectedNeurons(),
//            [
//                &total, &sum
//            ]
//            (
//                int aWeightIndex,
//                Neuron* aNeuronFrom,
//                Neuron* aNeuronTo,
//                Nerve* aNerve
//            )
//            {
//                total ++;
//                sum += aNerve -> getWeight( aWeightIndex );
//                return false;
//            }
//        );
//
//        if( total != 0 )
//        {
//            auto avgWeight = (double)sum / total;
//
//            limb -> nerveWeightLoop
//            (
//                limb -> getSelectedNeurons(),
//                [
//                    &avgWeight, &aDelta
//                ]
//                (
//                    int aWeightIndex,
//                    Neuron* aNeuronFrom,
//                    Neuron* aNeuronTo,
//                    Nerve* aNerve
//                )
//                {
//                    aNerve -> setWeight( aWeightIndex, avgWeight + aDelta.y * 0.1 );
//                    return false;
//                }
//            );
//        }
//
//        operation = true;
//    }


//    if( aScene.isKey( KEY_P ))
//    {
//        limb -> getSelectedNeurons() -> loop
//        (
//            []
//            ( Neuron* aNeuron )
//            {
//                aNeuron -> getLayer() -> childrenLoop
//                (
//                    aNeuron -> getIndex(),
//                    [
//                        &aNeuron
//                    ]
//                    (
//                        Layer* aParentLayer,
//                        int aParentIndex,
//                        Nerve* aNerve,
//                        double aWeight,
//                        int aIndexWeight
//                    )
//                    {
//                        aParentLayer -> setNeuronValue( aParentIndex, aNeuron -> getValue() );
//                        aNerve -> setWeight( aIndexWeight, 1 );
//                        return false;
//                    }
//                );
//                return false;
//            }
//        );
//        operation = true;
//    }
//

    if( aScene.isKey( KEY_S ))
    {
        limb -> setCursorRadius
        (
            limb -> getCursorRadius() * ( aDelta.y > 0.0 ? 1.1 : 0.9 )
        );
        operation = true;
    }

    if( aScene.isKey( KEY_V ))
    {
        if( limb -> getSelectedNeurons() -> getCount() > 0 )
        {
            auto avgValue = limb -> getSelectedNeurons() -> calcAvgValue();
            limb -> getSelectedNeurons() -> loop
            (
                [ &aScene, &aDelta, &avgValue ]
                ( NeuronUi* aNeuron )
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
        if( limb -> getSelectedNeurons() -> getCount() > 0 )
        {
            auto avgError = limb -> getSelectedNeurons() -> calcAvgError();
            limb -> getSelectedNeurons() -> loop
            (
                [ &aScene, &aDelta, &avgError ]
                ( NeuronUi* aNeuron )
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
        limb
        -> setSelected( &aScene )
        -> getSelectedFirst() != NULL
    )
    {
        camera -> moveTarget
        (
            limb
            -> getSelectedFirst()
            -> getWorld()
        );
    }
}



/*
    Run ui loop
*/
void Ui::onLoop()
{
    getScene() -> calcEvent();
    getScene() -> drawEvent();
}



void Ui::onLoopBefore()
{
    getScene()
    -> init()
    -> setPayload( this );
}
