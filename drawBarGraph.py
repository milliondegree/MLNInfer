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
  queryList = [9.44847775175644e-05, 0.00012698594847775175, 0.0002488618266978923, 0.00044092974238875876, 0.0008102927400468384, 0.001057814988290398, 0.0014867072599531617, 0.001601480093676815, 0.0025454028103044494, 0.005299372365339579]

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
  drawProbQueryTimes_7AVG()
  # drawInflQueryTimes_7()