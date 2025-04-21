#include "gtest/gtest.h"
#include "xi/ldopa/eventlog/csvlog/csvlog.h"
#include "xi/ldopa/pn/algos/generate/alpha/alpha_miner.h"
#include "../pn_test_settings.h"
#include "xi/ldopa/pn/algos/grviz/evlog_ptnets_dotwriter.h"

using namespace xi::ldopa::pn::alpha;
using namespace xi::ldopa::pn;
using namespace xi::ldopa::eventlog::csvlog;

// Определяем путь к тестовому SQLite файлу
const char* TEST_SQLITE_PATH = PN_TEST_MODELS_BASE_DIR "logs/log06.csv";

class AlphaMinerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаем тестовый лог событий
        _csvLog = new CSVLog();
        _csvLog->setFileName(TEST_SQLITE_PATH);
        _csvLog->open();
        ASSERT_TRUE(_csvLog->isOpen());
        _log = _csvLog;
    }

    void TearDown() override {
        if (_csvLog) {
            _csvLog->close();
            delete _csvLog;
        }
    }

    xi::ldopa::eventlog::IEventLog* _log;
    CSVLog* _csvLog;
};

// Тест на создание Alpha Miner
TEST_F(AlphaMinerTest, CreateAlphaMiner) {
    AlphaMiner miner;
    ASSERT_TRUE(true); // Просто проверяем, что объект создался
}

// Тест на извлечение активностей
TEST_F(AlphaMinerTest, ExtractActivities) {
    AlphaMiner miner;
    auto pn = miner.mine(*_log);
    ASSERT_NE(pn, nullptr);
    
    // Проверяем, что сеть Петри создана
    EXPECT_GT(pn->getPositionsNum(), 0);
    EXPECT_GT(pn->getTransitionsNum(), 0);
    
    delete pn;
}

TEST_F(AlphaMinerTest, WriteToFile) {
    AlphaMiner miner;
    auto pn = miner.mine(*_log);
    ASSERT_NE(pn, nullptr);

    EventLogPetriNetDotWriter writer;
    writer.write(PN_TEST_MODELS_BASE_DIR "pn/alpha_miner/alpha_miner.gv", *pn);
}

// Тест на построение отношений
TEST_F(AlphaMinerTest, BuildRelations) {
    AlphaMiner miner;
    auto pn = miner.mine(*_log);
    ASSERT_NE(pn, nullptr);
    
    // Проверяем, что есть начальное и конечное места
    bool hasStartPlace = false;
    bool hasEndPlace = false;
    
    auto positions = pn->getPositions();
    for (auto it = positions.first; it != positions.second; ++it) {
        auto pos = *it;
        const xi::ldopa::eventlog::IEventLog_traits::Attribute* attr = pn->getAttribute(pos);
        if (attr) {
            std::string label = attr->toString();
            if (label == "start") hasStartPlace = true;
            if (label == "end") hasEndPlace = true;
        }
    }
    
    EXPECT_TRUE(hasStartPlace);
    EXPECT_TRUE(hasEndPlace);
    
    delete pn;
}

// Тест на корректность структуры сети
TEST_F(AlphaMinerTest, NetworkStructure) {
    AlphaMiner miner;
    auto pn = miner.mine(*_log);
    ASSERT_NE(pn, nullptr);
    
    // Проверяем, что у начального места есть только исходящие дуги
    auto positions = pn->getPositions();
    for (auto it = positions.first; it != positions.second; ++it) {
        auto pos = *it;
        const xi::ldopa::eventlog::IEventLog_traits::Attribute* attr = pn->getAttribute(pos);
        if (attr && attr->toString() == "start") {
            EXPECT_EQ(pn->getInArcsNum(pos), 0);
            EXPECT_GT(pn->getOutArcsNum(pos), 0);
        }
    }
    
    // Проверяем, что у конечного места есть только входящие дуги
    positions = pn->getPositions();
    for (auto it = positions.first; it != positions.second; ++it) {
        auto pos = *it;
        const xi::ldopa::eventlog::IEventLog_traits::Attribute* attr = pn->getAttribute(pos);
        if (attr && attr->toString() == "end") {
            EXPECT_GT(pn->getInArcsNum(pos), 0);
            EXPECT_EQ(pn->getOutArcsNum(pos), 0);
        }
    }
    
    delete pn;
}

// Тест на корректность весов дуг
TEST_F(AlphaMinerTest, ArcWeights) {
    AlphaMiner miner;
    auto pn = miner.mine(*_log);
    ASSERT_NE(pn, nullptr);
    
    // Проверяем, что все дуги имеют вес 1
    auto arcs = pn->getArcs();
    for (auto it = arcs.first; it != arcs.second; ++it) {
        auto arc = *it;
        EXPECT_EQ(pn->getArcWeight(arc), 1);
    }
    
    delete pn;
} 