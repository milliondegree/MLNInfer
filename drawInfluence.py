
import matplotlib
import matplotlib.pyplot as plt
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42


def drawInfluence(p_str):
  probs = p_str.split()
  probs = [float(ele) for ele in probs]
  size = 1.0/(len(probs)-1)
  x = [size*i for i in range(len(probs))]
  plt.plot(x, probs, 'b-')



if __name__ == '__main__':
  strP = "0.63119 0.64252 0.65203 0.6597 0.66917 0.68045 0.6893 0.69806 0.70539 0.71551 0.72563 0.7349 0.74461 0.74778 0.76233 0.76694 0.773 0.7819 0.78956 0.79651 0.80369"
  strP1 = "0.63058 0.635755 0.64228 0.642825 0.64728 0.65304 0.654915 0.661795 0.65977 0.666395 0.668595"
  strP2 = "0.628885 0.65088 0.66749 0.68984 0.70669 0.72344 0.740815 0.76041 0.774985 0.78985 0.8026"

  cancer5_smoke1 = "0.63075 0.63581 0.6386 0.64321 0.64868 0.65236 0.65505 0.65567 0.65905 0.66497 0.66881"
  smoke2_smoke1 = "0.29401 0.31404 0.33202 0.35135 0.37244 0.39164 0.4121 0.43082 0.45931 0.4781 0.49993"
  smoke5_smoke1 = "0.62843 0.65 0.66788 0.69006 0.70511 0.72445 0.74358 0.75825 0.77251 0.78873 0.80274"
  drawInfluence(cancer5_smoke1)
  drawInfluence(smoke2_smoke1)
  drawInfluence(smoke5_smoke1)
  plt.show()