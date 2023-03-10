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
    set bind type
*/
Bind* Bind::setType
(
    BindType a
)
{
    type = a;
    return this;
}



/*
    Return weight bind
*/
BindType Bind::getType()
{
    return type;
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



