#/bin/bash
rm $1

echo "ACG - 'a'" >> $1
echo "SQR - 'b'" >> $1
echo "HOG - 'c'" >> $1
echo "MSE - 'd'" >> $1
echo "ACN - 'e'" >> $1
echo "WLF - 'f'" >> $1
echo "BER - 'g'" >> $1
echo "TPP - 'h'" >> $1
echo "ALG - 'i'" >> $1
echo "SWM - 'j'" >> $1
echo "MSS - 'k'  // Mouse in swamp" >> $1
echo "HOS - 'l'  // Hog in swamp" >> $1
echo "SNK - 'm'" >> $1
echo "PRC - 'n'" >> $1
echo "FSN - 'o'  // Full snake" >> $1


for i in `ls levels/P*L*`
do
echo '------------------------------' >> $1
echo $i  >> $1
./solve < $i  >> $1
done

