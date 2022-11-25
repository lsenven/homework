#include <list>

#include "hash/extendible_hash.h"
#include "page/page.h"
using namespace std;

namespace scudb {

/*
 * constructor
 * array_size: fixed array size for each bucket
 */
template <typename K, typename V>
ExtendibleHash<K, V>::ExtendibleHash(size_t size) :  globalDepth(0),bucketSize(size),bucketNum(1) {
  buckets.push_back(make_shared<Bucket>(0));
}
template<typename K, typename V>
ExtendibleHash<K, V>::ExtendibleHash() {
  ExtendibleHash(64);
}

/*
 * helper function to calculate the hashing address of input key
 */
template <typename K, typename V>
size_t ExtendibleHash<K, V>::HashKey(const K &key) const{
  return hash<K>{}(key);//返回HASH值
}

/*
 * helper function to return global depth of hash table
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetGlobalDepth() const{
  lock_guard<mutex> lock(latch);
  return globalDepth;//返回全局深度
}

/*
 * helper function to return local depth of one specific bucket
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetLocalDepth(int bucket_id) const {//返回局部深度
  //lock_guard<mutex> lck2(latch);
  if (buckets[bucket_id]) {
    lock_guard<mutex> lck(buckets[bucket_id]->latch);//设置桶互斥，防止数据丢失
    if (buckets[bucket_id]->kmap.size() == 0) return -1;//若桶深度为0，返回失败
    return buckets[bucket_id]->localDepth;
  }
  return -1;
}

/*
 * helper function to return current number of bucket in hash table
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetNumBuckets() const{
  lock_guard<mutex> lock(latch);//设置桶互斥，防止数据丢失
  return bucketNum;//设置桶
}

/*
 * lookup function to find value associate with input key
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Find(const K &key, V &value) {

  int idx = getIdx(key);
  lock_guard<mutex> lck(buckets[idx]->latch);
  if (buckets[idx]->kmap.find(key) != buckets[idx]->kmap.end()) {
    value = buckets[idx]->kmap[key];
    return true;

  }
  return false;
}

template <typename K, typename V>//设置函数返回键K
int ExtendibleHash<K, V>::getIdx(const K &key) const{
  lock_guard<mutex> lck(latch);//设置互斥，防止数据丢失
  return HashKey(key) & ((1 << globalDepth) - 1);
}

/*
 * delete <key,value> entry in hash table
 * Shrink & Combination is not required for this project
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Remove(const K &key) {
  int idx = getIdx(key);//获得键值
  lock_guard<mutex> lck(buckets[idx]->latch);//设置互斥，防止数据丢失
  shared_ptr<Bucket> temp = buckets[idx];
  if (temp->kmap.find(key) == temp->kmap.end()) {//若桶深度为0，查找失败
    return false;
  }
  temp->kmap.erase(key);
  return true;
}

/*
 * insert <key,value> entry in hash table
 * Split & Redistribute bucket when there is overflow and if necessary increase
 * global depth
 */
template <typename K, typename V>
void ExtendibleHash<K, V>::Insert(const K &key, const V &value) {
  int idx = getIdx(key);//返回键K
  shared_ptr<Bucket> temp = buckets[idx];
  while (true) {
    lock_guard<mutex> lck(temp->latch);
    if (temp->kmap.find(key) != temp->kmap.end() || temp->kmap.size() < bucketSize) {
      temp->kmap[key] = value;
      break;
    }
    int mask = (1 << (temp->localDepth));
    temp->localDepth++;

    {
      lock_guard<mutex> lck2(latch);//设置互斥
      if (temp->localDepth > globalDepth) {

        size_t leth = buckets.size();
        for (size_t i = 0; i < leth; i++) {//遍历整个桶
          buckets.push_back(buckets[i]);
        }
        globalDepth++;

      }
      bucketNum++;//
      auto newBucket = make_shared<Bucket>(temp->localDepth);

      typename map<K, V>::iterator it;
      for (it = temp->kmap.begin(); it != temp->kmap.end(); ) {//遍历整个map
        if (HashKey(it->first) & mask) {
          newBucket->kmap[it->first] = it->second;
          it = temp->kmap.erase(it);//
        } else it++;
      }
      for (size_t i = 0; i < buckets.size(); i++) {
        if (buckets[i] == temp && (i & mask))
          buckets[i] = newBucket;
      }
    }
    idx = getIdx(key);
    temp = buckets[idx];
  }
}



template class ExtendibleHash<page_id_t, Page *>;
template class ExtendibleHash<Page *, std::list<Page *>::iterator>;
// test purpose
template class ExtendibleHash<int, std::string>;
template class ExtendibleHash<int, std::list<int>::iterator>;
template class ExtendibleHash<int, int>;
} // namespace cmudb
