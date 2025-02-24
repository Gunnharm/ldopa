////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief     LDOPA Petri net Library
/// \author    Sergey Shershakov
/// \version   0.1.0
/// \date      01.08.2018
/// \copyright (c) xidv.ru 2014–2017.
///            This source is for internal use only — Restricted Distribution.
///            All rights reserved.
///
/// Classes for Petri net synthesis from TSs using theory of regions.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef XI_LDOPA_PN_ALGOS_REGIONS_PN_SYNTHESIS_H_
#define XI_LDOPA_PN_ALGOS_REGIONS_PN_SYNTHESIS_H_

#pragma once

// ldopa dll
#include "xi/ldopa/ldopa_dll.h"
#include <xi/ldopa/utils.h>

// ldopa
#include "xi/ldopa/ts/models/evlog_ts_red.h"
#include "xi/ldopa/utils/elapsed_time.h"

// std
//#include <unordered_set>
#include <set>
#include <map>


namespace xi { namespace ldopa { namespace pn {;   //


#pragma region Std extension types - кандидаты на вынос в отд. библиотеку


//==============================================================================
// class MapKeyIterator - кастомный итератор для перебора ключей мапы без создания
// отдельного множества
// TODO: вынести в xiLib!
// он по определению константный?!
// https://stackoverflow.com/questions/681943/how-can-i-get-a-stdset-of-keys-to-a-stdmap
//==============================================================================

// итератор по ключам — immutable sequence, т.е. это const_iterator!
template<typename TMap>
class MapKeyIterator : public TMap::const_iterator
{
public:
    typedef typename TMap::const_iterator MapIterator;
    typedef typename MapIterator::value_type::first_type KeyType;   // уже д.б. const!

    MapKeyIterator() : MapIterator() {}

    MapKeyIterator(const MapIterator& other) : MapIterator(other) {}

    KeyType& operator *()                                           // уже д.б. const!
    {
        return MapIterator::operator*().first;
    }
}; // class MapKeyIterator


// Подключает к мапе такие фишки, как получение пары итераторов для перебора диапазона ключей без
// создания новой коллекции.
template<typename TMap>
class XiMapDecorator : public TMap
{
public:
    // Types
    typedef TMap Base;                                          ///< Base map type.
    typedef MapKeyIterator<Base> KeyIterator;                   ///< Key iterator type.
    typedef std::pair<KeyIterator, KeyIterator> KeyIteratorPair;///< Pair of key iterators for iterating over map keys range.

public:
    // iterator helpers

    /// Returns a begin iterator over the range of keys.
    inline KeyIterator beginKey() const { return KeyIterator(begin()); }

    /// Returns an end iterator over the range of keys.
    inline KeyIterator endKey() const { return KeyIterator(end()); }

    inline KeyIteratorPair keys() const
    {
        return std::make_pair(beginKey(), endKey());
    }
}; // class XiMapDecorator


// Подключает к сету такие фишки, как определение подмножества, удаление надмножеств и т.д.
template<typename TSet>
class XiSetDecorator : public TSet
{
public:
    // Types
    typedef TSet Base;                          ///< Base set type.
    typedef typename Base::key_type Key;                 ///< Key type of the base set type.
    typedef typename Base::value_type Value;             ///< Value type of the base set type (eq. to Key).
public:
    /// Default constructor.
    XiSetDecorator() : Base() { }

    /// Initializer list constructor.
    XiSetDecorator(const std::initializer_list<Value>& il) : Base(il) { }
    // см. соотв. конструктор std::set

public:
    /** \brief Returns true if the given state \a s in in the set, false otherwise. */
    // #appnote: здесь не имеет смысл с traits-ами для плоских типов изгаляться, т.к. в 
    // find все равно используется find(const key_type& _Keyval)
    inline bool contains(const Key& key) const
    {
        return (find(key) != end());
    }

    /** \brief Checks whether \a this is not necessarily a strict subset of \a rhv.
     *
     *  \returns true if all the elements of \a this are also in \a rhv, false otherwise.
     */   
    inline bool isSubsetOf(const XiSetDecorator& rhv) const
    {
        return std::includes(rhv.begin(), rhv.end(), this->begin(), this->end());
    }

    /** \brief Checks whether \a this is a strict subset of \a rhv.
     *
     *  \returns true if all the elements of \a this are also in \a rhv and the sets
     *  are not equal, false otherwise.
     */
    inline bool isStrictSubsetOf(const XiSetDecorator& rhv) const
    {
        // доп. условие: подмножество и неравны размеры
        return (this->isSubsetOf(rhv) && this->size() != rhv.size());
    }

    /** \brief Checks whether \a this is not necessarily a strict superset of \a rhv.
     *
     *  \returns true if all the elements of \a rhv are also in \a this, false otherwise.
     */   
    inline bool isSupersetOf(const XiSetDecorator& rhv) const
    {
        return rhv.isSubsetOf(*this);
    }

    /** \brief Checks whether \a this is a strict superset of \a rhv.
     *
     *  \returns true if all the elements of \a rhv are also in \a this and the sets
     *  are not equal, false otherwise.
     */ 
    inline bool isStrictSupersetOf(const XiSetDecorator& rhv) const
    {
        // доп. условие: подмножество и неравны размеры
        return rhv.isStrictSubsetOf(*this);
    }

    /** \brief Removes from the set \a this all strict supersets of the set \a r.
     *
     *  \retuns number of supersets that has been removed.
     */
    size_t removeAllStrictSupersets(const Key& r)
    {
        // перебираем все множества в s и те из них, для которых r является строгим подмножеством
        // (т.е. такие, которые строгие надмножества над r), удаляем
        // "правильное" удаление элементов из множества при итерировании:
        // https://stackoverflow.com/questions/20627458/how-to-remove-elements-from-an-stdset-while-iterating-over-it
        size_t num = 0;         // число удаленных
        for(Base::iterator it = begin(); it != end(); )
        {
            const Key& ri = *it;
            if (r.isStrictSubsetOf(ri))
            {
                it = erase(it);
                ++num;
            }                
            else
                ++it;
        }
        
        return num;  
    }

    /** \brief Removes from the set \a this all not necessarily strict supersets of the set \a r.
     *
     *  \retuns number of supersets that has been removed.
     *  
     *  #thinkof: вообще, этот и предыдущий метод справедливы только для случая, когда
     *  элементами данного множества является в свою очередь множество, т.е. к параметру
     *  \a r применимы операции над множествами. Хорошо ли это в таком шаблоне оставлять?..
     */
    size_t removeAllSupersets(const Key& r)
    {
        // перебираем все множества в s и те из них, для которых r является подмножеством
        // (т.е. такие, которые надмножества над r), удаляем
        size_t num = 0;         // число удаленных
        for(Base::iterator it = begin(); it != end(); )
        {
            const Key& ri = *it;
            if (r.isSubsetOf(ri))
            {
                it = erase(it);
                ++num;
            }                
            else
                ++it;
        }
        
        return num;  
    }


}; // XiSetDecorator


// Подключает к реф-врепперу сравнение по значению объекта
template<typename TRefWrapper>
class XiRefWrDecorator : public TRefWrapper
{
public:
    // Types
    typedef TRefWrapper Base;                          ///< Base set type.
public:
    /// Default constructor.
    XiRefWrDecorator() : Base() { }

    /// Initialize with a base wrapper.
    XiRefWrDecorator(const Base& ref) : Base(ref) {}

public:
    bool operator==(const XiRefWrDecorator& rhv) const
    {
        return get() == rhv.get();
    }

