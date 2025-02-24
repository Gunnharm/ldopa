///////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief Testing BidiGraph with fixture 1
///
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

// ldopa
#include "xi/ldopa/graphs/bidigraph.h"
#include "xi/ldopa/utils.h"

// Tests BidiGraph class 2
class BidiGraph_2Test : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        SetUpGraph();
    } // virtual void SetUp() 

    virtual void TearDown() 
    {

        delete _gr1;
    }


    // special setups
    void SetUpGraph()
    {
        using namespace xi::ldopa;

        _gr1_Mvs = new MapNamedVerticesStorage(nullptr);    // do not del manually!
        _gr1 = new BidiGraph(_gr1_Mvs, nullptr);            

        _gr1_Vs = _gr1->getVertexStorage();
        //_gr1_Lvs = dynamic_cast<xi::ldopa::ListVerticesStorage*>(_gr1_Vs);
        _gr1_Es = _gr1->getEdgeStorage();
        _gr1_Mes = dynamic_cast<xi::ldopa::OutInMMapEdgeStorage*>(_gr1_Es);
    }

    // BidiGraph objects
    xi::ldopa::BidiGraph* _gr1;
    xi::ldopa::IVerticesStorage* _gr1_Vs;
    xi::ldopa::MapNamedVerticesStorage* _gr1_Mvs;
    xi::ldopa::IEdgesStorage* _gr1_Es;
    xi::ldopa::OutInMMapEdgeStorage* _gr1_Mes;
}; // class BidiGraph_2Test


//===================================================================================

//------------- Tests itselves --------------

//
TEST_F(BidiGraph_2Test, storageEngines)
{
    EXPECT_EQ(_gr1_Vs, _gr1_Mvs);
    EXPECT_EQ(_gr1_Mvs->getLinkedGraph(), _gr1);    
}

TEST_F(BidiGraph_2Test, add1)
{
    using namespace xi::ldopa;
    
    BidiGraphVertex* v1 = _gr1_Mvs->addNewVertex("v1");
    BidiGraphVertex* v2 = _gr1_Mvs->addNewVertex("v2");

    // 2 vertices
    EXPECT_EQ(_gr1_Vs->getVerticesNum(), 2);

    // owner graph
    EXPECT_EQ(_gr1, v1->getOwnerGraph());
}


TEST_F(BidiGraph_2Test, addDuplicates)
{
    using namespace xi::ldopa;

    BidiGraphVertex* v1 = _gr1_Mvs->addNewVertex("v1");
    BidiGraphVertex* v2 = _gr1_Mvs->addNewVertex("v1");
    EXPECT_EQ(v2, nullptr);

    // 2 vertices
    EXPECT_EQ(_gr1_Vs->getVerticesNum(), 1);

    // vertex by name
    BidiGraphVertex* v1f = _gr1_Mvs->getVertex("v1");
    EXPECT_EQ(v1f, v1);
}


TEST_F(BidiGraph_2Test, enumerateVertices)
{
    using namespace xi::ldopa;

    BidiGraphVertex* v1 = _gr1_Mvs->addNewVertex("v1");
    BidiGraphVertex* v2 = _gr1_Mvs->addNewVertex("v2");
    IVertexEnumerator* en = _gr1_Mvs->enumerateAllVertices();

    int i = 0;
    while (en->hasNext())
    {
        BidiGraphVertex* v = en->getNext();
        ++i;
    }

    EXPECT_EQ(i, 2);
    delete en;
}


TEST_F(BidiGraph_2Test, searchVertex1)
{
    using namespace xi::ldopa;

    BidiGraphVertex* v1 = _gr1_Mvs->addNewVertex("v1");
    BidiGraphVertex* v2 = _gr1_Mvs->addNewVertex("v2");

    MapNamedVerticesStorage::Str2VertexMapConstIterator
        it = _gr1_Mvs->findVertexByName(v1);

    EXPECT_NE(it, _gr1_Mvs->getVertices().end());
    EXPECT_EQ(it->first, "v1");
    
    // get name by ptr 
    std::string vname;// = _gr1_Mvs->getNameOfVertex(v1);
    _gr1_Mvs->getNameOfVertex(v1, vname);
    EXPECT_EQ(vname, "v1");
}


TEST_F(BidiGraph_2Test, searchReverseLoockupMap1)
{
    using namespace xi::ldopa;

    // independant graph
    MapNamedVerticesStorage* vs = new MapNamedVerticesStorage(true);
    BidiGraph gr(vs);
    IEdgesStorage* es = gr.getEdgeStorage();

    BidiGraphVertex* v1 = vs->addNewVertex("v1");
    BidiGraphVertex* v2 = vs->addNewVertex("v2");
    BidiGraphVertex* v3 = vs->addNewVertex("v3");

    EXPECT_EQ(vs->getVerticesNum(), 3);
    EXPECT_EQ(vs->getVertices().size(), 3);
    EXPECT_EQ(vs->getReverseLookupMap()->size(), 3);

    // get name by ptr 
    std::string vname;// = vs->getNameOfVertex(v1, TODO);
    vs->getNameOfVertex(v1, vname);
    EXPECT_EQ(vname, "v1");

    gr.clearGraph();

    EXPECT_EQ(vs->getVerticesNum(), 0);
    EXPECT_EQ(vs->getVertices().size(), 0);
    EXPECT_EQ(vs->getReverseLookupMap()->size(), 0);     
}

