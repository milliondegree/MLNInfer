import matplotlib
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.use('Agg')
import matplotlib.pyplot as plt
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42
import numpy as np

def numberToStr(n):
  mag = 0
  while n/1000>1:
    n = n/1000
    mag += 1
  substring = ""
  if mag==1:
    substring = 'k'
  elif mag==2:
    substring = 'm'
  return str(n)+substring

def drawQueryTimes():
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000", "1100", "1200", "1300", "1400"]
  queryList = [0.301, 0.348, 0.437, 0.365, 0.381, 0.443, 0.464, 0.500, 1.689, 0.608, 3.596, 4.053, 82.309, 18.450]

  plt.bar(xList, queryList, color="blue")
  plt.xlabel("number of tuples")
  plt.ylabel("average time of provenance query")
  plt.show()


def drawQueryTimes_4():
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  queryList = [0.162, 0.236, 0.447, 0.777, 1.216, 2.22, 2.18, 2.19, 1.03, 6.47]

  plt.bar(xList, queryList, color="blue")
  plt.xlabel("number of tuples")
  plt.ylabel("average time of provenance query")
  plt.show()


def drawQueryTimes_5():
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  queryList = [0.126, 0.136, 0.216, 0.27, 0.364, 0.398, 0.623, 0.634, 0.712, 0.866]

  plt.bar(xList, queryList, color="blue")
  plt.xlabel("number of tuples")
  plt.ylabel("average time of provenance query")
  plt.show()


def drawMaintenanceTime_7():
  fig, ax = plt.subplots(figsize=(10, 6))
  ax2 = ax.twinx()
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1k"]
  queryList1 = [29.8, 59.1, 90.07, 120.8, 150.5, 186, 206.3, 237.3, 268.6, 296.1]
  queryList2 = [7.70942, 15.3306, 23.4762, 29.7336, 39.1901, 46.2846, 54.4626, 60.7309, 67.4802, 75.6743]
  queryList3 = [0.19584989227166277, 0.3430643559322034, 0.567240964669739, 0.8340344311926606, 1.1361911626139818,
                1.778336606980273, 2.1400285422960725, 2.665217135746606, 4.558864328335832, 7.186616805097452]
  x = np.arange(len(xList))
  w = 0.24
  lw = 0
  ax.bar(x-w, queryList2, w, label='Maintenance w/o provenance', color="tab:blue", edgecolor="black", linewidth=lw)
  ax.bar(x, queryList1, w, label='Maintenance with provenance', color="tab:orange", edgecolor="black", linewidth=lw)
  ax2.bar(x+w, queryList3, w, label="Provenance query", color="tab:green", edgecolor="black", linewidth=lw)
  box = ax.get_position()
  ax.set_position([box.x0, box.y0+box.height*0.1, box.width, box.height*0.8])

  fs1 = 23
  fs2 = 20
  ax.set_ylabel('Time (s)', fontsize=fs1)
  ax.set_yscale("linear")
  ax.set_xlabel('Sample size', fontsize=fs1)
  ax.set_xticks(x)
  ax.set_xticklabels(xList, fontsize=fs2)
  ax.set_yticks(list(range(0, 301, 50)))
  ax.set_yticklabels([str(ele) for ele in list(range(0, 301, 50))], fontsize=fs2)
  ax2.set_yticks(list(range(0, 9, 1)))
  ax2.set_ylabel("Time (s)", fontsize=fs1)
  ax2.set_yticklabels([str(ele) for ele in list(range(0, 8))], fontsize=fs2)
  fig.legend(fontsize=19, loc='upper center', bbox_to_anchor=(0.38,0.8),fancybox=True)
  ax.grid(axis='y')
  plt.savefig("./data/hypertext-class/sample7/images/maintain_query.pdf")


