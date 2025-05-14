#include "alpha_miner.h"
#include "xi/ldopa/pn/models/evlog_ptnets.h"

#include <boost/bimap.hpp>
#include <boost/dynamic_bitset/dynamic_bitset.hpp>


namespace xi { namespace ldopa { namespace pn { namespace alpha {
namespace {
    enum class ActivitiesRelationship {
        CAUSAL_FORWARD,
        CAUSAL_BACKWARD,
        PARALLEL,
        CHOICE
    };

    template <size_t N>
    bool is_subset(const std::bitset<N>& subset, const std::bitset<N>& set) {
        return (subset & set) == subset;
    }

    using DB = boost::dynamic_bitset<>;
    
    // Обёртка над dynamic_bitset для range-based for
    class BitSetIterable {
    public:
        // Итератор, возвращающий индексы установленных битов
        class Iterator {
        public:
            // Типы для соответствия требованиям Iterator concept
            using iterator_category = std::forward_iterator_tag;
            using value_type        = std::size_t;
            using difference_type   = std::ptrdiff_t;
            using pointer           = const std::size_t*;
            using reference         = const std::size_t&;
            
            Iterator(const DB* bs, std::size_t pos)
                : bset(bs), index(pos) { /* всё готово */ }
    
            // Операции итератора
            value_type operator*() const { return index; }
            Iterator& operator++() {
                index = bset->find_next(index);
                return *this;
            }
            bool operator==(const Iterator& o) const {
                return bset == o.bset && index == o.index;
            }
            bool operator!=(const Iterator& o) const {
                return !(*this == o);
            }
    
        private:
            const DB*    bset;
            std::size_t  index;
        };
    
        explicit BitSetIterable(const DB& bs) : b(bs) {}
    
        Iterator begin() const {
            std::size_t first = b.find_first();
            if (first == DB::npos)
                return end();
            return Iterator(&b, first);
        }
    
        Iterator end() const {
            return Iterator(&b, DB::npos);
        }
    
    private:
        const DB& b;
    };

    using Clique = DB;
    class BronKerbosh {
    public:
        BronKerbosh(const std::vector<DB> &adj) : adj(adj), R(adj.size()), left_mask(CreateBitSetRange(adj.size(), 0, adj.size() / 2)), right_mask(CreateBitSetRange(adj.size(), adj.size() / 2, adj.size())) {

        }

        std::vector<Clique> Run() {
            DB P(adj.size()), X(adj.size());
            P.set();
            R.reset();
            std::vector<Clique> out;
            RunInternal(0, P, X, out);
            return out;
        }

    private:
        void RunInternal(
            size_t iteration,            // глубина рекурсии, на первых двух шагах мы ищем кандидатов только с левой а затем с правой половины
            DB& P,
            DB& X,
            std::vector<Clique> &out     // результат
        ) {
            if (P.none() && X.none()) {
                if (iteration > 1) {
                    // R — максимальная клика
                    out.emplace_back(R);
                }
                return;
            }
            // Pivot u ∈ P∪X с max |P ∩ N(u)|
            DB PX = P | X;  // побитовое OR 
            ApplyMaskBasedOnIteration(iteration, PX);  // убираем невозможные на данной итерации пивоты
            size_t best_u = PX.find_first(), best_cnt = 0;
            for (size_t u = PX.find_first(); u != DB::npos; u = PX.find_next(u)) {
                // пересечение P ∧ N(u)
                DB tmp = P & adj[u];
                size_t cnt = tmp.count();
                if (cnt > best_cnt) { best_cnt = cnt; best_u = u; }
            }
            // кандидаты = P \ N(best_u)
            DB ext = P;
            if (best_cnt > 0) {
                ext &= (~adj[best_u]);  // P AND NOT N(u) 
            }
            ApplyMaskBasedOnIteration(iteration, ext);
    
    
            // по каждому v ∈ ext
            for (size_t v = ext.find_first(); v != DB::npos; v = ext.find_next(v)) {
                R.set(v);
                DB P2 = P & adj[v];  // обновл. P
                DB X2 = X & adj[v];  // обновл. X
                RunInternal(iteration + 1, P2, X2, out);
                R.reset(v);
                P.reset(v);
                X.set(v);
            }
        }

        void ApplyMaskBasedOnIteration(size_t iteration, DB& set) {
            switch (iteration) {
            case 0:
                set &= left_mask;
                break;
            case 1:
                set &= right_mask;
                break;
            }
        }

        static DB CreateBitSetRange(size_t size, size_t left, size_t right) {
            DB res(right - left);
            res.set();
            res.resize(size);
            res <<= left;
            return res;
        }

    private:
        const std::vector<DB> &adj; // матрица смежности
        DB R;                       // текущая клика

