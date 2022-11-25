/**
 * LRU implementation
 */
#include "buffer/lru_replacer.h"
#include "page/page.h"

namespace scudb {

template <typename T> LRUReplacer<T>::LRUReplacer() {
  head = make_shared<Node>();
  tail = make_shared<Node>();
  head->next = tail;
  tail->prev = head;
}//初始化

template <typename T> LRUReplacer<T>::~LRUReplacer() {}//重构函数

/*
 * Insert value into LRU
 */
template <typename T> void LRUReplacer<T>::Insert(const T &value) {
  lock_guard<mutex> lck(latch);//设置互斥
  shared_ptr<Node> temp;
  if (map.find(value) != map.end()) {
    temp = map[value];
    shared_ptr<Node> prev = temp->prev;
    shared_ptr<Node> succ = temp->next;
    prev->next = succ;
    succ->prev = prev;
  } else {
    temp = make_shared<Node>(value);//若LRU为空，直接插入
  }
  shared_ptr<Node> header= head->next;
  temp->next = header;
  header->prev = temp;
  temp->prev = head;
  head->next = temp;
  map[value] = temp;
  return;
}

/* If LRU is non-empty, pop the head member from LRU to argument "value", and
 * return true. If LRU is empty, return false
 */
template <typename T> bool LRUReplacer<T>::Victim(T &value) {
  lock_guard<mutex> lck(latch);//设置互斥
  if (map.empty()) {
    return false;
  }
  shared_ptr<Node> last = tail->prev;
  tail->prev = last->prev;
  last->prev->next = tail;
  value = last->val;//将最后一个元素删除，将其元素追踪
  map.erase(last->val);
  return true;
}

/*
 * Remove value from LRU. If removal is successful, return true, otherwise
 * return false
 */
template <typename T> bool LRUReplacer<T>::Erase(const T &value) {
  lock_guard<mutex> lck(latch);
  if (map.find(value) != map.end()) {//若LRU为不空
    shared_ptr<Node> temp = map[value];
    temp->prev->next = temp->next;
    temp->next->prev = temp->prev;//元素删除
  }
  return map.erase(value);
}

template <typename T> size_t LRUReplacer<T>::Size() {
  lock_guard<mutex> lck(latch);
  return map.size();
}

template class LRUReplacer<Page *>;
// test only
template class LRUReplacer<int>;

} // namespace cmudb