def drawQueryTimes_7():
  fig, ax = plt.subplots(figsize=(10,6))
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1k"]
  # queryList1 = [0.196, 0.343, 0.567, 0.834, 1.136, 1.778, 2.14, 2.66, 4.56, 7.18]
  queryList1 = [0.19584989227166277, 0.3430643559322034, 0.567240964669739, 0.8340344311926606, 1.1361911626139818,
                1.778336606980273, 2.1400285422960725, 2.665217135746606, 4.558864328335832, 7.186616805097452]
  # queryList2 = [0.274, 0.58, 0.94, 1.637, 2.174, 2.68, 3.2, 4.05, 5.3, 6.37]
  # queryList2 = [0.23808514354066984, 0.5371160580152672, 0.8954505701357466, 1.5968827611940297, 2.140646583703704,
  #               2.6401818698224853, 3.1556854452662724, 4.0063278197932055, 5.25113364844904, 6.307514858197933]
  # ql1 = np.array(queryList1)
  # ql2 = np.array(queryList2)
  # ql = (ql1+ql2)/2
  # queryList = list(ql)
  queryList = queryList1

  fs1 = 26
  fs2 = 20
  x = np.arange(len(xList))
  ax.bar(x, queryList, color="tab:green", width=0.6, edgecolor="black", linewidth=2)
  box = ax.get_position()
  ax.set_position([box.x0+box.width*0.05, box.y0+box.height*0.1, box.width, box.height*0.8])

  ax.set_xlabel("Sample size", fontsize=fs1)
  ax.set_xticks(x)
  ax.set_xticklabels(xList, fontsize=fs2)
  ax.set_ylabel("Time (s)", fontsize=fs1)
  yList = list(range(0, 9, 1))
  ax.set_yticks(yList)
  ax.set_yticklabels([str(ele) for ele in yList], fontsize=fs2)
  ax.set_yscale('linear')
  ax.grid(axis='y')
  plt.savefig("./data/hypertext-class/sample7/images/provenance_query.pdf")


def drawQueryTimes_11():
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  # queryList1 = [0.196, 0.343, 0.567, 0.834, 1.136, 1.778, 2.14, 2.66, 4.56, 7.18]
  queryList1 = [1.2335028867924527, 2.126787842900302, 2.8515190150150147, 4.512209073463269, 6.081573559701492,
                7.675888536512668, 11.057172661218425, 13.53308026923077, 17.16496398522895, 31.19561526440177]
  # queryList2 = [0.274, 0.58, 0.94, 1.637, 2.174, 2.68, 3.2, 4.05, 5.3, 6.37]
  # queryList2 = [0.23808514354066984, 0.5371160580152672, 0.8954505701357466, 1.5968827611940297, 2.140646583703704,
  #               2.6401818698224853, 3.1556854452662724, 4.0063278197932055, 5.25113364844904, 6.307514858197933]
  # ql1 = np.array(queryList1)
  # ql2 = np.array(queryList2)
  # ql = (ql1+ql2)/2
  # queryList = list(ql)
  queryList = queryList1

  plt.bar(xList, queryList, width=0.3, color="blue")
  plt.xlabel("number of tuples")
  plt.ylabel("average time of provenance query")
  plt.show()


def drawProbQueryTimes_7():
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  queryList = [0.0003, 0.0003, 0.0004, 0.0004, 0.0044, 0.0046, 0.0056, 0.0058, 0.0086, 0.0168]

  plt.bar(xList, queryList, color="blue")
  plt.xlabel("number of tuples")
  plt.ylabel("probability query time")
  plt.show()



def drawProbQueryTimes_7AVG():
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  queryList = [9.44847775175644e-05, 0.00013946569343065694, 0.0002791524966261808, 0.0004687876802096986, 0.0007806840101522842, 0.0010874110718492344, 0.0012913897142857143, 0.0017384758698092031, 0.002800705573080968, 0.0061624632124352334]
  plt.bar(xList, queryList, color="blue")
  plt.xlabel("number of tuples")
  plt.ylabel("probability query time")
  plt.show()


