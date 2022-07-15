#!/bin/bash
if [ ! -d "build" ]; then
    mkdir build
    cp config -r build
fi

if [ ! -d "databeammuons" ]; then
    mkdir databeammuons
    for num in {0..99}; do
        mkdir databeammuons/tempdir$num
    done
fi

if [ ! -d "databeammcp" ]; then
    mkdir databeammcp
    for num in {0..99}; do
        mkdir databeammcp/tempdir$num
    done
fi

if [ ! -d "datacosmicmuons" ]; then
    mkdir datacosmicmuons
    for num in {0..99}; do
        mkdir datacosmicmuons/tempdir$num
    done
fi

if [ ! -d "datagamma" ]; then
    mkdir datagamma
    for num in {0..99}; do
        mkdir datagamma/tempdir$num
    done
fi
