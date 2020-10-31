for v in 1 2 3 4 5 6 7 8 9 10
do 
  echo $v
  ../build/main -o ../data/hypertext-class/sample7/sample7$v.obs -p ../data/hypertext-class/sample7/prov/sample7$v.txt -q all > ../data/hypertext-class/sample7/records/influEquation$v.log
done