def drawProbQueryTimes_7AVG_Logscale():
  ax = plt.subplot(111)
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  queryList1 = [9.21943793911005e-05, 0.00014776058394160588, 0.00027945479082321117, 0.0004977667103538673, 0.0008124416243654811, 0.001099769140164896, 0.001604666285714281, 0.0017611257014590307, 0.002931464773922183, 0.006416753367875641]
  # queryList1 = [9.44847775175644e-05, 0.00013946569343065694, 0.0002791524966261808, 0.0004687876802096986, 0.0007806840101522842, 0.0010874110718492344, 0.0012913897142857143, 0.0017384758698092031, 0.002800705573080968, 0.0061624632124352334]
  queryList2 = [0.010655662763466042, 0.015650773722627746, 0.02869275708502028, 0.04739664875491479, 0.0729437956852791, 0.10779084098939927, 0.15211621714285697, 0.1646830179573512, 0.258714229232387, 0.536942683937824]
  # queryList2 = [0.095999131147541, 0.13945208905109488, 0.24944783130904186, 0.41301451245085186, 0.6425268667512689, 0.9123236230859836, 1.333311952, 1.4630163928170594, 2.2927532260778127, 4.74799883626943]
  x = np.arange(len(xList))
  w = 0.3
  ax.bar(x-0.5*w, queryList1, w, label='iterative', color='b')
  ax.bar(x+0.5*w, queryList2, w, label='sampling', color='r')
  ax.set_ylabel('Probability query time')
  # ax.set_yscale("log")
  ax.set_xlabel('sample size')
  ax.set_xticks(x)
  ax.set_xticklabels(xList)
  ax.legend()
  plt.show()


def drawProbQueryTimes_8AVG():
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  queryList = [0.000104622009569378, 0.00015262409288824383, 0.00025469005102040813, 0.00043397136038186157, 0.0005348886337543054, 0.0005971962513781698, 0.0006366638115631691, 0.0010169098360655738, 0.001502379207920792, 0.0018038508287292817]
  plt.bar(xList, queryList, color="blue")
  plt.xlabel("number of tuples")
  plt.ylabel("probability query time")
  plt.show()


def drawInflQueryTimes_7():
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  queryList = [0.0025, 0.0026, 0.006, 0.006, 0.39, 0.52, 0.74, 0.926, 1.736, 8.17]

  plt.bar(xList, queryList, color="blue")
  plt.xlabel("number of tuples")
  plt.ylabel("influence query time of all related tuples")
  plt.show()


def drawInflQueryTimes_7AVG():
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  queryList = [0.00016599063231850116, 0.00025002773722627736, 0.00048640080971659917, 0.0008574796854521624, 0.0013845469543147207, 0.0019340989399293286, 0.002450624, 0.003119818181818182, 0.00518013880126183, 0.011620722279792745]

  plt.bar(xList, queryList, color="blue")
  plt.xlabel("number of tuples")
  plt.ylabel("influence query time of all related tuples")
  plt.show()


def drawInflQueryTimes_7AVG_equa():
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  queryList = [0.0008745925058548009, 0.0008951445255474453, 0.0010676221322537113, 0.00139064875491481, 0.0022807677664974622, 0.0032176996466431094, 0.005587749714285715, 0.006172984287317621, 0.015330617245005259, 0.1047135274611399]

  plt.bar(xList, queryList, color="blue")
  plt.xlabel("number of tuples")
  plt.ylabel("influence query time of all related tuples")
  plt.show()


