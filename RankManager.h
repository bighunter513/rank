#ifndef RANK_MANAGER_H
#define RANK_MANAGER_H

#include <list>
#include <unordered_map>
#include "RankBlock.h"

typedef  std::list<RankBlock>  RankContainer;
typedef std::vector<RankNode>  RankList;
typedef  int BlockIndexType;

// [ userid => blockindex ]
typedef std::unordered_map<KeyType, BlockIndexType>  UID_BlockIndexMap;

// [blockindex => Block*]
//typedef std::unordered_map<BlockIndexType, RankBlock*>  IndexBlockMap;

// Notice: all rank sort is based value decrease trend

class RankManager
{
public:
    RankManager();
    ~RankManager();

    // update or insert node.
    int  upsert_node(const RankNode& node);
    int  get_rank_1(KeyType id);
    int  get_rank_2(KeyType id);

    //[max, min]
    ValueRange get_value_range() const; 
    size_t     rank_count() const;

    void get_top_rank(NodeList& top_list, int top_size = 10);

public:
    // block 达到 BlockExtentSize 时，裂变为2个block
    void fission(RankBlock* block);

    void daily_tick(); // recycle not used in last day's user

    RankContainer&  rank_container() { return m_rank_container;  }

protected:
    BlockIndexType get_block_index(KeyType id);
    RankBlock*   get_block(unsigned int index); // index start from 0 to container.size() -1

public:
    unsigned int   cache_hit;
    unsigned int   cache_miss;
    

private:
    //IndexBlockMap      m_index_block_map;
    UID_BlockIndexMap  m_uid_block_cache; // only cache most used in one day long. can config.

    RankContainer      m_rank_container;
};

#endif