    bool operator<(const XiRefWrDecorator& rhv) const
    {
        return get() < rhv.get();
    }
}; // class XiRefWrDecorator

#pragma endregion Std extension types


//==============================================================================
// class PnRegSynthesizer
//==============================================================================


/** \brief Template class for synthesis a Petri net from a TS.
 *
 *  \tparam TS type of a transition system.
 *  \tparam PN type of a Petri net.
 *  
 *  Requirements for template parameter types.
 *  \a PN is a petri net, which:
 *  *   has a method for addinf a new transition, named addTransition(lbl), and
 *      obtaining const Label& lbl as a parameter, where Label is the type TS::TrLabel.
 */
template<typename TS, typename PN>
class PnRegSynthesizer 
    : public ElapsedTimeStore       // вспомогательный класс для учета времени алгоритма
{
public:
#pragma region Types

    typedef typename TS::TrLabel        Label;              ///< Type for transition labels.
    typedef typename TS::State          TsState;            ///< TS State.
    typedef typename TS::StateIter      TsStateIter;        ///< TS State Iterator.
    typedef typename TS::Transition     TsTransition;       ///< TS transition.
    typedef typename TS::TransIter      TsTransIter;        ///< TS Transition Iterator.
    typedef typename TS::OtransIter     TsOtransIter;       ///< TS output transition iterator.

    typedef typename PN::Position       PnPosition;         ///< TODO: docs
    typedef typename PN::Transition     PnTransition;
    typedef typename PN::PTArc          PnPTArc;
    typedef typename PN::TPArc          PnTPArc;
    typedef typename PN::TransIter      TransIter;

    typedef XiSetDecorator< std::set < TsState > >                      StateSet;
    typedef XiSetDecorator< std::set < TsTransition > >                 TransSet;

    // special predicates/comparators for compare set elements
    
    /** \brief Custom comparator for ordering StateSet elements in such way 
     *  where the smallest (in term of its cardinality) set is going first.
     */
    struct StateSetSizeComp
        : public std::binary_function < StateSet, StateSet, bool >
    {	
        /// Functor for comparing.
        bool operator()(const StateSet& lhv, const StateSet& rhv) const
        {	
            if (lhv.size() == rhv.size())
                return lessComparator(lhv, rhv);

            return (lhv.size() < rhv.size());

        }

        std::less<StateSet> lessComparator;
    }; // struct StateSetSizeComp

    typedef XiSetDecorator< std::set < StateSet, StateSetSizeComp > >   SetOfStateSets;
    typedef typename SetOfStateSets::iterator SetOfStateSetsIter;
    
    //typedef XiRefWrDecorator< std::reference_wrapper < const Label > >  LabelCRefWr;


    /** \brief Represents a transition label accompanied w/ information about its splitting
     *  givent in the sorm of split index (integer). 
     *  
     *  В результате экспериментов стало понятно, что идея с LabelCRefWr провалится, т.к.
     *  со временными объектами беда получается, а эти метки должны жить себе долго. Т.е. 
     *  так, например, будет проблема (причем, сложноотлавливаемая), 
     *      synth.genMinPreRegions(SplitLabel(String("b")));
     *  а вот так замучаешься делать, да и проверять.
     *      PNSynth::Label llb(String("b"));
     *      SplitLabel slb(llb);
     *      synth.genMinPreRegions(slb);
     */
    class SplitLabel 
        : public std::pair < Label, unsigned int > 
    {
    public:
        typedef std::pair < Label, unsigned int > Base;    ///< Base type alias.
    public:
        // Constructors

        /// Default.
        SplitLabel() : Base() {}
        
        /// Constructs with the label \a lbl.
        SplitLabel(const Label& lbl) : Base(lbl, 0) {} // Base(std::cref(lbl), 0) {}

        /// Constructs with the label \a lbl and the split index \a ind.
        SplitLabel(const Label& lbl, unsigned int ind) : Base(lbl, ind) {}//Base(std::cref(lbl), ind) {}

    public:
        /// Returns a const transition label.
        const Label& label() const { return first/*.get()*/; }

        /// Returns a split index.
        unsigned int splitInd() const { return second; }

        /// Sets a new split index.
        void setSplitInd(unsigned int newInd) { second = newInd; }

    }; // class SplitLabel

    // hasher for TsTransition
    struct TsTransitionHash {

        //typedef UInt EDGERAW[2];           // структура для получения сырых данных edge

        std::size_t operator()(const TsTransition& t) const
        {
            // https://stackoverflow.com/questions/11742593/what-is-the-hashcode-for-a-custom-class-having-just-two-int-properties
            UInt* praw = (UInt*)&t;
            size_t res = *(praw++);
            res += *praw * 31;

            return res;
            //return 42;  // TODO:
        }
    }; // struct TsTransitionHash

    typedef std::map< TsTransition, unsigned int> TransIntMap;          // довольно медленно
    typedef typename TransIntMap::iterator TransIntMapIter;
    typedef typename TransIntMap::const_iterator TransIntMapCIter;

    typedef XiMapDecorator< std::map < SplitLabel, StateSet > >  Lbl2StateSetMap;
    typedef typename Lbl2StateSetMap::iterator              Lbl2StateSetMapIter;
    typedef typename Lbl2StateSetMap::KeyIterator           LblKeyIter;
    typedef typename Lbl2StateSetMap::KeyIteratorPair       LblKeyIterPair;

    
    typedef XiMapDecorator< std::map < SplitLabel, TransSet > >  Lbl2TransSetMap;
    typedef typename Lbl2TransSetMap::iterator              Lbl2TransSetMapIter;
    typedef typename Lbl2TransSetMap::KeyIterator           LblKeyTrSetIter;
    typedef typename Lbl2TransSetMap::KeyIteratorPair       LblKeyTrSetIterPair;


    /// Maps a label to a set of set of states (set of regions).
    typedef std::map < SplitLabel, SetOfStateSets > Lbl2SetOfStateSetsMap;
    typedef typename Lbl2SetOfStateSetsMap::iterator Lbl2SetOfStateSetsMapIter;
    typedef typename Lbl2SetOfStateSetsMap::const_iterator Lbl2SetOfStateSetsMapCIter;

    /// Mapping of TS' labels to integers.
    typedef std::map<Label, unsigned int> LabelIntMap;
    typedef typename LabelIntMap::const_iterator LabelIntMapCIter;

    /// Set of events/labels.
    typedef XiSetDecorator< std::set< SplitLabel > > SetOfEvents;
    typedef typename SetOfEvents::iterator SetOfEventsIter;
    typedef typename SetOfEvents::const_iterator SetOfEventsCIter;

    /// Mapping of SplitLabels to PN transitions for quick loopkup.
    typedef std::map < SplitLabel, PnTransition > SplitLblTransMap;

    /// Alias for EventLog Petri net marking
    typedef typename PN::Marking PnMarking;

    /** \brief Defines a region condtion holder. 
     * 
     *  Also used for representing relation a transition to a set of states.
     */
    class RegCond : public std::bitset < 4 > {
    public:
        //-----<Const>-----
        /** \brief List of pseudo-constants for numbering bits in bitset */
        enum {
            F_IN = 0,           ///< Event is inside a region.
            F_OUT,              ///< Event is outside a region.
            F_ENTER,            ///< Event enters a region.
            F_EXIT,             ///< Event exits a region.
        };

        // Methods
        inline bool isIn() const { return test(F_IN); }             ///< Tests for in.
        inline bool isOut() const { return test(F_OUT); }           ///< Tests for out.
        inline bool isEnter() const { return test(F_ENTER); }       ///< Tests for enter.
        inline bool isExit() const { return test(F_EXIT); }         ///< Tests for exit.

        inline void setIn() { set(F_IN); }                          ///< Sets in.
        inline void setOut() { set(F_OUT); }                        ///< Sets out.
        inline void setEnter() { set(F_ENTER); }                    ///< Sets enter.
        inline void setExit() { set(F_EXIT); }                      ///< Sets exit.

        //----<Predicates for Regions>-----

        ///** \brief Returns true if the given pair \a (e, r) violates the enter axiom for regions. */
        inline bool regEnterAxiomViolates()  const
        {
            return (isEnter() && (isIn() || isOut() || isExit()));
        }

        /** \brief Returns true if the given pair \a (e, r) violates the exit axiom for regions. */
        inline bool regExitAxiomViolates()  const
        {
            return (isExit() && (isIn() || isOut() || isEnter()));
        }

        /** \brief Returns true if any of the valid region axiom is violated. */
        inline bool regAxiomsViolates()  const
        { 
            return regEnterAxiomViolates() || regExitAxiomViolates(); 
        }

        // individual cases of the Lemma 4.2 [Cortadella et al. 98]
        
        /** \brief returns true if either case 1) or case 2) of Lemma 4.2 is violated. */
        inline bool isL42Cases12Violated() const
        {
            // если полагать in = x1, enter = x2, exit = 3, то условие
            // 1) Леммы 4.2 имеет вид: x1 && (x2 || x3) || x2 && x3 =
            // x1x2 || x1x3 || x2x3, т.е. больше не сократить
            return ( isIn() && (isEnter() || isExit()) )    // 1)
                || ( isEnter() && isExit() );               // 2)
        }


        /** \brief returns true if case 3) of Lemma 4.2 is violated. */
        inline bool isL42Case3Violated() const
        {
            return isOut() && isEnter();
        }


        /** \brief returns true if case 4) of Lemma 4.2 is violated. */
        inline bool isL42Case4Violated() const
        {
            return isOut() && isExit();
        }

        /** \brief Returns true if at least one condition of Lemms 4.2 is violated. */
        inline bool isL42Violated() const
        {
            return isL42Cases12Violated() || isL42Case3Violated()
                || isL42Case4Violated();
        }


        /** \brief Returns true if all 4 conditions are determined. */
        inline bool completed() const  { return all(); }


        //----<Predicates for Transition-StatesSet relation>-----
        
        /** \brief Returns true if a transition does not cross a set of states. */
        inline bool nocross() const
        {
            return isIn() || isOut();
        }



    }; // class RegCond


    /** \brief Helper class that maps a prepared events and regions to a PN elements. 
     * 
     *  It is no need to pass an individual collection of the event cause it also can be
     *  obtained from \a eventRegs mapping.
     */
    class RegionMapper {
    public:
        /// Region to PN Position mapping.
        typedef std::map <StateSet, PnPosition> Region2PnPosMap;
        typedef typename Region2PnPosMap::iterator Region2PnPosMapIter;
    public:
        
        /// Contrcuts a mapper object with the collection of events---set-of-regions mapping.
        RegionMapper(/*const Lbl2SetOfStateSetsMap& eventRegs, *//*const*/ PnRegSynthesizer* owner)
            //: _eventRegs(eventRegs)
            : _owner(owner)
        {
        }


        /** \brief Maps the events-regions to the given empty Petri net \a pn. */
        void map(PN* pn)
        {
            _pn = pn;
            mapEvents();                    // отображаем события сперва
            mapRegions();                   // отображаем регионы
        }


        /** \brief Reestablishes self-loops on the top of the synthesized PN.
         *
         *  Utilize the point that is a state \a s in the TS has a self-loop, this state
         *  corresponds to a set of regions (p1, p2, ...) the resulting PN, whics contain this
         *  \a s.
         *  These regions/place must be connected to the (new) corresponding PN-transition.
         */
        void reestablishSelfLoops(PN* pn)
        {
            _pn = pn;
            TS* ts = _owner->_ts;

            // вот туточки прямо перебираем все транзиции исходной TS
            TS::TransIter tIt, tEnd;
            boost::tie(tIt, tEnd) = ts->getTransitions();
            for (; tIt != tEnd; ++tIt)
            {
                TsTransition& t = *tIt;
                if (!_owner->isSelfLoop(t))                // пропускаем все непетли
                    continue;

                // петля
                TsState s = ts->getSrcState(t);            // состояние, к которому петля привязан
                const Label& lbl = ts->getTransLbl(t);     // пометка

                // создаем новую транзицию, помеченную соответствующим образом, в PN
                PnTransition slTr = _pn->addTransition(lbl);

                // рассматриваем все регионы, те из них, где есть это состояние s,
                // соответствуют позициям в сети Пети и надо петли навести
                for (auto regionsPair : _owner->_minPreRegions)
                {
                    const SetOfStateSets& evRegions = regionsPair.second;
                    for (const StateSet& region : evRegions)
                    {
                        // если регион не содержит соответсвующую букву, значит работаем с соответствующей
                        // ему позиций
                        if (!region.contains(s))
                            continue;

                        // содержит
                        PnPosition pnp = getPnPos(region);
                        _pn->getOrAddArc(pnp, slTr);
                        _pn->getOrAddArc(slTr, pnp);
                    }
                }
            }
        }

    protected:

        /// Maps events to Petri net transitions.
        void mapEvents()
        {
            _events2trans.clear();                       

            // ключами мапы будут как раз события, для каждого из них создадим транзицию
            // по каждому событию м.б. несколько пререгионов
            for (Lbl2SetOfStateSetsMapCIter erIt = _owner->_minPreRegions.begin(); 
                erIt != _owner->_minPreRegions.end(); ++erIt)
            {
                const SplitLabel& e = erIt->first;                  // событие
                
                // #apinote: в типе PN д.б. метод, принимающий const Label& lbl
                PnTransition trans = _pn->addTransition(e.label());
                _events2trans.insert( {e, trans} );                   // чтобы избежать []
            }
        }

        /** \brief Maps regions to PN places. 
         * 
         *  New version allowing self-loops.
         */
        void mapRegions()
        {
            TS* ts = _owner->_ts;           // shortcut

            _processedRegions.clear();

            // метод как есть без учета дублей!
            // по каждому событию м.б. несколько пререгионов
            for (Lbl2SetOfStateSetsMapCIter erIt = _owner->_minPreRegions.begin();
                erIt != _owner->_minPreRegions.end(); ++erIt)
            {
                // по каждому пререгиону из каждого события
                const SetOfStateSets& evRegions = erIt->second; // мн-во минпрергионов для буквы

                //for (SetOfStateSetsIter regIt = evRegions.begin(); regIt != evRegions.end(); ++regIt)
                for (const StateSet& region : evRegions)
                {
                    // каждому региону соответствует позиция в PN
                    PnPosition pnp = getOrAddPnPos(region);

                    // перебираем все буквы
                    LblKeyTrSetIter evIt, evEnd;
                    boost::tie(evIt, evEnd) = _owner->getEvents2();
                    for (; evIt != evEnd; ++evIt)
                    {
                        const SplitLabel& e = *evIt;                    // сплит-событие
                        PnTransition& pnt = _events2trans.at(e);        // соотв. ему транзиция

                        RegCond cond = _owner->checkRegCondition(e, region);

                        // конкретный вопрос: относится ли регион region к прергионам тек. события e?
                        const SetOfStateSets& mpre = _owner->_minPreRegions.at(e);
                        if (mpre.contains(region))                          // да! (случай 3.c модиф. алгоритма)
                        {
                            PnPTArc arc = _pn->getOrAddArc(pnp, pnt);

                            // тут же проверяем кейс с петлями (случай 3.c.i )
                            if (_owner->_options.getSelfLoopPolicy() == Options::slProcess)
                            {
                                // если для пререгиона ВДРУГ нашлась какая-то дуга, которая оказалась ВНУТРИ,
                                // значит это петля, а значит этот пререгион одновременно и пострегионом является
                                // TODO: а может ли быть снаружи?!?!?!
                                if (cond.isIn())                            // петля!
                                {
                                    PnTPArc arcBack = _pn->getOrAddArc(pnt, pnp);
                                }
                            }
                        }
                        else                                                // нет, не пререгион
                        {
                            // если есть хотя бы одна входящая дуга, значит это пострегион
                            if (cond.isEnter())
                            {
                                PnTPArc arc = _pn->getOrAddArc(pnt, pnp);
                            }
                        } // if 
                    } // for (const SplitEvent& e)
                } // for Region...
            } // for Regions...
        }


        /** \brief For the given region \a regions returns a corresponding position 
         *  in the Petri net, existing or new one.
         */
        PnPosition getOrAddPnPos(const StateSet& region)
        {
            Region2PnPosMapIter it = _processedRegions.find(region);

            // если регион ранее не был добавлен
            if (it == _processedRegions.end())
            {
                PnPosition pos =
                    (_owner->_options.isOutPlaceTitles()) ?
                    _pn->addPosition(Label(makeRegionTitle(region))) :
                    _pn->addPosition();

                // смотрим также, включает ли регион начальные состояния
                if (_owner->isContainsInitState(region))
                    _owner->_initMarking[pos] = 1;

                // добавляем мэппинг региона на новую позицию
                _processedRegions.insert({ region, pos });

                return pos;
            }

            // если регион уже был добавлен ранее, возвращаем просто соответствующую позицию
            return it->second;
        }

        /** \brief For the given region \a regions returns a corresponding position 
         *  in the Petri net, existing or new one.
         */
        PnPosition getPnPos(const StateSet& region)
        {
            Region2PnPosMapIter it = _processedRegions.find(region);

            // если регион ранее не был добавлен
            if (it == _processedRegions.end())
                throw LdopaException("There is no PN place for the given region.");

            return it->second;
        }

        /** \brief Nakes a string title for the given region. */
        std::string makeRegionTitle(const StateSet& region)
        {
            std::string res;
            for (TsState s : region)
            {
                res += _owner->_ts->makeStateIDStr(s);
                res += ',';
            }
            return res;
        }

    protected:


        /// A Petri net object for mapping onto.
        PN* _pn;

        /// Stores a mapping of events to the created PN transisions.
        SplitLblTransMap _events2trans;

        /// Stores owner synthesizer.
        /*const*/ PnRegSynthesizer* _owner;

        /// Maps processed regions to the corresponding PnPosition.
        Region2PnPosMap _processedRegions;

    }; // class RegionMapper

    friend class RegionMapper;


    /** \brief Synthesis options. */
    class Options {//: public std::bitset < 4 > {
    public:
        //-----<Const>-----
        /** \brief List of pseudo-constants for numbering bits in bitset (positions). */
        enum {
            P_MAKE_WFNET = 0,               ///< Makes a WF-net after PN synthesis.
            P_OUT_PLACE_TITLES = 1,         ///< Output place titles/regions as labels of transitions of ts.
            //
            _P_LAST_                        ///< Pseudoend.                        
        };

        /** \brief List of pseudo-constants for setting bits in bitset (values). */
        enum {
            F_MAKE_WFNET        = 1 << P_MAKE_WFNET,
            F_OUT_PLACE_TITLES  = 1 << P_OUT_PLACE_TITLES,

            F_DEFAULT = 0
        };

        /// Policy of handling self-loops. 
        enum SelfLoopPolicy {
            slIgnore,                       ///< Ingone self-loops (01).                        
            slReestablish,                  ///< Reestablish self-loops on the resulting PN (00).            
            slProcess,                      ///< New modification of algorithm, dealing with self-loops well (10).
            slBreak,                        ///< Break the synthesis (11).
        };

        typedef std::bitset < _P_LAST_ > Flags;     ///< Alias for the bitset-for-flags type.
    public:
        // constructors
        //Options() {}  //: Base() {}

        Options(unsigned long flags = F_DEFAULT, SelfLoopPolicy sLoopsPolicy = slIgnore) 
            : _flags(flags)
            , _sLoopsPolicy(sLoopsPolicy)
        {}
    public:
        // Methods
        inline bool isMakeWFNet() const { return _flags.test(P_MAKE_WFNET); }
        inline void setMakeWFNet(bool val) { _flags.set(P_MAKE_WFNET, val); }

        inline bool isOutPlaceTitles() const { return _flags.test(P_OUT_PLACE_TITLES); }
        inline void setOutPlaceTitles(bool val) { _flags.set(P_OUT_PLACE_TITLES, val); }

        inline SelfLoopPolicy getSelfLoopPolicy() const
        {
            return _sLoopsPolicy;

        }

        inline void setSelfLoopPolicy(SelfLoopPolicy slp)
        {
            _sLoopsPolicy = slp;
        }


        inline bool needBreakAtSelfLoop() const
        {
            return (getSelfLoopPolicy() == slBreak);
        }

        /// Stores flags.
        Flags _flags;

        /// Stores self-loops policy.
        SelfLoopPolicy _sLoopsPolicy;

    }; // class Options


#pragma endregion Types


public:

    /** \brief Default constructor. */
    PnRegSynthesizer(Options opts = Options())//Options::F_SLOOP_IGNORE) 
        : _ts(nullptr)
        , _pn(nullptr)
        , _options(opts)
        , _mapper(this)
        //, _elapsed(0)
    {
    }

    /** \brief Initializes the synthesizer with the given transition system \a ts. */
    PnRegSynthesizer(TS* ts, Options opts = Options())//Options::F_SLOOP_IGNORE)
        : _ts(ts)
        , _pn(nullptr)
        , _options(opts)
        , _mapper(this)
        //, _elapsed(0)
    {
    }

    /** \brief Destructor. */
    virtual ~PnRegSynthesizer()
    {
        clearPN();
    }

protected:
    PnRegSynthesizer(const PnRegSynthesizer&);                 // Prevent copy-construction
    PnRegSynthesizer& operator=(const PnRegSynthesizer&);      // Prevent assignment

public:
#pragma region Static helpers

    /** \brief Checks whether the given transition \a t of the TS \a ts 
     *  is a self-loop or not. 
     * 
     *  \returns true if \a t is a self-loop, false otherwise.
     *  TODO: вынести в определение класса TS!
     */
    static bool isSelfLoop(const TS* ts, const TsTransition& t)
    {
        return (ts->getSrcState(t) == ts->getTargState(t));
    }

#pragma endregion Static helpers

public:
    /** \brief Synthesize a PN.
     *
     *  DONE: политику работы с петлями
     *  TODO: политику возвращаемого значения
     */
    void synthesize()
    {
        clearPN();                              // очищаем предыдущие

        XI_LDOPA_ELAPSEDTIME_START(timer)
        _pn = new PN();                         // создаем новую сеть

        genPreRegions();                        // генерим пререгионы
        //findIrredundantCover();               // TODO: убираем избыточные регионы в соответствии со стратегией...
        mapRegsToPn();                          // выполняем отображение регионов на сетюпети


        // если нужно приделать петли, делаем
        if (_options.getSelfLoopPolicy() == Options::slReestablish)
            reestablishSelfLoops();

        // если установлена опция "сделать WF-сеть", делаем
        if (_options.isMakeWFNet())
            makeWFNet();
        XI_LDOPA_ELAPSEDTIME_STOP(timer)
    }


    /** \brief Makes a WF-net from the synthesized Petri net. */
    void makeWFNet()
    {
         
        // всегда добавляем сперва конечное состояние
        PnPosition acceptingPos = _pn->addPosition();

        PnTransition deltaDash;
        //bool found = false;
        UInt ddNum = 0;                                 // сохраняем число таких переходов
        TransIter tIt, tEnd;
        boost::tie(tIt, tEnd) = _pn->getTransitions();
        for (; tIt != tEnd; ++tIt)
        {
            const PnTransition& cur = *tIt;
            const Label* curLbl = _pn->getAttribute(cur);
            if (curLbl && (*curLbl == _wfTransLabel))
            {
                deltaDash = cur;                
                _pn->addArc(deltaDash, acceptingPos);
                ++ddNum;
            }            
        }

        // если таких дельта-штрих было 1 штука и к ней ведет одна дуга,
        // удаляем
        if (ddNum == 1 && (_pn->getInArcsNum(deltaDash) <= 1) )
        {
            _pn->removeVertex(deltaDash);
            _pn->removeVertex(acceptingPos);
        }

        // если же их вообще не было (не совсем корректная TS на входе — без нужных переходов)
        if (ddNum == 0)
        {
            _pn->removeVertex(acceptingPos);
        }

        // достраиваем начало
        size_t initMarkSz = _initMarking.getMap().size();
        if (initMarkSz == 0 || 
            initMarkSz == 1)            // в этом случае тоже не надо надстраивать: все уже ок
                return;

        // если же есть в начальной разметке что-то хорошее)
        PnPosition initPos = _pn->addPosition();
        PnTransition initTrans = _pn->addTransition();
        _pn->addArc(initPos, initTrans);
        
        for (PnMarking::PosNums::const_iterator kvIt = _initMarking.getMap().begin();
            kvIt != _initMarking.getMap().end(); ++kvIt)
        {
            const PnPosition& cur = kvIt->first;
             _pn->addArc(initTrans, cur);
        }
        
        // изменяем начальную разметку (ее, кстати, можно куда-нибудь сохранить)
        _initMarking.clear();
        _initMarking[initPos] = 1;
    }


    /** \brief Maps synthsized regions to the resulting Petri net.
     *
     *  According to the “Saturated PN synthesis” algorithm, p.864 [Cortadella et al, 1998].
     */
    void mapRegsToPn()
    {
        // для меппинга спец. хелперный объект
        _mapper.map(_pn);

    }

    /** \brief Returns true if the given set of states \a ss contains any initial state
     *  of the TS, false otherwise.
     */
    bool isContainsInitState(const StateSet& ss) const
    {
        return ss.contains(_ts->getInitState());
    }

    /** \brief For all classses of events generates pre-regions such that the Excitation Closure
     *  condition for them is hold.
     *
     *  The most important part of the Synthesis algorithm.
     */
    void genPreRegions()
    {
        // очищаем пред. структуры на всякий случай
        clearStructures();

        // маппируем все транзиции по их буквам (только для самого первого раза!)
        mapTransitions();

        // генерируем минимальные пререгионы по событиям до тех пор, пока для всех
        // событий не начнет соблюдаться EC-условие
        bool split;                                             // был ли сплит
        
        do 
        {
            split = false;                                      // пока не было сплита            
            LblKeyTrSetIter evIt, evEnd;
            boost::tie(evIt, evEnd) = getEvents2();          // TODO:1 getEvents2 !
            for (; evIt != evEnd; ++evIt)                       // перебираем все ивенты
            {
                    const SplitLabel& e = *evIt;                    // сплит-событие

                // структуры, которые одновременно в неск. методах используются
                SetOfStateSets explored;                        // мн-во проверенных множествосостояний
                StateSet minRegIntersect;                       // мн-во состояний, образованное пересечением минпререгионов
                
                genMinPreRegions(e, explored);                  // генерируем минпререгионы для тек. события
                if (!isExcitationClosureHold(e, minRegIntersect))// проверяем, соблюдается ли для него EC-условие 
                {
                    // не соблюдается!
                    splitLabels(e, explored, minRegIntersect);  // делаем сплит по текущему состоянию и все по-новой
                    _minPreRegions.clear();                     // пред. регионы нафиг
                    split = true;
                    break;                                      // прерываем цикл for
                }
            } // foreach(SplitEvent& e)...
        } while (split);                                        // крутим, пока был хотя бы один сплит
    }



    /** \brief Generates minimal preregions for the event \a ev.
     *
     *  According to the algorithm presented by [Cortadella et al., 1998].
     *  
     *  According to the Prop. 4.1 the method generates all minimal preregions of the ECTS 
     *  that are predecessors of the event \a ev. This regions a mapped to the event \a
     *  through the mapping \a _minPreRegions[ev].
     *  
     *  Method assumes that ERs are calculated in advance.
     */
    //void genMinPreRegions(const Label& ev)
    void genMinPreRegions(const SplitLabel& e)
    {
        SetOfStateSets explored;                                // множество уже проверенных регионов
        genMinPreRegions(e, explored);
    }


    /** \brief Overloaded version of the method genMinPreRegions(), obtaining 
     *  ref to the set of explored regions (for further using).
     */
    void genMinPreRegions(const SplitLabel& e, SetOfStateSets& explored)
    {
        // здесь, наверное, BDD существенно ускорить дело могут
        SetOfStateSets& minPreRegs = _minPreRegions[e];         // все минимальные прергионы прямо извнутри мапы (поэтому &)
        minPreRegs.clear();                                     // очищаем минпрергионы для тек. события

        StateSet r = getExcRegion(e);
        expandStates(r, minPreRegs, explored);                  // рекурсивно расширяем r до региона 
    }


    /** \brief Returns true if the given set of states is a valid region, false otherwise. 
     *
     *  The notion of region is given according to the Def. 2.2 of [Cortadella ert al., 1998].
     *  // TODO: для открытой части метода надо предусмотреть проверку, что уже были определены
     *  // excitation regions
     */
    bool isRegion(const StateSet& r) const
    {
        
        LblKeyTrSetIter evIt, evEnd;
        boost::tie(evIt, evEnd) = getEvents2();          // TODO:1 getEvents2 !
        for (; evIt != evEnd; ++evIt)
        {
            const SplitLabel& e = *evIt;

            // если для данного события нарушается аксиома входа или выхода, уже не регион
            RegCond cond = checkRegCondition(e, r);
            if (cond.regAxiomsViolates())
                return false;
        }

        // если для всех событий не были нарушены аксиомы региона, все ок, это регион
        return true;
    }


    /** \brief Checks whether exists at least one subset of the set \a r in the set \a s.
     *
     *  \returns true if a subset exists, false if no subsets exists.
     */
    bool subsetExists(const SetOfStateSets& s, const StateSet& r)
    {
        //return s.subsetExists(r);
        // перебираем все множества и смотрим, есть ли среди них хотя бы одно подмножество s
        for (const StateSet& rj : s)
        {
            if (rj.isSubsetOf(r))
                return true;
        }
        // если перебрали все и не нашли ни одного подмножества
        return false;
    }


    // #todo лучше это не использовать, а использовать методы классов
    /** \brief Removes from the set \a s all strict supersets of the set \a r.
     *
     *  \retuns true if at least one such super has been removed.
     */
    size_t removeAllStrictSupersets(SetOfStateSets& s, const StateSet& r)
    {
        return s.removeAllStrictSupersets(r);
    }


    // #todo лучше это не использовать, а использовать методы классов
    /** \brief Checks whether \a b is not necessarily a strict subset of \a a.
     *
     *  \returns true if all the elements of \a b are also in \a a, false otherwise.
     */
    inline bool isSubset(const StateSet& a, const StateSet& b)
    {
        return b.isSubsetOf(a);
    }


    // #todo лучше это не использовать, а использовать методы классов
    /** \brief Checks whether \a b is a strict subset of \a a.
     *
     *  \returns true if all the elements of \a b are also in \a a and the sets
     *  are not equal, false otherwise.
     */
    inline bool isStrictSubset(const StateSet& a, const StateSet& b)
    {
        return b.isStrictSubsetOf(a);
    }


    /** \brief Calculates an intersection of all sets contained in the given \a sset. */
    StateSet intersection(const SetOfStateSets& sset)
    {
        StateSet isect;
        if (sset.size() == 0)
            return isect;

        isect = *(sset.begin());            // пересечение равно первому множеству, а дальше в цикле    
                                            // первый элемент всегда будет, поэтому сразу инкрементируем!
        for (SetOfStateSets::iterator it = ++(sset.begin()); it != sset.end(); ++it)
        {
            StateSet temp;                  // сюда ляжет пересечение
            const StateSet& s1 = *it;       // первое множество
            const StateSet& s2 = isect;     // второе множество

            std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(),
                std::inserter(temp, temp.begin()));

            isect = temp;                   // здесь может получиться очень неоптимально!
        }

        return isect;
    }


