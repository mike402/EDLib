//
// Created by iskakoff on 19/07/16.
//

#ifndef HUBBARD_HAMILTONIAN_H
#define HUBBARD_HAMILTONIAN_H

#include <vector>
#include <type_traits>

#include <fstream>
#include "Symmetry.h"
#include "EigenPair.h"
#include "EDParams.h"
#include "HubbardModel.h"
#include "CRSStorage.h"
#include "SOCRSStorage.h"

template<typename prec, class Storage, class Model>
class Hamiltonian {
public:
  typedef Model ModelType;
  /*
   * Allocate space for Hamiltonian matrix
   * \param [in] max_size - the maximum size of values array
   * \param [in] max_dim - the maximum dimension of Hamiltonian matrix
   * \param [in] p - alps::parameters
   */
  Hamiltonian(EDParams& p) :
    _model(p),
    _storage(p, _model){};

  /**
   * fill current sector
   */
  void fill() {
    _model.symmetry().init();
    _storage.reset(_model.symmetry().sector().size());
    int i =0;
    long long k = 0;
    int isign = 0;
    while (_model.symmetry().next_state()) {
      long long nst = _model.symmetry().state();
      // Compute diagonal element for current i state

      _storage.addDiagonal(i, _model.diagonal(nst));
      // non-diagonal terms calculation
      for(auto & state: _model.states()) {
        if(_model.valid(state, nst)) {
          _model.set(state, nst, k, isign);
          hopping(i, nst, k, state.value(), isign);
        }
      }
      i++;
    }
    // additional steps after all data
    _storage.endMatrix();
  }
  /**
   * perform Hamiltonian diagonalization
   * result will be stored in evals and evecs
   */
  void diag() {
    while(_model.symmetry().next_sector()) {
      fill();
      /**
       * perform ARPACK call
       */
      int info = _storage.diag();
      if(info != 0) {

      } else {
        const std::vector<prec>& evals = _storage.eigenvalues();
        const std::vector<std::vector<prec> >& evecs = _storage.eigenvectors();
        for(int i = 0; i<evals.size(); ++i) {
          _eigenpairs.push_back(EigenPair<prec, typename Model::Sector>(evals[i], evecs[i], _model.symmetry().sector()));
        }
      }
    }
    std::sort(_eigenpairs.begin(), _eigenpairs.end());
    std::cout<<"Here is the list of eigenvalues:"<<std::endl;
    for(auto& eigenpair : _eigenpairs) {
      std::cout<<eigenpair.eigenvalue()<<" ";
      eigenpair.sector().print();
      std::cout<<std::endl;
    }
  }

  Storage& storage() {
    return _storage;
  }

  const std::vector<EigenPair<prec, typename Model::Sector> > & eigenpairs() const {
    return _eigenpairs;
  };

  Model &model(){
    return _model;
  }
private:
  // CSR format Hamiltonian matrix storage
  Storage _storage;

  // Eigen-pairs
  std::vector<EigenPair<prec, typename Model::Sector> > _eigenpairs;

  /**
   * Model to diagonalize
   */
  Model _model;


  /**
   * \param i - current Hamiltonian matrix line
   * \param nst - current state
   * \param k - state to hop between
   * \param v - hopping value
   * \param sector - current conservation law sector
   */
  void inline hopping(const int& i, const long long& nst, const long long& k, const prec &v, const int &sign) {
    int k_index = _model.symmetry().index(k);
    _storage.addElement(i, k_index, v, sign);
  }

};

typedef Hamiltonian<double, CRSStorage<double> , HubbardModel<double> > CSRHubbardHamiltonian;
typedef Hamiltonian<double, SOCRSStorage<double, HubbardModel<double> > , HubbardModel<double> > SOCSRHubbardHamiltonian;

typedef Hamiltonian<double, CRSStorage<float> , HubbardModel<float> > CSRHubbardHamiltonian_float;
typedef Hamiltonian<double, SOCRSStorage<float, HubbardModel<float> > , HubbardModel<float> > SOCSRHubbardHamiltonian_float;

#endif //HUBBARD_HAMILTONIAN_H
