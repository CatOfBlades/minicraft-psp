/*
 * Anvil.h
 *
 *  Created on: 27 ���. 2013 �.
 *      Author: user
 */

#ifndef ANVIL_H_
#define ANVIL_H_

#include "Furniture.h"

class Player;

class Anvil : public Furniture  {
public:
	Anvil();
	virtual ~Anvil();

	bool use(Player * player, int attackDir);
};

#endif /* ANVIL_H_ */
