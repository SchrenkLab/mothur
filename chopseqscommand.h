#ifndef CHOPSEQSCOMMAND_H
#define CHOPSEQSCOMMAND_H

/*
 *  chopseqscommand.h
 *  Mothur
 *
 *  Created by westcott on 5/10/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */


#include "command.hpp"
#include "sequence.hpp"

class ChopSeqsCommand : public Command {
	
	public:
	
		ChopSeqsCommand(string);	
		~ChopSeqsCommand(){};
		int execute();
		void help();	
		
	private:
		string fastafile, outputDir, keep;
		bool abort, countGaps;
		int numbases;
		
		string getChopped(Sequence);
		
};

#endif

