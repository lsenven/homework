#pragma once
#include <list>
#include <mutex>

#include "buffer/lru_replacer.h"
#include "disk/disk_manager.h"
#include "hash/extendible_hash.h"
#include "logging/log_manager.h"
#include "page/page.h"
using namespace std;
namespace scudb {
class BufferPoolManager {
public:
  BufferPoolManager(size_t pool_size, DiskManager *disk_manager, LogManager *log_manager = nullptr);

  ~BufferPoolManager();

  Page *FetchPage(page_id_t page_id);

  bool UnpinPage(page_id_t page_id, bool is_dirty);

  bool FlushPage(page_id_t page_id);

  Page *NewPage(page_id_t &page_id);

  bool DeletePage(page_id_t page_id);

private:
  size_t pool_size_; 
  Page *pages_;      
  DiskManager *disk_manager_;
  LogManager *log_manager_;
  HashTable<page_id_t, Page *> *page_table_; 
  Replacer<Page *> *replacer_;   
  std::list<Page *> *free_list_;
  std::mutex latch_;             
  Page *GetVictimPage();
};
}
