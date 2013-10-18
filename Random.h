/*
 * Random.h
 *
 *  Created on: 16 ���. 2013 �.
 *      Author: user
 */

#ifndef RANDOM_H_
#define RANDOM_H_

class Random {
public:
	Random();
	virtual ~Random();
	void setSeed(unsigned long);
	int nextInt(int);
	float nextFloat();
	double nextGaussian();
	bool nextBoolean();
};

#endif /* RANDOM_H_ */
