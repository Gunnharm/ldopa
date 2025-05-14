
#include "maximal_set_constructor.h"

namespace xi { namespace ldopa { namespace pn { namespace alpha {;   //


MaximalSetConstructor::MaximalSetConstructor(size_t set_size, IsAppendableCallback is_appendable)
    : _set_size(set_size), _is_appendable(is_appendable)
{
}

MaximalSetConstructor::Iterator MaximalSetConstructor::begin() const
{
    return Iterator(*this);
}

MaximalSetConstructor::Iterator MaximalSetConstructor::end() const
{
    return Iterator(); // «пустой» итератор, помеченный как exhausted
}

MaximalSetConstructor::Iterator::Iterator()
    : _parent(nullptr), _exhausted(true)
{
}

MaximalSetConstructor::Iterator::Iterator(const MaximalSetConstructor& parent)
    : _parent(&parent), _exhausted(false), _currentSet(parent._set_size)
{
    // Найдём первое "максимальное" множество (или поймём, что их нет)
    bool found = fetchNextMaximalSet(0);
    if (!found) {
        _exhausted = true;
    }
}

MaximalSetConstructor::Iterator::reference MaximalSetConstructor::Iterator::operator*() const 
{
    return _currentSet;
}

MaximalSetConstructor::Iterator::pointer MaximalSetConstructor::Iterator::operator->() const 
{
    return &_currentSet;
}

bool MaximalSetConstructor::Iterator::operator==(const self_type& rhs) const 
{
    // Для InputIterator допустимо считать итераторы равными,
    // если оба "исчерпаны" (exhausted), либо если это один и тот же объект.
    if (_exhausted && rhs._exhausted) {
        return true;
    }
    if (_exhausted != rhs._exhausted) {
        return false;
    }
    return _currentSet == rhs._currentSet;
}

bool MaximalSetConstructor::Iterator::operator!=(const self_type& rhs) const 
{
    return !operator==(rhs);
}

MaximalSetConstructor::Iterator::self_type& MaximalSetConstructor::Iterator::operator++() 
{
    bool found = false;
    while (!found) {
        size_t last_bit_pos = resetLastBit();
        found = fetchNextMaximalSet(last_bit_pos + 1);
        if (!_currentSet.any()) {
            _exhausted = true;
            break;
        }
        if (checkPreviousAppendable(last_bit_pos + 1)) {
            found = false;
        }
    }
    return *this;
}

bool MaximalSetConstructor::Iterator::checkPreviousAppendable(size_t end_index) const {
    for (size_t i = 0; i < end_index; ++i) {
        if (!_currentSet.test(i)) {
            if (_parent->_is_appendable(_currentSet, i)) {
                return true;
            }
        }
    }
    return false;
}

bool MaximalSetConstructor::Iterator::fetchNextMaximalSet(size_t start_index) 
{
    bool found = false;
    while (start_index < _parent->_set_size) {
        if (_parent->_is_appendable(_currentSet, start_index)) {
            _currentSet.set(start_index);
            found = true;
        }
        start_index++;
    }

    return found;
}

size_t MaximalSetConstructor::Iterator::resetLastBit()
{
    size_t idx = _currentSet.find_first();
    if (idx == _currentSet.npos) {
        return idx;
    }
    size_t new_idx = _currentSet.find_next(idx);
    while (new_idx != _currentSet.npos) {
        idx = new_idx;
        new_idx = _currentSet.find_next(idx);
    }
    _currentSet.reset(idx);
    return idx;
}


}}}} // namespace xi { namespace ldopa { namespace pn { namespace alpha {
