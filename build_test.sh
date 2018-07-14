#!/bin/bash

g++ -std=c++11 -g \
    -I src/ \
    -o test test.cpp \
    src/ComponentTypeSet.cpp \
    src/EventsStandard.cpp \
    src/System.cpp \
    src/ComponentBase.cpp \
    src/Entity.cpp \
    src/Domain.cpp \
    src/EventEmitter.cpp \
    src/Pool.cpp

