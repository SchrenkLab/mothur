/*
 *  phylodiversitycommand.cpp
 *  Mothur
 *
 *  Created by westcott on 4/30/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "phylodiversitycommand.h"

//**********************************************************************************************************************
PhyloDiversityCommand::PhyloDiversityCommand(string option)  {
	try {
		globaldata = GlobalData::getInstance();
		abort = false;
		
		//allow user to run help
		if(option == "help") { help(); abort = true; }
		
		else {
			//valid paramters for this command
			string Array[] =  {"freq","rarefy","iters","groups","summary","collect","scale","outputdir","inputdir"};
			vector<string> myArray (Array, Array+(sizeof(Array)/sizeof(string)));
			
			OptionParser parser(option);
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
		
			//check to make sure all parameters are valid for command
			for (map<string,string>::iterator it = parameters.begin(); it != parameters.end(); it++) { 
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {  abort = true;  }
			}
			
			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found"){	outputDir = m->hasPath(globaldata->getTreeFile());		}
			
			if (globaldata->gTree.size() == 0) {//no trees were read
				m->mothurOut("You must execute the read.tree command, before you may execute the phylo.diversity command."); m->mothurOutEndLine(); abort = true;  }

			string temp;
			temp = validParameter.validFile(parameters, "freq", false);			if (temp == "not found") { temp = "100"; }
			convert(temp, freq); 
			
			temp = validParameter.validFile(parameters, "iters", false);			if (temp == "not found") { temp = "1000"; }
			convert(temp, iters); 
			
			temp = validParameter.validFile(parameters, "rarefy", false);			if (temp == "not found") { temp = "F"; }
			rarefy = m->isTrue(temp);
			if (!rarefy) { iters = 1;  }
			
			temp = validParameter.validFile(parameters, "summary", false);			if (temp == "not found") { temp = "T"; }
			summary = m->isTrue(temp);
			
			temp = validParameter.validFile(parameters, "scale", false);			if (temp == "not found") { temp = "F"; }
			scale = m->isTrue(temp);
			
			temp = validParameter.validFile(parameters, "collect", false);			if (temp == "not found") { temp = "F"; }
			collect = m->isTrue(temp);
			
			groups = validParameter.validFile(parameters, "groups", false);			
			if (groups == "not found") { groups = ""; Groups = globaldata->gTreemap->namesOfGroups;  globaldata->Groups = Groups;  }
			else { 
				m->splitAtDash(groups, Groups);
				globaldata->Groups = Groups;
			}
			
			if ((!collect) && (!rarefy) && (!summary)) { m->mothurOut("No outputs selected. You must set either collect, rarefy or summary to true, summary=T by default."); m->mothurOutEndLine(); abort=true; }
		}
		
	}
	catch(exception& e) {
		m->errorOut(e, "PhyloDiversityCommand", "PhyloDiversityCommand");
		exit(1);
	}			
}
//**********************************************************************************************************************

void PhyloDiversityCommand::help(){
	try {
		m->mothurOut("The phylo.diversity command can only be executed after a successful read.tree command.\n");
		m->mothurOut("The phylo.diversity command parameters are groups, iters, freq, scale, rarefy, collect and summary.  No parameters are required.\n");
		m->mothurOut("The groups parameter allows you to specify which of the groups in your groupfile you would like analyzed. The group names are separated by dashes. By default all groups are used.\n");
		m->mothurOut("The iters parameter allows you to specify the number of randomizations to preform, by default iters=1000, if you set rarefy to true.\n");
		m->mothurOut("The freq parameter is used indicate when to output your data, by default it is set to 100. But you can set it to a percentage of the number of sequence. For example freq=0.10, means 10%. \n");
		m->mothurOut("The scale parameter is used indicate that you want your ouptut scaled to the number of sequences sampled, default = false. \n");
		m->mothurOut("The rarefy parameter allows you to create a rarefaction curve. The default is false.\n");
		m->mothurOut("The collect parameter allows you to create a collectors curve. The default is false.\n");
		m->mothurOut("The summary parameter allows you to create a .summary file. The default is true.\n");
		m->mothurOut("The phylo.diversity command should be in the following format: phylo.diversity(groups=yourGroups, rarefy=yourRarefy, iters=yourIters).\n");
		m->mothurOut("Example phylo.diversity(groups=A-B-C, rarefy=T, iters=500).\n");
		m->mothurOut("The phylo.diversity command output two files: .phylo.diversity and if rarefy=T, .rarefaction.\n");
		m->mothurOut("Note: No spaces between parameter labels (i.e. groups), '=' and parameters (i.e.yourGroups).\n\n");

	}
	catch(exception& e) {
		m->errorOut(e, "PhyloDiversityCommand", "help");
		exit(1);
	}
}

//**********************************************************************************************************************

PhyloDiversityCommand::~PhyloDiversityCommand(){}

//**********************************************************************************************************************

int PhyloDiversityCommand::execute(){
	try {
		
		if (abort == true) { return 0; }
		
		//incase the user had some mismatches between the tree and group files we don't want group xxx to be analyzed
		for (int i = 0; i < globaldata->Groups.size(); i++) { if (globaldata->Groups[i] == "xxx") { globaldata->Groups.erase(globaldata->Groups.begin()+i);  break; }  }
		 
		vector<string> outputNames;
			
		vector<Tree*> trees = globaldata->gTree;
		
		//for each of the users trees
		for(int i = 0; i < trees.size(); i++) {
		
			if (m->control_pressed) { for (int i = 0; i < outputNames.size(); i++) {	remove(outputNames[i].c_str()); 	} return 0; }
			
			ofstream outSum, outRare, outCollect;
			string outSumFile = outputDir + m->getRootName(m->getSimpleName(globaldata->getTreeFile()))  + toString(i+1) + ".phylodiv.summary";
			string outRareFile = outputDir + m->getRootName(m->getSimpleName(globaldata->getTreeFile()))  + toString(i+1) + ".phylodiv.rarefaction";
			string outCollectFile = outputDir + m->getRootName(m->getSimpleName(globaldata->getTreeFile()))  + toString(i+1) + ".phylodiv";
			
			if (summary)	{ m->openOutputFile(outSumFile, outSum); outputNames.push_back(outSumFile);				}
			if (rarefy)		{ m->openOutputFile(outRareFile, outRare); outputNames.push_back(outRareFile);				}
			if (collect)	{ m->openOutputFile(outCollectFile, outCollect); outputNames.push_back(outCollectFile);	}
			
			int numLeafNodes = trees[i]->getNumLeaves();
			
			//create a vector containing indexes of leaf nodes, randomize it, select nodes to send to calculator
			vector<int> randomLeaf;
			for (int j = 0; j < numLeafNodes; j++) {  
				if (m->inUsersGroups(trees[i]->tree[j].getGroup(), globaldata->Groups) == true) { //is this a node from the group the user selected.
					randomLeaf.push_back(j); 
				}
			}
			
			numLeafNodes = randomLeaf.size();  //reset the number of leaf nodes you are using 
			
			//each group, each sampling, if no rarefy iters = 1;
			map<string, vector<float> > diversity;
			
			//each group, each sampling, if no rarefy iters = 1;
			map<string, vector<float> > sumDiversity;
			
			//find largest group total 
			int largestGroup = 0;
			for (int j = 0; j < globaldata->Groups.size(); j++) {  
				if (globaldata->gTreemap->seqsPerGroup[globaldata->Groups[j]] > largestGroup) { largestGroup = globaldata->gTreemap->seqsPerGroup[globaldata->Groups[j]]; }
				
				//initialize diversity
				diversity[globaldata->Groups[j]].resize(globaldata->gTreemap->seqsPerGroup[globaldata->Groups[j]]+1, 0.0);		//numSampled
																											//groupA		0.0			0.0
																											
				//initialize sumDiversity
				sumDiversity[globaldata->Groups[j]].resize(globaldata->gTreemap->seqsPerGroup[globaldata->Groups[j]]+1, 0.0);
			}	

			//convert freq percentage to number
			int increment = 100;
			if (freq < 1.0) {  increment = largestGroup * freq;  
			}else { increment = freq;  }
			
			//initialize sampling spots
			set<int> numSampledList;
			for(int k = 1; k <= largestGroup; k++){  if((k == 1) || (k % increment == 0)){  numSampledList.insert(k); }   }
			if(largestGroup % increment != 0){	numSampledList.insert(largestGroup);   }
			
			//add other groups ending points
			for (int j = 0; j < globaldata->Groups.size(); j++) {  
				if (numSampledList.count(diversity[globaldata->Groups[j]].size()-1) == 0) {  numSampledList.insert(diversity[globaldata->Groups[j]].size()-1); }
			}

			for (int l = 0; l < iters; l++) {
				random_shuffle(randomLeaf.begin(), randomLeaf.end());
		
				//initialize counts
				map<string, int> counts;
				map< string, set<int> > countedBranch;  
				for (int j = 0; j < globaldata->Groups.size(); j++) {  counts[globaldata->Groups[j]] = 0; countedBranch[globaldata->Groups[j]].insert(-2);  }  //add dummy index to initialize countedBranch sets
				
				for(int k = 0; k < numLeafNodes; k++){
						
					if (m->control_pressed) { for (int i = 0; i < outputNames.size(); i++) {	remove(outputNames[i].c_str()); 	} return 0; }
					
					//calc branch length of randomLeaf k
					float br = calcBranchLength(trees[i], randomLeaf[k], countedBranch);
			
					//for each group in the groups update the total branch length accounting for the names file
					vector<string> groups = trees[i]->tree[randomLeaf[k]].getGroup();
					
					for (int j = 0; j < groups.size(); j++) {
						int numSeqsInGroupJ = 0;
						map<string, int>::iterator it;
						it = trees[i]->tree[randomLeaf[k]].pcount.find(groups[j]);
						if (it != trees[i]->tree[randomLeaf[k]].pcount.end()) { //this leaf node contains seqs from group j
							numSeqsInGroupJ = it->second;
						}
						
						if (numSeqsInGroupJ != 0) {	diversity[groups[j]][(counts[groups[j]]+1)] = diversity[groups[j]][counts[groups[j]]] + br;  }
						
						for (int s = (counts[groups[j]]+2); s <= (counts[groups[j]]+numSeqsInGroupJ); s++) {
							diversity[groups[j]][s] = diversity[groups[j]][s-1];  //update counts, but don't add in redundant branch lengths
						}
						counts[groups[j]] += numSeqsInGroupJ;
					}
				}
				
				if (rarefy) {
					//add this diversity to the sum
					for (int j = 0; j < globaldata->Groups.size(); j++) {  
						for (int g = 0; g < diversity[globaldata->Groups[j]].size(); g++) {
							sumDiversity[globaldata->Groups[j]][g] += diversity[globaldata->Groups[j]][g];
						}
					}
				}
				
				if ((collect) && (l == 0)) {  printData(numSampledList, diversity, outCollect, 1);  }
				if ((summary) && (l == 0)) {  printSumData(diversity, outSum, 1);  }
			}
			
			if (rarefy) {	printData(numSampledList, sumDiversity, outRare, iters);	}
		}
		
	
		if (m->control_pressed) { for (int i = 0; i < outputNames.size(); i++) {	remove(outputNames[i].c_str()); 	} return 0; }

		m->mothurOutEndLine();
		m->mothurOut("Output File Names: "); m->mothurOutEndLine();
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i]); m->mothurOutEndLine();	}
		m->mothurOutEndLine();

		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "PhyloDiversityCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************

void PhyloDiversityCommand::printSumData(map< string, vector<float> >& div, ofstream& out, int numIters){
	try {
		
		out << "numSampled\t";
		for (int i = 0; i < globaldata->Groups.size(); i++) { out << globaldata->Groups[i] << '\t';  }
		out << endl;
		
		out.setf(ios::fixed, ios::floatfield); out.setf(ios::showpoint);
		
		set<int> num;
		//find end points to output
		for (map<string, vector<float> >::iterator itEnds = div.begin(); itEnds != div.end(); itEnds++) {	num.insert(itEnds->second.size()-1);  }
		
		for (set<int>::iterator it = num.begin(); it != num.end(); it++) {  
			int numSampled = *it;
			
			out << numSampled << '\t';  
			
			for (int j = 0; j < globaldata->Groups.size(); j++) {
				if (numSampled < div[globaldata->Groups[j]].size()) { 
					float score;
					if (scale)	{  score = (div[globaldata->Groups[j]][numSampled] / (float)numIters) / (float)numSampled;	}
					else		{	score = div[globaldata->Groups[j]][numSampled] / (float)numIters;	}
					
					out << setprecision(4) << score << '\t';
	
				}else { out << "NA" << '\t'; }
			}
			out << endl;
		}
		
		out.close();
		
	}
	catch(exception& e) {
		m->errorOut(e, "PhyloDiversityCommand", "printSumData");
		exit(1);
	}
}
//**********************************************************************************************************************

void PhyloDiversityCommand::printData(set<int>& num, map< string, vector<float> >& div, ofstream& out, int numIters){
	try {
		
		out << "numSampled\t";
		for (int i = 0; i < globaldata->Groups.size(); i++) { out << globaldata->Groups[i] << '\t';  }
		out << endl;
		
		out.setf(ios::fixed, ios::floatfield); out.setf(ios::showpoint);
		
		for (set<int>::iterator it = num.begin(); it != num.end(); it++) {  
			int numSampled = *it;
			
			out << numSampled << '\t';  
			
			for (int j = 0; j < globaldata->Groups.size(); j++) {
				if (numSampled < div[globaldata->Groups[j]].size()) { 
					float score;
					if (scale)	{  score = (div[globaldata->Groups[j]][numSampled] / (float)numIters) / (float)numSampled;	}
					else		{	score = div[globaldata->Groups[j]][numSampled] / (float)numIters;	}

					out << setprecision(4) << score << '\t';
				}else { out << "NA" << '\t'; }
			}
			out << endl;
		}
		
		out.close();
		
	}
	catch(exception& e) {
		m->errorOut(e, "PhyloDiversityCommand", "printData");
		exit(1);
	}
}
//**********************************************************************************************************************
float PhyloDiversityCommand::calcBranchLength(Tree* t, int leaf, map< string, set<int> >& counted){
	try {

		//calc the branch length
		//while you aren't at root
		float sum = 0.0;
		int index = leaf;
		
		vector<string> groups = t->tree[leaf].getGroup();
					
		while(t->tree[index].getParent() != -1){
			
			//if you have a BL
			if(t->tree[index].getBranchLength() != -1){
				if (counted[groups[0]].count(index) == 0) { //you have not already counted this branch
					sum += abs(t->tree[index].getBranchLength());
					for (int j = 0; j < groups.size(); j++) {  counted[groups[j]].insert(index);  }
				}
			}
			index = t->tree[index].getParent();
		}
			
		//get last breanch length added
		if(t->tree[index].getBranchLength() != -1){
			if (counted[groups[0]].count(index) == 0) { //you have not already counted this branch
				sum += abs(t->tree[index].getBranchLength());
				for (int j = 0; j < groups.size(); j++) {  counted[groups[j]].insert(index);  }
			}
		}
		
		return sum;
	}
	catch(exception& e) {
		m->errorOut(e, "PhyloDiversityCommand", "calcBranchLength");
		exit(1);
	}
}
//**********************************************************************************************************************