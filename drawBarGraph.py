import matplotlib.pyplot as plt 

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


def drawQueryTimes_7():
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1000"]
  queryList1 = [0.196, 0.343, 0.567, 0.834, 1.136, 1.778, 2.14, 2.66, 4.56, 7.18]
  queryList2 = [0.274, 0.58, 0.94, 1.637, 2.174, 2.68, 3.2, 4.05, 5.3, 6.37]

  plt.bar(xList, queryList, color="blue")
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
  # drawProbQueryTimes_7AVG()
  # drawProbQueryTimes_8AVG()
  # drawInflQueryTimes_7()
  drawInflQueryTimes_7AVG()
