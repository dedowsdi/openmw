#/bin/bash
ctags \
    -R \
    --sort=foldcase \
    --links=yes \
    --fields=KsSi \
    --excmd=number \
    --c++-kinds=+p \
    --c-kinds=+p \
    --language-force=c++ \
    --exclude=build \
    --exclude=cmake \
    --exclude=CMake \
    --exclude=CMakeLists \
    --exclude=docs \
    --exclude=CI \
    --exclude=manual \
