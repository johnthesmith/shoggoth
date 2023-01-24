
int Layer::indexByPos
(
    double x, /* x-position */
    double y, /* y-position */
    double z  /* z-position */
)
{
    return x + y*xDim + z*xDim*yDim;
}
