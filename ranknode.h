#ifndef RANK_NODE_H
#define RANK_NODE_H

#include <string.h>

enum { KEY_BUFF_MAX = 1024, };

typedef unsigned long KeyType;
typedef unsigned long ValueType;


struct NodeData
{
    KeyType    id;
    ValueType    value;
    unsigned short   buf_data_len;
    char  buf[KEY_BUFF_MAX];

    void init() {
        memset(this, 0, sizeof(*this));
    }

    NodeData() { init(); }
    NodeData(const NodeData& rhs) {
        assign(rhs);
    }

    NodeData& operator=(const NodeData& rhs) {
        if (this == &rhs) return *this;

        assign(rhs);
        return *this;
    }

private:
    void assign(const NodeData& rhs)
    {
        id = rhs.id;
        value = rhs.value;
        buf_data_len = rhs.buf_data_len < KEY_BUFF_MAX ? rhs.buf_data_len : KEY_BUFF_MAX;
        memcpy(buf, rhs.buf, buf_data_len);
    }
};

class RankNode
{
public:
    RankNode();
    RankNode(const RankNode& rhs);
    RankNode& operator=(const RankNode& rhs);
    ~RankNode();

    const NodeData& data() const { return m_data; }
    void  set_data(const NodeData& rhs) { m_data = rhs; }

private:
    friend struct RankNodeComparator;
    NodeData  m_data;
};

struct RankNodeComparator
{
    bool operator()(const RankNode& a, const RankNode& b) const
    {
        return a.m_data.value > b.m_data.value;
    }
};

#endif



