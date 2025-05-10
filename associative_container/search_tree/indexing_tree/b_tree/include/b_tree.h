#ifndef MP_OS_B_TREE_H
#define MP_OS_B_TREE_H

#include <iterator>
#include <utility>
#include <boost/container/static_vector.hpp>
#include <stack>
#include <pp_allocator.h>
#include <search_tree.h>
#include <initializer_list>
#include <logger_guardant.h>

template <typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5>
class B_tree final : private logger_guardant, private compare
{
public:

    using tree_data_type = std::pair<tkey, tvalue>;
    using tree_data_type_const = std::pair<const tkey, tvalue>;
    using value_type = tree_data_type_const;

private:

    static constexpr const size_t minimum_keys_in_node = t - 1;
    static constexpr const size_t maximum_keys_in_node = 2 * t - 1;

    // region comparators declaration

    inline bool compare_keys(const tkey& lhs, const tkey& rhs) const;
    inline bool compare_pairs(const tree_data_type& lhs, const tree_data_type& rhs) const;

    // endregion comparators declaration


    struct btree_node
    {
        boost::container::static_vector<tree_data_type, maximum_keys_in_node + 1> _keys;
        boost::container::static_vector<btree_node*, maximum_keys_in_node + 2> _pointers;
        btree_node() noexcept;
//        std::vector<tree_data_type, pp_allocator<tree_data_type>> _keys;
//        std::vector<btree_node*, pp_allocator<btree_node*>> _pointers;

//        btree_node(pp_allocator<value_type> al);
    };

    pp_allocator<value_type> _allocator;
    logger* _logger;
    btree_node* _root;
    size_t _size;

    logger* get_logger() const noexcept override;
    pp_allocator<value_type> get_allocator() const noexcept;

public:

    // region constructors declaration

