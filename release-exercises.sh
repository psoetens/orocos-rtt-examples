#!/bin/bash

if [ x$1 = x ] ; then 
echo "Please provide version-string parameter (e.g. 1.2.1)"
exit 1
fi;

if [ ! -d export ] ; then 
mkdir export || exit 1
fi;

VERSION=$1 

#cleanup
rm -rf export/rtt-exercises-$VERSION
rm -f rtt-exercises-$VERSION.tar.gz 
#copy over and make sure it's clean.
cp -a rtt-exercises export/rtt-exercises-$VERSION
cd export/rtt-exercises-$VERSION
for i in hello*; do
    make -C $i clean
done
rm -rf controller-1/build

#create solution dir.
#hacky, need to improve this
cp -a controller-1 controller-1-solution
cd controller-1-solution
git diff master..orocos-examples/solution | patch -p3 | exit 1
cd ../..

tar -cvzf rtt-exercises-$VERSION.tar.gz rtt-exercises-$VERSION
cd ..
echo "Packaging done."

echo "Press a key to copy files to server, Ctrl-C to abort..."
read -s -n1

  USER=orocos
  SERVER=ftp.mech.kuleuven.be
  SPREFIX=pub_html/pub
  BRANCH=stable

# Orocos Examples
ssh -l$USER $SERVER "mkdir -p $SPREFIX/$BRANCH/examples/rtt/tutorial" || exit 1
scp rtt-exercises-$VERSION.tar.gz $USER@$SERVER:$SPREFIX/$BRANCH/examples/rtt/tutorial || exit 1
scp rtt-exercises-$VERSION/README $USER@$SERVER:$SPREFIX/$BRANCH/examples/rtt/tutorial/README.txt || exit 1

echo "Copied files to $SERVER. Done!"
