///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Testing BidiGraph with fixture 1
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

// ldopa
#include "xi/ldopa/graphs/bidigraph.h"
#include "xi/ldopa/utils.h"

// special checker class
class TrivialEdgeExistanceChecker : public xi::ldopa::IEdgeAddChecker {
public:
    // overriding allow adding if no edge between vertices still exists
    bool ifAnEdgeCanBeAdded(xi::ldopa::BidiGraphVertex* srcv, xi::ldopa::BidiGraphVertex* dstv)
    {
        if (!srcv || !dstv)
            throw xi::ldopa::LdopaException("Null vertex");

        xi::ldopa::BidiGraph* gr = srcv->getOwnerGraph();

        // simply check if an edge is already presented
        return !gr->getEdgeStorage()->isOutEdgeExists(srcv, dstv);
    } 
}; // class TrivialEdgeExistanceChecker


// Tests BidiGraph class 
class BidiGraph_1Test : public ::testing::Test {
protected:
    virtual void SetUp() 
    {
        SetUpGraph1();
    } // virtual void SetUp() 

    virtual void TearDown() {}

    // special setups
    void SetUpGraph1()
    {
        _gr1_Vs = _gr1.getVertexStorage();
        _gr1_Lvs = dynamic_cast<xi::ldopa::ListVerticesStorage*>(_gr1_Vs);
        _gr1_Es = _gr1.getEdgeStorage();
        _gr1_Mes = dynamic_cast<xi::ldopa::OutInMMapEdgeStorage*>(_gr1_Es);
        _gr1_v1 = _gr1_Lvs->addNewVertex();
        _gr1_v2 = _gr1_Lvs->addNewVertex();
    }

    // BidiGraph objects
    xi::ldopa::BidiGraph _gr1;
    xi::ldopa::IVerticesStorage* _gr1_Vs;
    xi::ldopa::ListVerticesStorage* _gr1_Lvs;
    xi::ldopa::IEdgesStorage* _gr1_Es;
    xi::ldopa::OutInMMapEdgeStorage* _gr1_Mes;
    xi::ldopa::BidiGraphVertex* _gr1_v1;
    xi::ldopa::BidiGraphVertex* _gr1_v2;
}; // class BidiGraph_1Test

//===================================================================================

//------------- Tests itselves --------------

TEST_F(BidiGraph_1Test, add1)
{
    // 2 vertices
    EXPECT_EQ(_gr1_Vs->getVerticesNum(), 2);
       
    // owner graph
    EXPECT_EQ(&_gr1, _gr1_v1->getOwnerGraph());
}

//
TEST_F(BidiGraph_1Test, clearGraph1)
{
    _gr1.clearGraph();
    
    EXPECT_EQ(_gr1_Vs->getVerticesNum(), 0);
}

//
TEST_F(BidiGraph_1Test, addEdge1)
{
    using namespace xi::ldopa;

    // if no edge exist
    EXPECT_EQ(_gr1_Es->getFirstOutEdge(_gr1_v1, _gr1_v2), nullptr);

    BidiGraphEdge* e1 = _gr1_Es->addEdge(_gr1_v1, _gr1_v2);
    BidiGraphEdge* e2 = _gr1_Es->getFirstOutEdge(_gr1_v1, _gr1_v2);
    EXPECT_EQ(e1, e2);

    // the same for input edges
    BidiGraphEdge* e3 = _gr1_Es->getFirstInEdge(_gr1_v1, _gr1_v2);  // это неправильно, т.к. из v2 в v1 нет дуг!
    EXPECT_EQ(e3, nullptr);
    
    // input arc to v2 from v1
    e3 = _gr1_Es->getFirstInEdge(_gr1_v2, _gr1_v1);
    EXPECT_EQ(e1, e3);

    // need to check both with the following secion and without
    _gr1.clearGraph();
    EXPECT_EQ(_gr1_Vs->getVerticesNum(), 0);
    EXPECT_EQ(_gr1_Es->getEdgesNum(), 0);
}

//
TEST_F(BidiGraph_1Test, addEdgeDupl)
{
    using namespace xi::ldopa;

    BidiGraphEdge* e1 = _gr1_Es->addEdge(_gr1_v1, _gr1_v2);
    BidiGraphEdge* e2 = _gr1_Es->addEdge(_gr1_v1, _gr1_v2, false);
    EXPECT_EQ(e2, nullptr);		// no duplicate allowed

    e2 = _gr1_Es->addEdge(_gr1_v1, _gr1_v2, true);
    EXPECT_NE(e2, nullptr);		// duplicates allowed
}


