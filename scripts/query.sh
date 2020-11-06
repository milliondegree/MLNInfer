for v in 1 2 3 4 5 6 7 8 9 10
do 
  echo $v
  ../build/main -o ../data/hypertext-class/sample11/sample11$v.obs -p ../data/hypertext-class/sample11/prov/sample11$v.txt -q all -r 10000 -m pgibbs > ../data/hypertext-class/sample11/records/sample$v.log
done