def drawInflQueryTimes_7AVG_Logscale():
  ax = plt.subplot(111)
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  queryList1 = [0.00016599063231850116, 0.00025002773722627736, 0.00048640080971659917, 0.0008574796854521624, 0.0013845469543147207, 0.0019340989399293286, 0.002450624, 0.003119818181818182, 0.00518013880126183, 0.011620722279792745]
  # queryList2 = [0.0008745925058548009, 0.0008951445255474453, 0.0010676221322537113, 0.00139064875491481, 0.0022807677664974622, 0.0032176996466431094, 0.005587749714285715, 0.006172984287317621, 0.015330617245005259, 0.1047135274611399]
  # queryList2 = [0.0010785433255269322, 0.001183160583941605, 0.0012251417004048594, 0.0014627614678899077, 0.001631749999999998, 0.001915108362779739, 0.0025346582857142903, 0.0026178316498316483, 0.004002253417455311, 0.011812788601036281]
  # queryList2 = [0.000851231850117097, 0.0009633532846715341, 0.0009565883940620771, 0.0010794718217562258, 0.001320187817258885, 0.001468820965842167, 0.0018628857142857162, 0.0020209786756453384, 0.0030647686645636145, 0.009090883937823813]
  queryList2 = [1.623185011709595e-05, 2.3065693430656937e-05, 5.8993252361673106e-05, 0.00014281913499344622, 0.00032702411167512887, 0.0004985265017667858, 0.000887157714285712, 0.000968673400673398, 0.002207691903259727, 0.00862582383419686]

  x = np.arange(len(xList))
  w = 0.3
  ax.bar(x-0.5*w, queryList1, w, label='approx', color='b')
  ax.bar(x+0.5*w, queryList2, w, label='exact', color='r')
  ax.set_ylabel('Influence query time')
  # ax.set_yscale("log")
  ax.set_xlabel('sample size')
  ax.set_xticks(x)
  ax.set_xticklabels(xList)
  ax.legend()
  plt.show()


def drawInflQueryTimesDiff_7AVG_Logscale():
  ax = plt.subplot(111)
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  queryList1 = [0.00016599063231850116, 0.00025002773722627736, 0.00048640080971659917, 0.0008574796854521624, 0.0013845469543147207, 0.0019340989399293286, 0.002450624, 0.003119818181818182, 0.00518013880126183, 0.011620722279792745]
  # queryList2 = [0.0008745925058548009, 0.0008951445255474453, 0.0010676221322537113, 0.00139064875491481, 0.0022807677664974622, 0.0032176996466431094, 0.005587749714285715, 0.006172984287317621, 0.015330617245005259, 0.1047135274611399]
  # queryList2 = [0.0010785433255269322, 0.001183160583941605, 0.0012251417004048594, 0.0014627614678899077, 0.001631749999999998, 0.001915108362779739, 0.0025346582857142903, 0.0026178316498316483, 0.004002253417455311, 0.011812788601036281]
  # queryList2 = [0.000851231850117097, 0.0009633532846715341, 0.0009565883940620771, 0.0010794718217562258, 0.001320187817258885, 0.001468820965842167, 0.0018628857142857162, 0.0020209786756453384, 0.0030647686645636145, 0.009090883937823813]
  queryList2 = [1.623185011709595e-05, 2.3065693430656937e-05, 5.8993252361673106e-05, 0.00014281913499344622, 0.00032702411167512887, 0.0004985265017667858, 0.000887157714285712, 0.000968673400673398, 0.002207691903259727, 0.00862582383419686]
  queryList = [ele1-ele2 for ele1, ele2 in zip(queryList1, queryList2)]

  plt.bar(xList, queryList, color="blue")
  plt.xlabel("number of tuples")
  plt.ylabel("influence query time of all related tuples")
  plt.show()


def drawInflQueryTimes_8AVG_Logscale():
  ax = plt.subplot(111)
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  queryList1 = [0.00018817224880382776, 0.00028358925979680695, 0.00048099617346938775, 0.0007562613365155131, 0.0009523053960964409, 0.0010464454244762957, 0.0010977248394004285, 0.001843921106557377, 0.0027388465346534652, 0.0032127062615101287]
  queryList2 = [0.0008622966507177033, 0.0009067140783744556, 0.0009898354591836734, 0.0011177374701670643, 0.0011818805970149253, 0.0012453605292171996, 0.0012736573875803, 0.0024333237704918034, 0.005076008910891088, 0.006671186003683241]
  x = np.arange(len(xList))
  w = 0.3
  ax.bar(x-0.5*w, queryList1, w, label='approx', color='b')
  ax.bar(x+0.5*w, queryList2, w, label='exact', color='r')
  ax.set_ylabel('Influence query time')
  ax.set_yscale("log")
  ax.set_xlabel('sample size')
  ax.set_xticks(x)
  ax.set_xticklabels(xList)
  ax.legend()
  plt.show()


