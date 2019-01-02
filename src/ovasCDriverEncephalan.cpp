#include "ovasCDriverEncephalan.h"
#include "ovasCConfigurationEncephalan.h"

#include <toolkit/ovtk_all.h>

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

//___________________________________________________________________//
//                                                                   //

CDriverEncephalan::CDriverEncephalan(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_oSettings("AcquisitionServer_Driver_Encephalan", m_rDriverContext.getConfigurationManager())
	,m_pCallback(NULL)
	,m_ui32SampleCountPerSentBlock(0)
	,m_pSample(NULL)
{
	m_oHeader.setSamplingFrequency(250);
	m_oHeader.setChannelCount(64);
	
	// The following class allows saving and loading driver settings from the acquisition server .conf file
	m_oSettings.add("Header", &m_oHeader);
	// To save your custom driver settings, register each variable to the SettingsHelper
	//m_oSettings.add("SettingName", &variable);
	m_oSettings.load();	
}

CDriverEncephalan::~CDriverEncephalan(void)
{
}

const char* CDriverEncephalan::getName(void)
{
	return "Encephalan";
}

//___________________________________________________________________//
//                                                                   //

bool CDriverEncephalan::initialize(
	const uint32_t ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	if(m_rDriverContext.isConnected()) return false;
	if(!m_oHeader.isChannelCountSet()||!m_oHeader.isSamplingFrequencySet()) return false;
	
	// Builds up a buffer to store
	// acquired samples. This buffer
	// will be sent to the acquisition
	// server later...
	m_pSample=new float32[m_oHeader.getChannelCount()*ui32SampleCountPerSentBlock];
	if(!m_pSample)
	{
		delete [] m_pSample;
		m_pSample=NULL;
		return false;
	}
	
	// ...
	// initialize hardware and get
	// available header information
	// from it
	// Using for example the connection ID provided by the configuration (m_ui32ConnectionID)
	// ...

	// Saves parameters
	m_pCallback=&rCallback;
	m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;
	return true;
}

bool CDriverEncephalan::start(void)
{
	if(!m_rDriverContext.isConnected()) return false;
	if(m_rDriverContext.isStarted()) return false;

	// ...
	// request hardware to start
	// sending data
	// ...

	return true;
}

bool CDriverEncephalan::loop(void)
{
	if(!m_rDriverContext.isConnected()) return false;
	if(!m_rDriverContext.isStarted()) return true;

	OpenViBE::CStimulationSet l_oStimulationSet;

	// ...
	// receive samples from hardware
	// put them the correct way in the sample array
	// whether the buffer is full, send it to the acquisition server
	//...
	m_pCallback->setSamples(m_pSample);
	
	// When your sample buffer is fully loaded, 
	// it is advised to ask the acquisition server 
	// to correct any drift in the acquisition automatically.
	m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());

	// ...
	// receive events from hardware
	// and put them the correct way in a CStimulationSet object
	//...
	m_pCallback->setStimulationSet(l_oStimulationSet);

	return true;
}

bool CDriverEncephalan::stop(void)
{
	if(!m_rDriverContext.isConnected()) return false;
	if(!m_rDriverContext.isStarted()) return false;

	// ...
	// request the hardware to stop
	// sending data
	// ...

	return true;
}

bool CDriverEncephalan::uninitialize(void)
{
	if(!m_rDriverContext.isConnected()) return false;
	if(m_rDriverContext.isStarted()) return false;

	// ...
	// uninitialize hardware here
	// ...

	delete [] m_pSample;
	m_pSample=NULL;
	m_pCallback=NULL;

	return true;
}

//___________________________________________________________________//
//                                                                   //
bool CDriverEncephalan::isConfigurable(void)
{
	return true; // change to false if your device is not configurable
}

bool CDriverEncephalan::configure(void)
{
	// Change this line if you need to specify some references to your driver attribute that need configuration, e.g. the connection ID.
	CConfigurationEncephalan m_oConfiguration(m_rDriverContext, OpenViBE::Directories::getDataDir() + "/applications/acquisition-server/interface-Encephalan.ui");
	
	if(!m_oConfiguration.configure(m_oHeader))
	{
		return false;
	}
	m_oSettings.save();
	
	return true;
}
