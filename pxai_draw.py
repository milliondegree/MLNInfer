import matplotlib
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib import gridspec
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
  if n.is_integer():
    n = int(n)
  return str(n)+substring


def alchemyInfluenceTime():
  fig, ax = plt.subplots(figsize=(6, 6))
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1k"]
  x = np.arange(len(xList))

  y = [15, 18*2, 27*3, 33*4, 36*5, 44*6, 56*7, 68*8, 79*9, 90*10]
  y_total = [95, 121*2, 131*3, 146*4, 154*5, 175*6, 187*7, 211*8, 229*9, 269*10]

  ax.plot(x, y, marker="*", markersize=20, color="tab:red", linewidth=3, alpha=0.8, label="Model inference time")
  ax.plot(x, y_total, marker="D", markersize=15, color="tab:orange", linewidth=3, alpha=0.8, label="Total running time")


  fs1 = 24
  fs2 = 20
  ax.set_ylabel('Time (s)', fontsize=fs1)
  ax.set_xlabel('Number of input features', fontsize=fs1)
  ax.set_xticks(x)
  ax.set_xticklabels(xList, fontsize=16)

  y_ticks = range(0, 3000, 300)
  y_ticklabels = [numberToStr(ele*1.0) for ele in y_ticks]
  ax.set_yticks(y_ticks)
  ax.set_yticklabels(y_ticklabels, fontsize=fs2)
  ax.grid()
  ax.legend(fontsize=fs2)
#   ax.set_yscale("log")
  plt.tight_layout()
  plt.savefig("./data/hypertext-class/sample7/images/alchemy_influence_time.pdf")

def drawMaintenanceQuery():
  fig = plt.figure(figsize=(13, 5.5))
  gs = gridspec.GridSpec(1, 2, width_ratios=[4.4, 3.6])
  # ax1 = fig.add_subplot(1, 2, 1)
  ax1 = plt.subplot(gs[0])
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1k"]
  origin = [0.063348, 0.142561, 0.209507, 0.273852, 0.348826, 0.419871, 0.484845, 0.553744, 0.621982, 0.690081 ]
  maintenance_fine = [0.424083, 0.887966, 1.31785, 1.81795, 2.24077, 2.64885, 3.07401, 3.68914, 4.05341, 4.39845 ]
  query_fine = [0.0011376, 0.0014631, 0.00197892, 0.00302829, 0.00391794, 0.00522415, 0.00588835, 0.00659315, 0.00801068, 0.00910626,]
  query_fine = np.array(query_fine)*1000
  maintenance_coarse = [0.206752, 0.476931, 0.766481, 0.98802, 1.23495, 1.54035, 1.76545, 1.99586, 2.22024, 2.41902]
  query_coarse = [0.000264171, 0.000294356, 0.000330009, 0.000365438, 0.0003871, 0.000420653, 0.000438427, 0.000464096, 0.000474362, 0.000492065]
  query_coarse = np.array(query_coarse)*1000
  x = np.arange(len(xList))
  w = 0.25
  lw = 2
  ax1.bar(x-w, origin, w, label='W/o provenance', color="tab:blue", edgecolor="black", linewidth=lw)
  ax1.bar(x, maintenance_fine, w, label='W/ finer provenance', color="tab:orange", edgecolor="black", linewidth=lw)
  ax1.bar(x+w, maintenance_coarse, w, label="W/ coarser provenance", color="tab:green", edgecolor="black", linewidth=lw)

  ax2 = plt.subplot(gs[1])
  ax2.plot(x, query_fine, label="Finer provenance", marker="^", markersize=15, color="tab:orange", linewidth=3, alpha=0.9)
  ax2.plot(x, query_coarse, label="Coarser provenance", marker="s", markersize=15, color="tab:green", linewidth=3, alpha=0.9)
  # box = ax.get_position()
  # ax.set_position([box.x0, box.y0+box.height*0.1, box.width, box.height*0.8])

  fs1 = 24
  fs2 = 20
  ax1.set_ylabel('Model inference time (s)', fontsize=fs1)
  ax1.set_yscale("linear")
  ax1.set_xlabel('Sample size\n(a)', fontsize=fs1)
  ax1.set_xticks(x)
  ax1.set_xticklabels(xList, fontsize=fs2)
  ax1.tick_params(axis="y", which="both", labelsize=fs2)
  ax1.tick_params(axis="y", which="both", labelsize=fs2)
  ax2.set_ylabel('Backward trace time (ms)', fontsize=fs1)
  ax2.set_yscale("linear")
  ax2.set_xlabel('Sample size\n(b)', fontsize=fs1)
  ax2.set_xticks(x)
  ax2.set_xticklabels(xList, fontsize=fs2)
  ax2.tick_params(axis="y", which="both", labelsize=fs2)
  # ax.set_yticks(list(range(0, 301, 50)))
  # ax.set_yticklabels([str(ele) for ele in list(range(0, 301, 50))], fontsize=fs2)
  # ax2.set_yticks(list(range(0, 9, 1)))
  # ax2.set_yticklabels([str(ele) for ele in list(range(0, 8))], fontsize=fs2)
  # fig.legend(fontsize=19, loc='upper center', bbox_to_anchor=(0.3,0.8),fancybox=True)
  ax1.grid()
  ax2.grid()
  ax1.legend(fontsize=fs2)
  ax2.legend(fontsize=fs2)
  plt.tight_layout()
  plt.savefig("./data/hypertext-class/sample7/images/pxai_maintain_query_coarse.pdf")


