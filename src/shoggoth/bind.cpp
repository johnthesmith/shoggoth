#include <iostream>
#include "bind.h"
#include "func.h"



/*
    set bind weight
*/
Bind* Bind::setWeight
(
    double a
)
{
    weight = a;
    return this;
}



/*
    Return weight bind
*/
double Bind::getWeight()
{
    return weight;
}



/*
    Add bind weight
*/
Bind* Bind::addWeight
(
    double a
)
{
    weight = FUNC_SIGMOID_PLUS_MINUS( a + weight, 20 );
    return this;
}



/*
    Set bind parent
*/
Bind* Bind::setParent
(
    Neuron* a
)
{
    parent = a;
    return this;
}




/*
    Return parent neuron
*/
Neuron* Bind::getParent()
{
    return parent;
}



/*
    Set bind child
*/
Bind* Bind::setChild
(
    Neuron* a
)
{
    child = a;
    return this;
}


/*
    Return child neuron
*/
Neuron* Bind::getChild()
{
    return child;
}



