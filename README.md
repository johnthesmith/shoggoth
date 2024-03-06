# Shoggoth

This is [Shoggoth](https://github.com/johnthesmith/shab) neural network with visual control.



## Run

Shoggoth is building at Ubuntu 18.04 and more.

You need to install the following components:

```
sudo apt install libglfw3 libglfw3-dev libglu1-mesa-dev
sudo apt install libglew-dev
sudo apt install graphicsmagick-libmagick-dev-compat 
sudo apt install upx
```

// libgraphicsmagick++1-dev libmagick++-6-headers
// libmagick++-dev 



After that you need to run:
```
./make
```



## Authors

- [@johnthesmith](https://www.github.com/johnthesmith)
- [@igptx](https://www.github.com/igptx)



## Components

The kernel was written in c++ and contains next components:

1. [app](#app) - application
0. [lib](#lib) - baisic c++ libraries
0. [graph](#opengl) - graphics librayes
0. [shoggoth](#neuronet) - neurlal net Shoggoth libraryes
0. [shab](#shab) - small bash maker for Shoggoth

```mermaid
flowchart
    graphics[graph]    

    lib --> graphics
    graphics --> app
    shoggoth --> app
    lib --> shoggoth
    lib --> app
    shab
```



### Lib

1. Lib is contain the tiniy libraries for application with users controllers.



## Graph

1. User interface based on on [glfw](https://www.glfw.org/).
0. Supports:
    1. mouse events
    0. keyboards events
    0. math library for vectors operations
    0. matrix transformation



### OpenGL objects scheme

```mermaid
flowchart BT

subgraph graph
    ScenePayload
    Scene
    Point3d
    Point4d
    Point2d
    Matrix4
    Rect2d
    Rgba
    Object
    Camera
end

ScenePayload--> Form

Payload --> ScenePayload
Camera -.-o Form

ScenePayload-.-o Scene

Object --> Camera
Result --> Scene
Result --> Object

Scene -.-o main

Point3d -.-o Scene
Point4d -.-o Scene
Point2d -.-o Scene
Matrix4 -.-o Scene
Rect2d -.-o Scene
Rgba -.-o Scene

Log -.-o Scene
```

3. Solid line - extends class
0. Dotted line - uses class



### Shoggoth

1. Shoggoth - is a simple neural network (neuronet). Contains following components:
    1. [Neuron](#neuron) - element of neuronet with value and error.
    0. [Neuron Extention](#neuron-extention) - structure contains the optionl specific information about neuron like id, name etc.
    0. [Bind](#bind) - link between two neurns.
    0. [Layer](#layer) - the layer contains neurons.
    0. [Nerve](#nerve) - connection between layers, contains many of binds.
    0. [Net](#net) - it is a general oblect of neuronet with list of neurons.
0. Shoggoth library located at [src/shoggoth](src/shoggoth).


## Math definition

1. Each neuron have the:
    1. v - value [ 0; +1 ] 
    0. e - error [ -1; +1 ]
0. Neurons may have the bind between them.
0. Each bind have the:
    1. w - weight of bind [ -1; +1 ].
    0. type of bind
    0. p - parent neuron
    0. c - child neuron
0. Shogoth have the next linear sigmoid r form argument a:
    1. sig[0;1] = a < 0.0 ? a = 0.0 : ( a > 1.0 ? 1.0 : a )
    2. sig[-1;+1] = a < -1.0 ? a = -1.0 : ( a > 1.0 ? 1.0 : a )



### Neuron value calculated

1. Value of each neuron c calculated like sigmoid from sum of production of parents value on bind weight.
    c.v = sig[0;1]( sum( w * p.v ) )



### Neuron

1. The element of neuronet with Value and Error. Element can has the Extention for optional information.
0. Neuron is defined in [Neuron](./lib/neuron/neuron.h). 
0. Neuron uses the sigmoid function to calculate Value and derivative sigmoid function to calculate Error in learning mode.



### Neuron extention

1. It is a optional structure for neurons. 
0. Each neuron may or not may has one Extantion.
0. The extention will be created for the neuron on demand, when user setts the advinced parameter.
0. No extentions needed for huge [layers](#layer).



### Nerve

1. Nerve analogue of the biological nerve.
0. The nerve connects the two layers and creates multiple connections between the neurons of the parent and child layers.
0. Each nerve has:
    1. the parent layer
    0. the children layer
    0. type of nerve
    0. connection type


### Layer

1. The layer it is the [object](#object) and contains neurons.
0. The layer is the analog of biological organ or cortex section.
0. Each layer has a unique auto-generated identifier for identification and a human readable name.




### Net






## Shoggoth scheme

![](shoggoth.svg)


















```mermaid
flowchart LR

ScenePayload
Camera

math((math<br>libs))

subgraph app
    main
    Form
end

subgraph lib
    Heap
    Result
    Payload
    Log
    Rnd
    Utils
end

subgraph graph
    Object
    Camera
    ScenePayload
    Scene
    math
    Point3d
    Point4d
    Point2d
    Matrix4
    Rect2d
    Rgba

end

subgraph neuron
    Bind
    BindList
    Layer
    LayerList
    Net
    Neuron
    NeuronList
    Func
end


Payload --> ScenePayload
Camera -.-> Form


ScenePayload-.-> Scene

Object --> Camera
Result --> Payload
Result --> Scene
Result --> Object


Log -.-> main

Form -.-> main
Scene -.-> main

Point3d -.-> math
Point4d -.-> math
Point2d -.-> math
Matrix4 -.-> math
Rect2d -.-> math

math -.-> Scene

Net --> Form
ScenePayload--> Form


Func -.-> Neuron
Neuron -.-> NeuronList
NeuronList -.-> Layer
Layer -.-> LayerList
LayerList -.-> Net
Bind -.-> BindList
BindList -.-> Neuron
Heap --> BindList
Heap --> NeuronList
```




```mermaid
flowchart LR

start((b))
stop(((e)))
calcCompleteBefore{calcComplete}
netLoad
resetSync

start -->  calcCompleteBefore
calcCompleteBefore --> |+| netLoad
calcCompleteBefore --> |-| calcFront
netLoad --> layersLoad --> nervesLoad --> resetSync
resetSync --> calcFront

calcFront --> calcBack -->

calcCompleteAfter{calcComplete}-->|+| saveNerves
calcCompleteAfter-->|-| stop

saveNerves --> saveLayers --> stop
```
