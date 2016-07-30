#pragma once

#include <functional>
#include <set>
#include <memory>
#include <mutex>

#include "bmap_types.h"

namespace mpv{
  template<class _Kty, class _Ty>
  struct TItemMap {
    typedef std::shared_ptr<_Kty> shared_ptr_key;
    typedef std::shared_ptr<_Ty> shared_ptr_value;
    typedef std::pair<shared_ptr_key, shared_ptr_value> shared_ptr_item;
  };
  template<class _Type>
  struct TItem {
    typedef std::shared_ptr<_Type> shared_ptr_type;
  };

  template<class _Pr, class _Kty, class _Ty, size_t _Idx = 0>
  struct TSharedPtrItemInSetLessPredicate :
    public std::binary_function<typename TItemMap<_Kty, _Ty>::shared_ptr_item, typename TItemMap<_Kty, _Ty>::shared_ptr_item, bool> {
  private:
      typedef _Pr comparator;
      typedef typename TItemMap<_Kty, _Ty>::shared_ptr_item value_type;
      comparator _comparator;
  public:
    TSharedPtrItemInSetLessPredicate(const comparator& pr=comparator()) : _comparator(pr){}
    result_type operator()(const first_argument_type& l, const second_argument_type& r) const{
      return _comparator(*std::get<_Idx>(l), *std::get<_Idx>(r));
    }
  };

  namespace tags{
    enum : size_t{
      key = 0,
      value
    };
    struct left_relation {};
    struct right_relation {};
  }
  namespace relation{

    template <
      class _Tags,
      class _Kty, 
      class _Ty, 
      class _KeyPr,
      class _ValuePr 
    >  
    struct relation_traits;

    // Like Left Relation 
    //
    template <
      class _Kty, 
      class _Ty, 
      class _KeyPr,
      class _ValuePr 
    >  
    struct relation_traits<tags::left_relation, _Kty,  _Ty, _KeyPr, _ValuePr>{
      typedef typename TItemMap<_Kty,_Ty>::shared_ptr_item shared_ptr_item;            
      typedef TSharedPtrItemInSetLessPredicate<_KeyPr, _Kty, _Ty, tags::key> TSharedPtrComparator;
      typedef std::set<shared_ptr_item, TSharedPtrComparator > TSetOfSharedPtrItem;
    };

    // Like Right Relation 
    //
    template <
      class _Kty, 
      class _Ty, 
      class _KeyPr,
      class _ValuePr 
    >  
    struct relation_traits<tags::right_relation, _Kty,  _Ty, _KeyPr, _ValuePr>{
      typedef typename TItemMap<_Kty,_Ty>::shared_ptr_item shared_ptr_item;      
      typedef TSharedPtrItemInSetLessPredicate<_ValuePr, _Kty, _Ty, tags::value> TSharedPtrComparator;
      typedef std::set<shared_ptr_item, TSharedPtrComparator > TSetOfSharedPtrItem;
    };
    template <
      class _Kty, 
      class _Ty, 
      class _KeyPr,
      class _ValuePr 
    >  
    struct binary_traits{
      typedef relation_traits<tags::left_relation, _Kty, _Ty, _KeyPr, _ValuePr> left_traits;
      typedef relation_traits<tags::right_relation, _Kty, _Ty, _KeyPr, _ValuePr> right_traits;
    };
  }

  template<
    class _Kty, 
    class _Ty,
    class _KeyPr,
    class _ValuePr,
    class _Alloc
  >
  class bmap_base
  {

#pragma region Typedef Section (bmap_base)
  public:

    typedef bmap_base<_Kty, _Ty, _KeyPr, _ValuePr, _Alloc> _Myt;

    typedef _Kty key_type;
    typedef const key_type* const_ptr_key_type;
    typedef _Ty value_type;
    typedef const value_type* const_ptr_value_type;
    typedef _KeyPr compare_key;
    typedef _ValuePr compare_value;
    typedef typename std::common_type<key_type, value_type>::type common_type;

    typedef typename TItemMap<_Kty, _Ty>::shared_ptr_key shared_ptr_key;
    typedef typename TItemMap<_Kty, _Ty>::shared_ptr_value shared_ptr_value;
    typedef typename TItemMap<_Kty, _Ty>::shared_ptr_item shared_ptr_item;
    typedef typename TItemMap<_Kty, _Ty>::shared_ptr_item item_type;

    typedef typename relation::binary_traits<key_type, value_type, compare_key, compare_value>::left_traits left_traits;
    typedef typename relation::binary_traits<key_type, value_type, compare_key, compare_value>::right_traits right_traits;

    typedef typename left_traits::TSharedPtrComparator compare_from_key;
    typedef typename right_traits::TSharedPtrComparator compare_from_value;

    typedef typename left_traits::TSetOfSharedPtrItem set_from_key;
    typedef typename right_traits::TSetOfSharedPtrItem set_from_value;
    
