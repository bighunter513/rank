#include "RankManager.h"
#include <iostream>
#include <assert.h>
using namespace std;

RankManager::RankManager() : cache_hit(0), cache_miss(0)
{
}


RankManager::~RankManager()
{
}

ValueRange RankManager::get_value_range() const
{
    if (m_rank_container.size() == 0) return ValueRange(-1, -1);
    else if (m_rank_container.size() == 1) return m_rank_container.begin()->get_value_range();
    else {
        return ValueRange(m_rank_container.begin()->get_value_range().first, m_rank_container.rbegin()->get_value_range().second);
    }

}
size_t  RankManager::rank_count() const
{
    size_t  total = 0;
    for (RankContainer::const_iterator itor = m_rank_container.begin(); itor != m_rank_container.end(); ++itor)
    {
        total += (*itor).rank_count();
    }
    return total;
}


// update or insert node.
int RankManager::upsert_node(const RankNode& node)
{
    if (m_rank_container.size() == 0) // first node
    {
        //
        m_rank_container.emplace_front(this); // construct and place new elem at front
        RankBlock&   new_block = *(m_rank_container.begin());
        return new_block.set_node(node);
    } 
    else
    {
        // greater than max , or less than min ?
        ValueRange  range = get_value_range();
        if (node.data().value >= range.first)
        {
            return m_rank_container.begin()->set_node(node);
        }
        else if (node.data().value <= range.second)
        {
            return m_rank_container.rbegin()->set_node(node);
        }
        else {
            for (auto& block : m_rank_container)
            {
                range = block.get_value_range();
                if (node.data().value >= range.second)
                {
                    return block.set_node(node);
                }
            }
        } 
    }

    return -1;
}

int  RankManager::get_rank_1(KeyType id)
{
    int  rank = 0;
    bool found = false;
    for (RankContainer::iterator itor = m_rank_container.begin(); itor != m_rank_container.end(); ++itor)
    {
        RankBlock&  block = (*itor);
        if (block.has(id))
        {
            rank += block.get_rank_offset(id) + 1;
            found = true;
            break;
        }

        rank += block.rank_count();
    }

    if (found)
    {
        return rank;
    }
    else
    {
        return -1;
    }
}

int  RankManager::get_rank_2(KeyType id)
{
    BlockIndexType block_index = get_block_index(id);
    if (block_index < 0) return get_rank_1(id);

    RankBlock*   block = get_block(block_index);
    if (block == nullptr) return get_rank_1(id);
    
    return block->start_index() + block->get_rank_offset(id) - 1;
}

void RankManager::get_top_rank(NodeList& top_list, int top_size)
{
    if (top_size < 1) return;

    int count = 0;
    for (RankContainer::iterator itor = m_rank_container.begin(); itor != m_rank_container.end(); ++itor)
    {
        RankBlock&  block = (*itor);
        for (auto& node : block.node_list())
        {
            if (top_size == count++) return;

            top_list.push_back(node);
        }
    }
}

void RankManager::daily_tick()
{

}

RankBlock*   RankManager::get_block(unsigned int index)
{
    if (index >= m_rank_container.size()) return nullptr;

    RankContainer::iterator  itor = m_rank_container.begin();
    for (unsigned int i = 0; i < index; ++i, ++itor)
        ;
    
    return &(*itor);
}

BlockIndexType RankManager::get_block_index(KeyType id)
{
    if (m_rank_container.size() == 0)
    {
        if (m_uid_block_cache.count(id)) {
            cache_miss++;
            m_uid_block_cache.erase(id);
        }
        return -1;
    }
    
    if (m_rank_container.begin()->has(id))
    {
        cache_hit++;
        m_uid_block_cache[id] = 0;
        return 0;
    }

    unsigned int index = -1;
    if (m_uid_block_cache.count(id))   index = m_uid_block_cache[id];

    RankBlock*  block = nullptr;
    if (index != -1)
    {
        // is it still valid ?
        block = get_block(index);
        if (block != nullptr && block->has(id))
        {
            cache_hit++;
            return index;
        }
        else 
        {
            cache_miss++;
            m_uid_block_cache.erase(id);
        }
    }
    
    if (m_rank_container.size() >= 2) {
        // find 
        index = 1;
        RankContainer::const_iterator itor = m_rank_container.begin();
        ++itor;
        for (; itor != m_rank_container.end(); ++itor)
        {
            if ((*itor).has(id)) return index;
            else ++index;
        }
    }

    return -1;
}

void RankManager::fission(RankBlock* block)
{
    if (block == nullptr)
    {
        cout << "fission:Block is NULL" << endl;
        return;
    }

    BlockIndexType  index = 0;
    bool  found = false;
    RankContainer::iterator itor = m_rank_container.begin();
    for (; itor != m_rank_container.end(); ++itor, ++index)
    {
        if (&(*itor) == block)
        {
            // found it
            found = true;
            break;
        }
    }

    if (!found)
    {
        cout << "fission: Block not found" << endl;
        return;
    }

    RankBlock  tmp_block(this);
    RankContainer::iterator new_block_itor = m_rank_container.insert(++itor, tmp_block);

    NodeList&  old_node_list = block->node_list();
    NodeList::iterator node_itor = old_node_list.begin();

    index = 0;
    for (; node_itor != old_node_list.end() && index < BlockStandardSize ; ++index, ++node_itor)
    {
    }

    if (node_itor != old_node_list.end() && new_block_itor != m_rank_container.end())
    {
        RankBlock&  new_block = *new_block_itor;

        // assign second half to new block
        new_block.node_list().assign(node_itor, old_node_list.end());

        // remove second half from old node
        old_node_list.erase(node_itor, old_node_list.end());
        block->update(block->start_index());

        KeyType  start_index = block->start_index() + block->rank_count();
        new_block.update(start_index);  // update value range , etc...

        start_index += new_block.rank_count();
        ++new_block_itor;
        for (; new_block_itor != m_rank_container.end(); ++new_block_itor)
        {
            (*new_block_itor).set_start_index(start_index);
            start_index += (*new_block_itor).rank_count();
        }
    }
    else
    {
        // shouldn't be here.
        cout << "fission itor error." << endl;
    }

}



