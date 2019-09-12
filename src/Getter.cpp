#include "Getter.h"

ClassImp(Pid::Getter)

namespace Pid {

// -----   Default constructor   -------------------------------------------
Getter::Getter() = default;

/**
* Constructs fit function as a sum of individual particle species. Parameters are also propagated
* @param p track momentum (or x-axis value, in general)
* @param m2 square mass for TOF (or y-axis value, in general)
* @return map with probabilities for all particle species
*/
std::map<uint, double> Getter::GetBayesianProbability(double p, double m2) {
  std::map<uint, double> prob{};

  if (p > maxx_ || p < minx_)
    return prob;

  double sum{0.};
  for (auto &specie : species_) {
    const double iprob = specie.second.Eval(p, m2);
    sum += iprob;
    prob[specie.first] = iprob;
  }
//     std::cout << sum << std::endl;
  if (sum < 1.) return prob; // no particles in this (p, m2) piont

  for (auto &iprob : prob) {
    iprob.second /= sum;
  }

  return prob;
}

}
