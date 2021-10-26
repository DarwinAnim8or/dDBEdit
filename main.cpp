#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>

#include "tinyxml2.h"

using namespace tinyxml2;

XMLDocument* doc;

enum class Option : unsigned int {
	invalid = 0,
	addEnviromentObject,
	addZone,
	addSmashable,
};

struct LastIDs {
	unsigned int objectsID;
	unsigned int physicsID;
	unsigned int renderID;
	unsigned int zoneID;
	unsigned int destructibleID;
};

Option ChooseOption() {
	std::cout << "Please select an option: " << std::endl;
	std::cout << "1) Add Enviromental object" << std::endl;
	std::cout << "2) Add Zone" << std::endl;
	std::cout << "3) Add Smashable" << std::endl;

	std::string in;
	std::getline(std::cin, in);
	int chosen = std::stoi(in);
	Option choice = Option::invalid;

	if (chosen > 3) {
		choice = ChooseOption();
	}

	choice = Option(chosen);
	return choice;
}

XMLElement* GetTable(const std::string& tableName) {
	auto root = doc->RootElement();
	auto current = root->FirstChildElement();

	while (current != nullptr) {
		const char* name;
		current->QueryStringAttribute("name", &name);
		std::string sName = name;

		if (sName == tableName) return current;

		current = current->NextSiblingElement();
	}

	return nullptr;
}

LastIDs GetLastIDs() {
	LastIDs toReturn;
	std::ifstream file;
	file.open("./lastIDs.txt");

	if (!file) {
		std::cout << "Couldn't find lastIDs.txt!" << std::endl;
		return LastIDs();
	}

	std::string line;
	int lineCounter = 0;
	while (std::getline(file, line)) {
		switch (lineCounter) {
		case 0:
			toReturn.objectsID = std::stoi(line);
			break;

		case 1:
			toReturn.physicsID = std::stoi(line);
			break;

		case 2:
			toReturn.renderID = std::stoi(line);
			break;

		case 3:
			toReturn.zoneID = std::stoi(line);
			break;

		case 4:
			toReturn.destructibleID = std::stoi(line);
			break;
		}

		lineCounter++;
	}

	file.close();

	if (toReturn.destructibleID == 0) toReturn.destructibleID = 2240;
	return toReturn;
}

LastIDs lastIDs = LastIDs();

std::string GetStringInput(const std::string& message) {
	std::cout << message << std::endl;
	std::string toReturn;
	std::getline(std::cin, toReturn);

	return toReturn;
}