    // #todo: isExcitationClosureHold() перенести в protected после обкатки
    /** \brief Checks whether for the given set of (min-pre-)-regions of the event \a e 
     *  the excitation closure property is hold (true) of not (false).
     *  
     *  The excitation closure property is checked according to the axioms from Def. 3.2 
     *  from [Cortadella et al. 98]
     *  \param e is an event, for which the excitation closure property is checked.
     *  \returns true if EC-property is hold, false otherwise.
     */
    //bool isExcitationClosureHold(const Label& e)
    bool isExcitationClosureHold(const SplitLabel& e)
    {
        StateSet minRegIsect;
        return isExcitationClosureHold(e, minRegIsect);   
    }

    /** \brief Overloaded version of the isExcitationClosureHold() method, obtainind 
     *  set for storing intersection or minpreregs as paramter \a minRegIsect.
     * 
     * */
    bool isExcitationClosureHold(const SplitLabel& e, StateSet& minRegIsect)
    {        
        // по определению 3.2, которое по сути и нужно проверить, система переходов 
        // является ECTS, если для каждого события e множество минимальных пререгионов
        // непусто и пересечение всех пререгионов дает в результате ER(e)

        // все минимальные прергионы прямо извнутри мапы (поэтому &) 
        const SetOfStateSets& minPreRegs = _minPreRegions[e];

        if (minPreRegs.size() == 0)                 // если прергионов нет,
            return false;                           // св-во автоматически не работает

        // необходимо найти пересечение всех пререгионов
        /*StateSet*/ minRegIsect = intersection(minPreRegs);
        StateSet er = getExcRegion(e);

        return (minRegIsect == er);
    }


