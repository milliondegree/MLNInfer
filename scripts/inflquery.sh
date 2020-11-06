for v in 1 2 3 4 5 6 7 8 9 10
do 
  echo $v
  ../build/main -o ../data/hypertext-class/sample11/sample$v.obs -p ../data/hypertext-class/sample11/prov/sample$v.txt -q all > ../data/hypertext-class/sample11/records/influEquation$v.log
done
