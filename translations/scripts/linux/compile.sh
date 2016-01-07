# /bin/sh

# Check for translation file existance
if [ ! -f ../../qnetmap_ru.ts ]; then
   echo "Error! File qnetmap_ru.ts does not exist."
   exit 1
fi

# Check for directories existance
if [ ! -d ../../../build ]; then
   mkdir ../../../build || { echo "Error creating result folder."; exit 1; }
fi

if [ ! -d ../../../build/debug ]; then
   mkdir ../../../build/debug || { echo "Error creating result folder."; exit 1; }
fi

if [ ! -d ../../../build/release ]; then
   mkdir ../../../build/release || { echo "Error creating result folder."; exit 1; }
fi

lrelease -compress ../../qnetmap_ru.ts -qm ../../../build/debug/qnetmap_ru.qm
cp -f ../../../build/debug/qnetmap_ru.qm ../../../build/release/qnetmap_ru.qm