    /** \brief Performs labels splitting for the event \event, for which the excitation closure
     *  condition does not hold.
     *
     *  Accordiing to the Sec. 4.3 [Cortadella et al. 1998].
     *  \param explored is the set of set-of-states explored during ER expansion while 
     *  minPreRegions calculated. Will be modified after the method executed.
     *  \param minRegIsect is the intersection of all minPreRegions for the event \a e,
     *  obtained while checking EC condition.
     *  Both \a explored and \a minRegIsect are reused for caching purpose.
     */
    void splitLabels(const SplitLabel& event, SetOfStateSets& explored, const StateSet& minRegIsect)
    {
        StateSet er_e = getExcRegion(event);
        explored.insert(er_e);

        // удаляем из множества исследованных множеств-состояний все те, которые являются
        // надмножествами пересечения (ограничение справа), включая само это пересечение
        explored.removeAllStrictSupersets(minRegIsect);

        // переберем все подходящие сеты (см. тж. переживания по поводу того, какие являются подходящими)
        SetOfEvents violEvts;                           // сет с метками, минимальным число нарушающими условия
        const StateSet* minViolSet = findMinViolatingSet(explored, violEvts);

        // перебираем все метки для сплитта, а не вообще все возможные!
        for (SetOfEventsIter evIt= violEvts.begin(); evIt != violEvts.end(); ++evIt)
        {
            const SplitLabel& e = *evIt;                    // сплит-событие

            // определим новые сплит-индексы для меток выходящих, непересекающих и входящих
            // пока-что там будет ноль, но как только впервые понадобится, изменим индекс

            unsigned int newExit    = 0;
            unsigned int newEnter   = 0;
            unsigned int newNocross = 0;


            // перебираем все транзиции, помеченные буковой e
            const TransSet& eTranss = getTransSet(e);
            for (const TsTransition& t : eTranss)
            {
                if (omitAsSelfLoop(t))          // петли либо исключаем, либо тут исключение
                    continue;


                // если транзиция помечена нужной меткой:
                RegCond trRel = checkTransSSetRelations(t, *minViolSet);// смотрим соотношение с множеством

                                                                        //===случай первый: транзиция не пересекает 
                if (trRel.nocross())                                    //===границы множества
                {
                    if (newNocross == 0)                                // если еще не задали новый сплитиндекс
                        newNocross = getNextSplitIndex(e.label());

                    setTrSplitIndex(t, newNocross);

                    // заводим новую сплит-метку, берем ей соответствующее множество отображения
                    // меток на транзиции и добавляем туда теперешнюю транзицию
                    putTransIntoLblGroup(t, e.label(), newNocross);
                }                
                else if (trRel.isExit())                                //===случай второй: транзиция выходит
                {
                    if (newExit == 0)
                        newExit = getNextSplitIndex(e.label());
                    setTrSplitIndex(t, newExit);
                    putTransIntoLblGroup(t, e.label(), newExit);
                }
                else if (trRel.isEnter())                                //===случай третий: транзиция входит
                {
                    if (newEnter == 0)
                        newEnter = getNextSplitIndex(e.label());
                    setTrSplitIndex(t, newEnter);
                    putTransIntoLblGroup(t, e.label(), newEnter);
                }
                else
                    throw std::logic_error("Invalid transition-stateset relation status.");
            } // foreach (TsTransition& t...)

            // DONE: событие e тут оказывается гарантированно расспличенным, поэтому его удаляем!
            removeLblTransSet(e);
        } // foreach (const SplitLabel& e...)
    }


