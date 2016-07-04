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

// A larger virtualNodeReplicaFactor produces a fairer key distribution, but
// use more memory and the searching is slower.
class ConsistentHashRing {
 public:
  ConsistentHashRing()
      : virtualNodeReplicaFactor_(10) {}
  ConsistentHashRing(uint32_t virtualNodeReplicaFactor)
      : virtualNodeReplicaFactor_(virtualNodeReplicaFactor) {}

   void AddNode(const std::string node) {
     std::vector<std::string>::iterator it;
     it = std::lower_bound(nodes_.begin(), nodes_.end(), node);
     if (it == nodes_.end() || *it != node) {
       std::string virtual_node;
       for (uint32_t r = 0; r < virtualNodeReplicaFactor_; r++) {
         VirtualNode n;
         n.node = node;
         virtual_node = node;
         virtual_node += std::to_string(r);
         murmurhash3_x86_32(virtual_node.c_str(), virtual_node.length(), 0,
                            &n.hash);
         ring_.insert(
             std::lower_bound(ring_.begin(), ring_.end(), n, VirtualNodeLess), n);
       }

       nodes_.insert(it, node);
     }
  }
  void RemoveNode(const std::string node) {
    std::vector<std::string>::iterator it;
    it = std::lower_bound(nodes_.begin(), nodes_.end(), node);
    if (it != nodes_.end() && *it == node) {
      std::string virtual_node;
      for (unsigned int r = 0; r < virtualNodeReplicaFactor_; r++) {
        VirtualNode n;
        virtual_node = node;
        virtual_node += std::to_string(r);
        murmurhash3_x86_32(virtual_node.c_str(), virtual_node.length(), 0,
                           &n.hash);
        std::vector<VirtualNode>::iterator low;
        low = std::lower_bound(ring_.begin(), ring_.end(), n, VirtualNodeLess);
        while (low != ring_.end() && low->hash == n.hash) {
          if (low->node == node) {
            ring_.erase(low);
          }
          ++low;
        }
      }

      nodes_.erase(it);
    }
  }
  // GetNode reture the node that the request should be sent to
  const std::string GetNode(const std::string &request) const {
    VirtualNode n;
    murmurhash3_x86_32(request.c_str(), request.length(), 0, &n.hash);
    std::vector<VirtualNode>::const_iterator low;
    low = std::lower_bound(ring_.begin(), ring_.end(), n, VirtualNodeLess);
    if (low == ring_.end()) {
      low = ring_.begin();
    }
    return low->node;
  }
  std::vector<std::string> & GetNodeList() {
    return nodes_;
  }
  bool IsNodeActive(const std::string & node) {
     return std::binary_search(nodes_.begin(), nodes_.end(), node);
  }

 private:
  struct VirtualNode {
    uint32_t hash;
    std::string node;  // store a uint32_t node_id of external node_list may reduce the memory usage.
  };
  struct {
    bool operator()(const VirtualNode &a, const VirtualNode &b) { return a.hash < b.hash; }
  } VirtualNodeLess;
  uint32_t virtualNodeReplicaFactor_;
  std::vector<VirtualNode> ring_;
  std::vector<std::string> nodes_;
};


}  // namespace ming

#endif  // MING_CONSISTENT_HASH_H_
