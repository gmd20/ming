#ifndef MING_CONSISTENT_HASH_H_
#define MING_CONSISTENT_HASH_H_

#include <stdint.h>
#include <string>
#include <vector>
#include <algorithm>
#include "ming/hash.h"

namespace ming {

//-----------------------------------------------------------------------------
// Rendezvous or Highest Random Weight (HRW) hashing
// A good alternative of "Consistent hashing" if the clusters are very small
//
// From:
// https://www.quora.com/How-does-rendezvous-hashing-compare-with
//         -consistent-hashing-When-should-one-be-chosen-over-the-other
// Rendezvous hashing provides a very even distribution of keys on each node,
// even while node are being added/removed. Consistent hashing can fail to
// provide an even distribution for small clusters (though this can be fixed
// to a large extent by using many virtual replicas for each node). This is
// the biggest advantage of Rendezvous hashing over consistent hashing
//
// Wiki mention a O(log n) implementation:
// https://en.wikipedia.org/wiki/Rendezvous_hashing
//-----------------------------------------------------------------------------
int hrw_hash(const std::vector<std::string>& nodes, const std::string &key) {
  int n = nodes.size();

  uint32_t score = 0;
  uint32_t highest_score = 0;
  int node_selected = 0;
  std::string s;
  for (int i=0; i< n; i++) {
    s = nodes[i];
    s += key;
    murmurhash3_x86_32(s.c_str(), s.length(), 0, &score);
    if (score > highest_score) {
      highest_score = score;
      node_selected = i;
    }
  }
  return node_selected;
}

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

// A larger virtualNodeReplicaFactor(100-200) provides a very even distribution
// of keys on each node,, but use more memory and the searching is slower.
template <class T>
std::string NodeToString(const T &t) {
  return std::to_string(t);
}
template <>
std::string NodeToString(const std::string &str) {
  return str;
}
template <typename NodeType> //  NodeType should implement != < and NodeToString
class ConsistentHashRing {
 public:
  ConsistentHashRing() : virtualNodeReplicaFactor_(256) {}
  ConsistentHashRing(uint32_t virtualNodeReplicaFactor)
      : virtualNodeReplicaFactor_(virtualNodeReplicaFactor) {}
  void AddNode(const NodeType node) {
    typename std::vector<NodeType *>::iterator it;
    it = std::lower_bound(nodes_.begin(), nodes_.end(), &node, NodePointerLess);
    if (it == nodes_.end() || *(*it) != node) {
      NodeType *new_node = new NodeType(node);
      std::string vname;
      for (uint32_t i = 0; i < virtualNodeReplicaFactor_; i++) {
        VirtualNode vnode;
        vnode.node = new_node;
        vname = NodeToString(node);
        vname += std::to_string(i);
        murmurhash3_x86_32(vname.c_str(), vname.length(), 0, &vnode.hash);
        ring_.insert(std::lower_bound(ring_.begin(), ring_.end(), vnode,
                                      VirtualNodeLess),
                     vnode);
      }
      nodes_.insert(it, new_node);
    }
  }
  void RemoveNode(const NodeType node) {
    typename std::vector<NodeType *>::iterator it;
    it = std::lower_bound(nodes_.begin(), nodes_.end(), &node, NodePointerLess);
    if (it != nodes_.end() && *(*it) == node) {
      std::string vname;
      for (unsigned int i = 0; i < virtualNodeReplicaFactor_; i++) {
        VirtualNode vnode;
        vname = node;
        vname += std::to_string(i);
        murmurhash3_x86_32(vname.c_str(), vname.length(), 0, &vnode.hash);
        typename std::vector<VirtualNode>::iterator low;
        low = std::lower_bound(ring_.begin(), ring_.end(), vnode,
                               VirtualNodeLess);
        while (low != ring_.end() && low->hash == vnode.hash) {
          if (*(low->node) == node) {
            ring_.erase(low);
          }
          ++low;
        }
      }
      NodeType *release_node = *it;
      nodes_.erase(it);
      delete release_node;
    }
  }
  // GetNode reture the node that the request should be sent to
  // The return node object is avaliable until the next AddNode/RemoveNode
  const NodeType &GetNode(const std::string &request) const {
    VirtualNode vnode;
    murmurhash3_x86_32(request.c_str(), request.length(), 0, &vnode.hash);
    typename std::vector<VirtualNode>::const_iterator low;
    low = std::lower_bound(ring_.begin(), ring_.end(), vnode, VirtualNodeLess);
    if (low == ring_.end()) {
      low = ring_.begin();
    }
    return *(low->node);
  }
  const std::vector<NodeType *> &GetNodeList() { return nodes_; }
  bool IsNodeActive(const NodeType node) {
    return std::binary_search(nodes_.begin(), nodes_.end(), node);
  }

 private:
  struct VirtualNode {
    uint32_t hash;
    NodeType *node;
  };
  struct {
    bool operator()(const VirtualNode &a, const VirtualNode &b) {
      return a.hash < b.hash;
    }
  } VirtualNodeLess;
  struct {
    bool operator()(const NodeType *a, const NodeType *b) { return *a < *b; }
  } NodePointerLess;
  uint32_t virtualNodeReplicaFactor_;
  std::vector<VirtualNode> ring_;
  std::vector<NodeType *> nodes_;
};
typedef ConsistentHashRing<std::string> HashRing;

}  // namespace ming

#endif  // MING_CONSISTENT_HASH_H_
