for v in 1 2 3 4 5 6 7 8 9 10
do 
  scp jz598@aries.cs-georgetown.net:/home/jz598/rapidnet_v1.0/data/hyper-text-class/sample8/sample$v.log ./data/hypertext-class/sample8/prov/sample$v.log
  python3 hyperClassToProv.py -input data/hypertext-class/sample8/prov/sample$v.log -output data/hypertext-class/sample8/prov/sample$v.txt
done