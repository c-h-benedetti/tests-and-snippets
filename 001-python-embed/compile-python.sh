cd /home/clement/Desktop/tests-and-snippets/001-python-embed/cpython-3.10
./configure --enable-shared --enable-optimizations
make -j6
mv "./pyconfig.h" "./Include/pyconfig.h"
ln -s "./libpython3.10.so.1.0" ..