def drawTupleNumber_Logscale():
  ax = plt.subplot(111)
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  queryList1 = [1.2014051522248244, 1.4496350364963504, 2.1417004048582995, 3.0235910878112713, 4.362944162436548, 5.730270906949352, 7.688, 7.927048260381594, 11.990536277602523, 22.56062176165803]
  queryList2 = [0.1617900351288056, 0.29600839753466873, 0.4953646528417818, 0.7535317385321101, 1.0699165395136778,
                1.6836761851289834, 2.0418132129909368, 2.5484677601809955, 4.498209130434782, 7.109328710644677]
  x = np.arange(len(xList))
  w = 0.3
  ax.bar(x-0.5*w, queryList1, w, label='tuple number', color='b')
  ax.bar(x+0.5*w, queryList2, w, label='prob query time', color='r')
  # ax.set_ylabel('Influence query time')
  ax.set_yscale("log")
  ax.set_xlabel('sample size')
  ax.set_xticks(x)
  ax.set_xticklabels(xList)
  ax.legend()
  plt.show()


def drawCliqueNumber_Logscale():
  ax = plt.subplot(111)
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  queryList1 = [2.6955503512880563, 3.578102189781022, 5.89608636977058, 9.10878112712975, 13.605329949238579, 19.12603062426384, 27.806857142857144, 30.578002244668912, 46.19032597266036, 93.01036269430051]
  queryList2 = [0.00010991100702576098, 0.0001740175182481752, 0.00033010391363023006, 0.0006114652686762776, 0.0008949682741116748, 0.0012785170789163737, 0.0019138102857142885, 0.0020643322109988767, 0.003341485804416405, 0.00738495751295337]
  x = np.arange(len(xList))
  w = 0.3
  ax.bar(x-0.5*w, queryList1, w, label='approx', color='b')
  ax.bar(x+0.5*w, queryList2, w, label='exact', color='r')
  ax.set_ylabel('Influence query time')
  ax.set_yscale("log")
  ax.set_xlabel('sample size')
  ax.set_xticks(x)
  ax.set_xticklabels(xList)
  ax.legend()
  plt.show()


def drawTupleNumber_11_Logscale():
  ax = plt.subplot(111)
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  queryList1 = [2.5849056603773586, 3.2642045454545454, 4.014844804318488, 5.136422136422136, 5.897149938042132, 6.141190198366394, 14.230936819172113, 17.97257383966245, 22.804554079696395, 24.391304347826086]
  queryList2 = [9.370020964360592e-05, 0.00018930113636363548, 0.0002717165991902844, 0.0004482277992277989, 0.0006381734820322162, 0.0006515472578763092, 0.0037706448801742916, 0.005627967299578072, 0.008833262808349144, 0.01018415818686402]
  x = np.arange(len(xList))
  w = 0.3
  ax.bar(x-0.5*w, queryList1, w, label='approx', color='b')
  ax.bar(x+0.5*w, queryList2, w, label='exact', color='r')
  ax.set_ylabel('Influence query time')
  ax.set_yscale("log")
  ax.set_xlabel('sample size')
  ax.set_xticks(x)
  ax.set_xticklabels(xList)
  ax.legend()
  plt.show()


def drawProbTimes():
  errors = ["1e-1", "1e-2", "1e-3", "1e-4", "1e-5", "1e-6", "1e-7"]
  iterations = [2, 7, 13, 20, 26, 33, 39]
  plt.bar(errors, iterations, color="blue")
  plt.xlabel("approx error")
  plt.ylabel("iterations to converge")
  plt.show()


