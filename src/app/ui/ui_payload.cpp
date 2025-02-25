/* User libraries */
#include "ui.h"

#include "../../../../lib/core/rnd.h"


/* Local libraries */
#include "../math.h"

#include "../shoggoth/limb/layer_ui.h"
#include "../shoggoth/limb/neuron_ui.h"
#include "../shoggoth/limb/neuron_list_ui.h"

#include "../shoggoth/io.h"



using namespace std;




/*
    Constructor
*/
UiPayload::UiPayload
(
    /* Application object */
    UiApplication* a,
    /* Payload id */
    string aPayloadId,
    /* Net id */
    string aNetId,
    /* Net version */
    string aNetVersion
)
/* Call parent constructor */
: ScenePayload( a, aPayloadId )
{
//    limb = LimbUi::create( aNet );
//    camera = Camera::create();
//
//    setScene( Scene::create( aNet -> getApplication() -> getLogManager() ));
//
//    getScene()
//    -> getFont()
//    -> setFontName
//    (
//        getApplication()
//        -> getConfig()
//        -> getString( Path{ "tasks", taskToString( TASK_UI ), "fontName" } )
//    )
//    -> setGliphSize
//    (
//        getApplication()
//        -> getConfig()
//        -> getInt( Path{ "tasks", taskToString( TASK_UI ), "gliphSize" }, 16 )
//    )
//    -> setCharSet
//    (
//        getApplication()
//        -> getConfig()
//        -> getString( Path{ "tasks", taskToString( TASK_UI ), "charSet" })
//    );
//
//    getLog()
//    -> trace( "Config source" )
//    -> prm( "file", getApplication()
//    -> getConfigFileName() );
}



/*
    Destructor
*/
UiPayload::~Ui()
{
//    getScene() -> finit() -> destroy();
//    camera -> destroy();
//    limb -> destroy();
}



/*
    Creator
*/
Ui* UiPayload::create
(
    /* Application object */
    UiApplication* a,
    /* Payload id */
    string aPayloadId,
    /* Net id */
    string aNetId,
    /* Net version */
    string aNetVersion
)
{
    return new Ui( a, aPayloadId, aNetId, aNetVersion );
}



/*
    Destructor
*/
void UiPayload::destroy()
{
    delete this;
}



ShoggothRoleApplication* UiPayload::getApplication()
{
    return ( ShoggothRoleApplication* ) Payload::getApplication();
}



Ui* UiPayload::help()
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
void UiPayload::onActivate
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
void UiPayload::onCalc
(
    Scene* aScene   /* Scene object */
)
{
    limb
    -> getNet()

    /* Prepare UI limb, check and copy structure from net */
    -> syncToLimb( limb, true )

    /* Read from net weight and errors */
    -> swapValuesAndErrors
    (
        Actions{ READ_VALUES, READ_ERRORS },    /* Action */
        TASK_UI,                                /* Role */
        limb,                                   /* Participant object */
        true
    );


    /* Fill all weight TODO - remove it */
    limb -> getNerveList() -> weightsAllocate
    (
        []( Nerve* nerve )
        {
            nerve -> fill( NULL, 1.0, 1.0 );
        }
    );

// Dump weights from net
// This must be remove from net
//    limb -> dumpWeightsExchange();

    limb -> getNet()
    -> swapValuesAndErrors
    (
        Actions{ READ_VALUES, READ_ERRORS },    /* Action */
        TASK_UI,                                /* Role */
        limb,                                   /* Participant object */
        true
    );
}


/*
    Main draw method
*/
void UiPayload::onDraw
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
    -> drawNerves( aScene )
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
    -> drawMatrix( Point3d( 0, aScene -> getViewport().height, 0 ) )
    -> textCR()
    -> setTextHorisontalAlign( ALIGN_LEFT )
    -> text
    (
        limb
        -> getNet()
        -> getConfig()
        -> getString( Path{ "version", "current" })
    );

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
void UiPayload::onKeyUp
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
void UiPayload::onKeyDown
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
        case KEY_W:
            limb -> dumpWeightsExchange();
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
        case KEY_M:
        {
            string version = "";

            /* Клонирование */
            auto io = Io::create( limb -> getNet() );
            io -> getRequest() -> setBool( "mutation", true );

            if( io -> call( CMD_CLONE_NET ) -> isOk() )
            {
                version = io -> getAnswer() -> getString( "version" );
                getLog()
                -> info( "Net clone" )
                -> prm( "net", io -> getAnswer() -> getString( "id" ))
                -> prm( "version",  version )
                -> lineEnd();

                /* Переключение на клон */
                auto io1 = Io::create( limb -> getNet() );
                io1 -> getRequest() -> setString( "version", version );
                io1 -> call( CMD_SWITCH_NET ) -> disconnect() -> destroy();
            }
            io -> destroy();
        }
        break;
        case KEY_LEFT_CONTROL:
            camera -> setEyeLock( true );
        break;
        case KEY_LEFT_SHIFT:
            camera -> setTargetLock( true );
        break;
    }
}




void UiPayload::onMouseMove
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
bool UiPayload::onLeftDragBegin
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
void UiPayload::onLeftDragEnd
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
void UiPayload::onLeftDrag
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
void UiPayload::onRightDrag
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
void UiPayload::onMouseWheel
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
void UiPayload::onLeftClick
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
void UiPayload::onLeftDblClick
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
void UiPayload::onLoop()
{
    getScene() -> calcEvent();
    getScene() -> drawEvent();
}



void UiPayload::onLoopBefore()
{
    getScene()
    -> init()
    -> setPayload( this );
}
