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
			log::debug("Removing.... {} {}", this->m_levels->count(), this->m_itemCount);
			for (int i = 0; i < this->m_levels->count(); i++) {
				auto lvl = static_cast<GJGameLevel*>(this->m_levels->objectAtIndex(i));
				log::debug("{}: {} {}", i, lvl->m_levelName, lvl->m_levelFolder);
				if (lvl->m_levelFolder) {
					this->m_levels->removeObjectAtIndex(i);
					i--;
				}
			}
			// return LevelBrowserLayer::setupLevelBrowser(p0);
		}

		LevelBrowserLayer::setupLevelBrowser(p0);

		CCContentLayer* levelLayer = getLevelNodeList();
		if (!levelLayer) return;
		CCArray* levels = levelLayer->getChildren();

		for (int i = 0; i < levels->count(); i++) {
			LevelCell* levelObj = static_cast<LevelCell*>(levels->objectAtIndex(i));
			int lvlFolder = levelObj->m_level->m_levelFolder;

			auto nameSprite = levelObj->m_mainLayer->getChildByID("level-name");
			
			auto sprite = CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png");
			sprite->setID("folder"_spr);
			sprite->setAnchorPoint(CCPoint(0.0f, 0.5f));
			sprite->setScale(0.75f);
			sprite->setPosition(nameSprite->getPosition() + CCPoint(
				nameSprite->getContentSize().width * nameSprite->getScaleX() + 7.0f,
				-3.75f * nameSprite->getScaleY()
			));

			auto textSprite = CCLabelBMFont::create(
				numToString(lvlFolder, 0).c_str(),
				"bigFont.fnt"
			);
			textSprite->setAnchorPoint(CCPoint(0.5f, 0.5f));
			textSprite->setPosition(CCPoint(
				sprite->getContentSize().width / 2,
				sprite->getContentSize().height / 2
			));
			textSprite->setScale(0.55f);
			sprite->addChild(textSprite);

			levelObj->m_mainLayer->addChild(sprite);
		}

	};

};