TEST_F(BidiGraph_1Test, getOrAddEdge)
{
    using namespace xi::ldopa;

    BidiGraphEdge* e1 = _gr1_Es->getOrAddNewEdge(_gr1_v1, _gr1_v2);
    EXPECT_NE(e1, nullptr);		// new edge

    BidiGraphEdge* e2 = _gr1_Es->getOrAddNewEdge(_gr1_v1, _gr1_v2);
    EXPECT_EQ(e1, e2);			// the same edge
}


TEST_F(BidiGraph_1Test, addEdgeWithCheckExistance)
{
    using namespace xi::ldopa;

    TrivialEdgeExistanceChecker exChecker;
    
    BidiGraphEdge* e1 = _gr1_Es->addEdge(_gr1_v1, _gr1_v2, &exChecker);
    EXPECT_NE(e1, nullptr);		    // new edge

    BidiGraphEdge* e2 = _gr1_Es->addEdge(_gr1_v1, _gr1_v2, &exChecker);
    EXPECT_EQ(e2, nullptr);			// the same edge
}


TEST_F(BidiGraph_1Test, addingMultipleEdges)
{
    using namespace xi::ldopa;

    BidiGraphEdge* e1 = _gr1_Es->addEdge(_gr1_v1, _gr1_v2);
    EXPECT_NE(e1, nullptr);		    // new edge

    BidiGraphEdge* e2 = _gr1_Es->addEdge(_gr1_v1, _gr1_v2);
    EXPECT_NE(e2, nullptr);			// new edge between the same vertices

    // extract all edges between given pair of vertices
    std::list<BidiGraphEdge*> edgesList;
    _gr1_Es->getAllEdges(_gr1_v1, _gr1_v2, edgesList);
    EXPECT_EQ(edgesList.size(), 2);

    edgesList.clear();
    _gr1_Es->getAllEdges(_gr1_v2, _gr1_v1, edgesList);   // the converse is false
    EXPECT_EQ(edgesList.size(), 0);

    // with help of enumerators
    IEdgesEnumerator* en = _gr1_Es->enumerateAllEdges();

    int i = 0;
    while (en->hasNext())
    {
        BidiGraphEdge* e = en->getNext();
        ++i;
    }

    EXPECT_EQ(i, 2);
    delete en;
}


TEST_F(BidiGraph_1Test, addingMultipleEdgesForDifVertices)
{
    using namespace xi::ldopa;

    // additional vertex
    BidiGraphVertex* v3 = _gr1_Lvs->addNewVertex();

    BidiGraphEdge* e1 = _gr1_Es->addEdge(_gr1_v1, _gr1_v2);
    BidiGraphEdge* e2 = _gr1_Es->addEdge(_gr1_v1, _gr1_v2);
    BidiGraphEdge* e3 = _gr1_Es->addEdge(_gr1_v1, v3);

    // extract all edges between given pair of vertices
    std::list<BidiGraphEdge*> edgesList;
    _gr1_Es->getAllEdges(edgesList);
    EXPECT_EQ(edgesList.size(), 3);

    // with help of enumerators
    IEdgesEnumerator* en = _gr1_Es->enumerateAllEdges();
    int i = 0;
    for (; en->hasNext(); ++i)
    {
        BidiGraphEdge* e = en->getNext();
        //++i;
    }
    EXPECT_EQ(i, 3);
    delete en;

    // test const enumerators
    IEdgesEnumeratorConst* constEn = _gr1_Es->enumerateAllEdgesConst();

    //int 
        i = 0;
    for (; constEn->hasNext(); ++i)
    {
        const BidiGraphEdge* e = constEn->getNext();
        //++i;
    }
    EXPECT_EQ(i, 3);
    delete constEn;
}


TEST_F(BidiGraph_1Test, enumerateVertices)
{
    using namespace xi::ldopa;

    IVertexEnumerator* en = _gr1_Lvs->enumerateAllVertices();

    int i = 0;
    while (en->hasNext())
    {
        BidiGraphVertex* v = en->getNext();
        ++i;
    }

    EXPECT_EQ(i, 2);
    delete en;

    // const version
    IVertexEnumeratorConst* constEn = _gr1_Lvs->enumerateAllVerticesConst();

    i = 0;
    while (constEn->hasNext())
    {
        const BidiGraphVertex* v = constEn->getNext();
        ++i;
    }

    EXPECT_EQ(i, 2);
    delete constEn;
}

