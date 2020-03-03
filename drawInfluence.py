import matplotlib.pyplot as plt




if __name__ == '__main__':
  strP = "0.63119 0.64252 0.65203 0.6597 0.66917 0.68045 0.6893 0.69806 0.70539 0.71551 0.72563 0.7349 0.74461 0.74778 0.76233 0.76694 0.773 0.7819 0.78956 0.79651 0.80369"
  probs = strP.split()
  x = [0.05*i for i in range(21)]
  plt.plot(x, probs, 'b.')
  plt.show()