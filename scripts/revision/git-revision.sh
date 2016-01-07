# /bin/sh
# This script gets the the commit count across all branches in the given repo.
# Usage: git-revision.sh <revision file>
# The script runs from the repo's root

REVISION_FILE=$1
REVISION_HASH_FILE=${REVISION_FILE}ash

# Create ./version folder if absent
if [ ! -d version ]; then
   mkdir version || { echo "Can't create version folder."; exit 1; }
fi

CURRENT_VERSION=`git rev-list --all --count`

# if not integer regecting
if [ "$CURRENT_VERSION" -ne "$CURRENT_VERSION" ] 2>/dev/null; then
   echo 0 > $REVISION_FILE
   echo 0x0000000000 > $REVISION_HASH_FILE
   exit 0
fi

if [ ! -f $REVISION_FILE ]; then 
   echo $CURRENT_VERSION > $REVISION_FILE
   CURRENT_HASH=`git rev-parse --short=10 HEAD`
   echo 0x${CURRENT_HASH} > $REVISION_HASH_FILE
   exit 0
fi

FILE_VERSION=`cat $REVISION_FILE`
if [ "$CURRENT_VERSION" -ne "$FILE_VERSION" ] 2>/dev/null; then
   echo $CURRENT_VERSION > $REVISION_FILE
   CURRENT_HASH=`git rev-parse --short=10 HEAD`
   echo 0x${CURRENT_HASH} > $REVISION_HASH_FILE
fi
