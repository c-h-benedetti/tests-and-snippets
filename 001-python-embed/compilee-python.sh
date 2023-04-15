# codeBlock{"id": "compileScript", "other":4};
cd path/to/the/python/directory
./configure --enable-shared --enable-optimizations
make -j6
mv "./pyconfig.h" "./Include/pyconfig.h"
ln -s "./libpython3.10.so.1.0" ..
# codeBlockEnd;