    typedef typename set_from_key::iterator iterator_key;
    typedef typename set_from_key::const_iterator const_iterator_key;
    typedef typename set_from_key::allocator_type allocator_type_from_key;

    typedef typename set_from_value::iterator iterator_value;
    typedef typename set_from_value::const_iterator const_iterator_value;
    typedef typename set_from_value::allocator_type allocator_type_from_value;

    typedef typename iterator_key iterator;
    typedef typename const_iterator_key const_iterator;

#pragma endregion

#pragma region Protected Members Section (bmap_base)

  protected:
    set_from_key m_set_key;
    set_from_value m_set_value;
    _Alloc m_allocator;
    std::recursive_mutex _mutex;

#pragma endregion

#pragma region Constructors Section (bmap_base)

  public:    
    _Myt(const _KeyPr& pr_key, const _ValuePr& pr_value, const _Alloc& alloc) :
      m_set_key(compare_from_key(pr_key),allocator_type_from_key(alloc)), 
      m_set_value(compare_from_value(pr_value),allocator_type_from_value(alloc)){ }
#pragma endregion

  protected:
    shared_ptr_item new_item(const key_type& key, const value_type& value){
      shared_ptr_key new_key(new key_type(key));
      shared_ptr_value new_value(new value_type(value));
      return std::make_pair(new_key,new_value);
    }

    iterator find_from_value(const shared_ptr_item& some_item){
      std::lock_guard<std::recursive_mutex> l_g(_mutex);
      const_iterator_value it_val = m_set_value.find(some_item);
      if (it_val != m_set_value.end())
        return this->find(*std::get<tags::key>(*it_val));
      return m_set_key.end();
      }

    void erase_from_value(iterator it){
      std::lock_guard<std::recursive_mutex> l_g(_mutex);
      iterator_value it_val = m_set_value.find(*it);
      m_set_value.erase(it_val);
    }

  public:

#pragma region Public member-function insert/erase/find/clear

    iterator find(const key_type& some_value){
      std::lock_guard<std::recursive_mutex> l_g(_mutex);
      shared_ptr_key temp_key(new key_type(some_value));
      shared_ptr_value temp_value(new value_type());
      return m_set_key.find(std::make_pair(temp_key,temp_value));
    }

    iterator find(const key_type key, const value_type& value){
      std::lock_guard<std::recursive_mutex> l_g(_mutex);
      auto newItem = new_item(key,value);
      iterator it_key = m_set_key.find(newItem);
      if (it_key != m_set_key.end())
        return it_key;
      return this->find_from_value(newItem);
    }
    
    std::pair<iterator,bool> insert(const key_type& key, const value_type& value){
      std::lock_guard<std::recursive_mutex> l_g(_mutex);
      auto newItem = new_item(key,value);      
      iterator it_result = m_set_key.insert(newItem).first;
      if (it_result != m_set_key.end())
        if (m_set_value.insert(newItem).second == true)
          return std::make_pair(it_result, true);
      return std::make_pair(this->end(), false);
    }

    iterator erase(iterator it){
      std::lock_guard<std::recursive_mutex> l_g(_mutex);
      if(it == m_set_key.end())
        return it;
      erase_from_value(it);
      return m_set_key.erase(it);
    }

    size_t erase(const key_type& key, const value_type& value){   
      std::lock_guard<std::recursive_mutex> l_g(_mutex);
      auto newItem = new_item(key,value);
      iterator it_key = m_set_key.find(newItem);      
      if (it_key != m_set_key.end()){
        this->erase(it_key);
        return 1;
      }
      iterator it_value = this->find_from_value(newItem);
      if (it_value != m_set_key.end()){
        this->erase(it_value);
        return 1;
      }
      return 0;
    }

    size_t erase(const key_type& key){    
      std::lock_guard<std::recursive_mutex> l_g(_mutex);
      auto newItem = new_item(key,value_type());      
      iterator it = m_set_key.find(newItem);
      if (it!=m_set_key.end()){        
        erase(it);
        return 1;
      }
      return 0;      
    }

    void clear(){
      m_set_key.clear();
      m_set_value.clear();
    }

    iterator begin() {return m_set_key.begin();}
    const_iterator begin() const {return m_set_key.begin();}
    iterator end() {return m_set_key.end();}
    const_iterator end() const {return m_set_key.end();}
    iterator rbegin() {return m_set_key.rbegin();}
    const_iterator rbegin() const {return m_set_key.rbegin();}

#pragma endregion
  
    bool empty() const {
      return m_set_key.empty();
    }
    size_t size() const{
      //TODO: assert(m_set_key.size() == m_set_value.size()
      return m_set_key.size();
    }
};

#pragma region Binary Map for Equal Types (bmap_eq_types)