        const DB left_mask;         // маска для левой половины
        const DB right_mask;        // маска для правой половины
    };
}

AlphaMiner::PN* AlphaMiner::mine(IEventLog& log) {
    // 1. Extract activities
    using ActivityToIndex = boost::bimap<Attribute, size_t>;
    ActivityToIndex activity_to_index;

    size_t trace_count = log.getTracesNum();
    std::vector<std::vector<size_t>> boiled_down_log(trace_count);
    std::string act_attr_id = log.getEvActAttrId();
    assert(!act_attr_id.empty());
    const char* act_attr_id_cstr = act_attr_id.c_str();
    for (size_t i = 0; i < trace_count; ++i) {
        IEventTrace* trace = log.getTrace(i);
        assert(trace);
        size_t events_count = trace->getSize();
        boiled_down_log[i].reserve(events_count);
        for (size_t j = 0; j < events_count; ++j) {
            IEvent* event = trace->getEvent(j);
            assert(event);
            Attribute attr;
            bool found_activity = event->getAttr(act_attr_id_cstr, attr);
            assert(found_activity);
            if (activity_to_index.left.find(attr) == activity_to_index.left.end()) {
                activity_to_index.insert(ActivityToIndex::value_type(attr, activity_to_index.size()));
            }
            boiled_down_log[i].push_back(activity_to_index.left.at(attr));
        }
    }
    assert(activity_to_index.size() == log.getActivitiesNum());
    const size_t activities_count = activity_to_index.size();
    assert(activities_count > 0);
    if (activities_count > MAX_ACTIVITIES) {
        throw LdopaException("Too many activities");
    }

    // 2. Find initial and final activities
    std::set<size_t> initial_activities;
    std::set<size_t> final_activities;
    for (const auto& trace : boiled_down_log) {
        assert(!trace.empty());
        initial_activities.insert(trace.front());
        final_activities.insert(trace.back());
    }

    // 3. Build direct succession matrix
    std::vector<std::vector<bool>> direct_succession_matrix(activities_count, std::vector<bool>(activities_count, false));
    for (const auto& trace_activities : boiled_down_log) {
        for (size_t i = 0; i < trace_activities.size() - 1; ++i) {
            auto current_activity = trace_activities[i];
            auto next_activity = trace_activities[i + 1];
            direct_succession_matrix[current_activity][next_activity] = true;
        }
    }

    // 4. Build footprint matrix
    std::vector<std::vector<ActivitiesRelationship>> footprint_matrix(activities_count, std::vector<ActivitiesRelationship>(activities_count, ActivitiesRelationship::CHOICE));
    for (size_t i = 0; i < activities_count; ++i) {
        for (size_t j = 0; j < activities_count; ++j) {
            if (i == j) {
                footprint_matrix[i][j] = ActivitiesRelationship::CHOICE;
                continue;
            }
            bool forward = direct_succession_matrix[i][j];
            bool backward = direct_succession_matrix[j][i];
            if (forward && backward) {
                footprint_matrix[i][j] = ActivitiesRelationship::PARALLEL;
            } else if (!forward && !backward) {
                footprint_matrix[i][j] = ActivitiesRelationship::CHOICE;
            } else if (forward) {
                footprint_matrix[i][j] = ActivitiesRelationship::CAUSAL_FORWARD;
            } else {
                footprint_matrix[i][j] = ActivitiesRelationship::CAUSAL_BACKWARD;
            }
        }
    }

    // 5. Build Graph
    std::vector<DB> graph(activities_count*2, DB(activities_count*2));
    for (size_t i = 0; i < activities_count; ++i) {
        for (size_t j = i + 1; j < activities_count; ++j) {
            switch (footprint_matrix[i][j]) {
            case ActivitiesRelationship::CAUSAL_FORWARD:
                graph[i].set(activities_count + j);
                graph[activities_count + j].set(i);
                break;
            case ActivitiesRelationship::CAUSAL_BACKWARD:
                graph[activities_count + i].set(j);
                graph[j].set(activities_count + i);
                break;
            case ActivitiesRelationship::CHOICE:
                graph[i].set(j);
                graph[j].set(i);

                graph[activities_count + i].set(activities_count + j);
                graph[activities_count + j].set(activities_count + i);
                break;
            case ActivitiesRelationship::PARALLEL:
                break;
            }
        }
    }

    // Find all maximal cliques (Y)
    BronKerbosh bk(graph);
    std::vector<Clique> Y = bk.Run();


    PN* net = new PN();
    // 6. Construct Petri net places
    auto start_place = net->addPosition("start");
    auto end_place = net->addPosition("end");
    std::vector<PN::Position> places;
    places.reserve(Y.size());
    for (const auto& y : Y) {
        places.push_back(net->addPosition());
    }

    // 7. Construct Petri net transitions
    std::vector<PN::Transition> transitions;
    transitions.reserve(activities_count);
    for (size_t i = 0; i < activities_count; ++i) {
        transitions.push_back(net->addTransition(activity_to_index.right.at(i)));
    }

    // 8. Construct Petri net arcs
    for (size_t i = 0; i < Y.size(); ++i) {
        for (size_t vertex : BitSetIterable(Y[i])) {
            if (vertex < activities_count) {
                net->addArc(transitions[vertex], places[i]);
            } else {
                net->addArc(places[i], transitions[vertex - activities_count]);
            }
        }
    }
    for (size_t t : initial_activities) {
        net->addArc(start_place, transitions[t]);
    }
    for (size_t t : final_activities) {
        net->addArc(transitions[t], end_place);
    }

    return net;
}

}}}} // namespace xi { namespace ldopa { namespace pn { namespace alpha {
