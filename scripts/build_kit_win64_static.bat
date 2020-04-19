
mkdir build-win64-static-%1
cd build-win64-static-%1

cmake .. -G "Visual Studio 15 2017 Win64" -DCMAKE_INSTALL_PREFIX=%CD%/win64-static-%1 -DNEO_BUILD_SHARED_LIBS=OFF -DNO_NET=TRUE -DNO_VR=TRUE -DCMAKE_BUILD_TYPE="%1" -DNO_DOCS=TRUE -DNO_TESTS=ON -DENABLE_OPENGL_RENDERER=ON -DNO_TESTGAME=ON -DENABLE_SDL2_BUILD=TRUE
cmake --build . --target install --config %1

cd ..
