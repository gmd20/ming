#ifndef MING_CONSISTENT_HASH_H_
#define MING_CONSISTENT_HASH_H_

#include <stdint.h>
#include <string>
#include <vector>
#include <algorithm>
#include "ming/hash.h"

namespace ming {

//-----------------------------------------------------------------------------
// Note: This function is only suitable for cases that the servers are always
// online. One bucker_id's server can not be added/removed from the list, but
// can resize the bucket to add more server and shrink num_buckets to remove
// servers. Sed the following paper for details.
//
// https://arxiv.org/ftp/arxiv/papers/1406/1406.2294.pdf
// A Fast Minimal Memory Consistent Hash Algorithm
// John Lamping, Eric Veach
//     Google
//-----------------------------------------------------------------------------

inline int32_t jump_consistent_hash(uint64_t key, int32_t num_buckets) {
  int64_t b = -1, j = 0;
  while (j < num_buckets) {
    b = j;
    key = key * 2862933555777941757ULL + 1;
    j = (int64_t)((b + 1) * (double(1LL << 31) / double((key >> 33) + 1)));
  }
  return (int32_t)b;
}

//-----------------------------------------------------------------------------
// Karger's Consistent Hash
// https://en.wikipedia.org/wiki/Consistent_hashing
// http://www.martinbroadhurst.com/Consistent-Hash-Ring.html
// https://arxiv.org/ftp/arxiv/papers/1406/1406.2294.pdf
// https://github.com/ioriiod0/consistent_hash/blob/master/consistent_hash_map.hpp
//-----------------------------------------------------------------------------

// A larger VIRTUAL_NODE_REPLICA_FACTOR produces a fairer key distribution, but
// cause the more memory and the searching is slower.
template <uint32_t VIRTUAL_NODE_REPLICA_FACTOR = 10>
class ConsistentHashRing {
 public:
  void AddNode(const std::string& node) {
    std::string virtual_node;
    for (uint32_t r = 0; r < replicas_; r++) {
      NodeHash n;
      n.node = node;
      virtual_node = node;
      virtual_node += std::to_string(r);
      murmurhash3_x86_32(virtual_node.c_str(), virtual_node.length(), 0, &n.hash);
      ring_.insert(std::upper_bound(ring_.begin(), ring.end(), n, NodeHashLess),
                   n);
    }
  }
  void RemoveNode(const std::string& node) {
    std::string virtual_node;
    for (unsigned int r = 0; r < replicas_; r++) {
      NodeHash n;
      std::string virtual_node = node;
      virtual_node += std::to_string(r);
      murmurhash3_x86_32(virtual_node.c_str(), virtual_node.length(), 0, &n.hash);
      std::vector<NodeHash>::iterator low;
      low = std::lower_bound(ring_.begin(), ring_.end(), n, NodeHashLess);
      if (low != ring_.end() && low.hash == n.hash) {
        ring_.erase(low);
      }
    }
  }
  const std::string& GetNode(const std::string& request) const {
    NodeHash n;
    murmurhash3_x86_32(request.c_str(), request.length(), 0, &n.hash);
    std::vector<NodeHash>::iterator low;
    low = std::lower_bound(ring_.begin(), ring_.end(), n, NodeHashLess);
    if (low == ring_.end()) {
      low = ring_.begin();
    }
	return low->node;
  }

 private:
  struct NodeHash {
    uint32_t hash;
    std::string node;  // store a uint32_t node_id of external node_list may reduce the memory usage.
  };
  struct {
    bool operator()(NodeHash& a, NodeHash& b) { return a.hash < b.hash; }
  } NodeHashLess;
  std::vector<NodeHash> ring_;
};

}  // namespace ming

#endif  // MING_CONSISTENT_HASH_H_
