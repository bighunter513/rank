#ifndef RANK_BLOCK_H
#define RANK_BLOCK_H

#include "ranknode.h"
#include <vector>
#include <map>
#include <set>
#include <list>

typedef std::set<KeyType>   KeySet;
typedef std::list<RankNode>  NodeList;
typedef std::pair<KeyType, KeyType>  RankRange;
typedef std::pair<ValueType, ValueType>  ValueRange;

enum {
    BlockStandardSize = 200,   // 块的标准大小
    BlockExtentSize = 2 * BlockStandardSize, // 块的延展大小
};

// 算法描述
// 每个块的标准容量是 BlockStandardSize， 初始为0，直达增加到BlockStandardSize
// 当刚超过500时，如果此块是最后一个块，那么直接增加新的块
// 如果此块在链表的中间，那么继续往此块增加node，直到容量达到 2 * BlockStandardSize 时，
// 此块进行分裂操作，变为2个容量BlockStandardSize的块，此方法的优势是不需要大量移动元素，
// 在中间更新某个节点时，只会影响周围的几个操作。

// 节点本身并不记录 rank排名，因为是时刻变化的。
// rank 是通过其block的index 和各个block的容量相加 ，已经在最后一个block的偏移量得到的。

// rank start from 1

class RankManager;

class RankBlock
{
public:
    RankBlock(RankManager*  rank_mgr);
    ~RankBlock();
    RankBlock(const RankBlock& rhs);
    RankBlock& operator=(const RankBlock& rhs);


    // @return: offset in this block
    int set_node(const RankNode& node);

    // offset in this block
    KeyType get_rank_offset(KeyType id);

    size_t  rank_count() const { return m_node_list.size(); }

    RankRange get_rank_range() const { return std::make_pair(m_start_index, m_start_index + m_node_list.size() -1 ); }

    //[max, min]
    ValueRange get_value_range() const { return m_value_range; }

    bool    has(KeyType id) const { return m_keys.count(id) > 0; }

    const KeySet&  keys() const { return m_keys; }

    NodeList&  node_list() { return m_node_list; }
    const NodeList&  node_list() const { return m_node_list; }

    KeyType start_index() const { return m_start_index; }
    void set_start_index(KeyType start_index) { m_start_index = start_index; }

    RankManager*  rank_mgr() { return m_rank_mgr; }
    const RankManager*  rank_mgr() const { return m_rank_mgr; }

    void update(KeyType  start_index); 

private:
    KeyType    m_start_index;
    RankManager*  m_rank_mgr;
    KeySet     m_keys;
    NodeList   m_node_list;

    ValueRange  m_value_range;
    // RankRange  m_rank_range;
};

#endif




