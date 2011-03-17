#include <iostream>
#include <map>
#include "md2model.h"

using namespace std;

#ifndef MODEL_MANAGER_H_
#define MODEL_MANAGER_H_

#define MODEL_PATH	"../models/"


class ModelManager	{

	int modelCount;
	map<string, MD2Model*> models;

public:
	ModelManager()	{
		modelCount = 0;
	}

	~ModelManager()	{
		unloadAllModels();
	}

	void unloadAllModels()	{
		map<string, MD2Model*>::iterator itr;
		
		for(itr = models.begin(); itr != models.end(); itr++)	{
			cout << "Releasing model " << itr->first << "...";
			MD2Model* md2 = itr->second;
			models.erase(itr);
			delete (md2);
			cout << endl;
		}

		modelCount = 0;
	}

//	MD2Model* getMD2Model(string modelName)	{
//		MD2Model* md2 = models[modelName];
//		return md2;
//	}

	MD2Model* loadMD2Model(string modelName)	{
		MD2Model* md2;
		md2 = MD2Model::load(modelName.c_str());

		if( md2 )	{
			models[modelName] = md2;
			modelCount++;
			return md2;
		}
		
		cout << "Couldn't load MD2Model " << modelName << endl;
		return NULL;
	}

	bool removeMD2Model(string modelName)	{
		MD2Model* md2 = models[modelName];

		if( md2 )	{
			delete(md2);
			modelCount--;
			return true;
		}
		
		cout << "Couldn't remove MD2Model " << modelName << " not found." << endl;
		return false;
	}

	void drawMD2Model(string modelName, float advAmount)	{
		models[modelName]->advance(advAmount);
		models[modelName]->draw();
	}

	void setMD2ModelAnimation(string modelName, string animName)	{
		models[modelName]->setAnimation(animName.c_str());
	}

};

#endif