def overallComparison():
  # alchemy_bp = [95, 121*2, 131*3, 146*4, 154*5, 175*6, 187*7, 211*8, 229*9, 269*10]
  alchemy_bp = [15, 18*2, 27*3, 33*4, 36*5, 44*6, 56*7, 68*8, 79*9, 90*10]
  alchemy_bp = np.array(alchemy_bp)
  # alchemy_bp *= 1000
  

  fig, ax = plt.subplots(figsize=(16, 7))
  ax2 = ax.twinx()
  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1k"]
  w = 0.3
  lw = 2
  x = np.arange(len(xList))

  ax2.plot(x, alchemy_bp, label='ICE on Alchemy (LBP only)', marker="D", markersize=20, color="tab:blue", linewidth=4, alpha=0.9)
  # ax.plot(x, alchemy_bp, label='ICE on Alchemy (LBP only)', marker="D", markersize=20, color="tab:blue", linewidth=4, alpha=0.9)

  approx_times = [0.00547228, 0.00469242, 0.00479458, 0.00544853, 0.00569537, 0.00629687, 0.00644849, 0.00663826, 0.00715675, 0.00755559, ]
  approx_times = np.array(approx_times)*1000
  ax.bar(x-w/2, approx_times, w, color="tab:purple", label="Approx subgraph searching", edgecolor="black", linewidth=lw, hatch='x', alpha=0.7)

  influ_times = [0.000374841, 0.00072196, 0.00117098, 0.00237264, 0.00352312, 0.00645057, 0.00869729, 0.0106812, 0.0140727, 0.0173866, ]
  influ_times = np.array(influ_times)*1000
  ax.bar(x+w/2, influ_times, w, label='ICE on original graph', color="tab:orange", hatch='.', edgecolor="black", linewidth=lw, alpha=0.7)

  # counter_times = [0.04, 0.1, 0.2, 0.45, 1, 1.9, 2.3, 3, 5.5, 9.061]
  # ax.bar(x+w/2, counter_times, w, bottom=influ_times, label='CFE w/o Approx Query', color="tab:green", hatch='++', edgecolor="black", linewidth=0, alpha=0.7)
  
  influ_times_approx = [0.000157281, 0.000232689, 0.000351128, 0.000627773, 0.000951316, 0.00250335, 0.00387824, 0.00494315, 0.00629363, 0.00786202, ]
  influ_times_approx = np.array(influ_times_approx)*1000
  ax.bar(x-w/2, influ_times_approx, w, bottom=approx_times, label="ICE on approx subgraph", color="tab:orange", hatch='x', edgecolor="black", linewidth=lw, alpha=0.7)

  # counter_times_approx = [4, 8, 12, 15, 24, 50, 56, 85, 100, 123]
  # ax.bar(x-w/2, counter_times_approx, w, bottom=[approx_times[i]+influ_times_approx[i] for i in range(10)], label="CFE w/ Approx Query", color="tab:green", hatch='xx', edgecolor="black", linewidth=0, alpha=0.7)
  
  box = ax.get_position()
  ax.set_position([box.x0, box.y0+box.height*0.05, box.width, box.height*0.95])

  fs1 = 30
  fs2 = 26
  ax.set_ylabel('PXAI running time (ms)', fontsize=fs1)
  ax.set_xlabel('Sample size', fontsize=fs1)
  ax2.set_ylabel("Alchemy running time (s)", fontsize=fs1)
  ax.set_xticks(x)
  ax.set_xticklabels(xList, fontsize=fs2)
  ax.tick_params(axis="y", which="both", labelsize=fs2)
  ax2.tick_params(axis="y", which="both", labelsize=fs2)
  yList2 = list(range(0, 14, 2))
  # ax2.set_yticks(yList2)
  # ax2.set_yticklabels([str(ele) for ele in yList2], fontsize=fs2)
  ax.set_yscale("linear")
  fig.legend(fontsize=26, ncol=1, loc='upper center', bbox_to_anchor=(0.33,0.88),fancybox=True)
  # ax.legend(fontsize=20)
  ax.grid(axis='y')
  # plt.tight_layout()
  plt.savefig("./data/hypertext-class/sample7/images/pxai-approx-ice-counter.pdf")


