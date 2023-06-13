#ifndef _OPTIMIZER_H
#define _OPTIMIZER_H


#include "MLN.h"


using namespace std;

/*
1: gradient descent
2: momentum
3: nesteriv
4: adam
5: logical optimizer
6: proximal + adam
*/


class Optimizer {
public:

  Optimizer();

  Optimizer(double update_rate) {
    this->update_rate = update_rate;
  }

  virtual void computeGradients() {
  }

  virtual void update(MLN& mln, vector<pair<string, double>>& gradient_vector) = 0;

protected:
  double update_rate;
  string name;
};



// 1
class GradientDescent : public Optimizer {

public:

  GradientDescent(double update_rate) : Optimizer(update_rate) {
    name = "Gradient Descent";
    cout << "Using " << name << endl;
  }

  void update(MLN& mln, vector<pair<string, double>>& gradient_vector) {
    for (auto it : gradient_vector) {
      string obs_literal = it.first;
      double obs_gradient = it.second;
      double tmp = mln.prob[obs_literal]-this->update_rate*obs_gradient;
      mln.prob[obs_literal] = max(0.0, min(1.0, tmp));
    }
  }

};



// 2
class Momentum : public Optimizer {

public:

  Momentum(double update_rate) : Optimizer(update_rate) {
    name = "Momentum";
    cout << "Using " << name << endl;
  }

  void update(MLN& mln, vector<pair<string, double>>& gradient_vector) {
    if (velocity.size()==0) velocity = vector<double> (gradient_vector.size(), 0);
    for (int i=0; i<velocity.size(); i++) {
      string obs_literal = gradient_vector[i].first;
      velocity[i] = gamma*velocity[i] + update_rate*gradient_vector[i].second;
      double tmp = mln.prob[obs_literal] - velocity[i];
      mln.prob[obs_literal] = max(0.0, min(1.0, tmp));
    }
  }

private:
  vector<double> velocity;
  double gamma = 0.9;
};



// 3
class Nesterov : public Optimizer {

public:

  Nesterov(double update_rate) : Optimizer(update_rate) {
    name = "Nesterov";
    cout << "Using " << name << endl;
  }

  void update(MLN& mln, vector<pair<string, double>>& gradient_vector) {
    if (velocity.size()==0) velocity = vector<double> (gradient_vector.size(), 0);
    for (int i=0; i<velocity.size(); i++ ) {
      string obs_literal = gradient_vector[i].first;
      double tmp = mln.prob[obs_literal] - velocity[i]*update_rate;
    }

  }


private:
  vector<double> velocity;
  double gamma = 0.9;
};



// 4
class Adam : public Optimizer {

public:
  Adam(double update_rate) : Optimizer(update_rate) {
    name = "Adam";
    cout << "Using " << name << endl;
    iteration = 1;
  }

  void update(MLN& mln, vector<pair<string, double>>& gradient_vector) {
    if (m.size()==0) {
      m = vector<double> (gradient_vector.size(), 0);
      v = vector<double> (gradient_vector.size(), 0);
    }
    for (int i=0; i<gradient_vector.size(); i++) {
      string obs_literal = gradient_vector[i].first;
      m[i] = beta1*m[i] + (1-beta1)*gradient_vector[i].second;
      v[i] = beta2*v[i] + (1-beta2)*pow(gradient_vector[i].second, 2);
      double m_ = m[i] / (1-pow(beta1, iteration*1.0));
      double v_ = v[i] / (1-pow(beta2, iteration*1.0));
      double tmp = mln.prob[obs_literal] - update_rate * m_ / (sqrt(v_)+epsilon);
      mln.prob[obs_literal] = max(0.0, min(1.0, tmp));
    }
    iteration++;
  }

private:
  int iteration;
  double beta1 = 0.9;
  double beta2 = 0.999;
  double epsilon = 1e-8;
  vector<double> m;
  vector<double> v;
};


// 5
class LogicalOptimizer : public Optimizer {

public:

  LogicalOptimizer(double update_rate) : Optimizer(update_rate) {
    name = "logical optimization";
    // cout << "Using " << name << endl;
  }

  void update(MLN& mln, vector<pair<string, double>>& gradient_vector) {
    for (auto it : gradient_vector) {
      string obs_literal = it.first;
      double obs_gradient = it.second;
      if (Parser::isRuleName(obs_literal)) {
        mln.prob[obs_literal] -= update_rate*obs_gradient;
        break;
      }
      else {
        if (obs_gradient>0 && mln.prob[obs_literal]==1.0) {
          mln.prob[obs_literal] = 0.0;
          break;
        }
        else if (obs_gradient<0 && mln.prob[obs_literal]==0.0) {
          mln.prob[obs_literal] = 1.0;
          break;
        }
        else {
          continue;
        }
      }
    }
  }

private:
  // unordered_set<string> visited;
};


// 6
class ProximalOptimizer : public Optimizer {

public:
  ProximalOptimizer(double update_rate) : Optimizer(update_rate) {
    name = "Proximal Optimizer";
    cout << "Using " << name << endl;
    iteration = 1;
  }

  void update(MLN& mln, vector<pair<string, double>>& gradient_vector) {
    if (m.size()==0) {
      m = vector<double> (gradient_vector.size(), 0);
      v = vector<double> (gradient_vector.size(), 0);
    }
    for (int i=0; i<gradient_vector.size(); i++) {
      string obs_literal = gradient_vector[i].first;
      m[i] = beta1*m[i] + (1-beta1)*gradient_vector[i].second;
      v[i] = beta2*v[i] + (1-beta2)*pow(gradient_vector[i].second, 2);
      double m_ = m[i] / (1-pow(beta1, iteration));
      double v_ = v[i] / (1-pow(beta2, iteration));
      double tmp = mln.prob[obs_literal] - update_rate / (sqrt(v_)+epsilon) * m_;
      tmp = max(0.0, min(1.0, tmp));
      // double diff = 
    }
    iteration++;
  }

private:
  int iteration;
  double beta1 = 0.9;
  double beta2 = 0.999;
  double epsilon = 1e-8;
  vector<double> m;
  vector<double> v;
};


#endif