    explicit B_tree(const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    explicit B_tree(pp_allocator<value_type> alloc, const compare& comp = compare(), logger *logger = nullptr);

    template<input_iterator_for_pair<tkey, tvalue> iterator>
    explicit B_tree(iterator begin, iterator end, const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    // endregion constructors declaration

    // region five declaration

    B_tree(const B_tree& other);

    B_tree(B_tree&& other) noexcept;

    B_tree& operator=(const B_tree& other);

    B_tree& operator=(B_tree&& other) noexcept;

    ~B_tree() noexcept override;

    // endregion five declaration

    // region iterators declaration

    class btree_iterator;
    class btree_reverse_iterator;
    class btree_const_iterator;
    class btree_const_reverse_iterator;

    class btree_iterator final
    {
        std::stack<std::pair<btree_node**, size_t>> _path;
        size_t _index;

    public:
        using value_type = tree_data_type_const;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_const_iterator;
        friend class btree_const_reverse_iterator;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_iterator(const std::stack<std::pair<btree_node**, size_t>>& path = std::stack<std::pair<btree_node**, size_t>>(), size_t index = 0);

    };

    class btree_const_iterator final
    {
        std::stack<std::pair<btree_node* const*, size_t>> _path;
        size_t _index;

    public:

        using value_type = tree_data_type_const;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_const_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_iterator;
        friend class btree_const_reverse_iterator;

        btree_const_iterator(const btree_iterator& it) noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_const_iterator(const std::stack<std::pair<btree_node* const*, size_t>>& path = std::stack<std::pair<btree_node* const*, size_t>>(), size_t index = 0);
    };

    class btree_reverse_iterator final
    {
        std::stack<std::pair<btree_node**, size_t>> _path;
        size_t _index;

    public:

        using value_type = tree_data_type_const;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_reverse_iterator;

        friend class B_tree;
        friend class btree_iterator;
        friend class btree_const_iterator;
        friend class btree_const_reverse_iterator;

        btree_reverse_iterator(const btree_iterator& it) noexcept;
        operator btree_iterator() const noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_reverse_iterator(const std::stack<std::pair<btree_node**, size_t>>& path = std::stack<std::pair<btree_node**, size_t>>(), size_t index = 0);
    };

    class btree_const_reverse_iterator final
    {
        std::stack<std::pair<btree_node* const*, size_t>> _path;
        size_t _index;

    public:

        using value_type = tree_data_type_const;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_const_reverse_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_const_iterator;
        friend class btree_iterator;

        btree_const_reverse_iterator(const btree_reverse_iterator& it) noexcept;
        operator btree_const_iterator() const noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_const_reverse_iterator(const std::stack<std::pair<btree_node* const*, size_t>>& path = std::stack<std::pair<btree_node* const*, size_t>>(), size_t index = 0);
    };

    friend class btree_iterator;
    friend class btree_const_iterator;
    friend class btree_reverse_iterator;
    friend class btree_const_reverse_iterator;

    // endregion iterators declaration

    // region element access declaration

    /*
     * Returns a reference to the mapped value of the element with specified key. If no such element exists, an exception of type std::out_of_range is thrown.
     */
    tvalue& at(const tkey&);
    const tvalue& at(const tkey&) const;

    /*
     * If key not exists, makes default initialization of value
     */
    tvalue& operator[](const tkey& key);
    tvalue& operator[](tkey&& key);

    // endregion element access declaration
    // region iterator begins declaration

    btree_iterator begin();
    btree_iterator end();

    btree_const_iterator begin() const;
    btree_const_iterator end() const;

    btree_const_iterator cbegin() const;
    btree_const_iterator cend() const;

    btree_reverse_iterator rbegin();
    btree_reverse_iterator rend();

    btree_const_reverse_iterator rbegin() const;
    btree_const_reverse_iterator rend() const;

    btree_const_reverse_iterator crbegin() const;
    btree_const_reverse_iterator crend() const;

    // endregion iterator begins declaration

    // region lookup declaration

    size_t size() const noexcept;
    bool empty() const noexcept;

    /*
     * Returns end() if not exist
     */

    btree_iterator find(const tkey& key);
    btree_const_iterator find(const tkey& key) const;

    btree_iterator lower_bound(const tkey& key);
    btree_const_iterator lower_bound(const tkey& key) const;

    btree_iterator upper_bound(const tkey& key);
    btree_const_iterator upper_bound(const tkey& key) const;

    bool contains(const tkey& key) const;

    // endregion lookup declaration

    // region modifiers declaration

    void clear() noexcept;

    /*
     * Does nothing if key exists, delegates to emplace.
     * Second return value is true, when inserted
     */
    std::pair<btree_iterator, bool> insert(const tree_data_type& data);
    std::pair<btree_iterator, bool> insert(tree_data_type&& data);

    template <typename ...Args>
    std::pair<btree_iterator, bool> emplace(Args&&... args);

    /*
     * Updates value if key exists, delegates to emplace.
     */
    btree_iterator insert_or_assign(const tree_data_type& data);
    btree_iterator insert_or_assign(tree_data_type&& data);

    template <typename ...Args>
    btree_iterator emplace_or_assign(Args&&... args);

    /*
     * Return iterator to node next ro removed or end() if key not exists
     */
    btree_iterator erase(btree_iterator pos);
    btree_iterator erase(btree_const_iterator pos);

    btree_iterator erase(btree_iterator beg, btree_iterator en);
    btree_iterator erase(btree_const_iterator beg, btree_const_iterator en);


    btree_iterator erase(const tkey& key);

    // endregion modifiers declaration

private:
    std::pair<size_t, bool> find_index(const tkey& key, btree_node* node) const noexcept {
        if (node == nullptr) return {0, false};

        size_t begin = 0;
        size_t end = node->_keys.size();

        while (begin < end) {
            size_t mid = begin + (end - begin) / 2;
            int cmp = compare_keys(key, node->_keys[mid].first);
            if (cmp > 0) {
                begin = mid + 1;
            } else if (cmp < 0) {
                end = mid;
            } else {
                return {mid, true};
            }
        }

        return {begin, false};
    }

    void split_node(std::stack<std::pair<btree_node**, size_t>>& path, btree_node*& node, size_t& index) {
        btree_node* current = *path.top().first;

        size_t mid_index = current->_keys.size() / 2; // center is right elem
        auto middle_pair = std::move(current->_keys[mid_index]);

        // Создаем новый правый узел
        auto* new_node = _allocator.template new_object<btree_node>();

        // Копируем правую половину ключей
        new_node->_keys.assign(
            std::make_move_iterator(current->_keys.begin() + mid_index + 1),
            std::make_move_iterator(current->_keys.end())
        );
        current->_keys.erase(current->_keys.begin() + mid_index, current->_keys.end());

        // Копируем указатели, если это не лист
        if (!is_terma(current)) {
            new_node->_pointers.assign(
                current->_pointers.begin() + mid_index + 1,
                current->_pointers.end()
            );
            current->_pointers.erase(current->_pointers.begin() + mid_index + 1, current->_pointers.end());
        }

        // Если мы сплитим корень
        if (path.size() == 1) {
            auto* new_root =  _allocator.template new_object<btree_node>();
            new_root->_keys.push_back(std::move(middle_pair));
            new_root->_pointers[0] = current;
            new_root->_pointers[1] = new_node;
            _root = new_root;

            path.pop();
            path.push(std::make_pair(&_root, 0)); // для возможных следующих сплитов
        } else {
            path.pop();  // Текущий узел уже не нужен

            auto [parent_ptr, parent_index] = path.top();
            btree_node* parent = *parent_ptr;

            // Вставляем в родителя значение и правый указатель
            parent->_keys.insert(parent->_keys.begin() + parent_index, std::move(middle_pair));
            parent->_pointers.insert(parent->_pointers.begin() + parent_index + 1, new_node);

            // Обновляем ссылку на текущий узел
            node = parent;
            index = parent_index;
        }


    }

    std::pair<std::stack<std::pair<btree_node**, size_t>>, size_t>
    insert_inside(tree_data_type&& data, std::stack<std::pair<btree_node**, size_t>>& path) {
        if (_root == nullptr) {
            _root = _allocator.template new_object<btree_node>();
            _root->_keys.push_back(std::move(data));
            _size = 1;
            path.push({ &_root, 0 });
            return { std::move(path), 0 };
        }

        btree_node* current_node = *path.top().first;
        auto [index, found] = find_index(data.first, current_node);
        if (found) {
            current_node->_keys[index].second = std::move(data.second);
            return { std::move(path), index };
        }

        current_node->_keys.insert(current_node->_keys.begin() + index, std::move(data));
        _size++;

        while (!path.empty()) {
            btree_node*& node = *path.top().first;
            if (node->_keys.size() <= maximum_keys_in_node) break;
            split_node(path, node, index); // need to check if current_node updates after split
        }

        return { std::move(path), index };
    }

    tvalue erase_inside(std::stack<std::pair<btree_node**, size_t>>& path, size_t& index) noexcept;

    bool is_terma(btree_node *node) const noexcept {
        return node == nullptr || node->_pointers.empty() || node->_pointers[0] == nullptr;
    }

    std::pair<std::stack<std::pair<btree_node**, size_t>>, std::pair<size_t,bool>> find_path(const tkey& key) const noexcept { // need check
        std::stack<std::pair<btree_node**, size_t>> path;
        if (_root == nullptr) return {std::move(path), {0, false}};

        btree_node* current_node = _root;
        size_t index = 0;
        bool found = false;

        while (!found && !is_terma(*path.top().first)) {
            auto [i, f] = find_index(key, current_node); // find_index shows at what index passed key might be in current_node
            index = i;
            found = f;

            if (index == 0 || is_terma(current_node)) {
                path.push({&current_node, index});
            } else {
                path.push({&current_node, index - 1});
            }
            if (is_terma(current_node)) break;
            // path.push({&current_node, index});

            current_node = current_node->_pointers[index];
        }

        return {std::move(path), {index, found}};
    }

    // new functions
    std::pair<size_t, bool> find_index_new(const tkey& key, btree_node* node) const { // тут просто линейный поиск используется вместо бин поиска в ориге find_index
        size_t i = 0;
        const size_t keys_count = node->_keys.size();
        while (i < keys_count && compare_keys(node->_keys[i].first, key)) {
            ++i;
        }
        bool found = (i < keys_count && !compare_keys(key, node->_keys[i].first));
        return {i, found};
    } // useless?

    std::pair<btree_node*, size_t> search_node(const tkey& key) {
        btree_node* current = _root;
        while (current != nullptr) {
            const auto [index, found] = find_index(key, current);
            if (found) {
                return {current, index};
            }
            if (is_terma(current)) {
                return {nullptr, 0};
            }
            current = current->_pointers[index]; // возможно стоит проверять в какое поддерево малой ноды идти (в левое или правое), тк сейчас выходит что мы идем все время в левое поддерево малой ноды
        }
        return {nullptr, 0};
    } // analog is find + find_path, useless?

    tvalue node_delete(btree_node* node, size_t index) {
        tvalue deleted_value = std::move(node->_keys[index].second);
        node->_keys.erase(node->_keys.begin() + index);
        if (!is_terma(node)) {
            node->_pointers.erase(node->_pointers.begin() + 1);
        }
        return deleted_value;
    }

    size_t node_insert(btree_node* node, tree_data_type&& data) { // needs review
        auto it = std::lower_bound(node->_keys.begin(), node->_keys.end(), data, [this](const auto& a, const auto& b) {
            return compare_keys(a.first, b.first);
        });
        size_t index = it - node->_keys.begin();
        node->_keys.insert(it, std::move(data));
        if (!is_terma(node)) {
            node->_pointers.insert(node->_pointers.begin() + index + 1, nullptr);
        }
        return index;
    }
    void split_child(btree_node* parent, size_t child_index) {
        btree_node* to_split = parent->_pointers[child_index];
        btree_node* new_node = _allocator.template new_object<btree_node>();
        size_t split_position = minimum_keys_in_node - 1;
        new_node->_keys.reserve(minimum_keys_in_node - 1);
        std::move(to_split->_keys.begin() + split_position + 1,
                  to_split->_keys.end(), std::back_inserter(new_node->_keys));
        to_split->_keys.resize(split_position);
        if (!is_terma(to_split)) {
            std::copy(to_split->_pointers.begin() + split_position + 1,
                      to_split->_pointers.end(), new_node->_pointers.begin());
            to_split->_pointers.resize(split_position + 1);
        }
        auto median = std::move(to_split->_keys[split_position]);
        size_t insert_position = node_insert(parent, std::move(median));
        parent->_pointers[insert_position + 1] = new_node;
    }
//returns true if new root
    bool merge_children(btree_node* parent, size_t index) {
        btree_node* left = parent->_pointers[index];
        btree_node* right = parent->_pointers[index + 1];
        left->_keys.push_back(std::move(parent->_keys[index]));
        parent->_keys.erase(parent->_keys.begin() + index);
        left->_keys.reserve(left->_keys.size() + right->_keys.size());
        std::move(right->_keys.begin(), right->_keys.end(), std::back_inserter(left->_keys));
        if (!is_terma(left)) {
            left->_pointers.reserve(left->_pointers.size() + right->_pointers.size());
            std::move(right->_pointers.begin(), right->_pointers.end(), std::back_inserter(left->_pointers));
        }
        _allocator.template delete_object<btree_node>(right);
        parent->_pointers.erase(parent->_pointers.begin() + index + 1);
        if (parent->_keys.empty() && parent == _root) {
            _allocator.template delete_object<btree_node*>(_root);
            _root = left;
            return true;
        }
        return false;
    }
    void insert_another(const tkey& key, const tvalue& value) {
        if (_root == nullptr) {
            _root = _allocator.template new_object<btree_node>();
            _root->_keys.emplace_back(key, value);
            _size = 1;
            return;
        }
        if (_root->_keys.size() == maximum_keys_in_node) {
            btree_node* new_root = _allocator.template new_object<btree_node>();
            new_root->_pointers[0] = _root;
            _root = new_root;
            split_child(_root, 0);
        }
        btree_node* current = _root;
        std::stack<std::pair<btree_node**, size_t>> path;
        while (true) {
            auto [index, found] = find_index(key, current);
            if (is_terma(current)) {
                node_insert(current, {key, value});
                ++_size;
                break;
            }
            path.push(std::make_pair(&current, index));
            btree_node* child = current->_pointers[index];
            if (child->_keys.size() == maximum_keys_in_node) {
                split_child(current, index);
                if (compare_keys(current->_keys[index].first, key)) {
                    index++;
                }
            }
            current = current->_pointers[index];
        }
        while (!path.empty()) {
            auto [node_ptr, index] = path.top();
            path.pop();
            if ((*node_ptr)->_keys.size() > maximum_keys_in_node) {
                split_child(*node_ptr, index);
            }
        }
    }

//returns true if modified
    bool fix_child_size(btree_node* parent, size_t index) {
        btree_node* child = parent->_pointers[index];
        if (child->_keys.size() >= minimum_keys_in_node) {
            return false;
        }
        if (index > 0) {
            btree_node* left = parent->_pointers[index - 1];
            if (left->_keys.size() > minimum_keys_in_node) {
                child->_keys.insert(child->_keys.begin(), std::move(parent->_keys[index - 1]));
                if (!is_terma(child)) {
                    child->_pointers.insert(child->_pointers.begin(), left->_pointers.back());
                    left->_pointers.pop_back();
                }
                parent->_keys[index - 1] = std::move(left->_keys.back());
                left->_keys.pop_back();
                return true;
            }
        }
        if (index < parent->_keys.size()) {
            btree_node* right = parent->_pointers[index + 1];
            if (right->_keys.size() > minimum_keys_in_node) {
                child->_keys.push_back(std::move(parent->_keys[index]));
                if (!is_terma(child)) {
                    child->_pointers.push_back(right->_pointers.front());
                    right->_pointers.erase(right->_pointers.begin());
                }
                parent->_keys[index] = std::move(right->_keys.front());
                right->_keys.erase(right->_keys.begin());
                return true;
            }
        }
        if (index > 0) {
            merge_children(parent, index - 1);
            return true;
        }
        merge_children(parent, index);
        return true;
    }
    tvalue remove_another(const tkey& key) {
        if (_root == nullptr) {
            throw std::out_of_range("tree is empty");
        }
        std::stack<btree_node*> path;
        btree_node* current = _root;
        tvalue removed_value;
        while (true) {
            auto [index, found] = find_index_new(key, current);
            if (found) {
                removed_value = std::move(current->_keys[index].second);
                if (is_terma(current)) {
                    node_delete(current, index);
                    --_size;
                    break;//error: мы же не проверяем, если нода уменьшилась
                }
                btree_node* left_child = current->_pointers[index];
                btree_node* right_child = current->_pointers[index + 1];
                if (left_child->_keys.size() >= minimum_keys_in_node) {
                    btree_node* pred = left_child;
                    while (!is_terma(pred)) {
                        pred = pred->_pointers.back();
                    }
                    current->_keys[index] = std::move(pred->_keys.back());
                    node_delete(pred, pred->_keys.size() - 1);
                    --_size;
                    break;
                } else if (right_child->_keys.size() >= minimum_keys_in_node) {
                    btree_node* successor = right_child;
                    while (!is_terma(successor)) {
                        successor = successor->_pointers.front();
                    }
                    current->_keys[index] = std::move(successor->_keys.front());
                    node_delete(successor, 0);
                    --_size;
                    break;
                } else {
                    merge_children(current, index);
                    current = left_child;
                    continue;
                }
            } else {
                if (is_terma(current)) {
                    throw std::out_of_range("this key doesn't exist");
                }
                fix_child_size(current, index);
                path.push(current);
                current = current->_pointers[index];
            }
        }
        while (!path.empty()) {
            btree_node* parent = path.top();
            path.pop();
            size_t child_index = 0;
            while (child_index < parent->_pointers.size()
                   && parent->_pointers[child_index] != current) {
                child_index++;
            }
            if (current->_keys.size() < minimum_keys_in_node - 1) {
                fix_child_size(parent, child_index);
            }
            current = parent;
        }
        if (_root->_keys.empty() && !is_terma(_root)) {
            btree_node* new_root = _root->_pointers[0];
            _allocator.delete_object(_root);
            _root = new_root;
        }
    }

    // / new functions
};


template<std::input_iterator iterator, compator<typename std::iterator_traits<iterator>::value_type::first_type> compare = std::less<typename std::iterator_traits<iterator>::value_type::first_type>,
        std::size_t t = 5, typename U>
B_tree(iterator begin, iterator end, const compare &cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
       logger *logger = nullptr) -> B_tree<typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare, t>;

template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5, typename U>
B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare &cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
       logger *logger = nullptr) -> B_tree<tkey, tvalue, compare, t>;

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::compare_pairs(const B_tree::tree_data_type &lhs,
                                                     const B_tree::tree_data_type &rhs) const
{
    return compare_keys(lhs.first, rhs.first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::compare_keys(const tkey &lhs, const tkey &rhs) const
{
    return compare::operator()(lhs, rhs);
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_node::btree_node() noexcept: _keys(), _pointers()
{
    _pointers.resize(maximum_keys_in_node + 2, nullptr);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
logger* B_tree<tkey, tvalue, compare, t>::get_logger() const noexcept
{
    return _logger;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
pp_allocator<typename B_tree<tkey, tvalue, compare, t>::value_type> B_tree<tkey, tvalue, compare, t>::get_allocator() const noexcept
{
    return _allocator;
}

// region constructors implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* logger): _root(nullptr), _size(0), _allocator(alloc), _logger(logger), compare(cmp)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(
        pp_allocator<value_type> alloc,\
        const compare& comp,
        logger* logger): _root(nullptr), _size(0), _allocator(alloc), _logger(logger), compare(comp)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<input_iterator_for_pair<tkey, tvalue> iterator>
B_tree<tkey, tvalue, compare, t>::B_tree(
        iterator begin,
        iterator end,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* logger): _root(nullptr), _size(0), _allocator(alloc), _logger(logger), compare(cmp)
{
    iterator temp = begin;
    while (temp != end) {
        insert(*temp);
        ++temp;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(
        std::initializer_list<std::pair<tkey, tvalue>> data,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* logger): _root(nullptr), _size(0), _allocator(alloc), _logger(logger), compare(cmp)
{
            for (const auto& temp: data) {
                emplace(temp.first, temp.second);
            }
}

// endregion constructors implementation

// region five implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::~B_tree() noexcept
{
    clear();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(const B_tree& other)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> B_tree<tkey, tvalue, compare, t>::B_tree(const B_tree& other)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>& B_tree<tkey, tvalue, compare, t>::operator=(const B_tree& other)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> B_tree<tkey, tvalue, compare, t>& B_tree<tkey, tvalue, compare, t>::operator=(const B_tree& other)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(B_tree&& other) noexcept:
_root(other._root), _allocator(std::move(other._allocator)), _size(other._size), _logger(other._logger)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>& B_tree<tkey, tvalue, compare, t>::operator=(B_tree&& other) noexcept
{
    if (this != &other) {
        clear();
        _root = other._root;
        _allocator = std::move(other._allocator);
        _size = other._size;
        _logger = other._logger;
    }
    return *this;
}

// endregion five implementation

// region iterators implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_iterator::btree_iterator(
    const std::stack<std::pair<btree_node**, size_t>>& path,
    size_t index
    ): _path(path), _index(index)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator*() const noexcept
{
    return (*(_path.top().first))->_keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator->() const noexcept
{
    return &((*(_path.top().first))->_keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator&
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++()
{
    if (_path.empty()) return *this;

    if (is_terminate_node()) {
        _index++;
        while (!_path.empty() && _index == ((*(_path.top().first))->_keys).size() - 1) {
            _index = _path.top().second;
            _path.pop();
        }
    } else {
        _path.push({&(*(_path.top().first))->_pointers[_index + 1], 0});
        _index = 0;
        while (!is_terminate_node()) {
            _path.push({&(*(_path.top().first))->_pointers[0], 0});
        }
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++(int)
{
    auto temp = *this;
    ++(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator&
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--()
{
    if (_path.empty()) return *this;

    if (is_terminate_node()) {
        if (_index == 0) {
            while (!_path.empty() && _index == 0) {
                _index = _path.top().second;
                _path.pop();
            }
        } else {
            --_index;
        }
    } else {
        _path.push({&(*_path.top().first)->_pointers[_index], _index});
        while (!is_terminate_node()) {
            _index = (*(_path.top().first))->_keys.size();
            _path.push({&((*(_path.top().first))->_pointers[_index]), _index - 1});
        }
        _index = (*_path.top().first)->_keys.size() - 1;
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--(int)
{
    auto temp = *this;
    --(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator==(const self& other) const noexcept
{
    if (_path.empty() && other._path.empty()) {
        return true;
    }
    return (!_path.empty() && !other._path.empty() && _path.size() == other._path.size() &&
            *(_path.top().first) == *(other._path.top().first) && _index == other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator!=(const self& other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::depth() const noexcept
{
    return _path.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::current_node_keys_count() const noexcept
{
    if (_path.empty()) {
        return 0;
    }
    btree_node* current_node = *_path.top().first;
    if (current_node != nullptr) {
        return current_node->_keys.size();
    }
    return 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::is_terminate_node() const noexcept
{
    if (_path.empty()) {
        return true;
    }
    btree_node* current_node = *_path.top().first;
    return is_terma(current_node);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::index() const noexcept
{
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(
        const std::stack<std::pair<btree_node* const*,
        size_t>>& path, size_t index):
    _path(path), _index(index)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(
        const btree_iterator& it) noexcept:
    _index(it._index)
{
    std::stack<std::pair<btree_node* const*, size_t>> converted;
    std::stack<std::pair<btree_node**, size_t>> temp = it._path;

    std::vector<std::pair<btree_node** , size_t>> reversed;
    while (!temp.empty()) {
        reversed.push_back(temp.top());
        temp.pop();
    }

    for (auto it = reversed.rbegin(); it != reversed.rend(); ++it) {
        btree_node* const* ptr = const_cast<btree_node* const*>((*it).first);
        converted.emplace(ptr, it->second);
    }

    _path = std::move(converted);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator*() const noexcept
{
    return (*(_path.top().first))->_keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator->() const noexcept
{
    return &(*(_path.top().first))->_keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++()
{
    if (_path.empty()) return *this;

    if (is_terminate_node()) {
        _index++;
        while (!_path.empty() && _index == ((*(_path.top().first))->_keys).size() - 1) {
            _index = _path.top().second;
            _path.pop();
        }
    } else {
        _path.push({&(*(_path.top().first))->_pointers[_index + 1], 0});
        _index = 0;
        while (!is_terminate_node()) {
            _path.push({&(*(_path.top().first))->_pointers[0], 0});
        }
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++(int)
{
    auto temp = *this;
    ++(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--()
{
    if (_path.empty()) return *this;

    if (is_terminate_node()) {
        if (_index == 0) {
            while (!_path.empty() && _index == 0) {
                _index = _path.top().second;
                _path.pop();
            }
        } else {
            --_index;
        }
    } else {
        _path.push({&(*_path.top().first)->_pointers[_index], _index});
        while (!is_terminate_node()) {
            _index = (*(_path.top().first))->_keys.size();
            _path.push({&((*(_path.top().first))->_pointers[_index]), _index - 1});
        }
        _index = (*_path.top().first)->_keys.size() - 1;
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--(int)
{
    auto temp = *this;
    --(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator==(const self& other) const noexcept
{
    if (_path.empty() && other._path.empty()) {
        return true;
    }
    return (!_path.empty() && !other._path.empty() && _path.size() == other._path.size() &&
            *(_path.top().first) == *(other._path.top().first) && _index == other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator!=(const self& other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::depth() const noexcept
{
    if (_path.empty()) {
        return 0;
    }
    return _path.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::current_node_keys_count() const noexcept
{
    if (_path.empty()) {
        return 0;
    }
    btree_node* current_node = *_path.top().first;
    if (current_node != nullptr) {
        return current_node->_keys.size();
    }
    return 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::is_terminate_node() const noexcept
{
    if (_path.empty()) {
        return true;
    }
    const btree_node* current_node = *_path.top().first;
    return current_node == nullptr || current_node->_pointers[0] == nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::index() const noexcept
{
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(
        const std::stack<std::pair<btree_node**, size_t>>& path, size_t index): _path(path), _index(index)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(
        const btree_iterator& it) noexcept
{
    btree_iterator temp = it;
    --temp;
    _path = temp._path;
    _index = temp._index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator B_tree<tkey, tvalue, compare, t>::btree_iterator() const noexcept
{
    btree_iterator result(_path, _index);
    ++result;
    return result;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator*() const noexcept
{
    return (*(_path.top().first))->_keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator->() const noexcept
{
    return &(*(_path.top().first))->_keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++()
{
    if (_path.empty()) return *this;
    btree_iterator fwd = static_cast<btree_iterator>(*this);
    --fwd;
    *this = btree_reverse_iterator(fwd);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++(int)
{
    auto temp = *this;
    ++(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--()
{
    if (_path.empty()) return *this;
    btree_iterator fwd = static_cast<btree_iterator>(*this);
    ++fwd;
    *this = btree_reverse_iterator(fwd);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--(int)
{
    auto temp = *this;
    --(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator==(const self& other) const noexcept
{
    if (_path.empty() && other._path.empty()) {
        return true;
    }
    return (!_path.empty() && !other._path.empty() && _path.size() == other._path.size() &&
            *(_path.top().first) == *(other._path.top().first) && _index == other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator!=(const self& other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::depth() const noexcept
{
    if (_path.empty()) {
        return 0;
    }
    return _path.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::current_node_keys_count() const noexcept
{
    if (_path.empty()) {
        return 0;
    }
    btree_node* current_node = *_path.top().first;
    if (current_node != nullptr) {
        return current_node->_keys.size();
    }
    return 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::is_terminate_node() const noexcept
{
    if (_path.empty()) {
        return true;
    }
    const btree_node* current_node = *_path.top().first;
    return current_node == nullptr || current_node->_pointers[0] == nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::index() const noexcept
{
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(
        const std::stack<std::pair<btree_node* const*, size_t>>& path, size_t index): _path(path), _index(index)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(
        const btree_reverse_iterator& it) noexcept: _index(it._index)
{
    std::stack<std::pair<btree_node* const*, size_t>> converted;
    std::stack<std::pair<btree_node**, size_t>> temp = it._path;

    std::vector<std::pair<btree_node**, size_t>> reversed;
    while (!temp.empty()) {
        reversed.push_back(temp.top());
        temp.pop();
    }

    for (auto iter = reversed.rbegin(); iter != reversed.rend(); ++iter) {
        btree_node* const* ptr = const_cast<btree_node* const*>(iter->first);
        converted.emplace(ptr, iter->second);
    }

    _path = std::move(converted);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator B_tree<tkey, tvalue, compare, t>::btree_const_iterator() const noexcept
{
    btree_const_iterator result(_path, _index);
    ++result;
    return result;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator*() const noexcept
{
    return (*(_path.top().first))->_keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator->() const noexcept
{
    return &(*(_path.top().first))->_keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++()
{
    if (_path.empty()) return *this;
    btree_const_iterator fwd = static_cast<btree_const_iterator>(*this);
    --fwd;
    *this = btree_reverse_iterator(fwd);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++(int)
{
    auto temp = *this;
    ++(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--()
{
    if (_path.empty()) return *this;
    btree_const_iterator fwd = static_cast<btree_const_iterator>(*this);
    ++fwd;
    *this = btree_reverse_iterator(fwd);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--(int)
{
    auto temp = *this;
    --(*this);
    return temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator==(const self& other) const noexcept
{
    if (_path.empty() && other._path.empty()) {
        return true;
    }
    return (!_path.empty() && !other._path.empty() && _path.size() == other._path.size() &&
            *(_path.top().first) == *(other._path.top().first) && _index == other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator!=(const self& other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::depth() const noexcept
{
    if (_path.empty()) {
        return 0;
    }
    return _path.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::current_node_keys_count() const noexcept
{
    if (_path.empty()) {
        return 0;
    }
    btree_node* current_node = *_path.top().first;
    if (current_node != nullptr) {
        return current_node->_keys.size();
    }
    return 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::is_terminate_node() const noexcept
{
    if (_path.empty()) {
        return true;
    }
    const btree_node* current_node = *_path.top().first;
    return current_node == nullptr || current_node->_pointers[0] == nullptr; // we can use is_term func here
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::index() const noexcept
{
    return _index;
}

// endregion iterators implementation

// region element access implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::at(const tkey& key)
{
    auto path = find_path(key);
    if (path.second.second == false) {
        throw std::logic_error("this key doesn't exist");
    }
    return ((*(path.first.top().first))->_keys[path.second.first]).second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
const tvalue& B_tree<tkey, tvalue, compare, t>::at(const tkey& key) const
{
    auto path = find_path(key);
    if (path.second.second == false) {
        throw std::logic_error("this key doesn't exist");
    }
    return ((*(path.first.top().first))->_keys[path.second.first]).second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::operator[](const tkey& key)
{
    auto path = find_path(key);
    if (path.second.second == false) {
        throw std::logic_error("this key doesn't exist");
    }
    return ((*(path.first.top().first))->_keys[path.second.first]).second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::operator[](tkey&& key)
{
    auto path = find_path(key);
    if (path.second.second == false) {
        throw std::logic_error("this key doesn't exist");
    }
    return ((*(path.first.top().first))->_keys[path.second.first]).second;
}

// endregion element access implementation

// region iterator begins implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::begin()
{
    if (_root == nullptr) return btree_iterator();

    std::stack<std::pair<btree_node**, size_t>> result_stack;
    result_stack.push({&_root, 0});

    while (!is_terma(result_stack.top().first)) {
        result_stack.push({&(*(result_stack.top().first)->_pointers[0]), 0});
    }
    return btree_iterator(std::move(result_stack), 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::end()
{
    return btree_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::begin() const
{

    if (_root == nullptr) return btree_const_iterator();

    std::stack<std::pair<btree_node**, size_t>> result_stack;
    result_stack.push(std::make_pair(&_root, 0));

    while (!is_terma(result_stack.top().first)) {
        result_stack.push(std::make_pair(&(*(result_stack.top().first)->_pointers[0]), 0));
    }
    return btree_const_iterator(std::move(result_stack), 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::end() const
{
    return btree_const_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cbegin() const
{
    return begin();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cend() const
{
    return end();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin()
{
    return btree_reverse_iterator(end());
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend()
{
    return btree_reverse_iterator(begin());
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin() const
{
    return btree_const_reverse_iterator(btree_reverse_iterator(end()));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend() const
{
    return btree_const_reverse_iterator(btree_reverse_iterator(begin()));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crbegin() const
{
    return rbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crend() const
{
    return rend();
}

// endregion iterator begins implementation

// region lookup implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::size() const noexcept
{
    return _size;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::empty() const noexcept
{
    return _root == nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::find(const tkey& key)
{

    auto [path, index_found] = find_path(key);
    auto [index, found] = index_found;
    if (!found) {
        return end();
    }
    return btree_iterator(path, index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::find(const tkey& key) const
{
    auto [path, index_found] = find_path(key);
    auto [index, found] = index_found;
    if (!found) {
        return end();
    }
    return btree_iterator(path, index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key) const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key) const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key) const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key) const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::contains(const tkey& key) const
{
    auto path = find_path(key);
    return path.second.second;
}

// endregion lookup implementation

// region modifiers implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::clear() noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> void B_tree<tkey, tvalue, compare, t>::clear() noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::insert(const tree_data_type& data)
{
    auto [path, index_found] = find_path(data.first);
    auto [index, found] = index_found;
    if (found == true) {
        return {btree_iterator(path, index), false}; // such key already exists
    }
    auto [inserted_path, inserted_index] = insert_inside(data, std::move(path));
    return {btree_iterator(std::move(inserted_path), inserted_index), true};
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::insert(tree_data_type&& data)
{
    auto [path, index_found] = find_path(data.first);
    auto [index, found] = index_found;
    if (found == true) {
        return {btree_iterator(path, index), false}; // such key already exists
    }
    auto [inserted_path, inserted_index] = insert_inside(std::move(data), std::move(path));
    return {btree_iterator(std::move(inserted_path), inserted_index), true};
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<typename... Args>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::emplace(Args&&... args)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "template<typename... Args>\n"
                          "std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>\n"
                          "B_tree<tkey, tvalue, compare, t>::emplace(Args&&... args)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::insert_or_assign(const tree_data_type& data)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_iterator\n"
                          "B_tree<tkey, tvalue, compare, t>::insert_or_assign(const tree_data_type& data)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::insert_or_assign(tree_data_type&& data)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_iterator\n"
                          "B_tree<tkey, tvalue, compare, t>::insert_or_assign(tree_data_type&& data)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<typename... Args>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::emplace_or_assign(Args&&... args)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "template<typename... Args>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_iterator\n"
                          "B_tree<tkey, tvalue, compare, t>::emplace_or_assign(Args&&... args)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_iterator pos)
{
    return erase(*pos);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator pos)
{
    return erase(*pos);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_iterator beg, btree_iterator en)
{
    btree_iterator temp = beg;
    while (temp != en) {
        erase(*temp);
        ++temp;
    }
    //что возвращать
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator beg, btree_const_iterator en)
{
    btree_const_iterator temp = beg;
    while (temp != en) {
        erase(*temp);
        ++temp;
    }
    //что возвращать ??
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(const tkey& key)
{
    auto path = find_path(key);
    if (path.second.second == false) {
        throw std::logic_error("this value doesn't exist");
    }
    //TODO добавить возврат итератора
    return erase_inside(path.first, path.second.first);
}

// endregion modifiers implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool compare_pairs(const typename B_tree<tkey, tvalue, compare, t>::tree_data_type &lhs,
                   const typename B_tree<tkey, tvalue, compare, t>::tree_data_type &rhs)
{
    return compare::operator()(lhs.first, rhs.first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool compare_keys(const tkey &lhs, const tkey &rhs)
{
    return compare::operator()(lhs, rhs);
}



#endif