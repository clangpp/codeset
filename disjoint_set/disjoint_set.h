#ifndef DISJOINT_SET_H_
#define DISJOINT_SET_H_

#include <algorithm>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <vector>

// disjoint set algorithm
namespace disjoint_set {

typedef std::ptrdiff_t ssize_type;

// ************ notes **************
//  concept RandomAccessIterator {
//      std::iterator_traits<RandomAccessIterator>::value_type
//          is compatible with ssize_type
//  };
// ************ notes **************

// pre-condition: root1 and root2 are root node of disdinct subsets
template <typename RandomAccessIterator>
inline void union_sets_plain(RandomAccessIterator sets, ssize_type root1,
                             ssize_type root2) {
  sets[root2] = root1;
}

template <typename RandomAccessIterator>
inline void union_sets_by_size(RandomAccessIterator sets, ssize_type root1,
                               ssize_type root2) {
  if (sets[root1] <= sets[root2]) {  // size(root1) >= size(root2)
    sets[root1] += sets[root2];      // add root2's size to root1's
    sets[root2] = root1;             // attach tree root2 to tree root1
  } else {                           // size(root1) < size(root2)
    sets[root2] += sets[root1];      // add root1's size to root2's
    sets[root1] = root2;             // attach tree root1 to tree root2
  }
}

template <typename RandomAccessIterator>
inline void union_sets_by_height(RandomAccessIterator sets, ssize_type root1,
                                 ssize_type root2) {
  if (sets[root1] <= sets[root2]) {  // height(root1) >= height(root2)
    if (sets[root1] == sets[root2]) {
      --sets[root1];  // update height if same
    }
    sets[root2] = root1;  // attach tree root2 to tree root1
  } else {                // height(root1) < height(root2)
    sets[root1] = root2;  // attach tree root1 to tree root2
  }
}

// pre-condition: sets[s] is referencable
template <typename RandomAccessIterator>
ssize_type find_plain(RandomAccessIterator sets, ssize_type s) {
  return sets[s] < 0 ? s : find_plain(sets, sets[s]);
}

template <typename RandomAccessIterator>
ssize_type find_compress_path(RandomAccessIterator sets, ssize_type s) {
  if (sets[s] < 0) {
    return s;
  }
  return sets[s] = find_compress_path(sets, sets[s]);
}

}  // namespace disjoint_set

// disjoint set class
class DisjointSet {
 public:
  typedef std::size_t size_type;
  typedef disjoint_set::ssize_type ssize_type;
  typedef ssize_type setid_type;

 public:
  explicit DisjointSet(size_type n = 0) : sets_(n, -1) {}

  // find root node of subset that s belongs to
  setid_type find(setid_type s) const {
    if (s < 0 || s >= ssize_type(sets_.size()))
      throw std::invalid_argument("invalid setid");
    return disjoint_set::find_plain(sets_.begin(), s);
  }

  // find root node of subset that s belongs to
  setid_type find(setid_type s) {
    if (s < 0 || s >= ssize_type(sets_.size()))
      throw std::invalid_argument("invalid setid");
    return disjoint_set::find_compress_path(sets_.begin(), s);
  }

  // union two subsets which s1 and s2 belongs to
  // return true if union takes place, otherwise false.
  bool union_sets(setid_type s1, setid_type s2) {
    setid_type root1 = find(s1);
    setid_type root2 = find(s2);
    if (root1 == root2) return false;
    disjoint_set::union_sets_by_size(sets_.begin(), root1, root2);
    return true;
  }

  // get nodes amount in disjoint set
  size_type size() const { return sets_.size(); }

  // get subsets amount in disjoint set
  size_type set_count() const {
    return std::count_if(sets_.begin(), sets_.end(),
                         [](setid_type set) { return set < 0; });
  }

  // get nodes amount in subset that s belongs to
  size_type set_size(setid_type s) const {
    setid_type root = find(s);
    return static_cast<size_type>(-sets_[root]);
  }

  // get nodes amount in subset that s belongs to
  size_type set_size(setid_type s) {
    setid_type root = find(s);
    return static_cast<size_type>(-sets_[root]);
  }

 private:
  std::vector<ssize_type> sets_;
};

#endif  // DISJOINT_SET_H_
