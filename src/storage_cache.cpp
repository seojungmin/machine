// STORAGE CACHE SOURCE

#include "storage_cache.h"

namespace machine {

StorageCache::StorageCache(DeviceType device_type,
                           CachingType caching_type,
                           size_t capacity) :
                               device_type_(device_type),
                               caching_type_(caching_type){

  switch(caching_type_){

    case CACHING_TYPE_FIFO:
      fifo_cache = new Cache<int, int, FIFOCachePolicy<int>>(capacity);
      break;

    case CACHING_TYPE_LRU:
      lru_cache = new Cache<int, int, LRUCachePolicy<int>>(capacity);
      break;

    case CACHING_TYPE_LFU:
      lfu_cache = new Cache<int, int, LFUCachePolicy<int>>(capacity);
      break;

    case CACHING_TYPE_ARC:
      arc_cache = new Cache<int, int, ARCCachePolicy<int>>(capacity);
      break;

    case CACHING_TYPE_INVALID:
    default:
      exit(EXIT_FAILURE);
  }

}

int StorageCache::Put(const int& key, const int& value){

  switch(caching_type_){

    case CACHING_TYPE_FIFO:
      return fifo_cache->Put(key, value);

    case CACHING_TYPE_LRU:
      return lru_cache->Put(key, value);

    case CACHING_TYPE_LFU:
      return lfu_cache->Put(key, value);

    case CACHING_TYPE_ARC:
      return arc_cache->Put(key, value);

    case CACHING_TYPE_INVALID:
    default:
      exit(EXIT_FAILURE);
  }

}

const int& StorageCache::Get(const int& key) const{

  switch(caching_type_){

    case CACHING_TYPE_FIFO:
      return fifo_cache->Get(key);

    case CACHING_TYPE_LRU:
      return lru_cache->Get(key);

    case CACHING_TYPE_LFU:
      return lfu_cache->Get(key);

    case CACHING_TYPE_ARC:
      return arc_cache->Get(key);

    case CACHING_TYPE_INVALID:
    default:
      exit(EXIT_FAILURE);
  }

}

size_t StorageCache::CurrentCapacity() const{

  switch(caching_type_){

    case CACHING_TYPE_FIFO:
      return fifo_cache->CurrentCapacity();

    case CACHING_TYPE_LRU:
      return lru_cache->CurrentCapacity();

    case CACHING_TYPE_LFU:
      return lfu_cache->CurrentCapacity();

    case CACHING_TYPE_ARC:
      return arc_cache->CurrentCapacity();

    case CACHING_TYPE_INVALID:
    default:
      exit(EXIT_FAILURE);
  }

}

std::ostream& operator<< (std::ostream& stream,
                          const StorageCache& cache){

  std::cout << "-------------------------------\n";
  std::cout << "[" << DeviceTypeToString(cache.device_type_) << "] ";
  std::cout << "[" << CachingTypeToString(cache.caching_type_) <<"] ";

  switch(cache.caching_type_){

     case CACHING_TYPE_FIFO:
       cache.fifo_cache->Print();
       return stream;

     case CACHING_TYPE_LRU:
       cache.lru_cache->Print();
       return stream;

     case CACHING_TYPE_LFU:
       cache.lfu_cache->Print();
       return stream;

     case CACHING_TYPE_ARC:
       cache.arc_cache->Print();
       return stream;

     case CACHING_TYPE_INVALID:
     default:
       exit(EXIT_FAILURE);
   }

}



}  // End machine namespace

