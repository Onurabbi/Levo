#include "../common.h"

#include "weapon.h"

void equipWeapon(Weapon * weapon, Actor *equipper)
{
    Entity * equippingEntity = equipper->owner;
    if(equippingEntity)
    {
        weapon->equipper = equipper;
        equipper->weapon = weapon;
    }
}