def drawQueries():
  '''
  avg graph time: 0.0202991
  avg prob time: 0.010455
  avg approx time: nan
  avg influ time: 3.06145
  avg counter time: 3.05681

  avg graph time: 0.0204404
  avg prob time: 0.0102869
  avg approx time: 0.0556107
  avg influ time: 0.00423326
  avg counter time: 0.00450855
  '''

  fig, ax = plt.subplots(figsize=(10, 6))
  ax2 = ax.twinx()
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1k"]
  w = 0.36
  x = np.arange(len(xList))
  
  graph_times = [4, 5, 6, 7, 7, 8, 9, 8, 9, 10]
  ax.bar(x-0.5*w, graph_times, w, label='Graph query', color="tab:blue", edgecolor="black", linewidth=0)

  approx_times = [8, 8, 10, 20, 43, 100, 160, 206, 227, 301]
  ax.bar(x-0.5*w, approx_times, w, bottom=graph_times, color="tab:purple", label="Approx query", edgecolor="black", linewidth=0)

  # prob_times_before = [0, 0, 0, 0, 0, 0, 0, 0, 0, 10]

  influ_times = [0.02, 0.04, 0.08, 0.1, 0.3, 0.75, 1.0, 1.2, 1.8, 3.061]
  ax2.bar(x+0.5*w, influ_times, w, label='Influ query w/o approx query', color="tab:orange", hatch='++', edgecolor="black", linewidth=0)

  counter_times = [0.04, 0.1, 0.2, 0.45, 1, 1.9, 2.3, 3, 5.5, 9.061]
  ax2.bar(x+0.5*w, counter_times, w, bottom=influ_times, label='CFE query w/o approx query', color="tab:green", hatch='++', edgecolor="black", linewidth=0)
  
  influ_times_approx = [2, 4, 6, 8, 12, 16, 20, 30, 36, 41]
  ax.bar(x-0.5*w, influ_times_approx, w, bottom=[graph_times[i]+approx_times[i] for i in range(10)], label="Influ query with approx query", color="tab:orange", hatch='xx', edgecolor="black", linewidth=0)

  counter_times_approx = [4, 8, 12, 15, 24, 50, 56, 85, 100, 123]
  ax.bar(x-0.5*w, counter_times_approx, w, bottom=[graph_times[i]+approx_times[i]+influ_times_approx[i] for i in range(10)], label="CFE query with approx query", color="tab:green", hatch='xx', edgecolor="black", linewidth=0)
  
  box = ax.get_position()
  ax.set_position([box.x0, box.y0+box.height*0.1, box.width, box.height*0.8])

  fs1 = 24
  fs2 = 18
  ax.set_ylabel('Time (ms)', fontsize=fs1)
  ax.set_xlabel('Sample size', fontsize=fs1)
  ax2.set_ylabel("Time (s)", fontsize=fs1)
  ax2.set_ylim(0, 14)
  ax.set_xticks(x)
  ax.set_xticklabels(xList, fontsize=fs2)
  ax.set_ylim(0, 502)
  yList1 = list(range(0, 501, 40))
  ax.set_yticks(yList1)
  ax.set_yticklabels([str(ele) for ele in yList1], fontsize=fs2)
  yList2 = list(range(0, 14, 2))
  ax2.set_yticks(yList2)
  ax2.set_yticklabels([str(ele) for ele in yList2], fontsize=fs2)
  ax.set_yscale("linear")
  fig.legend(fontsize=16, ncol=1, loc='upper center', bbox_to_anchor=(0.35,0.8),fancybox=True)
  ax.grid(axis='y')
  plt.savefig("./data/hypertext-class/sample7/images/queries.pdf")


