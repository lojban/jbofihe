#!/bin/sh

# $Header$

# The flow for building the DOS binary-only release is
# 1. Make usual Unix source release
# 2. Unpack a copy of the Unix source kit
# 3. Run doskit.fe
# 4. Boot into DOS and make the executables
# 5. Run some basic sanity checks
# 6. Reboot Linux and run this script

cp /dosc/jbofihe/jbofihe.exe .
cp /dosc/jbofihe/cmafihe.exe .
cp /dosc/jbofihe/jvocuhad.exe .
cp /dosc/jbofihe/smujajga.exe .
cp /dosc/djgpp/bin/cwsdpmi.exe .
cp /dosc/djgpp/bin/cwsdpmi.doc .

mv README.DOS README.DOS.tmp
perl -pe 'chomp; print $_."\r\n";' < README.DOS.tmp > README.DOS
perl -pe 'chomp; print $_."\r\n";' < COPYRIGHT > COPYRIGHT.txt
zip -9 jbofihe.zip COPYRIGHT.txt
zip -9 jbofihe.zip cwsdpmi.doc cwsdpmi.exe
zip -9 jbofihe.zip cmafihe.exe jbofihe.exe jvocuhad.exe smujajga.exe 
zip -9 jbofihe.zip jbofihe.txt cmafihe.txt smujajgau.txt jvocuhadju.txt 
zip -9 jbofihe.zip smujmaji.dat

echo "Now copy jbofihe.zip to jbofv_vv.zip"


