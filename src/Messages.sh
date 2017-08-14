#! /bin/sh
$EXTRACTRC `find -name '*.ui'`  >> rc.cpp
$XGETTEXT `find -name '*.cpp'` -o $podir/akonadiimportwizard.pot
rm -f rc.cpp