    /** \brief Iterates over the set \a explored, looking for a set with possibly mininmum size 
     *  and (at the same time) possibly minimum number of events that violats ?region? condition.
     * 
     *  \param[in] explored is a set of state-sets for looking optimal one.
     *  \param[out] a set of events that violats the condition above to be returned back to the caller.
     *  \returns a const ptr to the optimal finded set.
     */
    const StateSet* findMinViolatingSet(const SetOfStateSets& explored, SetOfEvents& violEvts) const
    {
        const StateSet* minViolSet = nullptr;           // сет с наименьшим числом нарушений по меткам
        unsigned int minViolEvNum = (unsigned int)-1;   // число меток, которые нарушают минимальное мн-во
        unsigned int minViolSetSize = (unsigned int)-1; // размер минимального множества с минимальными нарушениями        

        for (SetOfStateSets::iterator it = explored.begin(); it != explored.end(); ++it)
        {
            SetOfEvents violEvents;                     // метки, нарушающие для текущего множества
            const StateSet& s = *it;                    // текущее множество

            LblKeyTrSetIter evIt, evEnd;
            boost::tie(evIt, evEnd) = getEvents2();          // TODO:1 getEvents2 !
            for (; evIt != evEnd; ++evIt)
            {
                const SplitLabel& e = *evIt;            // сплит-событие
                RegCond cond = checkRegCondition(e, s); // (не)нарушаемые им аксиомы регионов для мн-ва s
                if (cond.isL42Violated())
                    violEvents.insert(e);
            }

            // смотрим размер мн-ва и число нарушаемых событий:
            // первое условие: число нарушаемых меток д.б. больше, второе — регион д.б. меньше
            if ( (violEvents.size() < minViolEvNum) && (s.size() < minViolSetSize) )
            {
                violEvts = violEvents;                  // определились с тек. минимальным множеством меток
                minViolEvNum = violEvents.size();       // минимальных число нарушающих меток обновилось
                
                minViolSetSize = s.size();              // размер минимально нарушающего множества — тоже
                minViolSet = &s;                        // само множество
                
            }
        } //for (SetOfStateSets::iterator it
        return minViolSet;
    }


public:
#pragma region  Setters/getters

