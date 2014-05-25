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
    BlockStandardSize = 200,   // ��ı�׼��С
    BlockExtentSize = 2 * BlockStandardSize, // �����չ��С
};

// �㷨����
// ÿ����ı�׼������ BlockStandardSize�� ��ʼΪ0��ֱ�����ӵ�BlockStandardSize
// ���ճ���500ʱ������˿������һ���飬��ôֱ�������µĿ�
// ����˿���������м䣬��ô�������˿�����node��ֱ�������ﵽ 2 * BlockStandardSize ʱ��
// �˿���з��Ѳ�������Ϊ2������BlockStandardSize�Ŀ飬�˷����������ǲ���Ҫ�����ƶ�Ԫ�أ�
// ���м����ĳ���ڵ�ʱ��ֻ��Ӱ����Χ�ļ���������

// �ڵ㱾������¼ rank��������Ϊ��ʱ�̱仯�ġ�
// rank ��ͨ����block��index �͸���block��������� ���Ѿ������һ��block��ƫ�����õ��ġ�

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




