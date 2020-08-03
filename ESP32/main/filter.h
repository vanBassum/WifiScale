/*
 * Filter.h
 *
 *  Created on: 3 Aug 2020
 *      Author: Bas
 */

#ifndef MAIN_FILTER_H_
#define MAIN_FILTER_H_


template <class T>
class Filter
{
public:
  int N = 0;                                                                  // Aantal te middelen waarde
  T *meetwaardes;                                                             // Opslag individuele meetwaarden
  T accumulator = 0;                                                          // Optelsom van alle individuele meetwaarden
  int index = 0;                                                              // Meetwaarden teller

  Filter(const int n)
  {
    meetwaardes = new T[n];
    N = n;
    for(int i = 0; i < n; i++)
    {
      meetwaardes[i] = 0;
    }
  }

  ~Filter()
  {
    delete[] meetwaardes;
  }

  void AddSample(T NewSample)
  {
    accumulator -= meetwaardes[index];
    meetwaardes[index] = NewSample;                                          // Bewaar nieuwe meetwaarde
    accumulator += meetwaardes[index];                                       // Accumuleer nieuwste meetwaarde
    index++;
    index %= N;                                                              // Teller loopt van 0 tot N-1
  }

  T Average()
  {
    return accumulator/N;                                                    // Bereken de gemmiddelde waarde
  }
};


#endif /* MAIN_FILTER_H_ */
