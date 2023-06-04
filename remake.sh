#!/bin/bash
cd ./mail2
rm * 
cp ../mail/* .
rm ../Albertshare.bmp
cp Albertshare.bmp ../
cd ..
/home/shadad/Desktop/cripto/ss d Albertshare.bmp $1 /home/shadad/Desktop/cripto/mail2
rm Albertshare.bmp
/home/shadad/Desktop/cripto/ss r Albertshare.bmp $1 /home/shadad/Desktop/cripto/mail2
chmod 777 Albertshare.bmp 
