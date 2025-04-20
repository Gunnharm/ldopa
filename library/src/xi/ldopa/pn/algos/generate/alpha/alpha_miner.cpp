#include "alpha_miner.h"
#include "xi/ldopa/pn/models/evlog_ptnets.h"

#include <boost/bimap.hpp>
#include <iostream>

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

    template <std::size_t N>
    class BitSetIterator
    {
    public:
        // Конструктор принимает ссылку на std::bitset
        explicit BitSetIterator(const std::bitset<N>& bs) : bitset_(bs) {}

        // Вложенный класс-итератор
        class iterator
        {
        public:
            // Конструктор итератора
            iterator(const std::bitset<N>* bs, std::size_t pos)
                : bitsetPtr_(bs), pos_(pos) {}

            // Возвращает текущую позицию установленного бита
            std::size_t operator*() const
            {
                return pos_;
            }

            // Переход к следующему установленному биту
            iterator& operator++()
            {
                // Ищем следующий установленный бит
                do {
                    ++pos_;
                } while (pos_ < N && !bitsetPtr_->test(pos_));
                return *this;
            }

            // Сравнение на неравенство для окончания итерации
            bool operator!=(const iterator& other) const
            {
                // Проверяем, одинаковы ли позиции и ссылаются ли они на один и тот же битсет
                return pos_ != other.pos_ || bitsetPtr_ != other.bitsetPtr_;
            }

        private:
            const std::bitset<N>* bitsetPtr_;
            std::size_t pos_;
        };

        // Метод для получения итератора на первый установленный бит
        iterator begin() const
        {
            // Ищем индекс первого установленного бита
            std::size_t firstSet = 0;
            while (firstSet < N && !bitset_.test(firstSet)) {
                ++firstSet;
            }
            return iterator(&bitset_, firstSet);
        }

        // Метод для получения итератора на конец (условно за границей)
        iterator end() const
        {
            return iterator(&bitset_, N);
        }

    private:
        const std::bitset<N>& bitset_;
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

    // 4. Build X
    std::vector<std::pair<std::bitset<MAX_ACTIVITIES>, std::bitset<MAX_ACTIVITIES>>> X;
    const auto check_mask_choice = [&](const std::bitset<MAX_ACTIVITIES>& mask) {
        for (size_t i = 0; i < activities_count; ++i) {
            if (!mask.test(i)) {
                continue;
            }
            for (size_t j = 0; j < activities_count; ++j) {
                if (!mask.test(j)) {
                    continue;
                }
                if (footprint_matrix[i][j] != ActivitiesRelationship::CHOICE) {
                    return false;
                }
            }
        }
        return true;
    };
    for (size_t mask_left = 1; mask_left < (1ull << activities_count); ++mask_left) {
        std::bitset<MAX_ACTIVITIES> current_mask_left(mask_left);
        if (!check_mask_choice(current_mask_left)) {
            continue;
        }
        for (size_t mask_right = 1; mask_right < (1ull << activities_count); ++mask_right) {
            std::bitset<MAX_ACTIVITIES> current_mask_right(mask_right);
            if (!check_mask_choice(current_mask_right)) {
                continue;
            }
            bool is_correct = true;
            for (size_t i = 0; i < activities_count && is_correct; ++i) {
                if (!current_mask_left.test(i)) {
                    continue;
                }
                for (size_t j = 0; j < activities_count && is_correct; ++j) {
                    if (!current_mask_right.test(j)) {
                        continue;
                    }
                    if (footprint_matrix[i][j] != ActivitiesRelationship::CAUSAL_FORWARD) {
                        is_correct = false;
                    }
                }
            }
            if (is_correct) {
                X.push_back(std::make_pair(current_mask_left, current_mask_right));
            }
        }
    }

    // 5. Build Y
    std::vector<std::pair<std::bitset<MAX_ACTIVITIES>, std::bitset<MAX_ACTIVITIES>>> Y;
    for (const auto& x_to_add : X) {
        bool is_correct = true;
        for (const auto& x_check : X) {
            if (x_to_add == x_check) {
                continue;
            }
            if (is_subset(x_to_add.first, x_check.first) && is_subset(x_to_add.second, x_check.second)) {
                is_correct = false;
                break;
            }
        }
        if (is_correct) {
            Y.push_back(x_to_add);
        }
    }

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
        const auto& A = Y[i].first;
        const auto& B = Y[i].second;
        for (const auto& a : BitSetIterator<MAX_ACTIVITIES>(A)) {
            net->addArc(transitions[a], places[i]);
        }
        for (const auto& b : BitSetIterator<MAX_ACTIVITIES>(B)) {
            net->addArc(places[i], transitions[b]);
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
