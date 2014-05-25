#include "RankBlock.h"
#include "RankManager.h"

#include <assert.h>
#include <iostream>
using namespace  std;

RankBlock::RankBlock(RankManager*  rank_mgr) :m_start_index(0), m_rank_mgr(rank_mgr)
{

}


RankBlock::~RankBlock()
{
}

RankBlock::RankBlock(const RankBlock& rhs)
{
    m_start_index = rhs.start_index();
    m_rank_mgr = const_cast<RankManager*>(rhs.rank_mgr());
    m_keys = rhs.keys();
    m_node_list = rhs.node_list();

    m_value_range = rhs.get_value_range();
}
RankBlock& RankBlock::operator=(const RankBlock& rhs)
{
    if (this == &rhs) return *this;

    m_start_index = rhs.start_index();
    m_rank_mgr = const_cast<RankManager*>(rhs.rank_mgr());
    m_keys = rhs.keys();
    m_node_list = rhs.node_list();

    m_value_range = rhs.get_value_range();

    return *this;
}

int RankBlock::set_node(const RankNode& node)
{
    if (m_rank_mgr == nullptr)
    {
        // shouldn't be here. 
        cout << "m_rank_mgr not set." << endl;
        return -1;
    }

    int offset = 0;
    if (m_node_list.size() == 0)
    {
        // first item
        m_node_list.push_front(node);

        m_keys.insert(node.data().id);
        m_value_range.first = m_value_range.second = node.data().value; 
        m_start_index = 1;

        return 0;
    }
    else {
        if (m_node_list.size() >= BlockExtentSize)
        {
            // shouldn't be here. 
            cout << "Block size error." << endl;
            return -1;
        }
        else
        {
            // find pos, and insert
            if (node.data().value < (*m_node_list.rbegin()).data().value)
            {
                m_node_list.push_back(node);
                m_value_range.second = node.data().value;
                offset = m_node_list.size() - 1;
                m_keys.insert(node.data().id);
            }
            else if (node.data().value > (*m_node_list.begin()).data().value)
            {
                m_node_list.push_front(node);
                m_value_range.first = node.data().value;
                offset = 0;
                m_keys.insert(node.data().id);
            }
            else
            {
                assert(m_node_list.size() >= 2);
                NodeList::iterator itor = m_node_list.begin();
                
                bool  ok = false;
                ValueType  tmp_value = node.data().value;
                //cout << "value: " << tmp_value << endl;
                for (; itor != m_node_list.end(); ++itor)
                {
                    ++offset;
                    if (tmp_value >= (*itor).data().value)
                    {
                        // insert before next
                        //cout << "block size1:" << m_node_list.size() << endl;
                        m_node_list.insert(itor, node);
                        //cout << "block size2:" << m_node_list.size() << endl;
                        m_keys.insert(node.data().id);
                        ok = true;
                        break;
                    }
                }
                if (!ok)
                {
                    cout << "something wrong happened." << endl;
                }
            }   
        }
    }

    if (m_node_list.size() == BlockExtentSize)
    {
        m_rank_mgr->fission(this);
    }

    //cout << "block size:" << m_node_list.size() << endl;

    return 0;
}

void RankBlock::update(KeyType  start_index)
{
    set_start_index(start_index);

    m_keys.swap(KeySet());
    for (NodeList::iterator itor = m_node_list.begin(); itor != m_node_list.end(); ++itor)
    {
        m_keys.insert((*itor).data().id);
    }

    m_value_range.first = (*m_node_list.begin()).data().value;
    m_value_range.second = (*m_node_list.rbegin()).data().value;
}

KeyType RankBlock::get_rank_offset(KeyType id)
{
    KeyType offset = 0;
    for (auto& node : m_node_list)
    {
        if (node.data().id == id)
        {
            return offset;
        }
        ++offset;
    }

    cout << id << " Rank not found!!!" << endl;
    return -1;
}