def drawApprox():
  fig, ax = plt.subplots(figsize=(10, 6))
  ax2 = ax.twinx()
  # xList = ["0.001", "0.002", "0.003", "0.004", "0.005", "0.006", "0.007", "0.008", "0.009", "0.01"]
  # cliques = [7.3, 6.36, 5.8, 5.6, 5.52, 5.26, 5.21, 5.15, 5.26, 3.57]
  # approx_time = [815, 733, 737, 584, 569, 541, 539, 542, 541, 269]
  # infl_time = [50, 43, 36, 40, 40, 37, 36, 39, 37, 19]
  # counter_time = [375, 350, 338, 377, 336, 337, 340, 337, 337, 168]
  # xList = ["0.001", "0.002", "0.004", "0.01"]
  # cliques = [7.3, 6.36, 5.6, 3.57]
  # approx_time = [815, 733, 584,269]
  # infl_time = [50, 43, 40, 19]
  # counter_time = [375, 350, 377, 168]

  # xList = ["0.001", "0.002", "0.003", "0.004", "0.005", "0.006", "0.007", "0.008", "0.009", "0.01"]
  # cliques = [6.08, 4.87, 4.31, 4.11, 3.88, 3.68, 3.61, 3.58, 3.49, 3.18]
  # approx_time = [816, 656, 559, 451, 377, 399, 344, 382, 402, 334]
  # infl_time = [25, 19, 19.2, 18.1, 18.5, 18.5, 16.6, 18, 19, 14]
  # counter_time = [319/2, 295/2, 278/2, 273/2, 282/2, 287/2, 263/2, 297/2, 311/2, 215/2]
  xList = ["0.0001", "0.001", "0.002", "0.003", "0.004", "0.005", "0.007","0.01"]
  cliques = [8.74, 6.08, 4.87, 4.31, 4.11, 3.88, 3.61, 3.18]
  approx_time = [1496, 816, 656, 559, 451, 377, 344, 334]
  infl_time = [51, 25, 19, 19.2, 18.1, 18.5, 16.6, 14]
  counter_time = [521/2, 319/2, 295/2, 278/2, 273/2, 282/2, 263/2, 215/2]
  x = np.arange(len(xList))
  w = 0.36
  lw = 2
  ax.bar(x-0.5*w, approx_time, w, label='Approx query', color="tab:purple", edgecolor="black", linewidth=lw)
  ax.bar(x+0.5*w, infl_time, w, label="Infl query", color="tab:orange", edgecolor="black", linewidth=lw)
  ax.bar(x+0.5*w, counter_time, w, bottom=infl_time, label="Cnt query", color="tab:green", edgecolor="black", linewidth=lw)
  ax2.plot(x, cliques, '^-', label="Clique number", color="tab:blue",  linewidth=lw, markersize=8)
  box = ax.get_position()
  ax.set_position([box.x0, box.y0+box.height*0.1, box.width, box.height*0.8])

  fs1 = 20
  fs2 = 16
  ax.set_xlabel('Sample size', fontsize=fs1)
  ax.set_xticks(x)
  ax.set_xticklabels(xList, fontsize=fs2)
  ax.set_ylabel('Time (ms)', fontsize=fs1)
  # ax.set_yscale("log")
  # ax.set_ylim(0, 1600)
  # yList1 = [0, 10, 100, 1000]
  # ax.set_yticks(yList1)
  # ax.set_yticklabels([str(ele) for ele in yList1], fontsize=fs2)
  ax.get_yaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
  # ax2.set_ylim(0, 9)
  # ax2.set_yscale("log")
  ax2.set_ylabel("Clique number", fontsize=fs1)
  # ax2.set_yticklabels([str(ele) for ele in yList2], fontsize=fs2)
  ax2.get_yaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
  yList2 = [3, 4, 5, 6, 7]
  ax2.set_yticks(yList2)
  fig.legend(fontsize=15, loc='upper center', bbox_to_anchor=(0.38,0.8),fancybox=True)
  ax.grid(axis='y')
  plt.savefig("./data/hypertext-class/sample7/images/approx.pdf")


