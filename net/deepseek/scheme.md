# Cхема

```memaid
flowchart TD
    %% Входной слой
    R[retina 30x30x1]
    
    %% Первый свёрточный уровень (4 фильтра)
    R -- "cortex-nerve<br>5x5 window" --> C1A[cortex_1a 26x26x1]
    R -- "cortex-nerve<br>5x5 window" --> C1B[cortex_1b 26x26x1]
    R -- "cortex-nerve<br>5x5 window" --> C1C[cortex_1c 26x26x1]
    R -- "cortex-nerve<br>5x5 window" --> C1D[cortex_1d 26x26x1]
    
    %% Пулинг первого уровня
    C1A -- "pool-nerve<br>2x2 MaxPool" --> P1A[pool_1a 13x13x1]
    C1B -- "pool-nerve<br>2x2 MaxPool" --> P1B[pool_1b 13x13x1]
    C1C -- "pool-nerve<br>2x2 MaxPool" --> P1C[pool_1c 13x13x1]
    C1D -- "pool-nerve<br>2x2 MaxPool" --> P1D[pool_1d 13x13x1]
    
    %% Второй свёрточный уровень (8 фильтров) – покажем только для pool_1a
    P1A -- "cortex-nerve<br>3x3 window" --> C2A[cortex_2a 11x11x1]
    P1B -- "cortex-nerve<br>3x3 window" --> C2A
    P1C -- "cortex-nerve<br>3x3 window" --> C2A
    P1D -- "cortex-nerve<br>3x3 window" --> C2A
    
    P1A -- "cortex-nerve<br>3x3 window" --> C2B[cortex_2b 11x11x1]
    P1B -- "cortex-nerve<br>3x3 window" --> C2B
    P1C -- "cortex-nerve<br>3x3 window" --> C2B
    P1D -- "cortex-nerve<br>3x3 window" --> C2B
    
    %% ... остальные cortex_2 слои (для краткости схематично)
    P1A -- "cortex-nerve<br>3x3 window" --> C2others[cortex_2c...h 11x11x1]
    P1B -- "cortex-nerve<br>3x3 window" --> C2others
    P1C -- "cortex-nerve<br>3x3 window" --> C2others
    P1D -- "cortex-nerve<br>3x3 window" --> C2others
    
    %% Пулинг второго уровня
    C2A -- "pool-nerve<br>2x2 MaxPool" --> P2A[pool_2a 5x5x1]
    C2B -- "pool-nerve<br>2x2 MaxPool" --> P2B[pool_2b 5x5x1]
    C2others -- "pool-nerve<br>2x2 MaxPool" --> P2others[pool_2c...h 5x5x1]
    
    %% Flatten
    P2A -- "flatten-nerve<br>ALL_TO_ALL" --> FL[flatten 200]
    P2B -- "flatten-nerve<br>ALL_TO_ALL" --> FL
    P2others -- "flatten-nerve<br>ALL_TO_ALL" --> FL
    
    %% Полносвязный слой
    FL -- "simple<br>ALL_TO_ALL" --> FC[fc 64]
    
    %% Выходной слой
    FC -- "simple<br>ALL_TO_ALL" --> RES[result 10]
    
    %% Смещение (bias) подключено ко всем обучаемым слоям
    BIAS[bias 1x1x1] -- "simple<br>ALL_TO_ALL" --> C1A
    BIAS -- "simple<br>ALL_TO_ALL" --> C1B
    BIAS -- "simple<br>ALL_TO_ALL" --> C1C
    BIAS -- "simple<br>ALL_TO_ALL" --> C1D
    BIAS -- "simple<br>ALL_TO_ALL" --> C2A
    BIAS -- "simple<br>ALL_TO_ALL" --> C2B
    BIAS -- "simple<br>ALL_TO_ALL" --> C2others
    BIAS -- "simple<br>ALL_TO_ALL" --> FC
    BIAS -- "simple<br>ALL_TO_ALL" --> RES
    
    %% Блок обучения
    SAMPLE[sample 10] -- "ALL_TO_ALL<br>weight=1" --> COMMAND[command 1x1x1]
    SAMPLE -- "ONE_TO_ONE<br>weight=-1" --> ERROR[error 10]
    COMMAND -- "ALL_TO_ALL<br>MUL weight=1" --> ERROR
    RES -- "ONE_TO_ONE<br>weight=1" --> ERROR
    
    %% Блок тестирования
    TEST[test 10] -- "ALL_TO_ALL<br>weight=1" --> T_COMMAND[test_command 1x1x1]
    TEST -- "ONE_TO_ONE<br>weight=-1" --> T_ERROR[test_error 10]
    T_COMMAND -- "ALL_TO_ALL<br>MUL weight=1" --> T_ERROR
    RES -- "ONE_TO_ONE<br>weight=1" --> T_ERROR
    
    %% Стили
    classDef input fill:#d4f7d4,stroke:#333
    classDef conv fill:#c6e2ff,stroke:#333
    classDef pool fill:#ffd8b8,stroke:#333
    classDef dense fill:#f0e6ff,stroke:#333
    classDef output fill:#ffd6d6,stroke:#333
    classDef bias fill:#fffacd,stroke:#333
    classDef teacher fill:#e6f7ff,stroke:#333
    
    class R,input
    class C1A,C1B,C1C,C1D,C2A,C2B,C2others,conv
    class P1A,P1B,P1C,P1D,P2A,P2B,P2others,pool
    class FL,FC,dense
    class RES,output
    class BIAS,bias
    class SAMPLE,COMMAND,ERROR,TEST,T_COMMAND,T_ERROR,teacher
    
    %% Легенда
    subgraph Legend
        direction LR
        L1[cortex-nerve] -- "оконная свёртка" --> L2[pool-nerve]
        L3[simple] -- "ALL_TO_ALL" --> L4[flatten-nerve]
        L5[ONE_TO_ONE] -- "поэлементная связь" --> L6[ALL_TO_ALL MUL]
    end
```
