#include <utility>

#ifndef UNORDERED_SET_H
#define UNORDERED_SET_H

#include <vector>
#include <assert.h>
#include <set>
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

        us_const_iterator operator++() {
            ++l_pos;

            if (l_pos == data[v_pos].size()) {
                if (v_pos == *bs.rbegin()) {
                    v_pos = data.size();
                    l_pos = 0;
                    return *this;
                }
                v_pos = *bs.lower_bound(v_pos + 1);
                l_pos = 0;
                return *this;
            }

            return *this;
        }

        us_const_iterator const operator++(int) {
            us_const_iterator res(*this);
            ++*this;
            return res;
        }

        friend bool operator==(us_const_iterator const& j,
                               us_const_iterator const& i)
        noexcept {
            return j.v_pos == i.v_pos && j.l_pos == i.l_pos;
        }

        friend bool operator!=(us_const_iterator const& j,
                               us_const_iterator const& i) {
            return j.v_pos != i.v_pos || j.l_pos != i.l_pos;
        }

        reference operator*() const {
            return data[v_pos][l_pos];
        }

        pointer operator->() const {
            return &(data[v_pos][l_pos]);
        }
    private:
        std::vector<std::vector<U>> const data;
        std::set<size_t> bs;
        size_t v_pos = 0, l_pos = 0;

        us_const_iterator(size_t l_pos, size_t v_pos,
                          std::vector<std::vector<U>> const& d,
                          std::set<size_t> s) :
                data(d), bs(std::move(s)), l_pos(l_pos), v_pos(v_pos) {}

        friend struct unordered_set;
    };

    using value_type = T;
    using iterator = us_const_iterator<T>;
    using const_iterator = us_const_iterator<T>;

    unordered_set() {
        table.resize(len);
    }

    unordered_set(unordered_set const& u) : cnt(u.cnt), len(u.len) {
        bucket = u.bucket;
        table = u.table;
    }

    ~unordered_set() = default;

    unordered_set &operator=(unordered_set const& u) {
        if (this == &u) {
            return *this;
        }

        table = u.table;
        bucket = u.bucket;
        cnt = u.cnt;
        len = u.len;
        return *this;
    }

    bool empty() const noexcept {
        return cnt == 0;
    }

    size_t size() const noexcept {
        return cnt;
    }

    void clear() noexcept {
        bucket.clear();
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
        bucket.insert(pos);

        table[pos].push_back(val);

        if (3 * bucket.size() > 2 * len) {
            reserve(2 * len);
        }
        return std::make_pair(iterator(table[pos].size() - 1, pos, table,
                bucket), true);
    }

    iterator erase(const_iterator it) {
        --cnt;
        if (table[it.v_pos].size() == 1) {
            bucket.erase(it.v_pos);
        }

        auto res = table[it.v_pos].erase(table[it.v_pos].begin() + it.l_pos);
        return iterator(res - table[it.v_pos].begin(), it.v_pos, table, bucket);
    }

    void swap(unordered_set &us) noexcept {
        std::swap(us.table, table);
        std::swap(us.bucket, bucket);
        std::swap(us.cnt, cnt);
        std::swap(us.len, len);
    }

    size_t count(size_t key) const {
        size_t pos = h(key);
        for (auto& v : table[pos]) {
            if (v == key) {
                return 1;
            }
        }
        return 0;
    }

    iterator find(size_t key) {
        size_t pos = h(key);
        for (auto it = table[pos].begin(); it != table[pos].end(); ++it) {
            if (*it == key) {
                return iterator(it - table[pos].begin(), pos, table, bucket);
            }
        }
        return end();
    }

    iterator begin() {
        return iterator(0, *bucket.begin(), table, bucket);
    }

    const_iterator begin() const {
        return iterator(0, *bucket.begin(),
                table, bucket);
    }

    iterator end() {
        return iterator(0, table.size(), table, bucket);
    }

    const_iterator end() const {
        return iterator(0, table.size(), table, bucket);
    }

    size_t bucket_count() const {
        return bucket.size();
    }

    void reserve(size_t sz) {
        len = std::max(len, sz);
        unordered_set tmp(*this);

        clear();
        table.resize(len);
        for (iterator it = tmp.begin(); it != tmp.end(); ++it) {
            size_t pos = h(*it);
            bucket.insert(pos);
            table[pos].insert(table[pos].begin(), *it);
        }
    }
private:
    std::vector<std::vector<T>> table;
    std::set<size_t> bucket;
    std::hash<T> h;
    size_t cnt = 0, len = 100;
};

#endif //UNORDERED_SET_H
