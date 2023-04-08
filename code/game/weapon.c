#include "../common.h"

#include "entity.h"
#include "weapon.h"

void equipWeapon(Weapon * weapon, Actor *equipper)
{

}

void updateWeapon(Entity *e)
{
    Weapon *weapon = (Weapon *)e;
    SimulationRegion simRegion = getSimulationRegion();
    //if not equipped, check if someone is trying to equip us.
    if(weapon->equipper == NULL)
    {
        
    }
    //if equipped, then handle weapon swinging.
}
