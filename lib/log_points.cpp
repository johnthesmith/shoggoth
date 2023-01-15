#include <iostream>
#include <string>

#include "log_points.h"
#include "log.h"
#include "point3.h"
#include "point4.h"
#include "matrix.h"


void LogPoints::write
(
    Log& aLog,
    const Point3& aPoint,
    const string aTitle
)
{
    aLog
    .pushColor()
    .pushAlign()

    .setColor( aLog.colorLabel )
    .text( "[" )
    .text( aTitle )
    .space()

    .setAlign( ALIGN_RIGHT )
    .setWidth( 12 )
    .value( aPoint.x )

    .setAlign()
    .setWidth( 0 )
    .setColor( aLog.colorLabel )
    .text( "|" )

    .setAlign( ALIGN_RIGHT )
    .setWidth( 12 )
    .value( aPoint.y )

    .setAlign()
    .setWidth( 0 )
    .setColor( aLog.colorLabel )
    .text( "|" )

    .setAlign( ALIGN_RIGHT )
    .setWidth( 12 )
    .value( aPoint.z )

    .setAlign()
    .setWidth( 0 )
    .setColor( aLog.colorLabel )
    .text( "]" )

    .popAlign()
    .popColor()
    ;
}



void LogPoints::write
(
    Log& aLog,
    const Point4& aPoint,
    const string aTitle
)
{
    aLog
    .pushColor()
    .pushAlign()

    .setColor( aLog.colorLabel )
    .text( "[" )
    .text( aTitle )
    .space()

    .setAlign( ALIGN_RIGHT )
    .setWidth( 12 )
    .value( aPoint.x )
    .setAlign()
    .setWidth( 0 )

    .setColor( aLog.colorLabel )
    .text( "|" )

    .setAlign( ALIGN_RIGHT )
    .setWidth( 12 )
    .value( aPoint.y )
    .setAlign()
    .setWidth( 0 )

    .setColor( aLog .colorLabel )
    .text( "|" )

    .setAlign( ALIGN_RIGHT )
    .setWidth( 12 )
    .value( aPoint.z )
    .setAlign()
    .setWidth( 0 )

    .setColor( aLog .colorLabel )
    .text( "|" )

    .setAlign( ALIGN_RIGHT )
    .setWidth( 12 )
    .value( aPoint.w )
    .setAlign()
    .setWidth( 0 )

    .setColor( aLog.colorLabel )
    .text( "]" )

    .popAlign()
    .popColor()
    ;
}




void LogPoints::write
(
    Log& aLog,
    const Matrix4& aMatrix,
    const string aTitle
)
{
    aLog.trace( aTitle ).trace();
    LogPoints::write( aLog, aMatrix.l1, "1:" );
    aLog.trace();
    LogPoints::write( aLog, aMatrix.l2, "2:" );
    aLog.trace();
    LogPoints::write( aLog, aMatrix.l3, "3:" );
    aLog.trace();
    LogPoints::write( aLog, aMatrix.l4, "4:" );
    aLog.lineEnd();
}
