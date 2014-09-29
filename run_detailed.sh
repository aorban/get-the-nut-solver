#/bin/bash
rm $1

echo "ACG - 'GOLDEN ACORN'" >> $1
echo "SQR - 'SQUIRREL'" >> $1
echo "HOG - 'HOG'" >> $1
echo "MSE - 'MOUSE'" >> $1
echo "ACN - 'ACORN'" >> $1
echo "WLF - 'WOLF'" >> $1
echo "BER - 'BEAR'" >> $1
echo "TPP - 'TRAP'" >> $1
echo "ALG - 'ALLIGATOR'" >> $1
echo "SWM - 'SWAMP'" >> $1
echo "MSS - 'MOUSE IN SWAMP'" >> $1
echo "HOS - 'HOG IN SWAMP'" >> $1
echo "SNK - 'SNAKE'" >> $1
echo "PRC - 'PORCUPINE'" >> $1
echo "FSN - 'SLEEPING SNAKE'" >> $1


for i in `ls levels/P*L*`
do
echo '------------------------------' >> $1
echo $i  >> $1
./solve < $i  >> $1
done

