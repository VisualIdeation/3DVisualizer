/***********************************************************************
RealMCNP - Reads a Mesh Tally file generated by a MCNP run. It stores the
values of these results and calls 3D Visualizer methods on these stored
results. This is a 3D Visualizer module.

Copyright (c) 2010 Vanessa Gertman

This module was modeled after the ExampleModule from 3D Visualizer.
The ExampleModule is free software under the GNU General Public License.
***********************************************************************/

#include <math.h>
#include <stdio.h>
#include <iostream>
#include <Plugins/FactoryManager.h>
#include <string>
#include <Concrete/RealMCNP.h>
#include <fstream>
#include <string>
#include <sstream>

#define PI 3.1415926535897932384626
namespace Visualization {

namespace Concrete {

/******************************
Methods of class RealMCNP:
******************************/

/***********************************************************************
Constructor for RealMCNP class. Contains no code except the
definition of its own name.
***********************************************************************/

RealMCNP::RealMCNP(void)
	:BaseModule("RealMCNP")
	{
	}

/***********************************************************************
Method to load a data set from a file, given a particular command line.
This method defines the format of the data files read by this module
class and has to be written.
***********************************************************************/

  Visualization::Abstract::DataSet* RealMCNP::load(const std::vector<std::string>& args, Comm::MulticastPipe* pipe) const
	{
	/* Create the result data set: */
	DataSet* result=new DataSet;

        /*Declare data to hold number of vertexes and help read file*/
        int x_dim=0;
        int y_dim=0;
        int z_dim=0;
        std::string line;
        std::ifstream myfile;
        size_t found;
        
        /*Read the Mesh Tally file provided*/
        myfile.open(args[0].c_str());
        for(int i=0; i<10000; i++)
        {
          //determining the number of x points
          found= line.find("X direction:");
          if(found!=std::string::npos)
          {
            line= line.substr(found+12, 100000);
            double xs;
            const char *p=line.c_str();
            int n;
            while(sscanf(p, "%lf%n ", &xs, &n)==1)
            {
              p+=n;//progress further through the string
              x_dim++;
            }
          }
          //determining the number of y points
            found= line.find("Y direction:");
            if(found!=std::string::npos)
            {
              line= line.substr(found+12, 100000);
              double ys;
              const char *p=line.c_str();
              int n;
              while(sscanf(p, "%lf%n ", &ys, &n)==1)
              {
                 p+=n;//progress further through the string
                 y_dim++;
              }
            }
            //determining the number of z points
            found= line.find("Z direction:");
            if(found!=std::string::npos)
            {
              line= line.substr(found+12, 100000);
              double zs;
              const char *p=line.c_str();
              int n;
              while(sscanf(p, "%lf%n ", &zs, &n)==1)
              {
                p+=n;//progress further through the string
                z_dim++;
              }
            }
            //searching for the begining of the data, once found, reading will temporarily stop, indicated by keyword "Result"
            getline(myfile, line);
            found= line.find("Result");
            if(found!= std::string::npos)//Once result is found, stop reading
            {
              break;
            }

        }

        DS::Index numVertices; // DS::Index is a helper type containing three integers NI, NJ, NK
        /* Use the numbers of points found above to set the numVerticies. Must subtract one because we counted a range:*/
        numVertices[0]=x_dim-1;
        numVertices[1]=y_dim-1;
        numVertices[2]=z_dim-1;
        
	/* Define the result data set's grid layout: */
	DS& dataSet=result->getDs(); // Get the internal data representation from the result data set
	dataSet.setGrid(numVertices); // Set the data set's number of vertices
	dataSet.addSlice(); // Add a single scalar variable to the data set's grid
	dataSet.addSlice(); // Add a single variable to the data set's grid, this call must occur for EACH variable to be stored
	
	/* Define the result data set's variables as they are selected in 3D Visualizer's menus: */
	DataValue& dataValue=result->getDataValue(); // Get the internal representations of the data set's value space
	dataValue.initialize(&dataSet); // Initialize the value space for the data set
        dataValue.setScalarVariableName(0,"Flux"); // Set the name of the first scalar variable
        dataValue.setScalarVariableName(1,"Relative Error"); // Set the name of the second scalar variable 
	
	/* Read all vertex positions flux, and relative error values where the reading of the file left off: */
	DS::Index index; // Index counting variable containing three integers I, J, K
	for(index[0]=0;index[0]<numVertices[0];++index[0]) // I varies most slowly
		for(index[1]=0;index[1]<numVertices[1];++index[1])
			for(index[2]=0;index[2]<numVertices[2];++index[2]) // K varies fastest
				{
				/* Read the vertex position, flux, and relative error values: */
                                  double pos[3],flux, relative_err;
                                  getline(myfile, line);
                                  sscanf(line.c_str(),"%lf %lf %lf %lf %lf",&pos[0],&pos[1],&pos[2],&flux, &relative_err);
				  
				  /*Rotating and resizing the data to fit correctly in the core*/
				  double temp_x= pos[0];
				  pos[0]= ((pos[0]*cos(PI/4)) - (pos[1]*sin(PI/4)))*0.01;
				  pos[1]= ((temp_x*sin(PI/4)) + (pos[1]*cos(PI/4)))*0.01;
				  pos[2]= (pos[2])*0.01;

				/* Store the positon and values in the data set: */
				dataSet.getVertexPosition(index)=DS::Point(pos); // Store the vertex' position
				dataSet.getVertexValue(0,index)=DS::ValueScalar(flux); // Store the vertex' flux
                                dataSet.getVertexValue(1,index)=DS::ValueScalar(relative_err); //Store the vertex's relative error
                                }
	
	/* Close the input file: */
        myfile.close();

        /* Finalize the data set's grid structure (required): */
	dataSet.finalizeGrid();
	
	/* Return the result data set: */
	return result;
	}

}

}

/***********************************************************************
Plug-in interface functions. These allow loading dynamically loading
modules into 3D Visualizer at run-time, and do not have to be changed
except for the name of the generated module class.
***********************************************************************/

extern "C" Visualization::Abstract::Module* createFactory(Plugins::FactoryManager<Visualization::Abstract::Module>& manager)
	{
	/* Create module object and insert it into class hierarchy: */
	Visualization::Concrete::RealMCNP* module=new Visualization::Concrete::RealMCNP();
	
	/* Return module object: */
	return module;
	}

extern "C" void destroyFactory(Visualization::Abstract::Module* module)
	{
	delete module;
	}