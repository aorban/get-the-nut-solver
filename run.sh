#/bin/bash
echo "level name, min solution length, solution, number of moves, max num candidates in mem, num visited states, visited state improved, visited state dropped"
for i in `ls levels/P*L*`
do
#echo '------------------------------'
#echo $i
#./solve simple < $i
echo $i, `./solve simple < $i`
done