    /// Returns the synthesized Petri net.
    PN* getPn() const { return _pn;  }

    /// Returns the init marking for a synthesized PN.
    const PnMarking& getInitMarking() const { return _initMarking; }

    /// Returns the TS.    
    TS* getTs() const { return _ts; }

    /// Sets a new TS.
    void setTs(TS* ts) 
    { 
        if (ts == _ts)
            return;

        // если новая СП
        _ts = ts; 
        _events.clear();                // множество событий тж другое
    }

    /// Returns the range of events as a keys of the lbl-transitions collection.
    inline LblKeyTrSetIterPair getEvents2() const
    {
        return _lbl2Trans.keys();
    }

    /// Returns the ER(e) extracted from the lbl-transitions collection.
    StateSet getExcRegion(const SplitLabel& e) const
    {
        StateSet er;

        const TransSet& trSet = _lbl2Trans.at(e);
        for (const TsTransition& t : trSet)
        {
            TsState src = _ts->getSrcState(t);                  // откуда дуга (войдет в ER)
            er.insert(src);
        }
        return er;
    }


    /** \brief Returns a cref to the full set of transitions labeled exctly
     *  by label \a e.
     *
     *  If no e-transitions set exists, throws an exception.
     */
    inline const TransSet& getTransSet(const SplitLabel& e) const
    {
        return _lbl2Trans.at(e);
    }

    /// Returns a mapping of all event classes to transisions labeled by these events.
    const Lbl2TransSetMap& getLbl2Transs() const { return _lbl2Trans; }

    /// Returns an equivalent split label for the transition \a t.
    inline SplitLabel getTransSplitLbl(const TsTransition& t) const
    {
        check4ts();
        return getTransSplitLblInt(t);
    }

    
    /** \brief Returns the current split index for the given label \a lbl. 
     * 
     *  If the label has not been splitted yet, returns 0.
     */
    unsigned int getLblSplitIndex(const Label& lbl) const
    {
        LabelIntMapCIter it = _lblCurrentSplitInd.find(lbl);
        if (it == _lblCurrentSplitInd.end())
            return 0;

        return it->second;
    }


    /** \brief Returns a next (new) split index for the label \a lbl.
     *
     *  Obtains a current split index, increates it, sets as the new current 
     *  split-index and returns it.
     */
    unsigned int getNextSplitIndex(const Label& lbl)
    {
        unsigned int cur = getLblSplitIndex(lbl);        
        _lblCurrentSplitInd[lbl] = ++cur;

        return cur;
    }

    
    /** \brief \returns a label, by which arcs leading to the artificial 
     *  single accepted state labeled. 
     */
    const Label& getWFTransLabel() const { return _wfTransLabel;  }

    /** \brief Sets a WF-accepted label. See getWFTransLabel() for explanation. */
    void setWFTransLabel(const Label& lbl ) { _wfTransLabel = lbl;  }

    /// Provides direct access to the synthesizer options.
    Options& options() { return _options;  }


#pragma endregion  Setters/getters

protected:

    /** \brief Reestablishes self-loops on the top of the synthesized PN.
     *
     *  Utilize the point that is a state \a s in the TS has a self-loop, this state
     *  corresponds to a set of regions (p1, p2, ...) the resulting PN, whics contain this
     *  \a s.
     *  These regions/place must be connected to the (new) corresponding PN-transition.
     */
    void reestablishSelfLoops()
    {
        _mapper.reestablishSelfLoops(_pn);
    }

    /** \brief Puts the transition \a t labeled with pure label \a lbl into the  
     *  group in lbl-transition mapping, defined as a pair (lbl, splitInd).
     */
    inline void putTransIntoLblGroup(const TsTransition& t, const Label& lbl, 
        unsigned int splitInd)
    {
        // заводим новую сплит-метку, берем ей соответствующее множество отображения
        // меток на транзиции и добавляем туда теперешнюю транзицию
        SplitLabel splitLbl(lbl, splitInd);
        TransSet& ts = _lbl2Trans[splitLbl];
        ts.insert(t);
    }


    /** \brief Remove a instance of a lbl-transitions mapping by the key \a lbl. */
    inline void removeLblTransSet(const SplitLabel& lbl)
    {
        _lbl2Trans.erase(lbl);
    }


    /** \brief Group all the transition by their labels. */
    void mapTransitions()
    {
        _lbl2Trans.clear();

        // перебираем дуги
        TS::TransIter trIt, trEnd;
        boost::tie(trIt, trEnd) = _ts->getTransitions();
        for (; trIt != trEnd; ++trIt)
        {
            TsTransition& t = *trIt;
            if (omitAsSelfLoop(t))          // петли либо исключаем, либо тут исключение
                continue;

            const SplitLabel& e = getTransSplitLblInt(t);       // сплит-метка
            TransSet& ts = _lbl2Trans[e];
            ts.insert(t);
        }
    }


