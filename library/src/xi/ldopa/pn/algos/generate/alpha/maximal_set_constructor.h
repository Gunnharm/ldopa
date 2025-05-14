////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Petri net Library: Maximal sets constructor
/// \author    ganvas
/// \version   0.1.0
/// \date      2025-04-19
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_PN_ALGOS_ALPHA_MAXIMAL_SET_ITERATOR_H_
#define XI_LDOPA_PN_ALGOS_ALPHA_MAXIMAL_SET_ITERATOR_H_

#pragma once

// boost
#include <boost/dynamic_bitset.hpp>
#include <boost/graph/bron_kerbosch_all_cliques.hpp>

namespace xi { namespace ldopa { namespace pn { namespace alpha {;   //

/** \brief Constructs maximal sets with conditions on appendability.
 */
class MaximalSetConstructor {
public:
    //----<Callbacks>----
    using IsAppendableCallback = std::function<bool(const boost::dynamic_bitset<>&, size_t)>;

    //----<Iterator>----
    class Iterator 
    {
        // Для удобства можно определить типы-синонимы,
        // соответствующие требованиям стандартных итераторов.
        using self_type             = Iterator;
        using value_type            = boost::dynamic_bitset<>;
        using reference             = const boost::dynamic_bitset<>&;
        using pointer               = const boost::dynamic_bitset<>*;
        using difference_type       = std::ptrdiff_t;
        using iterator_category     = std::input_iterator_tag;

    public:
        /** \brief Специальный конструктор для "end"-итератора
         */
        Iterator();

        /** \brief Конструктор "начального" итератора
         *  \param parent Указатель на MaximalSetConstructor и инициализирует стек для DFS.
         */
        explicit Iterator(const MaximalSetConstructor& parent);

        /** \brief Оператор разыменования — даёт текущее "максимальное" множество
         */
        reference operator*() const;

        /** \brief Оператор "стрелка" — для совместимости с итераторами
         */
        pointer operator->() const;

        bool operator==(const self_type& rhs) const;
        bool operator!=(const self_type& rhs) const;

        /** \brief Оператор инкремента: двигаемся к следующему максимальному множеству
         */
        self_type& operator++();
        self_type operator++(int) = delete;

    private:
        /** \brief Ищем следующее "максимальное" множество (или выясняем, что их больше нет)
         *  \param start_index Начинаем искать с этого индекса.
         */
        bool fetchNextMaximalSet(size_t start_index);

        /** \brief Сбрасываем последний бит и возвращаем его позицию.
         */
        size_t resetLastBit();

        /** \brief Проверяем, можно ли добавить любой из предыдущих
         *  еще не выставленных битов в множество.
         *  \param end_index Позиция последнего бита для проверки, не включительно
         */
        bool checkPreviousAppendable(size_t end_index) const;

    private:
        const MaximalSetConstructor* _parent;     // ссылка на родительский объект
        bool                   _exhausted;  // флаг, что итератор закончился
        boost::dynamic_bitset<> _currentSet; // текущее (последнее найденное) множество
    };

public:
    /** \brief Constructor.
     *  \param set_size The size of the sets to be constructed.
     *  \param is_appendable The callback to check if a set is appendable.
     */
    MaximalSetConstructor(size_t set_size, IsAppendableCallback is_appendable);

    //------------------------------------------------------------------------------------------
    // begin()/end() для range-based for. 
    // * begin() возвращает итератор, который «запускает» DFS с пустого множества.
    // * end() — специальный итератор, обозначающий отсутствие дальнейших значений.
    //------------------------------------------------------------------------------------------
    Iterator begin() const;
    Iterator end() const;

    friend class Iterator;

private:
    size_t _set_size;
    IsAppendableCallback _is_appendable;
};

}}}} // namespace xi { namespace ldopa { namespace pn { namespace alpha {

#endif // XI_LDOPA_PN_ALGOS_ALPHA_MAXIMAL_SET_ITERATOR_H_