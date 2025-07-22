#pragma once

#include <vector>
#include <unordered_map>
#include <typeindex>
#include <any>

namespace mocha
{

template <typename T>
class HandleMap
{
 public:
  /**
   * @brief if the HandleMap has the given id
   * 
   * @param identifier
   * @return true 
   * @return false 
   */
  bool has(int id)
  {
    for (int i : identifier_)
    {
      if (i == id)
        return true;
    }
    return false;
  }

  /**
   * @brief add an id and its data to HandleMap
   * 
   * @param id
   * @param item 
   */
  void add(int id, const T& item)
  {
    if (has(id)) return;

    identifier_.push_back(id);
    data_.push_back(item);

    connection_[id] = data_.size() - 1; 
  }

  void remove(int id)
  {
    if (!has(id)) return;

    // whole operation works as data_ and identifier_ are same size and each are 
    // stored in same position
    int index = connection_[id];    // index of data
    int last = identifier_.size()-1;        // last identifier

    std::swap(identifier_[index], identifier_[last]);
    std::swap(data_[index], data_[last]);

    connection_[identifier_[index]] = index;

    identifier_.pop_back();
    data_.pop_back();
    connection_.erase(id);
  }

  T* get(int id)
  {
    return &(data_[connection_[id]]);
  }

  void set(int id, const T& item)
  {
    if (!has(id)) return;

    data_[connection_[id]] = item;
  }

  void clear()
  {
    identifier_.clear();
    data_.clear();
    connection_.clear();
  }

  std::vector<int> view()
  {
    return identifier_;
  }

 private:
  std::vector<int>  identifier_;
  std::vector<T>    data_;
  std::unordered_map<int, int> connection_;
};

template<typename ...T>
class View
{
 public:
  View(HandleMap<T>&... args) : maps(args...) {}

  // returns view of identifier with type T
  std::vector<int> get()
  {
    std::vector<int> out;
    std::vector<int> base = std::get<0>(maps).view();
    ((base = (std::get<HandleMap<T>&>(maps).view().size() < base.size() 
                  ? std::get<HandleMap<T>&>(maps).view() : base)), ...);
    
    for (int i : base)
    {
      if ((std::get<HandleMap<T>&>(maps).has(i) && ...))
        out.push_back(i);
    }
    return out;
  }

 private:
  std::tuple<HandleMap<T>&...> maps;
};

class World
{
 public:
  static World& get()
  {
    static World inst;
    return inst;
  }

  int next() {return next_entity++;}
  void back() {next_entity--;}

  std::unordered_map<std::type_index, void*> view()
  {
    return maps_;
  }

  template<typename T>
  HandleMap<T>& view()
  {
    std::type_index t {typeid(T)};
    if (!maps_.contains(t))
      maps_[t] = new HandleMap<T>();
    
    return *static_cast<HandleMap<T>*>(maps_[t]);
  }

  void operator=(const World&) = delete;

 private:
  World(){};
  std::unordered_map<std::type_index, void*> maps_ {};
  int next_entity {0};
};

class System
{
 public:
  virtual void update(float dt) = 0;
};

int create();

void remove(int id);

template<typename T>
void emplace(int id, const T& data)
{
  World::get().view<T>().add(id, data);
}

template<typename T>
T* get(int id)
{
  return World::get().view<T>().get(id);
}

template<typename T>
bool has(int id)
{
  return World::get().view<T>().has(id);
}

template<typename ...T>
std::vector<int> view()
{
  return View<T...>(World::get().view<T>()...).get();
}

} // mocha