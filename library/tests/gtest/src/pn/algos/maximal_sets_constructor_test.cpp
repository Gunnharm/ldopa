#include <gtest/gtest.h>

#include "xi/ldopa/pn/algos/generate/alpha/maximal_set_constructor.h"

using namespace xi::ldopa::pn::alpha;

// Утилита для удобного вывода boost::dynamic_bitset в виде строки "0/1"
static std::string BitsetToString(const boost::dynamic_bitset<>& bs) {
    std::ostringstream oss;
    for (size_t i = 0; i < bs.size(); ++i) {
        oss << bs.test(i);
    }
    return oss.str();
}

//-------------------------------------------------------------------------------------------------
// 1) Тест: isAppendable = false (для любого i).
//    Ожидаем, что максимально возможное множество — пустое,
//    так как ни один элемент добавить нельзя.
//-------------------------------------------------------------------------------------------------
TEST(MaximalSetConstructorTest, AllFalse)
{
    // Размер множества 5, но ничего добавить нельзя
    MaximalSetConstructor constructor(
        5,
        [](const boost::dynamic_bitset<>&, size_t) {
            return false; 
        }
    );

    // Считываем результаты
    std::vector<boost::dynamic_bitset<>> results;
    for (const auto& maximalSet : constructor) {
        results.push_back(maximalSet);
    }

    ASSERT_EQ(results.size(), 0u);
}

//-------------------------------------------------------------------------------------------------
// 2) Тест: isAppendable = true (для любого i).
//    Ожидаем, что итоговое "максимальное" множество будет содержать все индексы,
//    так как мы можем добавлять любой индекс, а алгоритм не «пропускает» доступные индексы.
//    Итог тут один: множество из всех 5 элементов (битсет 11111).
//-------------------------------------------------------------------------------------------------
TEST(MaximalSetConstructorTest, AllTrue)
{
    MaximalSetConstructor constructor(
        5,
        [](const boost::dynamic_bitset<>&, size_t) {
            return true;
        }
    );

    std::vector<boost::dynamic_bitset<>> results;
    for (const auto& maximalSet : constructor) {
        results.push_back(maximalSet);
    }

    // Должно быть ровно одно "максимальное" множество, в котором все 5 элементов
    ASSERT_EQ(results.size(), 1u);
    EXPECT_EQ(results[0].count(), 5u) 
        << "Ожидается множество из всех 5 элементов, но получено: " 
        << BitsetToString(results[0]);
}

//-------------------------------------------------------------------------------------------------
// 3) Тест: позволяем добавлять только чётные индексы (index % 2 == 0).
//    Проверяем, какие "максимальные" множества будут сгенерированы.
//    Код алгоритма не даёт «принудительно» пропустить элемент, если он доступен,
//    значит при проходе от startIndex к концу добавляем все доступные (чётные).
//    В итоге получится ровно одно «максимальное» множество, содержащее все чётные индексы.
//-------------------------------------------------------------------------------------------------
TEST(MaximalSetConstructorTest, EvenIndicesOnly)
{
    MaximalSetConstructor constructor(
        6,  // Для наглядности возьмём размер 6
        [](const boost::dynamic_bitset<>&, size_t index) {
            return (index % 2 == 0);
        }
    );

    std::vector<boost::dynamic_bitset<>> results;
    for (const auto& maximalSet : constructor) {
        results.push_back(maximalSet);
    }

    // Ожидаем ровно одно множество: {0, 2, 4}, если размер = 6
    // Т.е. биты [5..0] = 101010 в обычном “строчном” виде (если выводить справа-налево),
    // но в типичном порядке print: "010101". Для проверки достаточно count() и тестов на нечётные индексы.
    ASSERT_EQ(results.size(), 1u) << "В алгоритме никогда не пропускаются доступные индексы. Должно быть одно";
    const auto& s = results[0];
    // Проверка, что установлены только 0, 2, 4
    EXPECT_TRUE(s.test(0));
    EXPECT_FALSE(s.test(1));
    EXPECT_TRUE(s.test(2));
    EXPECT_FALSE(s.test(3));
    EXPECT_TRUE(s.test(4));
    EXPECT_FALSE(s.test(5));
    EXPECT_EQ(s.count(), 3u);
}

