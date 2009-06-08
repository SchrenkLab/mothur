#ifndef HEATMAPSIM_H
#define HEATMAPSIM_H
/*
 *  heatmapsim.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 6/8/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */


#include "sharedrabundvector.h"
#include "datavector.hpp"
#include "globaldata.hpp"
#include "calculator.h"

/***********************************************************************/

class HeatMapSim {
	
	public:
		HeatMapSim();
		~HeatMapSim(){};
	
		void getPic(vector<SharedRAbundVector*>, vector<Calculator*>);

	private:
		void printLegend(int, float);

		GlobalData* globaldata;
		string format, groupComb;
		ofstream outsvg;
			
};

/***********************************************************************/

#endif

