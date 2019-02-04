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
	m_oHeader.setChannelCount(8);
	m_ui32ConnectionPort = 120;
	m_sConnectionIp = "127.0.0.1";
	
	// The following class allows saving and loading driver settings from the acquisition server .conf file
	m_oSettings.add("Header", &m_oHeader);
	// To save your custom driver settings, register each variable to the SettingsHelper
	//m_oSettings.add("ConnectionIp", &m_sConnectionIp);
	//m_oSettings.add("ConnectionPort", &m_ui32ConnectionPort);
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
	if (!connectEncephalan()) {
		return false;
	}
	return true;
}

bool CDriverEncephalan::start(void)
{
	if(!m_rDriverContext.isConnected()) return false;
	if(m_rDriverContext.isStarted()) return false;

	// ...
	// request hardware to start
	if (!sendRequestForData()) {
		return false;
	}
	// sending data
	// ...

	return true;
}

bool CDriverEncephalan::loop(void)
{
	if(!m_rDriverContext.isConnected()) return false;
	if(!m_rDriverContext.isStarted()) return true;

	// OpenViBE::CStimulationSet l_oStimulationSet;

	// ...
	// receive samples from hardware
	// put them the correct way in the sample array
	// whether the buffer is full, send it to the acquisition server
	//...
	m_ui32currentPoint = 0;
	ZeroMemory(m_pSample, sizeof(float32)*m_ui32SampleCountPerSentBlock*m_oHeader.getChannelCount());
	while (m_ui32currentPoint < m_ui32SampleCountPerSentBlock) {
		receiveData();
	}
	m_pCallback->setSamples(m_pSample);
	
	// When your sample buffer is fully loaded, 
	// it is advised to ask the acquisition server 
	// to correct any drift in the acquisition automatically.
	// m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());

	// ...
	// receive events from hardware
	// and put them the correct way in a CStimulationSet object
	//...
	// m_pCallback->setStimulationSet(l_oStimulationSet);

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

	closesocket(m_client);
	WSACleanup();

	return true;
}

bool CDriverEncephalan::connectEncephalan()
{
	WSADATA m_wsaData;
	int m_wsaret = WSAStartup(0x101, &m_wsaData);
	if (m_wsaret != 0) {
		m_rDriverContext.getLogManager() << LogLevel_Error << "Error WSAStartup (initialization windows socket api): " << WSAGetLastError() << "\n";
		return FALSE;
	}

	//m_client = socket(AF_INET, SOCK_STREAM, 0);
	m_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_client == INVALID_SOCKET)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Error socket (creation a socket): " << WSAGetLastError() << "\n";
		WSACleanup();
		return FALSE;
	}

	sockaddr_in	m_sockaddr;
	m_sockaddr.sin_family = AF_INET;
	m_sockaddr.sin_addr.s_addr = inet_addr(m_sConnectionIp);
	m_sockaddr.sin_port = htons((u_short)m_ui32ConnectionPort);
	int connectError = connect(m_client, (sockaddr*)&m_sockaddr, sizeof(m_sockaddr));
	if (connectError != 0) {
		m_rDriverContext.getLogManager() << LogLevel_Error << "Error connect (connection to a specified socket): " << WSAGetLastError() << "\n";
		return FALSE;
	}
	return TRUE;
}

bool CDriverEncephalan::sendRequestForData()
{
	BYTE * sendDataBuffer = new BYTE;

	DWORD ID = 0x0001; //EEG_ONLY_MODE
	//DWORD ID = 0x000C; //ALL_CHANNELS_MODE
	//DWORD ID = 0x000E; //HD_CHANNELS_MODE

	CopyMemory(sendDataBuffer, &ID, sizeof(ID));
	if (!sendData(sendDataBuffer, sizeof(ID))) {
		m_rDriverContext.getLogManager() << LogLevel_Error << "Error sendRequestForData: " << WSAGetLastError() << "\n";
		return FALSE;
	}
	m_rDriverContext.getLogManager() << LogLevel_Info << "sendRequestForData\n";
	return TRUE;
}

bool CDriverEncephalan::sendData(BYTE * data, int dataSize)
{
	if (m_client == INVALID_SOCKET) {
		return FALSE;
	}

	long fullLen = dataSize + sizeof(int);
	char *outData = new char[fullLen];

	CopyMemory(&outData[0], &dataSize, sizeof(int));
	CopyMemory(&outData[sizeof(int)], &data[0], dataSize);

	if (send(m_client, outData, fullLen, 0) != fullLen)
	{
		delete[]outData;
		return FALSE;
	}

	delete[]outData;
	return TRUE;
}

