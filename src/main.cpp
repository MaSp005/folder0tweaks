#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/LevelBrowserLayer.hpp>
class $modify(MyLevelBrowserLayer, LevelBrowserLayer) {

	CCContentLayer* getLevelNodeList() {
		if (auto listView = this->m_list->getChildByID("list-view")) {
			if (auto tableView = listView->getChildByType<TableView>(0)) {
				if (auto content = tableView->getChildByType<CCContentLayer>(0)) {
					return content;
				}
				else return nullptr;
			}
			else return nullptr;
		}
		else return nullptr;
	};

	void setupLevelBrowser(CCArray* p0) {

		if (this->m_searchObject->m_searchType != SearchType::MyLevels ||
			this->m_searchObject->m_folder) return LevelBrowserLayer::setupLevelBrowser(p0);

		if (Mod::get()->getSettingValue<bool>("hideNon0")) {
			try {
				log::debug("Removing.... {} {}", this->m_levels->count(), this->m_itemCount);
				for (int i = 0; i < this->m_levels->count(); i++) {
					auto obj = this->m_levels->objectAtIndex(i);
					auto lvl = static_cast<GJGameLevel*>(obj);
					if (lvl->m_levelFolder) {
						this->m_levels->removeObjectAtIndex(i);
						i--;
					}
				}
				return LevelBrowserLayer::setupLevelBrowser(p0);
			}
			catch (...) {
				log::warn("Removing levels failed, trying to show folders instead.");
			}
		}

		LevelBrowserLayer::setupLevelBrowser(p0);

		CCContentLayer* levelLayer = getLevelNodeList();
		if (!levelLayer) return;
		CCArray* levels = levelLayer->getChildren();

		for (int i = 0; i < levels->count(); i++) {
			LevelCell* levelObj = static_cast<LevelCell*>(levels->objectAtIndex(i));
			try {
				int lvlFolder = levelObj->m_level->m_levelFolder;

				auto nameSprite = levelObj->m_mainLayer->getChildByID("level-name");
				auto hasRevision = levelObj->m_mainLayer->getChildByID("level-revision") != nullptr;

				auto folderSprite = CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png");
				folderSprite->setID("folder"_spr);
				folderSprite->setAnchorPoint(CCPoint(0.f, 0.5f));
				folderSprite->setScale(hasRevision ? 0.5f : 0.75f);
				folderSprite->setPosition(nameSprite->getPosition() + CCPoint(
					nameSprite->getContentSize().width * nameSprite->getScaleX() + 7.f,
					-3.75f * nameSprite->getScaleY() + (hasRevision ? 10.f : 0.f)
				));

				auto textSprite = CCLabelBMFont::create(
					numToString(lvlFolder, 0).c_str(),
					"bigFont.fnt"
				);
				textSprite->setAnchorPoint(CCPoint(0.5f, 0.5f));
				textSprite->setPosition(CCPoint(
					folderSprite->getContentSize().width / 2,
					folderSprite->getContentSize().height / 2
				));
				textSprite->setScale(0.55f);
				folderSprite->addChild(textSprite);

				levelObj->m_mainLayer->addChild(folderSprite);
			} catch (...) {
				log::warn("Folder indicator on level {} failed.", levelObj->m_level->m_levelName);
			}
		}

	};

};