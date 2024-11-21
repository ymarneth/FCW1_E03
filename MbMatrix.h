// MbMatrix.h:                                                 HDO, 2019
// ----------
// Map-based Matrix (MbMatrix) is a generic class for 2D arrays where
// *  the two indices can be of any type providing operator< and
// *  operator[][] is applicable even to const MbMatrix objects.
// A  row   in an MbMatrix is represented by a map-based vector (MbVector).
// An entry in an MbMatrix can be seen as Triple consisting of
//   two indices and a value: (i, j, v).
//======================================================================

#pragma once
#ifndef MbMatrix_h
#define MbMatrix_h

#include <iosfwd>
#include <map>
#include <tuple>
#include <vector>

#include "ObjectCounter.h"


// --- generic class Triple ---

template<typename T1, typename T2, typename T3>
class Triple: public  std::tuple<T1, T2, T3>
    /*OC+*/ , private ObjectCounter<Triple<T1, T2, T3>> /*+OC*/ {

    typedef std::tuple<T1, T2, T3> Base;

  public:

    Triple(const T1 &v1, const T2 &v2, const T3 &v3)
    : Base(v1, v2, v3) {
    } // Triple

}; // Triple

template<typename T1, typename T2, typename T3>
std::ostream &operator<<(std::ostream &os, const Triple<T1, T2, T3> &t) {
  os << "(" << get<0>(t) << ", " << get<1>(t) << ", "<< get<2>(t) << ")";
  return os;
} // operator<<


// ---  generic class MbVector ---

template<typename IdxT, typename ElemT>
class MbVector: public  std::map<IdxT, ElemT>
      /*OC+*/ , private ObjectCounter<MbVector<IdxT, ElemT>> /*+OC*/ {

    typedef std::map<IdxT, ElemT> Base;
    static const ElemT constEmptyElement; // == ElemT()

  public:

    using Base::operator[]; // prevent hiding for non-const objects

    // non-inserting operator[] for const MbVector objects
    const ElemT &operator[](const IdxT &i) const {
      auto it  = Base::find(i);
      if ( it != Base::end() ) // an element exists
        return it->second;
      else
        return constEmptyElement;
    } // operator[]

}; // MbVector

template<typename IdxT, typename ElemT>
const ElemT MbVector<IdxT, ElemT>::constEmptyElement = ElemT();


// ---  generic class MbMatrix ---

template<typename IdxT1, typename IdxT2, typename ElemT>
class MbMatrix: public  std::map<IdxT1, MbVector<IdxT2, ElemT>>
      /*OC+*/ , private ObjectCounter<MbMatrix<IdxT1, IdxT2, ElemT>> /*+OC*/ {

    typedef std::map<IdxT1, MbVector<IdxT2, ElemT>> Base;
    static const MbVector<IdxT2, ElemT> constEmptyVector; // == MbVector<..>()

  public:

    using Base::operator[]; // prevent hiding for non-const objects

    // non-inserting operator[] for const MbMatrix objects
    const MbVector<IdxT2, ElemT> &operator[](const IdxT1 &i1) const {
      auto it  = Base::find(i1);
      if ( it != Base::end() ) // an MbVector with elements exists
        return it->second;
      else
        return constEmptyVector;
    } // operator[]

    std::vector<Triple<IdxT1, IdxT2, ElemT>> elements() const {
      std::vector<Triple<IdxT1, IdxT2, ElemT>> v;
      for (auto &p1: *this)
        for (auto &p2: p1.second)
          v.emplace_back(p1.first, p2.first, p2.second);
      return v;
    } // elements

}; // MbMatrix

template<typename IdxT1, typename IdxT2, typename ElemT>
const MbVector<IdxT2, ElemT> MbMatrix<IdxT1, IdxT2, ElemT>::constEmptyVector =
    MbVector<IdxT2, ElemT>();

template<typename IdxT1, typename IdxT2, typename ElemT>
std::ostream &operator<<(std::ostream &os, const MbMatrix<IdxT1, IdxT2, ElemT> &m) {
  for (auto &p1: m)
    for (auto &p2: p1.second)
      std::cout << "[" << p1.first << "][" << p2.first << "] = " << p2.second << std::endl;
  return os;
} // operator<<


#endif

// end of MbMatrix.h
//======================================================================

