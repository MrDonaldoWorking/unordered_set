#ifndef UNORDERED_SET_H
#define UNORDERED_SET_H

#include <vector>
#include <assert.h>
#include <functional>

template <typename T>
struct unordered_set {
    template <typename U>
    struct us_const_iterator {
        using iterator_tag = std::forward_iterator_tag;
        using value_type = U;
        using difference_type = std::ptrdiff_t;
        using pointer = U const *;
        using reference = U const &;

        us_const_iterator() = default;
        us_const_iterator(us_const_iterator const&) = default;
        us_const_iterator &operator=(us_const_iterator const&) = default;

        us_const_iterator &operator++() {
            if (l_pos + 1 != data[v_pos].size()) {
                ++l_pos;
            } else {
                ++v_pos;
                while (v_pos < data.size() && data[v_pos].empty()) {
                    ++v_pos;
                }
                l_pos = 0;
            }

            return *this;
        }

        us_const_iterator const operator++(int) {
            us_const_iterator res(*this);
            ++(*this);
            return res;
        }

        friend bool operator==(us_const_iterator const& j,
                               us_const_iterator const& i) noexcept {
            return j.v_pos == i.v_pos && j.l_pos == i.l_pos;
        }

        friend bool operator!=(us_const_iterator const& j,
                               us_const_iterator const& i) noexcept {
            return j.v_pos != i.v_pos || j.l_pos != i.l_pos;
        }

        reference operator*() const {
            return data[v_pos][l_pos];
        }

        pointer operator->() const {
            return &(data[v_pos][l_pos]);
        }
    private:
        std::vector<std::vector<T>> const& data;
        size_t v_pos = 0, l_pos = 0;

        us_const_iterator(size_t const& l_pos, size_t const& v_pos,
                          std::vector<std::vector<T>> const& d) :
                data(d), l_pos(l_pos), v_pos(v_pos) {}

        friend struct unordered_set;
    };

    using value_type = T;
    using iterator = us_const_iterator<T const>;
    using const_iterator = us_const_iterator<T const>;

    unordered_set() {
        table.resize(len);
    }

    unordered_set(unordered_set const& u) : cnt(u.cnt), len(u.len) {
        table = u.table;
    }

    ~unordered_set() = default;

    unordered_set &operator=(unordered_set const& u) {
        if (this != &u) {
            unordered_set(u).swap(*this);
        }
        return *this;
    }

    bool empty() const noexcept {
        return cnt == 0;
    }

    size_t size() const noexcept {
        return cnt;
    }

    void clear() noexcept {
        for (auto& l : table) {
            l.clear();
        }
    }

    std::pair<iterator, bool> insert(T const& val) {
        iterator it = find(val);
        if (it != end()) {
            return std::make_pair(it, false);
        }

        size_t pos = h(val) % len;
        ++cnt;
        table[pos].push_back(val);

        if (table[pos].size() * 2 > cnt) {
            reserve(len * 2);
        }

        return std::make_pair(iterator(table[pos].size() - 1, pos, table), true);
    }

    iterator erase(const_iterator it) {
        --cnt;

        auto res = table[it.v_pos].erase(table[it.v_pos].begin() + it.l_pos);
        return iterator(res - table[it.v_pos].begin(), it.v_pos, table);
    }

    void swap(unordered_set &us) noexcept {
        std::swap(us.table, table);
        std::swap(us.cnt, cnt);
        std::swap(us.len, len);
    }

    size_t count(size_t key) const noexcept {
        size_t pos = h(key);
        for (auto& v : table[pos]) {
            if (v == key) {
                return 1;
            }
        }
        return 0;
    }

    iterator find(T key) const noexcept {
        size_t pos = h(key) % len;
        for (size_t i = 0; i < table[pos].size(); ++i) {
            if (table[pos][i] == key) {
                return iterator(i, pos, table);
            }
        }
        return end();
    }

    iterator begin() {
        return iterator(0, get_next_bucket(-1), table);
    }

    const_iterator begin() const {
        return iterator(0, get_next_bucket(-1), table);
    }

    iterator end() {
        return iterator(0, table.size(), table);
    }

    const_iterator end() const {
        return iterator(0, table.size(), table);
    }

    void reserve(size_t sz) {
        len = std::max(len, sz);
        std::vector<std::vector<T>> new_table(len);
        for (iterator it = begin(); it != end(); ++it) {
            size_t pos = h(*it);
            new_table[pos].push_back(*it);
        }
        table.swap(new_table);
    }

private:
    std::vector<std::vector<T>> table;
    std::hash<T> h;
    size_t cnt = 0, len = 100;

    size_t get_next_bucket(size_t pos) {
        ++pos;
        while (pos < table.size() && table[pos].empty()) {
            ++pos;
        }

        return pos;
    }
};

#endif //UNORDERED_SET_H