    /** \brief Expands the given set of states \a r by adding necessary states until
     *  it becomes a region.
     *  
     *  \param r is a current set of states to be a region.
     *  \param minPreRegs is a set of state sets for collecting min preregions.
     *  \param explored is a set of state sets for storing previously processed set of states.
     *  \returns true is a new region has been developed, so it should be added to a set of explored.
     *  false if the given region \a r has already been added to explored set previously, so
     *  no actions were performed be the method. The returned value is to be used to optimize
     *  adding a new develoed regions to the set of explored regions.
     */
    bool expandStates(const StateSet& r, SetOfStateSets& minPreRegs, SetOfStateSets& explored)
    {
        // если текущее множество уже обработано, заканчиваем
        if (explored.contains(r))
            return false;

        // если текущее множество r является регионом, тогда из результирующего множества
        // регионом (минимальных) нужно удалить все регионы, являющиеся строгими надмножествами
        // текущего региона r, т.к. они не являются минимальными
        if (isRegion(r))
        {
            minPreRegs.removeAllStrictSupersets(r);               // удаляем из R все строгие надмножества r
            //if (!minPreRegs.subsetExists(r))                
            if (!subsetExists(minPreRegs, r))               // если в R нет ни одного подмножества r...    
                minPreRegs.insert(r);                       // ... добавляем r как минимальный в своем классе

            return true;                                    // меньше, чем r все равно нет
        }


        LblKeyTrSetIter evIt, evEnd;
        boost::tie(evIt, evEnd) = getEvents2();          // TODO:1 getEvents2 !
        for (; evIt != evEnd; ++evIt)
        {
            //const Label& e = *evIt;                         // событие
            const SplitLabel& e = *evIt;                    // сплит-событие
            RegCond cond = checkRegCondition(e, r);         // (не)нарушаемые им аксиомы регионов

            // TODO: а можно ли здесь и внизу использовать r, а не создавать новый r'?
            StateSet newr = r;                              // создаем копию руками!

            // проверяем индивидуальные кейсы леммы
            if (cond.isL42Cases12Violated())                // кейсы 1 или 2
            {
                addEssentialStates12(newr, r, e);           // добавляем необх. состояния для кейсов 1 и 2
                if(expandStates(newr, minPreRegs, explored))// рекурсивно расширяем
                    explored.insert(newr);          // TODO: убедиться, что if тут и внизу правильный!
                break;                                      // все с этим событием
            }

            if (cond.isL42Case3Violated())                  // кейс 3
            {
                // для этого кейса расширение в две стороны
                // в первую
                addEssentialStates3a(newr, r, e);           // расширение в одну сторону
                if(expandStates(newr, minPreRegs, explored))   // рекурсивно расширяем
                    explored.insert(newr);          // TODO: убедиться в if

                // во вторую
                newr = r;                                   // создаем еще одну копию
                addEssentialStates3b(newr, r, e);           // расширение в другую сторону
                if(expandStates(newr, minPreRegs, explored))   // рекурсивно расширяем
                    explored.insert(newr);          // TODO: убедиться в if
                break;                                      // все с этим событием
            }


            if (cond.isL42Case4Violated())                  // кейс 4
            {
                // для этого кейса расширение также в две стороны
                // в первую
                addEssentialStates4a(newr, r, e);           // расширение в одну сторону
                if(expandStates(newr, minPreRegs, explored))// рекурсивно расширяем
                    explored.insert(newr);          // TODO: убедиться в if

                // во вторую
                newr = r;                                   // создаем еще одну копию
                addEssentialStates4b(newr, r, e);           // расширение в другую сторону
                if(expandStates(newr, minPreRegs, explored))// рекурсивно расширяем
                    explored.insert(newr);          // TODO: убедиться в if
                                                            // все с этим событием
            }
        } // for (; evIt // цикл перебора событий

        // вышли — все, ок
        return true;
    }


    /** \brief Adds to the non-region \a r essential states such that
     *  it become to be a region, according to the Cases 1 and 2 of 
     *  the Lemma 4.2 [Cortadella et al. 1998].
     *
     *  \param r is a non-region to be expanded according to the cases 1 and 2 of the Lemma.
     *  \param e is an event, in relation to which the region is expanded.
     *  \param[out] newr is a new set, to which additional states is added. Must be pre-copied 
     *  from r in the caller!
     */
    //void addEssentialStates12(StateSet& newr, const StateSet& r, const Label& e)
    void addEssentialStates12(StateSet& newr, const StateSet& r, const SplitLabel& e)    
    {       
        // расширяем множество состояний в соответствии с кейсами 1 и 2
        // перебираем все транзиции, помеченные буковой e
        const TransSet& eTranss = getTransSet(e);
        for (const TsTransition& t : eTranss)
        {
            if (omitAsSelfLoop(t))          // петли либо исключаем, либо тут исключение
                continue;


            // дуга, помеченная нужным событием и не петля
            // если она одним своим концом принадлежит множеству r, тогда другой
            // ее конец (состояние) также добавляем к этому множеству
            TsState src = _ts->getSrcState(t);
            TsState targ = _ts->getTargState(t);
            
            if (r.contains(targ))                                   // концом в регионе
                newr.insert(src);                                   // начало в новый
            else if (r.contains(src))                               // началом в регионе
                newr.insert(targ);                                  // конец в новый
        }

    }


    /** \brief Adds to the non-region \a r essential states such that
     *  it become to be a region, according to the Case 3a (first direction) of 
     *  the Lemma 4.2 [Cortadella et al. 1998].
     *
     *  \param r is a non-region to be expanded according to the case 3a of the Lemma.
     *  \param e is an event, in relation to which the region is expanded.
     *  \param[out] newr is a new set, to which additional states is added. Must be pre-copied 
     *  from r in the caller!
     */
    //void addEssentialStates3a(StateSet& newr, const StateSet& r, const Label& e)
    void addEssentialStates3a(StateSet& newr, const StateSet& r, const SplitLabel& e)
    {
        // расширяем множество состояний в соответствии с кейсом 3а:
        // перебираем все транзиции, помеченные буковой e
        const TransSet& eTranss = getTransSet(e);
        for (const TsTransition& t : eTranss)
        {
            if (omitAsSelfLoop(t))          // петли либо исключаем, либо тут исключение
                continue;


            // дуга, помеченная нужным событием и не петля
            TsState src = _ts->getSrcState(t);
            TsState targ = _ts->getTargState(t);

            // если конец дуги в регионе, добавляем начало (но не наоборот)
            if (r.contains(targ))
                newr.insert(src);
        }
    }


    /** \brief Adds to the non-region \a r essential states such that
     *  it become to be a region, according to the Case 3b (second direction) of 
     *  the Lemma 4.2 [Cortadella et al. 1998].
     *
     *  \param r is a non-region to be expanded according to the case 3b of the Lemma.
     *  \param e is an event, in relation to which the region is expanded.
     *  \param[out] newr is a new set, to which additional states is added. Must be pre-copied 
     *  from r in the caller!
     */
    //void addEssentialStates3b(StateSet& newr, const StateSet& r, const Label& e)
    void addEssentialStates3b(StateSet& newr, const StateSet& r, const SplitLabel& e)
    {
        // расширяем множество состояний в соответствии с кейсом 3b:
        const TransSet& eTranss = getTransSet(e);
        for (const TsTransition& t : eTranss)
        {
            if (omitAsSelfLoop(t))          // петли либо исключаем, либо тут исключение
                continue;


            // дуга, помеченная нужным событием и не петля
            TsState src = _ts->getSrcState(t);
            TsState targ = _ts->getTargState(t);

            // если начало дуги НЕ в регионе, добавляем конец (но не наоборот)
            if (!r.contains(src))
                newr.insert(targ);
        }
    }


    /** \brief Adds to the non-region \a r essential states such that
     *  it become to be a region, according to the Case 4a (first direction) of 
     *  the Lemma 4.2 [Cortadella et al. 1998].
     *
     *  \param r is a non-region to be expanded according to the case 4a of the Lemma.
     *  \param e is an event, in relation to which the region is expanded.
     *  \param[out] newr is a new set, to which additional states is added. Must be pre-copied 
     *  from r in the caller!
     */
    //void addEssentialStates4a(StateSet& newr, const StateSet& r, const Label& e)
    void addEssentialStates4a(StateSet& newr, const StateSet& r, const SplitLabel& e)
    {
        // расширяем множество состояний в соответствии с кейсом 4а:
        const TransSet& eTranss = getTransSet(e);
        for (const TsTransition& t : eTranss)
        {
            if (omitAsSelfLoop(t))          // петли либо исключаем, либо тут исключение
                continue;


            // дуга, помеченная нужным событием и не петля
            TsState src = _ts->getSrcState(t);
            TsState targ = _ts->getTargState(t);

            // если начало дуги в регионе, добавляем конец (но не наоборот)
            if (r.contains(src))
                newr.insert(targ);
        }
    }


