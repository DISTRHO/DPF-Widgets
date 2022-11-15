#!/bin/bash

set -e

cd $(dirname ${0})

if [ ! -d .elements-git-src ]; then
    git clone --recursive git@github.com:cycfi/elements.git .elements-git-src
fi

rm -rf asio asio.hpp
cp -r .elements-git-src/lib/external/asio/asio/include/{asio,asio.hpp} .

rm -rf elements elements.hpp
cp -r .elements-git-src/lib/include/{elements,elements.hpp} .

rm -rf infra src
cp -r .elements-git-src/lib/infra/include/infra .
cp -r .elements-git-src/lib/src .

# no elements-managed app or windows for dpf, use dpf stuff instead please
rm -f elements/app.hpp
rm -f elements/window.hpp
sed -i -e '/#include <elements\/app.hpp>/d' elements.hpp
sed -i -e '/#include <elements\/window.hpp>/d' elements.hpp
sed -i -e '/#include <elements\/window.hpp>/d' src/view.cpp

# custom patching needed for now
patch -p1 -i dpf-changes.patch
