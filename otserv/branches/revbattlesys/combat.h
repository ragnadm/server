//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////////////

#ifndef __OTSERV_COMBAT_H__
#define __OTSERV_COMBAT_H__

#include "thing.h"
#include "definitions.h"
#include "enums.h"
#include "map.h"
#include "baseevents.h"

#include <vector>

class Condition;
class Creature;
class Position;
class Item;

//for luascript callback
class CombatCallBack : public CallBack{
public:
	void getMinMaxValues(Player* player, int32_t& min, int32_t& max) const;
};

struct CombatParams{
	CombatParams() {
		damageType = DAMAGE_NONE;
		blockedByArmor = false;
		blockedByShield = false;
		targetCasterOrTopMost = false;
		impactEffect = NM_ME_NONE;
		distanceEffect = NM_ME_NONE;
	}

	DamageType_t damageType;
	bool blockedByArmor;
	bool blockedByShield;
	bool targetCasterOrTopMost;

	uint8_t impactEffect;
	uint8_t distanceEffect;
};

class Combat{
public:
	Combat(CombatType_t _type);
	~Combat();

	static bool doCombatHealth(Creature* caster, Creature* target, 
		int32_t minChange, int32_t maxChange, const CombatParams& params);
	static void doCombatHealth(Creature* caster, Creature* target,
		int32_t minChange, int32_t maxChange, const CombatParams& params,
		const Condition* condition);
	static void doCombatHealth(Creature* caster, const Position& pos,
		const AreaCombat* area, int32_t minChange, int32_t maxChange, const CombatParams& params,
		const Condition* condition);

	static bool doCombatMana(Creature* caster, Creature* target,
		int32_t minChange, int32_t maxChange, const CombatParams& params);
	static void doCombatMana(Creature* caster, Creature* target,
		int32_t minChange, int32_t maxChange, const CombatParams& params,
		const Condition* condition);
	static void doCombatMana(Creature* caster, const Position& pos,
		const AreaCombat* area, int32_t minChange, int32_t maxChange, const CombatParams& params,
		const Condition* condition);

	static bool doCombatCondition(Creature* caster, Creature* target,
		const Condition* condition);
	static void doCombatCondition(Creature* caster, Creature* target,
		const Condition* condition, const CombatParams& params);
	static void doCombatCondition(Creature* caster, const Position& pos,
		const AreaCombat* area, const Condition* condition, const CombatParams& params);

	static void getCombatArea(const Position& centerPos, const Position& targetPos,
		const AreaCombat* area, std::list<Tile*>& list);
	static bool canDoCombat(const Creature* caster, const Tile* tile);

	void doCombat(Creature* caster, Creature* target) const;
	void doCombat(Creature* caster, const Position& pos) const;

	bool setCallback(CombatParam_t key);
	CallBack* getCallback();

	virtual bool setParam(CombatParam_t param, uint32_t value);
	void setArea(const AreaCombat* _area);
	void setCondition(const Condition* _condition);

protected:
	void getMinMaxValues(Creature* creature, int32_t& min, int32_t& max) const;

	//configureable
	CombatType_t combatType;

	CombatParams params;

	AreaCombat* area;
	Condition* condition;
	CombatCallBack* callback;
};

/*
class MagicField;

class CombatField : public Combat{
public:
	CombatField(MagicField* _field);
	~CombatField();

	virtual CombatField* getCombatField() {return this;};
	virtual const CombatField* getCombatField() const {return this;};
	
	void doCombat(Creature* attacker, Creature* target) const;
	void doCombat(Creature* attacker, const Position& pos) const;

protected:
	MagicField* field;
};
*/

template <typename T>
class Matrix
{
public:
	//Matrix() {}
  Matrix(uint32_t _rows, uint32_t _cols)
  {
		centerX = 0;
		centerY = 0;

		rows = _rows;
		cols = _cols;

		data_ = new T*[rows];

		for(uint32_t row = 0; row < rows; ++row){
			data_[row] = new T[cols];
			
			for(uint32_t col = 0; col < cols; ++col){
				data_[row][col] = 0;
			}
		}
  }

	~Matrix()
	{
		for(uint32_t row = 0; row < rows; ++row){
			delete[] data_[row];
		}

		delete[] data_;
	}

	void setValue(uint32_t row, uint32_t col, T value) const {data_[row][col] = value;}
	T getValue(uint32_t row, uint32_t col) const {return data_[row][col];}

	void setCenter(uint32_t y, uint32_t x) {centerX = x; centerY = y;}
	void getCenter(uint32_t& y, uint32_t& x) const {x = centerX; y = centerY;}

	size_t getRows() const {return rows;}
	size_t getCols() const {return cols;}

	inline const T* & operator[](uint32_t i) const { return data_[i]; }
	inline T* & operator[](uint32_t i) { return data_[i]; }

private:
	uint32_t centerX;
	uint32_t centerY;

	uint32_t rows;
	uint32_t cols;
	T** data_;
};

typedef Matrix<bool> MatrixArea;
typedef std::map<Direction, MatrixArea* > AreaCombatMap;

class AreaCombat{
public:
	AreaCombat();
	~AreaCombat();

	ReturnValue doCombat(Creature* attacker, const Position& pos, const Combat& combat) const;
	bool getList(const Position& centerPos, const Position& targetPos, std::list<Tile*>& list) const;

	void setupArea(const std::list<uint32_t>& list, uint32_t rows);
	void setupExtArea(const std::list<uint32_t>& list, uint32_t rows);
	void clear();

protected:
	enum MatrixOperation_t{
		MATRIXOPERATION_COPY,
		MATRIXOPERATION_MIRROR,
		MATRIXOPERATION_FLIP,
		MATRIXOPERATION_ROTATE90,
		MATRIXOPERATION_ROTATE180,
		MATRIXOPERATION_ROTATER90
	};

	MatrixArea* createArea(const std::list<uint32_t>& list, uint32_t rows);
	void copyArea(const MatrixArea* input, MatrixArea* output, MatrixOperation_t op) const;

	MatrixArea* getArea(const Position& centerPos, const Position& targetPos) const
	{
		int32_t dx = targetPos.x - centerPos.x;
		int32_t dy = targetPos.y - centerPos.y;

		Direction dir = NORTH;

		if(dx < 0){
			dir = WEST;
		}
		else if(dx > 0){
			dir = EAST;
		}
		else if(dy < 0){
			dir = NORTH;
		}
		else{
			dir = SOUTH;
		}

		if(hasExtArea){
			if(dx < 0 && dy < 0)
				dir = NORTHWEST;
			else if(dx > 0 && dy < 0)
				dir = NORTHEAST;
			else if(dx < 0 && dy > 0)
				dir = SOUTHWEST;
			else if(dx > 0 && dy > 0)
				dir = SOUTHEAST;
		}

		AreaCombatMap::const_iterator it = areas.find(dir);
		if(it != areas.end()){
			return it->second;
		}
		
		return NULL;
	}

	AreaCombatMap areas;
	bool hasExtArea;
};

class MagicField : public Item{
public:
	MagicField(uint16_t _type);
	~MagicField();

	virtual MagicField* getMagicField() {return this;};
	virtual const MagicField* getMagicField() const {return this;};

	DamageType_t getDamageType() const;
	const Condition* getCondition() const { return condition; }
	void setCondition(Condition* _condition) { condition = _condition; }

protected:
	Condition* condition;
	DamageType_t damageType;
};

#endif