    /** \brief Adds to the non-region \a r essential states such that
     *  it become to be a region, according to the Case 4b (second direction) of 
     *  the Lemma 4.2 [Cortadella et al. 1998].
     *
     *  \param r is a non-region to be expanded according to the case 4b of the Lemma.
     *  \param e is an event, in relation to which the region is expanded.
     *  \param[out] newr is a new set, to which additional states is added. Must be pre-copied 
     *  from r in the caller!
     */
    //void addEssentialStates4b(StateSet& newr, const StateSet& r, const Label& e)
    void addEssentialStates4b(StateSet& newr, const StateSet& r, const SplitLabel& e)
    {
        // расширяем множество состояний в соответствии с кейсом 4b:
        const TransSet& eTranss = getTransSet(e);
        for (const TsTransition& t : eTranss)
        {
            if (omitAsSelfLoop(t))          // петли либо исключаем, либо тут исключение
                continue;


            // дуга, помеченная нужным событием и не петля
            TsState src = _ts->getSrcState(t);
            TsState targ = _ts->getTargState(t);

            // если конец дуги НЕ в регионе, добавляем начало (но не наоборот)
            if (!r.contains(targ))
                newr.insert(src);
        }
    }

    
    // #appnote: в новой версии сразу берем множество транзиций под каждую букву, на чем
    // должны изрядно сэкономить
    /** \brief Checks valid region conditions for the given pair \a (e, r) and returns 
     *  the special object with conditions.
     */
    RegCond checkRegCondition(const SplitLabel& e, const StateSet& r) const
    {
        RegCond cond;

        // перебираем все транзиции, помеченные буковой e
        const TransSet& eTranss = getTransSet(e);
        for (const TsTransition& t : eTranss)
        {
            if (omitAsSelfLoop(t))          // петли либо исключаем, либо тут исключение
                continue;


            // дошли до очередной дуги, помеченной как и e
            TsState src = _ts->getSrcState(t);
            TsState trg = _ts->getTargState(t);

            // проверяем принадлежности концов
            bool targetIn = r.contains(trg);                        // сохраним флаг, что конец входит
            if (r.contains(src))                                    // если входит начало...
            {
                if (targetIn)                                       // ... и конец
                    cond.setIn();                                   // ...    значит лежит внутри
                else                                                // ... а если конец не входит
                    cond.setExit();                                 // ...    значит выходит
            }
            else                                                    // если начало не входит...
            {
                if (targetIn)                                       // ... а конец входит
                    cond.setEnter();                                // ...   значит входит
                else                                                // ... если же и конец не входит
                    cond.setOut();                                  // ...   значит лежит вовне
            }

            // если все 4 условия были детектированы, можно на этом и заканчивать, по сути
            if (cond.completed())
                return cond;
        }

        return cond;
    }




    /** \brief Checks a relation between the transition \a t and the set \a s.
     */
    RegCond checkTransSSetRelations(const TsTransition& t, const StateSet& s) const
    {
        bool contSrc =  s.contains(_ts->getSrcState(t));
        bool contTarg = s.contains(_ts->getTargState(t));

        RegCond res;
        if (contSrc)                // содержит начало
        {
            if (contTarg)           // и конец
                res.setIn();
            else                    // конец не содержит, только начало
                res.setExit();  
        }
        else                        // НЕ содержит начало
        {
            if (contTarg)           // а конец содержит
                res.setEnter();
            else                    // ничего не содержит
                res.setOut();
        }
        return res;
    }




    /** \brief Checks whether a ts \a _ts is set or not. If not, raises an exception. */
    void check4ts() const
    {
        if (_ts == nullptr)
            throw LdopaException("TS is not set.");
    }


    /** \brief Checks whether the given transition \a of the \a _ts t is a self-loop or not. 
     * 
     *  \returns true if \a t is a self-loop, false otherwise.
     */
    inline bool isSelfLoop(const TsTransition& t) const
    {
        //check4ts();                   // для защищенного метода не надо!
        return isSelfLoop(_ts, t);
    }


    /** \brief Checks whether the transition \a t is a self-loop.
     *
     *  If a self-loop proceeding policy is to forbid them, throws an exception.
     *  \return true, if the transision should be omitted as a self loop, false otherwise.
     */
    inline bool omitAsSelfLoop(const TsTransition& t) const
    {
        if (isSelfLoop(t))          // если петля
        {
            switch (_options.getSelfLoopPolicy()) {
                case Options::slIgnore:
                case Options::slReestablish:
                    return true;                        // просто пропустить
                case Options::slProcess:
                    return false;                       // не считать петлей, т.к. понадобится
                default:                                // вариант slBreak
                    throw LdopaException("The input TS contains a self-loop transition and the policy forbids it.");
            }

        }
        return false;
    }


    /// Internal implementation of the getTransSplitLbl() method w/io checkings.
    SplitLabel getTransSplitLblInt(const TsTransition& t) const
    {
        const Label& tlbl = _ts->getTransLbl(t);        // это всегда есть
        TransIntMapCIter it = _transsSplitLabels.find(t);   // отказались от ref-ов
        
        unsigned int ind = 0;
        if (it != _transsSplitLabels.end())             // если есть отображение
            ind = it->second;
                        
        return SplitLabel(tlbl, ind);
    }



    /** \brief Assigns for the given transition \a t its split index \a ind. */
    inline void setTrSplitIndex(const TsTransition& t, unsigned int ind)
    {
        _transsSplitLabels[t] = ind;           // отказались же от ссылок!
    }

    /** \brief Sets the current split index for the given label \a lbl to \a ind. 
     */
    inline void setLblSplitIndex(const Label& lbl, unsigned int ind)
    {
        _lblCurrentSplitInd[lbl] = ind;
    }

    /** \brief Clears the synthesized Petri net object. */
    void clearPN()
    {
        if (_pn)
        {
            delete _pn;
            _pn = nullptr;
        }

        // DONE: начальную разметку еще надо!
        _initMarking.clear();
    }


    /** \brief Clear all structures related to the synthesis process. */
    inline void clearStructures()
    {
        //_excRegions.clear();
        _minPreRegions.clear();
        _transsSplitLabels.clear();
        _lblCurrentSplitInd.clear();

        // UPD:
        _lbl2Trans.clear();
    }

protected:
    /** \brief Original transition system to be a source for synthesized Petri net. 
     * 
     *  The synthesizer does not manage the lifetime of this TS.   
     */
    TS* _ts;

    /// Synthesized Petri net.
    PN* _pn;

    /// Stores initial marking for the synthesized PN.
    PnMarking _initMarking;


    /** \brief Represents a mapping of all SplitLabels to sets of all transition labeled by these labels.
     *
     *  It is intended to replace excitation regions since all src states of all transitions of a state
     *  for a lable are to be exactly the ER of this label.
     */
    Lbl2TransSetMap _lbl2Trans;


    /** \brief Collection of all min-preregions mapped to an event. 
     * 
     *  -
     */
    Lbl2SetOfStateSetsMap _minPreRegions;

    /** \brief Mapping of transitions to equivalent split labels.
     *
     *  If a transition "t" intially labeled as "e" is not exists it the map, 
     *  then there was not splitting for this event/label "e". Therefore, its
     *  splitted equivalent would be <"e", 0>. Otherwise, it would be <"e", _42_>,
     *  where _42_ is an index of splitting for the event.
     *  
     *  Equivalent splitting is to be obtained by using getTransSplitLbl() (and its
     *  internal counterpart) method.
     */
    TransIntMap _transsSplitLabels;

    /** \brief Stores current highest split index for individul labels. 
     * 
     *  Is a label is not stored in this map, this label has not been splitted yet.
     */
    LabelIntMap _lblCurrentSplitInd;

    /** \brief Stores synthesis options. */
    Options _options;

    /// Stores a label, by which arcs leading to the artificcial single accepted state labeled.
    Label _wfTransLabel;

    /// Regions to PN mapper helper.
    RegionMapper _mapper;

}; // class PnRegSynthesizer




}}} // namespace xi { namespace ldopa { namespace pn {


#endif // XI_LDOPA_PN_ALGOS_REGIONS_PN_SYNTHESIS_H_
