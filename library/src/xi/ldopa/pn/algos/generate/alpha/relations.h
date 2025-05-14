////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Collections Library: Relations for Process Mining
/// \author    ganvas
/// \version   0.1.0
/// \date      2025-03-20
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_COLLECTIONS_RELATIONS_H_
#define XI_LDOPA_COLLECTIONS_RELATIONS_H_

#pragma once

// std
#include <map>
#include <set>

namespace xi { namespace ldopa { namespace pn {;   //

/** \brief Base class for binary relations over activities
 */
template<typename T>
class BinaryRelations {
public:
    typedef T ElementType;
    typedef std::map<ElementType, std::set<ElementType>> RelationMap;

    /** \brief Adds relation a->b */
    void add(const ElementType& a, const ElementType& b) {
        _relation[a].insert(b);
    }

    /** \brief Returns true if relation a->b exists */
    bool contains(const ElementType& a, const ElementType& b) const {
        auto it = _relation.find(a);
        if (it != _relation.end()) {
            return it->second.find(b) != it->second.end();
        }
        return false;
    }

protected:
    RelationMap _relation;
};

/** \brief Direct succession relation (>) */
template<typename T>
class DirectSuccessionRelations : public BinaryRelations<T> {
    // Specific methods for direct succession if needed
};

/** \brief Causal dependency relation (->) */
template<typename T>
class CausalDependencyRelations : public BinaryRelations<T> {
    // Specific methods for causal dependency if needed
};

/** \brief Parallel relation (||) */
template<typename T>
class ParallelRelations : public BinaryRelations<T> {
    // Specific methods for parallel relation if needed
};

}}} // namespace xi { namespace ldopa { namespace pn {

#endif // XI_LDOPA_COLLECTIONS_RELATIONS_H_