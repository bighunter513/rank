#include "ranknode.h"


RankNode::RankNode()
{
     
}


RankNode::~RankNode()
{
}

RankNode::RankNode(const RankNode& rhs) : m_data(rhs.data())
{
    
}
RankNode& RankNode::operator=(const RankNode& rhs)
{
    if (this == &rhs) return *this;

    m_data = rhs.m_data;

    return *this;
}


