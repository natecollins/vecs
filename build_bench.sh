#!/bin/bash

g++ -std=c++11 -O3 \
    -I src/ \
    -o bench bench.cpp \
    src/ComponentTypeSet.cpp \
    src/Event.cpp \
    src/EventsStandard.cpp \
    src/System.cpp \
    src/ComponentBase.cpp \
    src/Entity.cpp \
    src/EventReceiver.cpp \
    src/Domain.cpp \
    src/EventEmitter.cpp \
    src/Pool.cpp

