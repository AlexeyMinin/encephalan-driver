#ifndef __OpenViBE_AcquisitionServer_CConfigurationEncephalan_H__
#define __OpenViBE_AcquisitionServer_CConfigurationEncephalan_H__

#include "../ovasCConfigurationBuilder.h"
#include "ovasIDriver.h"

#include <gtk/gtk.h>

namespace OpenViBEAcquisitionServer
{
	/**
	 * \class CConfigurationEncephalan
	 * \author Alexey Minin (UrFU)
	 * \date Wed Jan 02 13:35:39 2019
	 * \brief The CConfigurationEncephalan handles the configuration dialog specific to the Encephalan device.
	 *
	 * TODO: details
	 *
	 * \sa CDriverEncephalan
	 */
	class CConfigurationEncephalan : public OpenViBEAcquisitionServer::CConfigurationBuilder
	{
	public:

		// you may have to add to your constructor some reference parameters
		// for example, a connection ID:
		//CConfigurationEncephalan(OpenViBEAcquisitionServer::IDriverContext& rDriverContext, const char* sGtkBuilderFileName, OpenViBE::uint32& rConnectionId);
		//CConfigurationEncephalan(OpenViBEAcquisitionServer::IDriverContext& rDriverContext, const char* sGtkBuilderFileName);
		CConfigurationEncephalan(OpenViBEAcquisitionServer::IDriverContext& rDriverContext, const char* sGtkBuilderFileName, OpenViBE::uint32& rConnectionPort, char* rConnectionIp);

		virtual bool preConfigure(void);
		virtual bool postConfigure(void);

		char* getConnectionIp();

	protected:

		OpenViBEAcquisitionServer::IDriverContext& m_rDriverContext;

	private:

		/*
		 * Insert here all specific attributes, such as a connection ID.
		 * use references to directly modify the corresponding attribute of the driver
		 * Example:
		 */
		// uint32_t& m_ui32ConnectionID;
		uint32_t& m_ui32ConnectionPort;
		char* m_sConnectionIp;
	};
};

#endif // __OpenViBE_AcquisitionServer_CConfigurationEncephalan_H__