void AddEnviromentObject() {
	//Get our objects table:
	auto root = doc->RootElement();
	auto objects = GetTable("Objects");
	auto componentsRegistry = GetTable("ComponentsRegistry");
	auto physicsComponent = GetTable("PhysicsComponent");
	auto renderComponent = GetTable("RenderComponent");

	if (objects && componentsRegistry && physicsComponent && renderComponent) {
		//First, insert into objects:
		//TODO: get the last id
		lastIDs.objectsID++;

		auto object = doc->NewElement("row");
		std::string name = "DLU - ";
		std::string desc;

		name += GetStringInput("Please enter object name: ");
		//desc = GetStringInput("Please enter object description (can be blank): ");

		object->SetAttribute("id", lastIDs.objectsID);
		object->SetAttribute("localize", "0");
		object->SetAttribute("name", name.c_str());
		object->SetAttribute("nametag", "0");
		object->SetAttribute("placeable", "1");
		object->SetAttribute("type", "Environmental");

		objects->LastChild()->LinkEndChild(object);

		//Now, we insert into physics:
		auto phys = doc->NewElement("row");
		lastIDs.physicsID++;
		std::string physicsAsset = GetStringInput("Please enter path to HKX file (sample: env\\env_won_yore_creator-building.hkx): ");

		phys->SetAttribute("airSpeed", "5");
		phys->SetAttribute("collisionGroup", "1");
		phys->SetAttribute("doublejump", "0");
		phys->SetAttribute("id", lastIDs.physicsID);
		phys->SetAttribute("jump", "0");
		phys->SetAttribute("jumpAirSpeed", "25");
		phys->SetAttribute("pcShapeType", "0");
		phys->SetAttribute("physics_asset", physicsAsset.c_str());
		phys->SetAttribute("static", "1");

		physicsComponent->LastChild()->LinkEndChild(phys);

		//Insert render:
		auto render = doc->NewElement("row");
		lastIDs.renderID++;
		std::string renderAsset = GetStringInput("Please enter path to NIF file (sample mesh\\env\\env_won_yore_creator-building.nif): ");

		render->SetAttribute("fade", "0");
		render->SetAttribute("fadeInTime", "1");
		render->SetAttribute("gradualSnap", "0");
		render->SetAttribute("id", lastIDs.renderID);
		render->SetAttribute("ignoreCameraCollision", "0");
		render->SetAttribute("maxShadowDistance", "0");
		render->SetAttribute("preloadAnimations", "0");
		render->SetAttribute("render_asset", renderAsset.c_str());
		render->SetAttribute("shader_id", "1");
		render->SetAttribute("usedropshadow", "0");

		renderComponent->LastChild()->LinkEndChild(render);

		//Insert into ComponentsRegistry:
		auto forRender = doc->NewElement("row");
		auto forPhys = doc->NewElement("row");

		forRender->SetAttribute("component_id", lastIDs.renderID);
		forRender->SetAttribute("component_type", "2");
		forRender->SetAttribute("id", lastIDs.objectsID);

		forPhys->SetAttribute("component_id", lastIDs.physicsID);
		forPhys->SetAttribute("component_type", "3");
		forPhys->SetAttribute("id", lastIDs.objectsID);

		componentsRegistry->LastChild()->LinkEndChild(forRender);
		componentsRegistry->LastChild()->LinkEndChild(forPhys);
	}
}

void AddZone() {
	auto zoneTable = GetTable("ZoneTable");

	auto zone = doc->NewElement("row");

	lastIDs.zoneID++;
	zone->SetAttribute("ghostdistance", "200");
	zone->SetAttribute("population_hard_cap", "120");
	zone->SetAttribute("population_soft_cap", "80");
	zone->SetAttribute("scriptID", "-1");
	zone->SetAttribute("zoneID", lastIDs.zoneID);
	zone->SetAttribute("zoneName", GetStringInput("Enter path to zone (sample: Test\\lauren\\test_lauren.luz):").c_str());

	zoneTable->LastChild()->LinkEndChild(zone);
}

