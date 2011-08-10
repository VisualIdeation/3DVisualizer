/***********************************************************************
Reservoir - This class reads an ASCII file which contains the results
of a Reservoir run.

Copyright (c) 2010 Vanessa Gertman

This module was modeled after the ExampleModule from 3D Visualizer.
The ExampleModule is free software under the GNU General Public License.
***********************************************************************/
#include <math.h>
#include <stdio.h>
#include <Plugins/FactoryManager.h>

#include <Concrete/Reservoir.h>
#define PI 3.1415926535897932384626
namespace Visualization {

namespace Concrete {

/******************************
Methods of class Reservoir:
******************************/

/***********************************************************************
Constructor for Reservoir class. Contains no code except the
definition of its own name.
***********************************************************************/

Reservoir::Reservoir(void)
	:BaseModule("Reservoir")
	{
	}

/***********************************************************************
Method to load a data set from a file, given a particular command line.
This method defines the format of the data files read by this module
class and has to be written.
***********************************************************************/

  Visualization::Abstract::DataSet* Reservoir::load(const std::vector<std::string>& args, Comm::MulticastPipe* pipe) const
	{
	/* Create the result data set: */
	DataSet* result=new DataSet;
	
	/* Open the input file: */
	FILE* file=fopen(args[0].c_str(),"rt"); // args[0] is the first module command line parameter
	
	int status;
	/* Read the input file's header: */
	DS::Index numVertices; // DS::Index is a helper type containing three integers NI, NJ, NK
	status = fscanf(file,"%d %d %d",&numVertices[0],&numVertices[1],&numVertices[2]);
	
	/* Define the result data set's grid layout: */
	DS& dataSet=result->getDs(); // Get the internal data representation from the result data set
	dataSet.setGrid(numVertices); // Set the data set's number of vertices

	dataSet.addSlice(); // Add a single scalar variable to the data set's grid, need as many calls as variables
	dataSet.addSlice(); // Add a single scalar variable to the data set's grid
	dataSet.addSlice(); // Add a single scalar variable to the data set's grid

	dataSet.addSlice(); // Add a single scalar variable to the data set's grid
	dataSet.addSlice(); // Add a single scalar variable to the data set's grid
	dataSet.addSlice(); // Add a single scalar variable to the data set's grid

	dataSet.addSlice(); // Add a single scalar variable to the data set's grid
	dataSet.addSlice(); // Add a single scalar variable to the data set's grid
	dataSet.addSlice(); // Add a single scalar variable to the data set's grid

	dataSet.addSlice(); // Add a single scalar variable to the data set's grid
	dataSet.addSlice(); // Add a single scalar variable to the data set's grid
	dataSet.addSlice(); // Add a single scalar variable to the data set's grid

	
	/* Define the result data set's variables as they are selected in 3D Visualizer's menus: */
	DataValue& dataValue=result->getDataValue(); // Get the internal representations of the data set's value space
	dataValue.initialize(&dataSet); // Initialize the value space for the data set
	dataValue.setScalarVariableName(0,"Depth");
	dataValue.setScalarVariableName(1,"DZ");
	dataValue.setScalarVariableName(2,"DZNet");

	dataValue.setScalarVariableName(3,"Permeability");
	dataValue.setScalarVariableName(4,"Porosity");
	dataValue.setScalarVariableName(5,"Pressure");

	dataValue.setScalarVariableName(6,"Gas");
	dataValue.setScalarVariableName(7,"Oil");
	dataValue.setScalarVariableName(8,"Water");

	dataValue.setScalarVariableName(9,"Vx");
	dataValue.setScalarVariableName(10,"Vy");
	dataValue.setScalarVariableName(11,"Vz");
	
	/* Read all vertex positions and temperature values: */
	DS::Index index; // Index counting variable containing three integers I, J, K
	for(index[2]=0;index[2]<numVertices[2];++index[2])
		for(index[1]=0;index[1]<numVertices[1];++index[1])
			for(index[0]=0;index[0]<numVertices[0];++index[0])
				{
				/* Read the vertex position and flux and relative error values: */
				  double pos[3],depth,dz,dznet,permeability,porosity,pressure,gas,oil,water,vx,vy,vz;
				  status = fscanf(file,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",&pos[0],&pos[1],&pos[2],&depth, &dz, &dznet, &permeability, &porosity, &pressure, &gas, &oil, &water, &vx, &vy, &vz);

				/* Store the position and value in the data set: */
				dataSet.getVertexPosition(index)=DS::Point(pos); // Store the vertex' position

				dataSet.getVertexValue(0,index)=DS::ValueScalar(depth);
				dataSet.getVertexValue(1,index)=DS::ValueScalar(dz);
				dataSet.getVertexValue(2,index)=DS::ValueScalar(dznet);

				dataSet.getVertexValue(3,index)=DS::ValueScalar(permeability);
				dataSet.getVertexValue(4,index)=DS::ValueScalar(porosity);
				dataSet.getVertexValue(5,index)=DS::ValueScalar(pressure);

				dataSet.getVertexValue(6,index)=DS::ValueScalar(gas);
				dataSet.getVertexValue(7,index)=DS::ValueScalar(oil);
				dataSet.getVertexValue(8,index)=DS::ValueScalar(water);

				dataSet.getVertexValue(9,index)=DS::ValueScalar(vx);
				dataSet.getVertexValue(10,index)=DS::ValueScalar(vy);
				dataSet.getVertexValue(11,index)=DS::ValueScalar(vz);
				}
	
	/* Close the input file: */
	fclose(file);
	
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
	Visualization::Concrete::Reservoir* module=new Visualization::Concrete::Reservoir();
	
	/* Return module object: */
	return module;
	}

extern "C" void destroyFactory(Visualization::Abstract::Module* module)
	{
	delete module;
	}
