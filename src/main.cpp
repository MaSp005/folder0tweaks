#include <Geode/modify/LevelBrowserLayer.hpp>

using namespace geode::prelude;

static void showArray(std::string title, CCArray* array) {
	log::debug("Array {} with length {}:", title, array->count());

	for (int i = 0; i < array->count(); i++) {
		GJGameLevel* object = static_cast<GJGameLevel*>(array->objectAtIndex(i));

		log::debug("{}: addr {}. Levelobj '{}' in folder {}", i, fmt::ptr(object), object->m_levelName, object->m_levelFolder);
	}
}

class $modify(MyLevelBrowserLayer, LevelBrowserLayer) {

	bool init(GJSearchObject* searchObject) {
		if (!LevelBrowserLayer::init(searchObject)) return false;

		if (searchObject->m_searchType != SearchType::MyLevels &&
			searchObject->m_searchType != SearchType::SavedLevels)
			return true;

		showArray("m_levels in init", this->m_levels);
		return true;
	}

	CCArray* getCellArray() {
		if (auto listView = this->m_list->m_listView) {
			if (auto tableView = listView->m_tableView) {
				if (auto content = tableView->m_cellArray) {
					return content;
				} else return nullptr;
			} else return nullptr;
		} else return nullptr;
	};

	CCArray* getLevelArray() {
		switch (m_searchObject->m_searchType) {
			case SearchType::MyLevels:
				return LocalLevelManager::get()->getCreatedLevels(m_searchObject->m_folder);
			case SearchType::SavedLevels:
				return GameLevelManager::get()->getSavedLevels(false, m_searchObject->m_folder);
			default:
				return nullptr;
		}
	};

	void setupLevelBrowser(CCArray* p0) {

		if (this->m_searchObject->m_searchType != SearchType::MyLevels &&
			this->m_searchObject->m_searchType != SearchType::SavedLevels ||
			this->m_searchObject->m_folder) return LevelBrowserLayer::setupLevelBrowser(p0);

		if (Mod::get()->getSettingValue<bool>("hideNon0")) {
			auto levelsSrc = getLevelArray();
			auto levels = CCArray::create();
			levels->addObjectsFromArray(levelsSrc);
			showArray("levels in setup", levels);

			if (typeinfo_cast<GJGameLevel*>(levels->objectAtIndex(0))) {
				log::debug("Removing.... {} {}", levels->count(), this->m_itemCount);

				for (int i = 0; i < levels->count(); i++) {
					auto obj = levels->objectAtIndex(i);
					auto lvl = static_cast<GJGameLevel*>(obj);
					if (lvl->m_levelFolder) {
						levels->removeObjectAtIndex(i);
						i--;
					}
				}

				m_levels = levels;
				m_itemCount = levels->count();
				m_pageEndIdx = std::min(m_itemCount - m_pageStartIdx, m_pageEndIdx);
				auto levelsSection = CCArray::create();
				for (int i = m_pageStartIdx; i < m_pageStartIdx + m_pageEndIdx && i < m_itemCount; i++) {
					levelsSection->addObject(levels->objectAtIndex(i));
				}

				auto pageStr = fmt::format("{} to {} of {}",
					m_pageStartIdx + 1, m_pageStartIdx + m_pageEndIdx, m_itemCount);
				this->m_countText->setString(pageStr.c_str());
				if (m_pageStartIdx + m_pageEndIdx >= m_itemCount) {
					m_rightArrow->setVisible(false);
				}

				showArray("levelsSection in setup hook", levelsSection);

				return LevelBrowserLayer::setupLevelBrowser(levelsSection);
			}
			else {
				log::warn("Removing levels failed, trying to show folders instead.");
			}
		}

		LevelBrowserLayer::setupLevelBrowser(p0);
		CCArray* levels = getCellArray();
		if (!levels) return;

		for (int i = 0; i < levels->count(); i++) {
			LevelCell* levelObj = static_cast<LevelCell*>(levels->objectAtIndex(i));
			int lvlFolder = levelObj->m_level->m_levelFolder;

			CCNode* alignNextTo;
			float scale;
			float yOffset;
			if (this->m_searchObject->m_searchType == SearchType::MyLevels) {
				alignNextTo = levelObj->m_mainLayer->getChildByID("level-name");
				bool hasRevision = levelObj->m_mainLayer->getChildByID("level-revision") != nullptr;
				scale = hasRevision ? 0.5f : 0.75f;
				yOffset = hasRevision ? 10.f : 0.f;
			} else {
				alignNextTo = levelObj->m_mainLayer->getChildByID("orbs-label");
				if (!alignNextTo)
					alignNextTo = levelObj->m_mainLayer->getChildByID("likes-label");
				scale = 0.5f;
				yOffset = 0.f;
			}

			if (!alignNextTo) {
				log::warn("Folder indicator on level {} failed.", levelObj->m_level->m_levelName);
				continue;
			}

			auto folderSprite = CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png");
			folderSprite->setID("folder"_spr);
			folderSprite->setAnchorPoint(CCPoint(0.f, 0.5f));
			folderSprite->setScale(scale);

			folderSprite->setPosition(alignNextTo->getPosition() + CCPoint(
				alignNextTo->getContentSize().width * alignNextTo->getScaleX() + 7.f,
				-3.75f * alignNextTo->getScaleY() + yOffset
			));

			auto textSprite = CCLabelBMFont::create(
				numToString(lvlFolder, 0).c_str(),
				"bigFont.fnt"
			);
			textSprite->setAnchorPoint({0.5f, 0.5f});
			textSprite->setPosition(folderSprite->getContentSize() / 2.f);
			textSprite->setScale(0.55f);
			folderSprite->addChild(textSprite);

			levelObj->m_mainLayer->addChild(folderSprite);
		}

	};

};