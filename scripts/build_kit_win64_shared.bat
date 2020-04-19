
mkdir build-win64-shared-%1
cd build-win64-shared-%1

cmake .. -G "Visual Studio 15 2017 Win64" -DCMAKE_INSTALL_PREFIX=%CD%/win64-shared-%1 -DNO_NET=TRUE -DNO_VR=TRUE -DCMAKE_BUILD_TYPE="%1" -DNEO_BUILD_SHARED_LIBS=ON -DNO_DOCS=TRUE -DNO_TESTS=ON -DENABLE_OPENGL_RENDERER=ON -DNO_TESTGAME=ON -DENABLE_SDL2_BUILD=TRUE
cmake --build . --target install --config %1

cd ..
