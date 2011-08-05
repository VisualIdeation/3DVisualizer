/***********************************************************************
GlobalIsosurfaceExtractor - Wrapper class to map from the abstract
visualization algorithm interface to a templatized isosurface extractor
implementation.
Copyright (c) 2006-2009 Oliver Kreylos

This file is part of the 3D Data Visualizer (Visualizer).

The 3D Data Visualizer is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2 of the License, or (at
your option) any later version.

The 3D Data Visualizer is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with the 3D Data Visualizer; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
***********************************************************************/

#ifndef VISUALIZATION_WRAPPERS_GLOBALISOSURFACEEXTRACTOR_INCLUDED
#define VISUALIZATION_WRAPPERS_GLOBALISOSURFACEEXTRACTOR_INCLUDED

#include <Misc/Autopointer.h>
#include <GLMotif/RadioBox.h>
#include <GLMotif/Slider.h>

#include <Abstract/DataSet.h>
#include <Abstract/Parameters.h>
#include <Abstract/Algorithm.h>

#include <Wrappers/Isosurface.h>

/* Forward declarations: */
namespace GLMotif {
class TextField;
}
namespace Visualization {
namespace Abstract {
class ScalarExtractor;
class Element;
}
namespace Templatized {
template <class ScalarParam,class SourceValueParam>
class ScalarExtractor;
template <class DataSetParam,class ScalarExtractorParam,class IsosurfaceParam>
class IsosurfaceExtractor;
}
namespace Wrappers {
template <class SEParam>
class ScalarExtractor;
}
}

namespace Visualization {

namespace Wrappers {

template <class DataSetWrapperParam>
class GlobalIsosurfaceExtractor:public Visualization::Abstract::Algorithm
	{
	/* Embedded classes: */
	public:
	typedef Visualization::Abstract::Algorithm Base; // Base class
	typedef DataSetWrapperParam DataSetWrapper; // Compatible data set type
	typedef typename DataSetWrapper::DS DS; // Type of templatized data set
	typedef typename DataSetWrapper::SE SE; // Type of templatized scalar extractor
	typedef typename SE::Scalar VScalar; // Value type of scalar extractor
	typedef typename DataSetWrapper::ScalarExtractor ScalarExtractor; // Compatible scalar extractor wrapper class
	typedef Visualization::Wrappers::Isosurface<DataSetWrapper> Isosurface; // Type of created visualization elements
	typedef Misc::Autopointer<Isosurface> IsosurfacePointer; // Type for pointers to created visualization elements
	typedef typename Isosurface::Surface Surface; // Type of low-level surface representation
	typedef Visualization::Templatized::IsosurfaceExtractor<DS,SE,Surface> ISE; // Type of templatized isosurface extractor
	
	private:
	class Parameters:public Visualization::Abstract::Parameters // Class to store extraction parameters for global isosurfaces
		{
		friend class GlobalIsosurfaceExtractor;
		
		/* Elements: */
		private:
		int scalarVariableIndex; // Index of the scalar variable to color the isosurface
		bool smoothShading; // Flag to enable smooth shading by calculating scalar field gradients at each vertex position
		VScalar isovalue; // The isosurface's isovalue
		
		/* Private methods: */
		template <class DataSourceParam>
		void readBinary(DataSourceParam& dataSource,bool raw,const Visualization::Abstract::VariableManager* variableManager); // Reads parameters from a binary data source
		template <class DataSourceParam>
		void writeBinary(DataSourceParam& dataSink,bool raw,const Visualization::Abstract::VariableManager* variableManager) const; // Writes parameters to a binary data source
		
		/* Constructors and destructors: */
		public:
		Parameters(int sScalarVariableIndex)
			:scalarVariableIndex(sScalarVariableIndex)
			{
			}
		
		/* Methods from Abstract::Parameters: */
		virtual bool isValid(void) const
			{
			return true;
			}
		virtual void read(Misc::File& file,bool ascii,Visualization::Abstract::VariableManager* variableManager);
		virtual void read(Comm::MulticastPipe& pipe,Visualization::Abstract::VariableManager* variableManager);
		virtual void read(Comm::ClusterPipe& pipe,Visualization::Abstract::VariableManager* variableManager);
		virtual void write(Misc::File& file,bool ascii,const Visualization::Abstract::VariableManager* variableManager) const;
		virtual void write(Comm::MulticastPipe& pipe,const Visualization::Abstract::VariableManager* variableManager) const;
		virtual void write(Comm::ClusterPipe& pipe,const Visualization::Abstract::VariableManager* variableManager) const;
		virtual Visualization::Abstract::Parameters* clone(void) const
			{
			return new Parameters(*this);
			}
		};
	
	/* Elements: */
	private:
	static const char* name; // Identifying name of this algorithm
	Parameters parameters; // The isosurface extraction parameters used by this extractor
	ISE ise; // The templatized isosurface extractor
	Visualization::Abstract::DataSet::VScalarRange valueRange; // Value range of the scalar variable used by this extractor
	
	/* UI components: */
	GLMotif::RadioBox* extractionModeBox; // Radio box with toggles for extraction modes
	GLMotif::TextField* isovalueValue; // Text field to display the current isovalue
	GLMotif::Slider* isovalueSlider; // Slider to select the current isovalue
	
	/* Private methods: */
	static const DS* getDs(const Visualization::Abstract::DataSet* sDataSet);
	static const SE& getSe(const Visualization::Abstract::ScalarExtractor* sScalarExtractor);
	
	/* Constructors and destructors: */
	public:
	GlobalIsosurfaceExtractor(Visualization::Abstract::VariableManager* sVariableManager,Comm::MulticastPipe* sPipe); // Creates an isosurface extractor
	virtual ~GlobalIsosurfaceExtractor(void);
	
	/* Methods from Visualization::Abstract::Algorithm: */
	virtual const char* getName(void) const
		{
		return name;
		}
	virtual bool hasGlobalCreator(void) const
		{
		return true;
		}
	virtual GLMotif::Widget* createSettingsDialog(GLMotif::WidgetManager* widgetManager);
	virtual Visualization::Abstract::Parameters* cloneParameters(void) const
		{
		return new Parameters(parameters);
		}
	virtual Visualization::Abstract::Element* createElement(Visualization::Abstract::Parameters* extractParameters);
	virtual Visualization::Abstract::Element* startSlaveElement(Visualization::Abstract::Parameters* extractParameters);
	
	/* New methods: */
	static const char* getClassName(void) // Returns the algorithm class name
		{
		return name;
		}
	const ISE& getIse(void) const // Returns the templatized isosurface extractor
		{
		return ise;
		}
	ISE& getIse(void) // Ditto
		{
		return ise;
		}
	void extractionModeBoxCallback(GLMotif::RadioBox::ValueChangedCallbackData* cbData);
	void isovalueSliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData);
	};

}

}

#ifndef VISUALIZATION_WRAPPERS_GLOBALISOSURFACEEXTRACTOR_IMPLEMENTATION
#include <Wrappers/GlobalIsosurfaceExtractor.cpp>
#endif

#endif