//-------------------------------------------------------------------------------------------------
// 4) Тест на частичный колбэк: скажем, разрешаем индексы больше либо равные 2.
//    Тогда при размере 5 мы можем установить индексы [2,3,4]. Алгоритм соберёт всех подряд.
//    Итоговое "максимальное" множество: {2,3,4}.
//-------------------------------------------------------------------------------------------------
TEST(MaximalSetConstructorTest, IndicesFrom2)
{
    MaximalSetConstructor constructor(
        5,
        [](const boost::dynamic_bitset<>&, size_t index) {
            return (index >= 2);
        }
    );

    std::vector<boost::dynamic_bitset<>> results;
    for (const auto& maximalSet : constructor) {
        results.push_back(maximalSet);
    }

    // По логике алгоритма, если index >=2 доступен, то мы всегда будем добавлять 2,3,4.
    // Значит одно «максимальное» множество: {2, 3, 4}.
    ASSERT_EQ(results.size(), 1u);
    EXPECT_EQ(results[0].count(), 3u);
    EXPECT_FALSE(results[0].test(0));
    EXPECT_FALSE(results[0].test(1));
    EXPECT_TRUE (results[0].test(2));
    EXPECT_TRUE (results[0].test(3));
    EXPECT_TRUE (results[0].test(4));
}

//-------------------------------------------------------------------------------------------------
// 5) Тест на случай set_size = 0 (пустое множество элементов).
//    Тогда никакие индексы не существуют, так что «максимальное» множество — одно,
//    а именно само пустое.
//-------------------------------------------------------------------------------------------------
TEST(MaximalSetConstructorTest, ZeroSize)
{
    MaximalSetConstructor constructor(
        0,
        [](const boost::dynamic_bitset<>&, size_t) {
            // Любое i >=0 уже вне диапазона, так что TryAppend вернёт false
            return true; 
        }
    );

    std::vector<boost::dynamic_bitset<>> results;
    for (const auto& maximalSet : constructor) {
        results.push_back(maximalSet);
    }

    ASSERT_EQ(results.size(), 0u);
}

//-------------------------------------------------------------------------------------------------
// 6) Тест, где элемент 3 и 4 нельзя иметь одновременно в одном множестве.
//    Проверяем, что алгоритм найдёт несколько максимальных множеств.
//-------------------------------------------------------------------------------------------------
TEST(MaximalSetConstructorTest, No34Together_MultipleFinalSets)
{
    // Определяем колбэк, который запрещает добавлять 3, если уже есть 4,
    // и наоборот (запрещает 4, если уже есть 3).
    auto isAppendable = [](const boost::dynamic_bitset<>& set, size_t index)
    {
        // Если мы хотим добавить 4, а уже есть 3 => нельзя
        if (index == 4 && set.test(3)) {
            return false;
        }
        // Если хотим добавить 3, а уже есть 4 => тоже нельзя
        if (index == 3 && set.test(4)) {
            return false;
        }
        // Во всех остальных случаях добавлять можно
        return true;
    };

    // Создаём поисковик максимальных множеств размером 5
    MaximalSetConstructor constructor(5, isAppendable);

    // Собираем результаты
    std::vector<boost::dynamic_bitset<>> results;
    for (const auto& maximalSet : constructor) {
        results.push_back(maximalSet);
    }

    // Проверяем, что множеств найдено ровно два (т. е. действительно несколько вариантов).
    ASSERT_EQ(results.size(), 2u) 
        << "Ожидается ровно два различных максимальных множества";

    // Посчитаем сколько раз каждй элемент встречается в результатах
    std::map<size_t, size_t> counts;
    for (const auto& s : results) {
        for (size_t i = 0; i < s.size(); ++i) {
            counts[i] += s.test(i);
        }
    }

    EXPECT_EQ(counts[3], 1u) << "Ожидается, что элемент 3 встречается ровно один раз";
    EXPECT_EQ(counts[4], 1u) << "Ожидается, что элемент 4 встречается ровно один раз";
    EXPECT_EQ(counts[0], 2u) << "Ожидается, что элемент 0 встречается 2 раза";
    EXPECT_EQ(counts[1], 2u) << "Ожидается, что элемент 1 встречается 2 раза";
    EXPECT_EQ(counts[2], 2u) << "Ожидается, что элемент 2 встречается 2 раза";
}
