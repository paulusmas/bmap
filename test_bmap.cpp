#include "bmap_head.h"
#include <iostream>
#include <thread>
#include <cassert>

#define TEST_THREAD

class Foo{
public:
  Foo() : _m(0) {}
  Foo(int i) : _m(i){}
  bool operator<(const Foo& r) const{
    return _m < r._m;
  }
private:
  int _m;
};

static mpv::bmap<int,Foo> gBMapIntFoo;

void init(){
  for (int i = 0; i<1000; ++i){
    gBMapIntFoo.insert(i,Foo(i));
    std::cout<<"init\n";
  }
}

void test_insert(){
  for (int i = 1000; i<2000; ++i){
    gBMapIntFoo.insert(i,Foo(i));
    std::cout<<"test_insert\tgBMapIntFoo.insert(i,Foo(i))\n";
  }
}

void test_find(){
  for (int i = 0; i<1000; ++i){
    gBMapIntFoo.find(i);
    std::cout<<"test_find\tgBMapIntFoo.find(i)\n";
  }
}

void test_erase(){
  for (int i = 100; i<1000; ++i){
    gBMapIntFoo.erase(Foo(i));
    std::cout<<"test_erase\tgBMapIntFoo.erase(Foo(i))\n";
  }
}

int main()
{  
  mpv::bmap<int,double> bm_dif;
  mpv::bmap<int,double>::iterator it;
  assert(true == bm_dif.insert(10,20.4).second);
  assert(false == bm_dif.insert(10,20.4).second);
  assert(true == bm_dif.insert(5,100.4).second);
  it = bm_dif.find(20.45);
  assert(it ==bm_dif.end());
  it = bm_dif.find(10); // find 10 -- 20.4
  assert(it !=bm_dif.end());
  it = bm_dif.erase(it); // erase 10 -- 20.4
  assert(bm_dif.erase(20.4) == 0); 
  assert( bm_dif.erase(5) == 1);
  assert( bm_dif.erase(5) == 0);

  mpv::bmap<int,int> bm_eq;
  mpv::bmap<int,int>::iterator it_eq;
  assert(true == bm_eq.insert(100,100).second);
  assert(false == bm_eq.insert(100,100).second);
  assert(true == bm_eq.insert(150,200).second);  
  it_eq = bm_eq.find(150);
  assert(it_eq != bm_eq.end());
  it_eq = bm_eq.find(200);
  assert(it_eq != bm_eq.end());
  it_eq = bm_eq.find(2000);
  assert(it_eq == bm_eq.end());
  assert(bm_eq.erase(it_eq) == bm_eq.end()); 
  assert(bm_eq.erase(1050) == 0);

#ifdef TEST_THREAD
  std::cout<<"Start Test_THREAD\n";
  std::thread t1(test_insert);
  std::thread t2(test_erase);
  std::thread t3(test_find);

  t1.join();
  t2.join();
  t3.join();
  std::cout<<"End Test_THREAD\n";
#endif

  return 0;
};