for v in 1 2 3 4 5 6 7 8 9 10
do 
  echo $v
  python3 ../hyperClassToProv.py -input ../data/hypertext-class/sample8/prov/sample8$v.log -output ../data/hypertext-class/sample8/prov/sample8$v.txt
done