def overallComparison():
  iters = [23, 37, 74, 83, 90, 39, 43, 43, 109, ]
  # alchemy_total = [1.04, 1.32, 1.68, 1.89, 2.09, 2.94, 2.67, 3.07, 3.1, ]
  alchemy_grounding = [148, 180, 184, 188, 204, 212, 304, 320, 356, 360]
  # alchemy_bp = [170, 220, 260, 340, 430, 480, 560, 760, 810, 960] # ms
  alchemy_bp = [95, 121*2, 131*3, 146*4, 154*5, 175*6, 187*7, 211*8, 229*9, 269*10]
  

  fig, ax = plt.subplots(figsize=(16, 8))
  ax2 = ax.twinx()
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1k"]
  w = 0.24
  x = np.arange(len(xList))

  # ax.bar(x-w, alchemy_bp, w, label='Probability Inference of Alchemy', color="crimson", edgecolor="black", linewidth=0, hatch='--', alpha=0.7)
  ax.plot(x, alchemy_bp, label='Total running time on Alchemy', marker="D", markersize=15, color="tab:orange", linewidth=3, alpha=0.7)
  
  # graph_times = [4, 5, 6, 7, 7, 8, 9, 8, 9, 10]
  # ax.bar(x, graph_times, w, label='Graph Query', color="tab:blue", edgecolor="black", linewidth=0, hatch='xx')

  approx_times = [8, 8, 10, 20, 43, 100, 160, 206, 227, 301]
  ax.bar(x-w/2, approx_times, w, color="tab:purple", label="Approx Subgraph Searching", linewidth=0, hatch='xx', alpha=0.7)

  # prob_times_before = [0, 0, 0, 0, 0, 0, 0, 0, 0, 10]

  influ_times = [0.02, 0.04, 0.08, 0.1, 0.3, 0.75, 1.0, 1.2, 1.8, 3.061]
  ax2.bar(x+w/2, influ_times, w, label='ICE w/o Approx Query', color="tab:orange", hatch='++', edgecolor="black", linewidth=0, alpha=0.7)

  counter_times = [0.04, 0.1, 0.2, 0.45, 1, 1.9, 2.3, 3, 5.5, 9.061]
  ax2.bar(x+w/2, counter_times, w, bottom=influ_times, label='CFE w/o Approx Query', color="tab:green", hatch='++', edgecolor="black", linewidth=0, alpha=0.7)
  
  influ_times_approx = [2, 4, 6, 8, 12, 16, 20, 30, 36, 41]
  ax.bar(x-w/2, influ_times_approx, w, bottom=approx_times, label="ICE w/ Approx Query", color="tab:orange", hatch='xx', edgecolor="black", linewidth=0, alpha=0.7)

  counter_times_approx = [4, 8, 12, 15, 24, 50, 56, 85, 100, 123]
  ax.bar(x-w/2, counter_times_approx, w, bottom=[approx_times[i]+influ_times_approx[i] for i in range(10)], label="CFE w/ Approx Query", color="tab:green", hatch='xx', edgecolor="black", linewidth=0, alpha=0.7)
  
  box = ax.get_position()

  fs1 = 24
  fs2 = 18
  ax.set_ylabel('Time (ms)', fontsize=fs1)
  ax.set_xlabel('Sample size', fontsize=fs1)
  ax2.set_ylabel("Time (s)", fontsize=fs1)
  ax2.set_ylim(0, 14)
  ax.set_xticks(x)
  ax.set_xticklabels(xList, fontsize=fs2)
  # ax.set_ylim(0, 1000)
  # yList1 = list(range(0, 1001, 200))
  # ax.set_yticks(yList1)
  # ax.set_yticklabels([str(ele) for ele in yList1], fontsize=fs2)
  yList2 = list(range(0, 14, 2))
  ax2.set_yticks(yList2)
  ax2.set_yticklabels([str(ele) for ele in yList2], fontsize=fs2)
  ax.set_yscale("linear")
  fig.legend(fontsize=20, ncol=1, loc='upper center', bbox_to_anchor=(0.26,0.98),fancybox=True)
  # ax.legend(fontsize=20)
  ax.grid(axis='y')
  # plt.tight_layout()
  plt.savefig("./data/hypertext-class/sample7/images/overall_3.pdf")


if __name__ == '__main__':
  # drawQueryTimes()
  # drawProbTimes()
  # drawQueryTimes_4()
  # drawQueryTimes_5()
  # drawQueryTimes_7()
  # drawQueryTimes_11()
  drawMaintenanceTime_7()
  # drawProbQueryTimes_7AVG()
  # drawProbQueryTimes_7AVG_Logscale()
  # drawProbQueryTimes_8AVG()
  # drawInflQueryTimes_7()
  # drawInflQueryTimes_7AVG()
  # drawInflQueryTimes_7AVG_equa()
  # drawInflQueryTimes_7AVG_Logscale()
  # drawInflQueryTimesDiff_7AVG_Logscale()
  # drawInflQueryTimes_8AVG_Logscale()
  # drawTupleNumber_Logscale()
  # drawTupleNumber_11_Logscale()

  # drawQueries()
  # drawApprox()
  overallComparison()

