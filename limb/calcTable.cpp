/*
    Check the layer is calculates
    Return true when layer in this item is calculated
*/
bool CalcItem::isCalculated()
{
    return calculated;
}



/*
    Return true if all parents layers by nerves is calculated
*/
bool CalcItem::isParentCalculated()
{

}



/*
    Return true if all children layers by nerves is calculated
*/
bool CalcItem::isChildrenCalculated()
{
}



/*
    Set calculated at true
*/
CalcItem* CalcItem::calculated()
{
    calculated = true;
    return this;
}



CalcTable CalcTable::loop
(
    Net*,
    function <bool ( CalcItem*, Item* )>
)
{
}
