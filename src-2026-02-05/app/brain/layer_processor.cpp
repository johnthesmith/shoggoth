#include "../../shoggoth/limb.h"
#include "layer_processor.h"



/*
    Constructor
*/
LayerProcessor::LayerProcessor
(
    Limb* aLimb,
    string aId
)
:Layer( aLimb, aId )
{
    getLog() -> trace( "... it was a layer Processor" );
}



/*
    Internal destructor
*/
LayerProcessor::~LayerProcessor()
{
}



/*
    Creator
*/
LayerProcessor* LayerProcessor::create
(
    Limb* aLimb,
    string aId
)
{
    return new LayerProcessor( aLimb, aId );
}



/*
    Destructor
*/
void LayerProcessor::destroy()
{
    delete this;
}



/******************************************************************************
    Specific methods
*/



