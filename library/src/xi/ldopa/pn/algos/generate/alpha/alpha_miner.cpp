#include "alpha_miner.h"
#include "xi/ldopa/pn/models/evlog_ptnets.h"

namespace xi { namespace ldopa { namespace pn { namespace alpha {

AlphaMiner::AlphaMiner() 
    : _log(nullptr)
    , _pn(nullptr)
{
}

AlphaMiner::~AlphaMiner() {
    // В mine мы возвращаем PetriNet во владение вызывающему коду
    // Здесь он может остаться если во время исполнения произошел exception
    if (_pn) {
        delete _pn;
    }
}

EventLogPetriNet<>* AlphaMiner::mine(IEventLog* log) {
    _log = log;
    if (!validateInput()) {
        return nullptr;
    }

    extractActivities();
    buildDirectSuccession();
    buildCausalDependency();
    buildParallelRelation();
    findMaximalSets();
    constructPetriNet();

    EventLogPetriNet<>* result = _pn;
    _pn = nullptr;
    return result;
}

bool AlphaMiner::validateInput() {
    if (!_log) {
        return false;
    }
    
    // Проверяем, что есть хотя бы один трейс
    IEventTrace* trace = _log->getTrace(0);
    if (!trace) {
        return false;
    }
    return true;
}

void AlphaMiner::extractActivities() {
    _activities.clear();
    
    // Получаем ID атрибута активности
    std::string actAttrId = _log->getEvActAttrId();
    
    for (int i = 0; ; ++i) {
        IEventTrace* trace = _log->getTrace(i);
        if (!trace) break;
        
        for (int j = 0; j < trace->getSize(); ++j) {
            IEvent* event = trace->getEvent(j);
            if (!event) continue;
            
            Attribute attr;
            if (event->getAttr(actAttrId.c_str(), attr)) {
                _activities.insert(attr);
            }
        }
    }
}

void AlphaMiner::buildDirectSuccession() {
    _directSuccession = DirectSuccessionRelations<Attribute>();
    
    // Получаем ID атрибута активности
    std::string actAttrId = _log->getEvActAttrId();
    
    for (int i = 0; i < _log->getTracesNum(); ++i) {
        IEventTrace* trace = _log->getTrace(i);
        if (!trace) continue;
        
        for (int j = 0; j < trace->getSize() - 1; ++j) {
            IEvent* current = trace->getEvent(j);
            IEvent* next = trace->getEvent(j + 1);
            if (!current || !next) continue;
            
            Attribute currentAttr, nextAttr;
            if (current->getAttr(actAttrId.c_str(), currentAttr) && 
                next->getAttr(actAttrId.c_str(), nextAttr)) {
                _directSuccession.add(currentAttr, nextAttr);
            }
        }
    }
}

void AlphaMiner::buildCausalDependency() {
    _causalDependency = CausalDependencyRelations<Attribute>();
    
    for (const auto& a : _activities) {
        for (const auto& b : _activities) {
            if (_directSuccession.contains(a, b) && !_directSuccession.contains(b, a)) {
                _causalDependency.add(a, b);
            }
        }
    }
}

void AlphaMiner::buildParallelRelation() {
    _parallel = ParallelRelations<Attribute>();
    
    for (const auto& a : _activities) {
        for (const auto& b : _activities) {
            if (_directSuccession.contains(a, b) && _directSuccession.contains(b, a)) {
                _parallel.add(a, b);
            }
        }
    }
}

void AlphaMiner::findMaximalSets() {
    _maximalSets.clear();

    // Находим все возможные пары множеств входных и выходных активностей
    std::vector<std::set<Attribute>> inputSets;
    std::vector<std::set<Attribute>> outputSets;

    // Генерируем все возможные подмножества входных активностей
    for (const auto& a : _activities) {
        std::set<Attribute> inputSet = {a};
        inputSets.push_back(inputSet);
        
        // Добавляем все активности, которые могут быть в одном входном множестве
        for (const auto& b : _activities) {
            if (a != b && _causalDependency.contains(a, b)) {
                inputSet.insert(b);
            }
        }
        if (inputSet.size() > 1) {
            inputSets.push_back(inputSet);
        }
    }

    // Генерируем все возможные подмножества выходных активностей
    for (const auto& a : _activities) {
        std::set<Attribute> outputSet = {a};
        outputSets.push_back(outputSet);
        
        // Добавляем все активности, которые могут быть в одном выходном множестве
        for (const auto& b : _activities) {
            if (a != b && _causalDependency.contains(b, a)) {
                outputSet.insert(b);
            }
        }
        if (outputSet.size() > 1) {
            outputSets.push_back(outputSet);
        }
    }

    // Находим максимальные множества
    for (const auto& inputSet : inputSets) {
        for (const auto& outputSet : outputSets) {
            bool isValid = true;

            // Проверяем, что все активности из входного множества имеют причинно-следственную связь
            // со всеми активностями из выходного множества
            for (const auto& a : inputSet) {
                for (const auto& b : outputSet) {
                    if (!_causalDependency.contains(a, b)) {
                        isValid = false;
                        break;
                    }
                }
                if (!isValid) break;
            }

            // Проверяем, что нет параллельных отношений между активностями внутри множеств
            for (const auto& a : inputSet) {
                for (const auto& b : inputSet) {
                    if (a != b && _parallel.contains(a, b)) {
                        isValid = false;
                        break;
                    }
                }
                if (!isValid) break;
            }

            for (const auto& a : outputSet) {
                for (const auto& b : outputSet) {
                    if (a != b && _parallel.contains(a, b)) {
                        isValid = false;
                        break;
                    }
                }
                if (!isValid) break;
            }

            if (isValid) {
                // Проверяем, что это множество не является подмножеством уже существующего
                bool isMaximal = true;
                for (const auto& existingSet : _maximalSets) {
                    if (std::includes(existingSet.input.begin(), existingSet.input.end(), 
                                    inputSet.begin(), inputSet.end()) &&
                        std::includes(existingSet.output.begin(), existingSet.output.end(), 
                                    outputSet.begin(), outputSet.end())) {
                        isMaximal = false;
                        break;
                    }
                }

                if (isMaximal) {
                    _maximalSets.push_back({inputSet, outputSet});
                }
            }
        }
    }
}

void AlphaMiner::constructPetriNet() {
    // Создаем новую сеть Петри
    _pn = new EventLogPetriNet<>();

    

    // Создаем начальное и конечное места
    auto startPlace = _pn->addPosition("start");
    auto endPlace = _pn->addPosition("end");

    // Создаем переходы для каждой активности
    std::map<Attribute, typename EventLogPetriNet<>::Transition> activityTransitions;
    for (const auto& activity : _activities) {
        auto transition = _pn->addTransition(activity);
        activityTransitions[activity] = transition;
    }

    // Создаем места для максимальных множеств
    std::vector<typename EventLogPetriNet<>::Position> maximalSetPlaces;
    for (size_t i = 0; i < _maximalSets.size(); ++i) {
        std::string placeName = "p" + std::to_string(i);
        auto place = _pn->addPosition(placeName);
        maximalSetPlaces.push_back(place);
    }

    // Соединяем начальное место с переходами, у которых нет входных активностей
    for (const auto& activity : _activities) {
        bool hasInput = false;
        for (const auto& set : _maximalSets) {
            if (set.output.find(activity) != set.output.end()) {
                hasInput = true;
                break;
            }
        }
        if (!hasInput) {
            _pn->addArcW(startPlace, activityTransitions[activity]);
        }
    }

    // Соединяем переходы с конечным местом, у которых нет выходных активностей
    for (const auto& activity : _activities) {
        bool hasOutput = false;
        for (const auto& set : _maximalSets) {
            if (set.input.find(activity) != set.input.end()) {
                hasOutput = true;
                break;
            }
        }
        if (!hasOutput) {
            _pn->addArcW(activityTransitions[activity], endPlace);
        }
    }

    // Соединяем переходы с местами максимальных множеств
    for (size_t i = 0; i < _maximalSets.size(); ++i) {
        const auto& set = _maximalSets[i];
        auto place = maximalSetPlaces[i];

        // Соединяем входные активности с местом
        for (const auto& activity : set.input) {
            _pn->addArcW(activityTransitions[activity], place);
        }

        // Соединяем место с выходными активностями
        for (const auto& activity : set.output) {
            _pn->addArcW(place, activityTransitions[activity]);
        }
    }
}

}}}} // namespace xi { namespace ldopa { namespace pn { namespace alpha {
