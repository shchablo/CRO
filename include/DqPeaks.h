#ifndef DQPEAKS_h
#define DQPEAKS_h

#include <vector>
#include <math.h>
#include <algorithm>

class DqPeaks 
{
	
	public:
	
  /* Constructor, destructor. */ 
  DqPeaks() {};
  virtual ~DqPeaks() {};
	
	std::vector<int> findPedestal(std::vector<int> *data, std::vector<int> ref, int window, int thr, int offset) 
	{
		std::vector<int> ped;
		std::vector<int>::const_iterator first; 
		std::vector<int>::const_iterator last; 
  	int index = offset;
		int limit = window; 
  	double max = 0;  double min = 0;
		while(index > limit) {
      first = data->begin() + index - window;
			last = data->begin() + index;
			ped.insert(ped.end(), first, last);
  	 	max = *std::max_element(ped.begin(), ped.end()); 
  	 	min = *std::min_element(ped.begin(), ped.end()); 
			if(max - min >= thr) ped.clear();
      else return ped;
			index -= int(window/10);
		}
  	return ref;
	};
	int findLevel(std::vector<int> *ped)
	{ 
  	int average = 0; 
		auto n = ped->size(); 
		if(n != 0) 
			average = int(std::round(std::accumulate(ped->begin(), ped->end(), 0.0) / n));
		return average; 
	};
	std::vector<int> findPeaks(std::vector<int> *data, int level, int pol, int thr)
	{
		std::vector<int> peaks; 
		int tick = 0; std::vector<int> tmp; bool isFind = false; int offset = 0;
		for(unsigned int i = 0; i  < data->size(); i++) {
			tick = pol*(data->at(i) - level);
			if(!isFind && tmp.size() > 0) tmp.clear(); 
			if(tick > thr) {
				if(isFind == false) { 
					isFind = true;
					offset = i;
				}
				tmp.push_back(tick);
        continue;
			}
			else if(tmp.size() > 0) {
				isFind = false;
  	 		int max = std::distance(tmp.begin(), std::max_element(tmp.begin(), tmp.end()));
				peaks.push_back(max + offset); 
			}
		}
		if(tmp.size() > 0 ) {
  		int max = std::distance(tmp.begin(), std::max_element(tmp.begin(), tmp.end()));
			peaks.push_back(max + offset); 
		}
		return peaks;
	};
	
  std::vector<int> findExtraPeaks(std::vector<int> *data, int level, int pol, int thr)
  {
		std::vector<int> peaks;
		//int nums[] = { 1, 2, 3, 3, 2, 4, 1, 5, 6, 3, 1, 10, 2, 8 };
  	//int peaks[sizeof(nums)/sizeof(nums[0])];
  	for (int i = 1; i < data->size() - 1; i++) {
  	  if (
  	      // (i == 0 && data->at(i + 1) < data->at(i)) ||   /* first element */
  	      // (i == data->size() && data->at(i - 1) < data->at(i)) || /* last element */
  	      (data->at(i) > data->at(i - 1) && data->at(i) > data->at(i + 1) && ( data->at(i - 1) - data->at(i) > thr )) /* elements in the middle */
  	    )
  	  {
  	    peaks.push_back(data->at(i));
  	  }
  	}

  	printf("%d ", peaks.size());
  	//for (int i = 0; i < peaks.size(); i++)
  	//  printf("%d ", peaks[i]);
		return peaks;
	}
};
	
#endif
