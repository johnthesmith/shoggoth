# Intro

1. To create truly universal consciousness:
    1. it should not be linked to human consciousness and habits, perhaps they are limited by evolution;
    0. it should not be focused on narrow tasks, they are a means, not an end;
    0. it should not be centralized;
    0. it should be free for development and exists.

Perhaps it will be *Shoggoth*.



# Shoggoth components

1. Processor - main calculation kernel proces;
0. Teacher - process for teaching the network;
0. Evolition - process for network evolition;
0. Ui - user interface.



# Requiremetns

1. Ubuntu 20.04 and more;
0. clang++ 17;
0. imagemagic libraries.



# Run

1. You need to install the following components:

```
sudo apt install -y \
    clang \
    libglfw3 libglfw3-dev libglu1-mesa-dev \
    libglew-dev \
    graphicsmagick-libmagick-dev-compat \
    upx
```

2. You need to clone following projects:
```
git clone git@github.com:johnthesmith/lib-core.git ./lib/core
git clone git@github.com:johnthesmith/lib-graph.git ./lib/graph
git clone git@github.com:johnthesmith/lib-sock.git ./lib/sock
git clone git@github.com:johnthesmith/lib-json.git ./lib/json
git clone git@github.com:johnthesmith/shab.git ./app/shab
git clone git@github.com:johnthesmith/shoggoth.git ./app/shoggoth
```

3. After that you need to run:
```
./make
```



## Components

1. The kernel of Shoggoth was written in c++ and requires the following projects:
    1. [lib/core](https://github.com/johnthesmith/lib-core) -  baisic c++ libraries;
    0. [lib/json](https://github.com/johnthesmith/lib-json) - object for key and values;
    0. [lib/graph](https://github.com/johnthesmith/lib-graph) - graphic library;
    0. [lib/sock](https://github.com/johnthesmith/lib-sock) - libraries for tcp sockets;
    0. [shoggoth](https://github.com/johnthesmith/shoggoth) - Shoggoth ptoject;
    0. [shab](#shab) - small bash maker for Shoggoth.


```mermaid
flowchart TB
    graphics[graph]

    proj --> app
    proj --> lib
    app --> shab
    app --> shggoth
    lib --> graphics
    lib --> json
    lib --> sock
    lib --> core
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

```mermaid
flowchart TD
    subgraph calc
        retina
        cortex_1
        cortex_2
        bias
        result
    end

    subgraph learning
        error_learn
        command_learn
        sample
    end

    subgraph work
        error_work
        command_work
        test
    end

    retina --->|+ x <-r;+r>| cortex_1
    cortex_1 -->|+ x <-r;+r>| cortex_2
    cortex_2 -->|+ x <-r;+r>| result
    bias -->|+ x <-r;+r>| cortex_1
    bias -->|+ x <-r;+r>| cortex_2
    bias -->|+ x <-r;+r>| result

    result -->|+ x <1;1>| error_learn
    sample -->|+ = <-1;-1>| error_learn
    sample -->|+ x <1;1>| command_learn
    command_learn -->|* x <-1;-1>| error_learn
    
    test --> |+ = <-1;-1>| error_work
    test --> |+ x <1;1>| command_work
    command_work --> | * x <-1;-1> | error_work
    result --> |+ x 1;1| error_work
    
    teacher1((teacher)) .-> retina
    teacher1((teacher)) .-> sample
    teacher1((teacher)) .-> bias
    error_learn .-> teacher3((teacher))
    evolution1((evolution)) .-> test

    classDef in      fill:#FF00FF50,stroke:#FF00FFFF;
    classDef cortex  fill:#FFA50050,stroke:#FFA500FF;
    classDef command fill:#00FF0050,stroke:#00FF00FF;
    classDef error   fill:#FF000050,stroke:#FF0000FF;
    classDef result  fill:#00FFFF50,stroke:#00FFFFFF;   

    class retina,bias,sample,test in;
    class cortex_1,cortex_2 cortex;
    class result result;
    class command_work,command_learn command;
    class error_work,error_learn error;
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



## Authors

- [@igptx](https://www.github.com/igptx)
- [@johnthesmith](https://www.github.com/johnthesmith)







TODO


Надо: что бы эволюшен мог проанализировать последовательность ошибок при этом желательно что бы для каждой смены ретины.

серверу сделали новую статистику ошибки. она пишется при изменнии слоя.

эволюшен притаскивает с сервера статистику и анализирует ошибку за последние тики. на основании нее решает завершать ли тестирование или нет
 и вообще решает успешно ли тестирование или нет. для этого потребуется что бы сервер сбрасывал статистику при смене режима mode.
+ статистика уже собирается
- тики не связаны с ретиной и на 10 смен ретины может быть 1000 тиков. 

>>> надо создать слои для работы режима тестировани и прописать эволюшену запрос статистикии.


Для режима тестирования надо выполнять батч только если тик изменился.
Вариант шлем ретины как получится. а там как процессор упеет.
