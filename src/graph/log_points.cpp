#include <iostream>
#include <string>

#include "../lib/log.h"

#include "log_points.h"

#include "point3.h"
#include "point4d.h"
#include "matrix.h"


void LogPoints::write
(
    Log* aLog,
    const Point3d& aPoint,
    const string aTitle
)
{
    aLog
    -> pushColor()
    -> pushAlign()

    -> setColor( aLog -> colorLabel )
    -> text( "[" )
    -> text( aTitle )
    -> space()

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aPoint.x )

    -> setAlign()
    -> setWidth( 0 )
    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aPoint.y )

    -> setAlign()
    -> setWidth( 0 )
    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aPoint.z )

    -> setAlign()
    -> setWidth( 0 )
    -> setColor( aLog -> colorLabel )
    -> text( "]" )

    -> popAlign()
    -> popColor()
    ;
}



void LogPoints::write
(
    Log* aLog,
    const Point3i& aPoint,
    const string aTitle
)
{
    aLog
    -> pushColor()
    -> pushAlign()

    -> setColor( aLog -> colorLabel )
    -> text( "[" )
    -> text( aTitle )
    -> space()

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aPoint.x )

    -> setAlign()
    -> setWidth( 0 )
    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aPoint.y )

    -> setAlign()
    -> setWidth( 0 )
    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aPoint.z )

    -> setAlign()
    -> setWidth( 0 )
    -> setColor( aLog -> colorLabel )
    -> text( "]" )

    -> popAlign()
    -> popColor()
    ;
}



void LogPoints::write
(
    Log* aLog,
    const Point4d& aPoint,
    const string aTitle
)
{
    aLog
    -> pushColor()
    -> pushAlign()

    -> setColor( aLog -> colorLabel )
    -> text( "[" )
    -> text( aTitle )
    -> space()

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aPoint.x )
    -> setAlign()
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aPoint.y )
    -> setAlign()
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aPoint.z )
    -> setAlign()
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aPoint.w )
    -> setAlign()
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "]" )

    -> popAlign()
    -> popColor()
    ;
}




void LogPoints::write
(
    Log* aLog,
    const Matrix4& aMatrix,
    const string aTitle
)
{
    aLog
    -> trace( aTitle )

    -> pushColor()
    -> pushAlign()

    /* Line X */
    -> trace()

    -> setColor( aLog -> colorLabel )
    -> text( "[" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aMatrix.m[ M_AX ] )
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setWidth( 12 )
    -> value( aMatrix.m[ M_BX ] )
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aMatrix.m[ M_CX ] )
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aMatrix.m[ M_DX ] )
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "]" )

    -> setAlign()


    /* Line Y */
    -> trace()

    -> setColor( aLog -> colorLabel )
    -> text( "[" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aMatrix.m[ M_AY ] )
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setWidth( 12 )
    -> value( aMatrix.m[ M_BY ] )
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aMatrix.m[ M_CY ] )
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aMatrix.m[ M_DY ] )
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "]" )

    -> setAlign()



    /* Line Z */
    -> trace()

    -> setColor( aLog -> colorLabel )
    -> text( "[" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aMatrix.m[ M_AZ ] )
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setWidth( 12 )
    -> value( aMatrix.m[ M_BZ ] )
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aMatrix.m[ M_CZ ] )
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aMatrix.m[ M_DZ ] )
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "]" )

    -> setAlign()


    /* Line W */
    -> trace()

    -> setColor( aLog -> colorLabel )
    -> text( "[" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aMatrix.m[ M_AW ] )
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setWidth( 12 )
    -> value( aMatrix.m[ M_BW ] )
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aMatrix.m[ M_CW ] )
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "|" )

    -> setAlign( ALIGN_RIGHT )
    -> setWidth( 12 )
    -> value( aMatrix.m[ M_DW ] )
    -> setWidth( 0 )

    -> setColor( aLog -> colorLabel )
    -> text( "]" )

    -> setAlign()

    -> popAlign()
    -> popColor();
}
