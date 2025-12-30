#include "ContainerListener.hpp"
#include "ContainerMenu.hpp"

void ContainerListener::refreshContainerItems(ContainerMenu* menu)
{
	refreshContainer(menu, menu->getItems());
}