  template<
    class _Kty, 
    class _Ty,
    class _KeyPr,
    class _ValuePr,
    class _Alloc /*= std::allocator<std::pair<const _Kty, const _Ty> >*/
  >
  class bmap_eq_types
    : public bmap_base<_Kty, _Ty, _KeyPr, _ValuePr, _Alloc>
  {
#pragma region Typedef Section (bmap_eq_types)

    typedef bmap_eq_types<_Kty, _Ty, _KeyPr, _ValuePr, _Alloc> _Myt;
    typedef bmap_base<_Kty, _Ty, _KeyPr, _ValuePr, _Alloc> _MyBase;
    typedef _Kty key_type;
    typedef _Ty value_type;
    typedef _KeyPr compare_key;
    typedef _ValuePr compare_value;

#pragma endregion

  public:
    iterator find(const common_type& some_value){  
      std::lock_guard<std::recursive_mutex> l_g(_mutex);
      return _MyBase::find(some_value, some_value);
    }

    size_t erase(const common_type& some_value){
      std::lock_guard<std::recursive_mutex> l_g(_mutex);
      return _MyBase::erase(some_value,some_value);
    }       

    iterator erase(iterator it){
      std::lock_guard<std::recursive_mutex> l_g(_mutex);
      return _MyBase::erase(it);
    }

  protected:
    _Myt(const _KeyPr& pr_key, const _ValuePr& pr_value, const _Alloc& alloc) : _MyBase(pr_key, pr_value, alloc)  {}    
  };

#pragma region Binary Map for Different Types (bmap_dif_types)
  template<
    class _Kty, 
    class _Ty,
    class _KeyPr,
    class _ValuePr,
    class _Alloc
  >
  class bmap_dif_types 
    : public bmap_base<_Kty, _Ty, _KeyPr, _ValuePr, _Alloc>
  {

#pragma region Typedef Section (bmap_dif_types)

    typedef bmap_dif_types<_Kty, _Ty, _KeyPr, _ValuePr, _Alloc> _Myt;
    typedef bmap_base<_Kty, _Ty, _KeyPr, _ValuePr, _Alloc> _MyBase;
    typedef _Kty key_type;
    typedef _Ty value_type;
    typedef _KeyPr compare_key;
    typedef _ValuePr compare_value;  
    typedef typename std::common_type<key_type, value_type>::type common_type;

#pragma endregion

  public:    

    iterator find(const key_type& some_value){
      std::lock_guard<std::recursive_mutex> l_g(_mutex);
      shared_ptr_item temp_item = _MyBase::new_item(key_type(some_value), value_type());
      return m_set_key.find(temp_item);
    }

    iterator find(const value_type& some_value){
      std::lock_guard<std::recursive_mutex> l_g(_mutex);
      shared_ptr_item temp_item = _MyBase::new_item(key_type(), value_type(some_value));
      return _MyBase::find_from_value(temp_item);
    }

    size_t erase(const key_type& key){
      std::lock_guard<std::recursive_mutex> l_g(_mutex);
      return _MyBase::erase(key);
    }

    size_t erase(const value_type& value){   
      std::lock_guard<std::recursive_mutex> l_g(_mutex);      
      iterator it =this->find(value);
      if (it!=m_set_key.end()){
        _MyBase::erase(it);
        return 1;
      }
      return 0;
    }    

    iterator erase(iterator it){
      std::lock_guard<std::recursive_mutex> l_g(_mutex);
      return _MyBase::erase(it);
    }
    

#pragma region Constructors Section (bmap_dif_types)

  protected:
    _Myt(const _KeyPr& pr_key, const _ValuePr& pr_value, const _Alloc& alloc) : _MyBase(pr_key, pr_value, alloc)  {}


#pragma endregion


  }; // end class bmap_dif_types

  template<
    class _Kty, 
    class _Ty,
    class _KeyPr = std::less<_Kty>,
    class _ValuePr = std::less<_Ty>,
    class _Alloc = std::allocator<std::pair<std::shared_ptr<_Kty>, std::shared_ptr<_Ty>> >
    >
  class bmap : public 
    bmap_types::select_type<
    bmap_types::is_equal_types<_Kty, _Ty>::value,
    bmap_eq_types<_Kty, _Ty, _KeyPr, _ValuePr, _Alloc>,
    bmap_dif_types<_Kty, _Ty, _KeyPr, _ValuePr, _Alloc>
    >::result_type
  {
  protected:
    typedef typename bmap_types::select_type<
      bmap_types::is_equal_types<_Kty, _Ty>::value,
      bmap_eq_types<_Kty, _Ty, _KeyPr, _ValuePr, _Alloc>,
      bmap_dif_types<_Kty, _Ty, _KeyPr, _ValuePr, _Alloc>
    >::result_type _MyBase;

    typedef _Kty key_type;
    typedef _Ty value_type;
    typedef _KeyPr compare_key;
    typedef _ValuePr compare_value;

    typedef bmap<_Kty, _Ty, _KeyPr, _ValuePr, _Alloc> _Myt; 
  public:    
    _Myt(const compare_key& pr_key=compare_key(),
      const compare_value& pr_value=compare_value(),
      const _Alloc& alloc = _Alloc())  : _MyBase(pr_key, pr_value, alloc) {}     

  };
}