#!/bin/sh

# $Header$

# The flow for building the DOS binary-only release is
# 1. Make usual Unix source release
# 2. Unpack a copy of the Unix source kit
# 3. Run doskit.fe
# 4. Boot into DOS and make the executables
# 5. Run some basic sanity checks
# 6. Reboot Linux and run this script

if [ ! -d /dosc/jbofihe ]; then
    echo "You need to mount /dosc first!"
    exit 1
fi

make jbofihe.txt
make cmafihe.txt
make smujajgau.txt
make jvocuhadju.txt
make vlatai.txt

cp /dosc/jbofihe/jbofihe.exe .
cp /dosc/jbofihe/cmafihe.exe .
cp /dosc/jbofihe/jvocuhad.exe ./jvocuhad.exe
cp /dosc/jbofihe/smujajga.exe ./smujajga.exe
cp /dosc/jbofihe/vlatai.exe .
cp /dosc/djgpp/bin/cwsdpmi.exe .
cp /dosc/djgpp/bin/cwsdpmi.doc .

mv README.DOS README.DOS.tmp
perl -pe 'chomp; print $_."\r\n";' < README.DOS.tmp > README.DOS
perl -pe 'chomp; print $_."\r\n";' < COPYING > COPYING.txt
mv smujajgau.txt smujajga.txt
mv jvocuhadju.txt jvocuhad.txt
zip -9 jbofihe.zip COPYING.txt
zip -9 jbofihe.zip cwsdpmi.doc cwsdpmi.exe
zip -9 jbofihe.zip cmafihe.exe jbofihe.exe jvocuhad.exe smujajga.exe vlatai.exe
zip -9 jbofihe.zip jbofihe.txt cmafihe.txt smujajga.txt jvocuhad.txt vlatai.txt
zip -9 jbofihe.zip smujmaji.dat

echo "Now copy jbofihe.zip to jbofv_vv.zip"


