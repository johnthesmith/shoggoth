/*
    Teacher main file for Shoggoth neural network
*/
#include "teacher_application.h"



using namespace std;



int main
(
    int argc,
    char** argv
)
{
    TeacherApplication::create( argc, argv )
    -> run()
    -> destroy();
    return 0;
}
