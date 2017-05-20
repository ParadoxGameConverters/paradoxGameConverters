#!/bin/bash
echo "Version Number?"
read version
rm -rf "EU4toV2-$version"
rm -rf "EU4toV2-$version.tar.bz2"

cd EU4toV2
rm -rf build
rm -rf Release-Linux
cmake -H. -Bbuild
cmake --build build -- -j3
cp -rf "Release-Linux" "../EU4toV2-$version"
cd ..

tar -cjf "EU4toV2-$version.tar.bz2" "EU4toV2-$version"