void AddSmashable() {
	auto root = doc->RootElement();
	auto objects = GetTable("Objects");
	auto componentsRegistry = GetTable("ComponentsRegistry");
	auto physicsComponent = GetTable("PhysicsComponent");
	auto renderComponent = GetTable("RenderComponent");
	auto destructibleComponent = GetTable("DestructibleComponent");

	if (objects && componentsRegistry && physicsComponent && renderComponent) {
		//First, insert into objects:
		//TODO: get the last id
		lastIDs.objectsID++;

		auto object = doc->NewElement("row");
		std::string name = "DLU - ";
		std::string desc;

		name += GetStringInput("Please enter object name: ");

		object->SetAttribute("id", lastIDs.objectsID);
		object->SetAttribute("localize", "0");
		object->SetAttribute("name", name.c_str());
		object->SetAttribute("nametag", "0");
		object->SetAttribute("placeable", "1");
		object->SetAttribute("type", "Smashables");

		objects->LastChild()->LinkEndChild(object);

		//Now, we insert into physics:
		auto phys = doc->NewElement("row");
		lastIDs.physicsID++;
		std::string physicsAsset = GetStringInput("Please enter path to HKX file (sample: env\\env_won_yore_creator-building.hkx): ");

		phys->SetAttribute("airSpeed", "5");
		phys->SetAttribute("collisionGroup", "1");
		phys->SetAttribute("doublejump", "0");
		phys->SetAttribute("id", lastIDs.physicsID);
		phys->SetAttribute("jump", "0");
		phys->SetAttribute("jumpAirSpeed", "25");
		phys->SetAttribute("pcShapeType", "0");
		phys->SetAttribute("physics_asset", physicsAsset.c_str());
		phys->SetAttribute("static", "1");

		physicsComponent->LastChild()->LinkEndChild(phys);

		//Insert render:
		auto render = doc->NewElement("row");
		lastIDs.renderID++;
		std::string renderAsset = GetStringInput("Please enter path to NIF file (sample mesh\\env\\env_won_yore_creator-building.nif): ");

		render->SetAttribute("fade", "0");
		render->SetAttribute("fadeInTime", "1");
		render->SetAttribute("gradualSnap", "0");
		render->SetAttribute("id", lastIDs.renderID);
		render->SetAttribute("ignoreCameraCollision", "0");
		render->SetAttribute("maxShadowDistance", "0");
		render->SetAttribute("preloadAnimations", "0");
		render->SetAttribute("render_asset", renderAsset.c_str());
		render->SetAttribute("shader_id", "1");
		render->SetAttribute("usedropshadow", "0");

		renderComponent->LastChild()->LinkEndChild(render);

		//Insert into DestructibleComponent:
		/*lastIDs.destructibleID++;
		auto dest = doc->NewElement("row");

		dest->SetAttribute("CurrencyIndex", "1");
		dest->SetAttribute("LootMatrixIndex", "227");
		dest->SetAttribute("armor", "0");
		dest->SetAttribute("attack_priority", "10");
		dest->SetAttribute("death_behavior", "2");
		dest->SetAttribute("faction", "6");
		dest->SetAttribute("id", lastIDs.destructibleID);
		dest->SetAttribute("imagination", "0");
		dest->SetAttribute("isSmashable", "1");
		dest->SetAttribute("isnpc", "0");
		dest->SetAttribute("level", "1");
		dest->SetAttribute("life", "1");

		destructibleComponent->LastChild()->LinkEndChild(dest);*/

		//Insert into ComponentsRegistry:
		auto forRender = doc->NewElement("row");
		auto forPhys = doc->NewElement("row");
		auto forDest = doc->NewElement("row");

		forRender->SetAttribute("component_id", lastIDs.renderID);
		forRender->SetAttribute("component_type", "2");
		forRender->SetAttribute("id", lastIDs.objectsID);

		forPhys->SetAttribute("component_id", lastIDs.physicsID);
		forPhys->SetAttribute("component_type", "3");
		forPhys->SetAttribute("id", lastIDs.objectsID);

		forDest->SetAttribute("component_id", "791");
		forDest->SetAttribute("component_type", "7");
		forDest->SetAttribute("id", lastIDs.objectsID);

		componentsRegistry->LastChild()->LinkEndChild(forRender);
		componentsRegistry->LastChild()->LinkEndChild(forPhys);
		componentsRegistry->LastChild()->LinkEndChild(forDest);
	}
}

void main(int argc, char** argv) {
	std::cout << "dDBEdit - v1.0" << std::endl;
	std::cout << "Compiled on: " << __TIMESTAMP__ << std::endl << std::endl;

	lastIDs = GetLastIDs();

	doc = new XMLDocument();
	auto error = doc->LoadFile("./lupdata.xml");

	if (error != XML_SUCCESS) {
		std::cout << "Got an error while loading xml: " << error << std::endl;
		return;
	}

	doc->SaveFile("./lupdata-backup.xml");
	Option choice = ChooseOption();
	
	switch (choice) {
	case Option::addEnviromentObject:
		AddEnviromentObject();
		break;

	case Option::addZone:
		AddZone();
		break;

	case Option::addSmashable:
		AddSmashable();
		break;

	default:
		std::cout << "Unknown option!" << std::endl;
	}

	doc->SaveFile("./lupdata.xml");

	std::ofstream fids;
	fids.open("./lastIDs.txt");

	fids << lastIDs.objectsID << std::endl;
	fids << lastIDs.physicsID << std::endl;
	fids << lastIDs.renderID << std::endl;
	fids << lastIDs.zoneID << std::endl;
	fids << lastIDs.destructibleID << std::endl;

	fids.close();
}