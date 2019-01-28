#ifndef __OpenViBE_AcquisitionServer_CDriverEncephalan_H__
#define __OpenViBE_AcquisitionServer_CDriverEncephalan_H__

#include "ovasIDriver.h"
#include "../ovasCHeader.h"
#include <openvibe/ov_all.h>

#include "../ovasCSettingsHelper.h"
#include "../ovasCSettingsHelperOperators.h"

#include "winsock.h"

namespace OpenViBEAcquisitionServer
{
	/**
	 * \class CDriverEncephalan
	 * \author Alexey Minin (UrFU)
	 * \date Wed Jan 02 13:35:39 2019
	 * \brief The CDriverEncephalan allows the acquisition server to acquire data from a Encephalan device.
	 *
	 * TODO: details
	 *
	 * \sa CConfigurationEncephalan
	 */
	class CDriverEncephalan : public OpenViBEAcquisitionServer::IDriver
	{
	public:

		CDriverEncephalan(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);
		virtual ~CDriverEncephalan(void);
		virtual const char* getName(void);

		virtual bool initialize(
			const uint32_t ui32SampleCountPerSentBlock,
			OpenViBEAcquisitionServer::IDriverCallback& rCallback);
		virtual bool uninitialize(void);

		virtual bool start(void);
		virtual bool stop(void);
		virtual bool loop(void);

		virtual bool isConfigurable(void);
		virtual bool configure(void);
		virtual const OpenViBEAcquisitionServer::IHeader* getHeader(void) { return &m_oHeader; }
		
		virtual bool isFlagSet(
			const OpenViBEAcquisitionServer::EDriverFlag eFlag) const
		{
			return eFlag==DriverFlag_IsUnstable;
		}

	protected:
		
		SettingsHelper m_oSettings;
		
		OpenViBEAcquisitionServer::IDriverCallback* m_pCallback;

		// Replace this generic Header with any specific header you might have written
		OpenViBEAcquisitionServer::CHeader m_oHeader;

		uint32_t m_ui32SampleCountPerSentBlock;
		OpenViBE::float32* m_pSample;
	
	private:

		bool connectEncephalan();

		/*
		 * Insert here all specific attributes, such as USB port number or device ID.
		 * Example :
		 */
		// uint32_t m_ui32ConnectionID;
		uint32_t m_ui32ConnectionPort;
		char* m_sConnectionIp;

		// Socket
		SOCKET m_client;
	};
};

#endif // __OpenViBE_AcquisitionServer_CDriverEncephalan_H__
