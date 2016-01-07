# /bin/sh

if [ ! -d ../../build ]; then
   mkdir ../../build || { echo "Error creating results folder."; exit 1; }
fi

cd ../../build
qmake ../projects/qmake/qnetmap.pro
make