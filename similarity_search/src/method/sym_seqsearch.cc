/**
 * Non-metric Space Library
 *
 * Authors: Bilegsaikhan Naidan (https://github.com/bileg), Leonid Boytsov (http://boytsov.info).
 * With contributions from Lawrence Cayton (http://lcayton.com/) and others.
 *
 * For the complete list of contributors and further details see:
 * https://github.com/searchivarius/NonMetricSpaceLib 
 * 
 * Copyright (c) 2014
 *
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 */

#include <thread>
#include <queue>

#include "space.h"
#include "rangequery.h"
#include "knnquery.h"
#include "knnqueue.h"
#include "logging.h"
#include "method/sym_seqsearch.h"

namespace similarity {

template <typename dist_t>
SymSeqSearch<dist_t>::SymSeqSearch(Space<dist_t>& space, const ObjectVector& data) : space_(space), data_(data) {
}

template <typename dist_t>
void SymSeqSearch<dist_t>::CreateIndex(const AnyParams& IndexParams) {
  SetQueryTimeParams(getEmptyParams());
}

template <typename dist_t>
void SymSeqSearch<dist_t>::SetQueryTimeParams(const AnyParams& params) {
  AnyParamManager pmgr(params);

  string s;
  pmgr.GetParamOptional(SYM_TYPE_PARAM, s, SYM_TYPE_MIN);
  symmType_ = getSymmType(s);
  
  pmgr.GetParamOptional(SYM_K_PARAM, symmCandK_, 1000);

  pmgr.CheckUnused();
}

template <typename dist_t>
void SymSeqSearch<dist_t>::Search(RangeQuery<dist_t>* query, IdType) const {
  PREPARE_RUNTIME_ERR(err) << "Range search is not supported!";
  THROW_RUNTIME_ERR(err);
}

template <typename dist_t>
void SymSeqSearch<dist_t>::Search(KNNQuery<dist_t>* query, IdType) const {
  priority_queue<pair<dist_t,IdType>> symRes;

  for (size_t i = 0; i < data_.size(); ++i) {
    dist_t d = SymmDistance(query, data_[i]);
    symRes.push(make_pair(d, i));
    if (symRes.size() > symmCandK_) symRes.pop();
  }

  while (!symRes.empty()) {
    query->CheckAndAddToResult(data_[symRes.top().second]);
    symRes.pop();
  }
}

template class SymSeqSearch<float>;
template class SymSeqSearch<double>;
template class SymSeqSearch<int>;

}