bool CDriverEncephalan::receiveData()
{
	int inpSize = 0;
	char *inpData = new char[sizeof(inpSize)*sizeof(BYTE)];
	int nReadSize = recv(m_client, inpData, sizeof(inpSize), 0);
	if (nReadSize <= 0) {
		return FALSE;
	}

	CopyMemory(&inpSize, &inpData[0], sizeof(inpSize));
	delete[]inpData;

	if ((inpSize > 0) && (inpSize < 1024 * 512))
	{
		DWORD ID = 0;
		inpData = new char[inpSize*sizeof(BYTE)];
		if (recv(m_client, inpData, inpSize, 0) == inpSize)
		{
			readData((BYTE*)inpData, inpSize);
		}
		delete[]inpData;
		inpData = NULL;
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

void CDriverEncephalan::readData(BYTE * data, int dataSize)
{
	BYTE * pCurData = data;
	int CurDataSize = dataSize;

	DWORD ID = 0;
	getData(pCurData, CurDataSize, &ID, sizeof(ID));
	switch (ID)
	{
		case 0x0002:
		{// Information on research
			m_rDriverContext.getLogManager() << LogLevel_Info << "Recieved information on research\n";
			// ReceiveEEGHeadData(pCurData, CurDataSize);
			break;
		}
		case 0x00E2:
		{// This prompted all the data including the kilohertz
			m_rDriverContext.getLogManager() << LogLevel_Info << "Recieved all the data including the kilohertz\n";
			break;
		}
		case 0x0003:
		{
			m_rDriverContext.getLogManager() << LogLevel_Info << "0x0003 data package\n";
			break;
		}
		case 0x0006:
		{
			m_rDriverContext.getLogManager() << LogLevel_Info << "0x0006 data package\n";
			break;
		}
		case 0x0008:
		{// We came signal data
			//  m_rDriverContext.getLogManager() << LogLevel_Info << "We came signal data\n";
			receiveEEGData(pCurData, CurDataSize);
			break;
		}
		case 0x00E8:
		{// Packet came with HD data
			m_rDriverContext.getLogManager() << LogLevel_Info << "Packet came with HD data\n";
			break;
		}
		case 0x0009:
		{
			m_rDriverContext.getLogManager() << LogLevel_Info << "0x0009 data package\n";
			break;
		}
		case 0x000A:
		{
			m_rDriverContext.getLogManager() << LogLevel_Info << "0x000A data package\n";
			break;
		}
		case 0x0101:
		{// Пришли данные со вторичкой
			m_rDriverContext.getLogManager() << LogLevel_Info << "0x0101 data package\n";
			break;
		}
		default:
		{
			m_rDriverContext.getLogManager() << LogLevel_Info << "Undefined data package\n";
		}
	}
}

// Parsing package with EEG data
void CDriverEncephalan::receiveEEGData(BYTE * pCurData, int CurDataSize)
{
	int SliceNum = 0;
	int PointsNumber = 0;
	getData(pCurData, CurDataSize, &SliceNum, sizeof(SliceNum));
	getData(pCurData, CurDataSize, &PointsNumber, sizeof(PointsNumber));

	short *pShortData = new short[PointsNumber];
	ZeroMemory(pShortData, sizeof(short)*PointsNumber);
	getData(pCurData, CurDataSize, pShortData, PointsNumber*sizeof(short));

	for (uint32_t iPos = 0; iPos < m_oHeader.getChannelCount(); iPos++)
	{
		float32 floatVal = (float32)pShortData[iPos];
		//m_pSample[m_ui32currentPoint + iPos * m_ui32SampleCountPerSentBlock] = (float32)pShortData[iPos];
		CopyMemory(m_pSample + (m_ui32currentPoint + iPos * m_ui32SampleCountPerSentBlock), &floatVal, sizeof(float32));
	}
	m_ui32currentPoint++;

	delete[]pShortData;
}

void CDriverEncephalan::getData(BYTE * & pData, int & DataSize, void * pTargetData, int TargetDataSize)
{
	if (DataSize < TargetDataSize)
	{
		return;
	}
	CopyMemory(pTargetData, pData, TargetDataSize);
	pData += TargetDataSize;
	DataSize -= TargetDataSize;
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
	CConfigurationEncephalan m_oConfiguration(m_rDriverContext, OpenViBE::Directories::getDataDir() + "/applications/acquisition-server/interface-Encephalan.ui", m_ui32ConnectionPort, m_sConnectionIp);
	
	if(!m_oConfiguration.configure(m_oHeader))
	{
		return false;
	}
	m_sConnectionIp = m_oConfiguration.getConnectionIp();
	m_oSettings.save();

	return true;
}
