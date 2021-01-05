#!/bin/sh

#rsync -azv /home/root/student.db /media/sdcard/

if [  -d  /media/sdcard  ]
then

line=`head -n 1 /usr/share/status/EEPROM-data`
sub=`date +"%d""T""%H%M%S"`
echo $sub

echo $line
mkdir /media/sdcard/$line

file1=`ls -l /home/root/mmt.db | awk '{print $5}'`
echo $file1
file2=`ls -l /media/sdcard/$line/mmt.db | awk '{print $5}'`
echo $file2	
if [ $file1 -lt $file2 ]
then
echo "File size in device is less"
mv /media/sdcard/$line/mmt.db /media/sdcard/$line/mmt.db_backup_$sub

fi

#rsync -azv /root/localdb /media/sdcard/$line/localdb
cp /home/root/mmt.db /media/sdcard/$line/mmt.db
else
echo "Sdcard not there"
fi


