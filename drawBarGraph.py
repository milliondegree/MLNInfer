import matplotlib.pyplot as plt 
import numpy as np

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
  ax = plt.subplot(111)
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  queryList1 = [29.8, 59.1, 90.07, 120.8, 150.5, 186, 206.3, 237.3, 268.6, 296.1]
  queryList2 = [7.70942, 15.3306, 23.4762, 29.7336, 39.1901, 46.2846, 54.4626, 60.7309, 67.4802, 75.6743]
  x = np.arange(len(xList))
  w = 0.3
  ax.bar(x-0.5*w, queryList2, w, label='maintenance without provenance', color='b')
  ax.bar(x+0.5*w, queryList1, w, label='maintenance with provenance', color='r')
  ax.set_ylabel('maintenance time')
  ax.set_yscale("linear")
  ax.set_xlabel('sample size')
  ax.set_xticks(x)
  ax.set_xticklabels(xList)
  ax.legend()
  plt.show()


def drawQueryTimes_7():
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  # queryList1 = [0.196, 0.343, 0.567, 0.834, 1.136, 1.778, 2.14, 2.66, 4.56, 7.18]
  queryList1 = [0.1617900351288056, 0.29600839753466873, 0.4953646528417818, 0.7535317385321101, 1.0699165395136778,
                1.6836761851289834, 2.0418132129909368, 2.5484677601809955, 4.498209130434782, 7.109328710644677]
  # queryList2 = [0.274, 0.58, 0.94, 1.637, 2.174, 2.68, 3.2, 4.05, 5.3, 6.37]
  queryList2 = [0.23808514354066984, 0.5371160580152672, 0.8954505701357466, 1.5968827611940297, 2.140646583703704,
                2.6401818698224853, 3.1556854452662724, 4.0063278197932055, 5.25113364844904, 6.307514858197933]
  ql1 = np.array(queryList1)
  ql2 = np.array(queryList2)
  ql = (ql1+ql2)/2
  queryList = list(ql)

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
  queryList1 = [9.44847775175644e-05, 0.00013946569343065694, 0.0002791524966261808, 0.0004687876802096986, 0.0007806840101522842, 0.0010874110718492344, 0.0012913897142857143, 0.0017384758698092031, 0.002800705573080968, 0.0061624632124352334]
  queryList2 = [0.095999131147541, 0.13945208905109488, 0.24944783130904186, 0.41301451245085186, 0.6425268667512689, 0.9123236230859836, 1.333311952, 1.4630163928170594, 2.2927532260778127, 4.74799883626943]
  x = np.arange(len(xList))
  w = 0.3
  ax.bar(x-0.5*w, queryList1, w, label='iterative', color='b')
  ax.bar(x+0.5*w, queryList2, w, label='sampling', color='r')
  ax.set_ylabel('Probability query time')
  ax.set_yscale("log")
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
  queryList2 = [0.0008745925058548009, 0.0008951445255474453, 0.0010676221322537113, 0.00139064875491481, 0.0022807677664974622, 0.0032176996466431094, 0.005587749714285715, 0.006172984287317621, 0.015330617245005259, 0.1047135274611399]

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


if __name__ == '__main__':
  # drawQueryTimes()
  # drawProbTimes()
  # drawQueryTimes_4()
  # drawQueryTimes_5()
  # drawQueryTimes_7()
  drawMaintenanceTime_7()
  # drawProbQueryTimes_7AVG()
  drawProbQueryTimes_7AVG_Logscale()
  # drawProbQueryTimes_8AVG()
  # drawInflQueryTimes_7()
  # drawInflQueryTimes_7AVG()
  # drawInflQueryTimes_7AVG_equa()
  drawInflQueryTimes_7AVG_Logscale()