def overallComparison2():
  # alchemy_bp = [95, 121*2, 131*3, 146*4, 154*5, 175*6, 187*7, 211*8, 229*9, 269*10]
  alchemy_bp = [15, 18*2, 27*3, 33*4, 36*5, 44*6, 56*7, 68*8, 79*9, 90*10]
  alchemy_bp = np.array(alchemy_bp)
  
  fig = plt.figure(figsize=(13, 5.5))
  gs = gridspec.GridSpec(1, 2, width_ratios=[4.4, 3.6])
  # ax1 = fig.add_subplot(1, 2, 1)
  ax = plt.subplot(gs[0])
  ax1 = plt.subplot(gs[1])

  xList = ["100", "200", "300", "400", "500", "600", "700", "800", "900", "1k"]
  w = 0.3
  lw = 2
  x = np.arange(len(xList))

  ax.plot(x, alchemy_bp, label='ICE on Alchemy (LBP only)', marker="D", markersize=15, color="tab:blue", linewidth=4, alpha=0.9)

  approx_times = [0.00547228, 0.00469242, 0.00479458, 0.00544853, 0.00569537, 0.00629687, 0.00644849, 0.00663826, 0.00715675, 0.00755559, ]
  ax1.bar(x-w/2, approx_times, w, color="tab:purple", label="Subset search", edgecolor="black", linewidth=lw, hatch='x', alpha=0.7)

  approx_time_prune = [0.185705, 0.51779, 0.806564, 2.33705, 5.34172, 6.78155, 7.9102, 10.6765, 18.6857, 20.195,]
  ax1.bar(x+w/2, approx_time_prune, w, color="tab:green", label="Prune", edgecolor="black", linewidth=lw, hatch='-', alpha=0.7)

  influ_times = [0.000374841, 0.00072196, 0.00117098, 0.00237264, 0.00352312, 0.00645057, 0.00869729, 0.0106812, 0.0140727, 0.0173866, ]
  ax.bar(x-w, influ_times, w, label='ICE on PXAI PROV graph', color="tab:orange", hatch='.', edgecolor="black", linewidth=lw, alpha=0.7)

  # counter_times = [0.04, 0.1, 0.2, 0.45, 1, 1.9, 2.3, 3, 5.5, 9.061]
  # ax.bar(x+w/2, counter_times, w, bottom=influ_times, label='CFE w/o Approx Query', color="tab:green", hatch='++', edgecolor="black", linewidth=0, alpha=0.7)
  
  influ_times_approx = [0.000157281, 0.000232689, 0.000351128, 0.000627773, 0.000951316, 0.00250335, 0.00387824, 0.00494315, 0.00629363, 0.00786202, ]
  ax.bar(x, influ_times_approx, w, label="ICE on approx subgraph (subset search)", color="tab:purple", hatch='x', edgecolor="black", linewidth=lw, alpha=0.7)


  influ_time_approx_prune = [0.000206327, 0.000346406, 0.000472525, 0.00107084, 0.00167829, 0.00235278, 0.00330923, 0.00407384, 0.00548656, 0.00631705,]
  ax.bar(x+w, influ_time_approx_prune, w, label="ICE on approx subgraph (prune)", color="tab:green", hatch='-', edgecolor="black", linewidth=lw, alpha=0.7)

  # counter_times_approx = [4, 8, 12, 15, 24, 50, 56, 85, 100, 123]
  # ax.bar(x-w/2, counter_times_approx, w, bottom=[approx_times[i]+influ_times_approx[i] for i in range(10)], label="CFE w/ Approx Query", color="tab:green", hatch='xx', edgecolor="black", linewidth=0, alpha=0.7)
  
  # box = ax.get_position()
  # ax.set_position([box.x0, box.y0+box.height*0.05, box.width, box.height*0.95])

  fs1 = 20
  fs2 = 16
  ax.set_ylabel('Running time of ICE (s)', fontsize=fs1)
  ax.set_xlabel('Sample size\n (a)', fontsize=fs1)
  ax1.set_ylabel('Running time of approx search (s)', fontsize=fs1)
  ax1.set_xlabel('Sample size\n (b)', fontsize=fs1)
  ax.set_yscale("log")
  ax1.set_yscale("log")
  ax.set_xticks(x)
  ax.set_xticklabels(xList, fontsize=fs2)
  ax.set_yticks([0.001, 0.01, 0.1, 1, 10, 100, 1000])
  ax.set_yticklabels(["1e-3","1e-2","1e-1","1","10","1e2", "1e3"], fontsize=fs2)
  ax1.set_xticks(x)
  ax1.set_xticklabels(xList, fontsize=fs2)
  ax1.set_yticks([0.01, 0.1, 1, 10])
  ax1.set_yticklabels(["1e-2","1e-1", "1", "10"], fontsize=fs2)
  ax.tick_params(axis="y", which="both", labelsize=fs2)
  ax1.tick_params(axis="y", which="both", labelsize=fs2)
  # fig.legend(fontsize=26, ncol=1, loc='upper center', bbox_to_anchor=(0.7,0.76),fancybox=True)
  ax.legend(fontsize=fs2)
  ax1.legend(fontsize=fs2)
  ax.grid(axis='y')
  ax1.grid(axis='y')
  plt.tight_layout()
  plt.savefig("./data/hypertext-class/sample7/images/pxai-approx-ice-counter_log_new.pdf")

if __name__=="__main__":
  # alchemyInfluenceTime()
  # drawMaintenanceQuery()
  # overallComparison()
  overallComparison2()