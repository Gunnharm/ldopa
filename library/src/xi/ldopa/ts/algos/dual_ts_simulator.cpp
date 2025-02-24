// starting from 06.12.2018 we use a /FI approach to force including stdafx.h:
// https://chadaustin.me/2009/05/unintrusive-precompiled-headers-pch/
//#include "stdafx.h"

#include "xi/ldopa/ts/algos/dual_ts_simulator.h"



namespace xi { namespace ldopa { namespace ts { ;   //

//==============================================================================
//  class DualTsSimulator
//==============================================================================


DualTsSimulator::DualTsSimulator()
    : _ts1(nullptr)
    , _ts2(nullptr)
{
}

//------------------------------------------------------------------------------

double DualTsSimulator::calcPrecision(TS* ts1, TS* ts2)
{
    _ts1 = ts1;
    _ts2 = ts2;

    if (!_ts1 || !_ts2)
        throw LdopaException("Can't simulate: either TS1 or TS2 is null.");

    _partStatePrecs.clear();        // очищаем с прошлого раза, если было
    
    // рассчитываем частичную точность по состояниям
    calcStatePrecision(_ts1->getInitState(), _ts2->getInitState());    

    return sumPartialPrecisions();
}

//------------------------------------------------------------------------------

void DualTsSimulator::calcStatePrecision(TS::State s1, TS::State s2)
{
    int pen = 0;        // число выходных переходов, которые не могут быть проиграны
   
    // перебираем все выходные транзиции состояния s1
    int numOfOutEdges = 0;                                  // а также посчитаем их число
    TS::OtransIter tCur, tEnd;    
    for (boost::tie(tCur, tEnd) = _ts1->getOutTransitions(s1); tCur != tEnd; ++tCur)
    {
        // соответствующая выходная транзиция из состояния s2
        TransRes trr = getMatchingTransOfTs2(*tCur, s2);
        if (trr.second)                                     // есть такая!
        {
            State ns1 = _ts1->getTargState(*tCur);
            State ns2 = _ts2->getTargState(trr.first);

            // итеративно вызываем метод расчет частичной точности для новых состояний
            // казалось бы, здесь могли бы попасть в цикл, т.к. _ts1 допускает циклы и
            // даже петли, но! _ts2 — по определению префиксное дерево, т.е. там циклов
            // точно не будет, а значит, однажды дойдя до листьев, алгоритм уйдет в 
            // ветку else этого цикла, откуда произойдет рекурсивный выход
            calcStatePrecision(ns1, ns2);
        }
        else                                                // нет такой! альтернативная ветка
            ++pen;                                          // с выходом из рекурсии и подсчет пенальти

        ++numOfOutEdges;                                    // еще одна выходная транзиция
    }

    // после выхода из цикла перебора выходных транзиций, надо посомтреть на
    // текущее состояние: если оно принимающее, это эквивалент того, 
    // что у него как бы еще одна выходная транзиция была бы
    if (_ts1->isStateAccepting(s1))
    {
        ++numOfOutEdges;
        // а вот если эквивалетное состояние в префиксном дереве не принимающее, 
        // то еще одна штрафочка
        if (!_ts2->isStateAccepting(s2))
            ++pen;
    }

    // перед выходом из рекурсии посчитаем частичный пресижн текущего состояния
    // здесь по-прежнему игнорируем проблему отсутствующих выходов у не-fit 
    // TS1 (например, "жирного")
    if (numOfOutEdges != 0)
    {
        // получастичный пресижн для текущего состояния и переподсчет частичного
        double semiPartStatePrec = (double)(numOfOutEdges - pen) / (double)numOfOutEdges;
        incStatePrecision(s1, semiPartStatePrec);         // пересчитываем
    }
}

//------------------------------------------------------------------------------

void DualTsSimulator::incStatePrecision(State s1, double semiPartStatePrec)
{
    // #apnote: в java-решении выполняется последовательное деление/умножение 
    // на число частичных пресижнов, что может привести к накоплению ошибки
    // дополнительной. В этой версии накапливаем сумму частичных пресижнов
    // и увеличивает число слагаемых, а подели один раз в конце!
    
    // берем парочку по ссылке так, что сразу можно писать в нее
    DblInt& pprec = getStatePrecision(s1);       // здесь либо сущ, либо нули
    
    pprec.first += semiPartStatePrec;           // суммируем частичную точноть
    ++pprec.second;                             // увеличиваем число слагаемых   
}

//------------------------------------------------------------------------------

double DualTsSimulator::sumPartialPrecisions()
{
    double sumPrec = 0;                         // сумма частичных пресижнов
    int num = 0;                                // их число

    // берем все состояния _ts1
    TS::StateIter tCur, tEnd;
    for (boost::tie(tCur, tEnd) = _ts1->getStates(); tCur != tEnd; ++tCur)
    {
        // берем частичный полупресижн для текущего состояния по ссылке, чтобы без копий
        const DblInt& stPr = getStatePrecision(*tCur);
        if (stPr.second != 0)                   // если есть хотя бы один полупресижн
        {                                       // добавляем среднее полупресижнов
            sumPrec += stPr.first / double(stPr.second);
            ++num;                              // число средних увеличили
        }
    }

    return sumPrec / double(num);               // результат — среднее
}

//------------------------------------------------------------------------------

DualTsSimulator::DblInt& DualTsSimulator::getStatePrecision(State s)
{
    StateDblIntMap::iterator it = _partStatePrecs.find(s);
    if (it != _partStatePrecs.end())
        return it->second;                  // есть такое

    // иначе вставим и вернем       
    it = (_partStatePrecs.insert(std::pair<TS::State, DblInt>   
        (s, std::make_pair(0., 0)))).first;
    return it->second;
}


}}} // namespace xi { namespace ldopa { namespace ts {
