#include "CreativeScreen.hpp"
#include "world/item/CreativeMenu.hpp"
#include "world/item/SimpleContainer.hpp"
#include "client/gui/screens/inventory/InventoryScreen.hpp"


Container* CreativeScreen::creativeGrid = new SimpleContainer(72, "Item selection");

CreativeScreen::CreativeScreen(Container* inventory)
    : ContainerScreen(new CreativeMenu(inventory, creativeGrid))
{
    m_imageHeight = 208;
}

void CreativeScreen::mouseClicked(int x, int y, int button)
{
    ContainerScreen::mouseClicked(x, y, button);

    if (x >= m_leftPos + 155 && x < m_leftPos + 169 && y >= m_topPos + 17 && y < m_topPos + 179) m_bIsScrolling = true;
}

void CreativeScreen::mouseScrolled(double x, double y, int scroll)
{
	CreativeMenu* creativeMenu = (CreativeMenu*)menu;
	creativeMenu->updateScroll(m_scrolled = Mth::clamp(m_scrolled - (float(scroll) / (creativeMenu->creativeItems.size() / 8 - 8 + 1)), 0.0f, 1.0f));
}

void CreativeScreen::mouseReleased(int x, int y, int button)
{
    m_bIsScrolling = false;
}

void CreativeScreen::mouseDragged(double x, double y, int button, double deltaX, double deltaY)
{
    ContainerScreen::mouseDragged(x, y, button, deltaX, deltaY);
    if (m_bIsScrolling) 
    {
        CreativeMenu* creativeMenu = (CreativeMenu*)menu;
        creativeMenu->updateScroll(m_scrolled = Mth::clamp((y - (m_topPos + 25)) / 144.0f, 0.0f, 1.0f));
    }
}

void CreativeScreen::slotClicked(Slot* slot, int index, int button, bool quick)
{
	Inventory* var5 = m_pMinecraft->m_pLocalPlayer->m_pInventory;
	std::shared_ptr<ItemInstance> var6 = nullptr;
	if (slot) {
		if (slot->container == creativeGrid) {
			var6 = var5->getCarried();
			auto var7 = slot->getItem();
			if (var6 && var7 && var6->m_itemID == var7->m_itemID) {
				if (!button) {
					if (quick) {
						var6->m_count = var6->getMaxStackSize();
					}
					else if (var6->m_count < var6->getMaxStackSize()) {
						++var6->m_count;
					}
				}
				else if (var6->m_count <= 1) {
					var5->setCarried(nullptr);
				}
				else {
					--var6->m_count;
				}
			}
			else if (var6) {
				var5->setCarried(nullptr);
			}
			else if (!var7) {
				var5->setCarried(nullptr);
			}
			else if (!var6 || var6->m_itemID != var7->m_itemID) {
				var5->setCarried(var7->copy());
				var6 = var5->getCarried();
				if (quick) {
					var6->m_count = var6->getMaxStackSize();
				}
			}
		}
		else {
			menu->clicked(slot->index, button, m_pMinecraft->m_pLocalPlayer.get());
			auto var8 = menu->getSlot(slot->index)->getItem();
			//@TODO Sync if online
		}
	}
	else {
		if (var5->getCarried()) {
			if (!button) {
				m_pMinecraft->m_pLocalPlayer->drop(var5->getCarried().get());
				//@TODO Sync if online
				var5->setCarried(nullptr);
			}

			if (button == 1) {
				var6 = var5->getCarried()->remove(1);
				m_pMinecraft->m_pLocalPlayer->drop(var6.get());
				//@TODO Sync if online
				if (!var5->getCarried()->m_count) {
					var5->setCarried(nullptr);
				}
			}
		}
	}
}

void CreativeScreen::tick()
{
	if (m_pMinecraft->m_pLocalPlayer->isSurvival())
	{
		m_pMinecraft->setScreen(new InventoryScreen(m_pMinecraft->m_pLocalPlayer.get()));
	}
}

void CreativeScreen::renderLabels() {
    m_pFont->draw(creativeGrid->getName(), 8, 6, 0x404040);
}

void CreativeScreen::renderBg(int mouseX, int mouseY, float partialTicks) {
    m_pMinecraft->m_pTextures->loadAndBindTexture("gui/allitems.png");
    glColor4f(1.0F, 1.0F, 1.0F, 1.0F);

    blit(m_leftPos, m_topPos, 0, 0, m_imageWidth, m_imageHeight, 256, 256);
    blit(m_leftPos + 154, m_topPos + 17 + (int)(145 * m_scrolled), 0, 208, 16, 16, 256, 256);
}
