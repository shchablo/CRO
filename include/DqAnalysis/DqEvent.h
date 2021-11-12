#ifndef __DQEVENT_H__
#define __DQEVENT_H__

#include <vector>

struct compare
{
    int key;
    compare(int const &i): key(i) {}
 
    bool operator()(int const &i) {
        return (i == key);
    }
};

class DqEvent
{
	
	public:
	
  /* Constructor, destructor. */ 
  DqEvent() {};
  virtual ~DqEvent() {};

  void coherent() 
  {
    for(int i = 0; i < _data.size()/32; i++) {
      std::vector<double> avg(10000, 0); 
      for(int j = 0; j < 32; j++) {
        for(int s = 0; s < _data[i*32+j].size(); s++) {
          avg[s] += _data[i*32+j][s];
        }
      }
      for(int j = 0; j < 32; j++) {
        double min = 10000000; int shift = 10; double prime = 10; int correction = 0; int tcount = 0;
        for(int m = -10; m < shift; m++) {
          double sum = 0; int count = 0;
          for(int s = 0; s < avg.size(); s++) {
            double jdiff = std::abs(avg[s]/32 - (_data[i*32+j][s] + m));
            if(jdiff < prime) {
              sum += jdiff;
              count += 1;
            }
          }
          if(min > sum) {
            min = sum;
            correction = m;
            tcount = count;
          }
        }
        std::cout << "Corr: " << correction << " Sum: " << min << " Count:" << tcount << std::endl;
      } 
    }
  };

	std::vector<double> getData(int view) {
 
    auto itr = std::find_if(_view.cbegin(), _view.cend(), compare(view));
    if(itr != _view.cend()) {
      int index = std::distance(_view.cbegin(), itr);
      return _data[index];   
    }
    else {
      std::vector<double> empty;
      return empty;   
    } 
  };

	void fill(int ch, int view, std::vector<double> *data) {
    _ch.push_back(ch);
    _view.push_back(view);
    _data.push_back(*data);
    _processed.push_back(*data);
  };
 
	void clear() {
    _ch.clear();
    _view.clear();
    _data.clear();
    _processed.clear();
  };
	
  private:

    std::vector<int> _ch; 
    std::vector<int> _view; 
    std::vector<std::vector<double>> _data;
    std::vector<std::vector<double>> _processed;
   
};
#endif
