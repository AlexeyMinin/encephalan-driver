#include "ovasCConfigurationEncephalan.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEAcquisitionServer;
using namespace std;

/*_________________________________________________

Insert callback to specific widget here
Example with a button that launch a calibration of the device:

//Callback connected to a dedicated gtk button:
static void button_calibrate_pressed_cb(::GtkButton* pButton, void* pUserData)
{
	CConfigurationEncephalan* l_pConfig=static_cast<CConfigurationEncephalan*>(pUserData);
	l_pConfig->buttonCalibratePressedCB();
}

//Callback actually called:
void CConfigurationGTecGUSBamp::buttonCalibratePressedCB(void)
{
	// Connect to the hardware, ask for calibration, verify the return code, etc.
}
_________________________________________________*/

// If you added more reference attribute, initialize them here
CConfigurationEncephalan::CConfigurationEncephalan(IDriverContext& rDriverContext, const char* sGtkBuilderFileName, OpenViBE::uint32& rConnectionPort, char* rConnectionIp)
	:CConfigurationBuilder(sGtkBuilderFileName)
	, m_rDriverContext(rDriverContext)
	, m_ui32ConnectionPort(rConnectionPort)
	, m_sConnectionIp(rConnectionIp)
{
}

bool CConfigurationEncephalan::preConfigure(void)
{
	if(! CConfigurationBuilder::preConfigure())
	{
		return false;
	}

	// Connect here all callbacks
	// Example:
	// g_signal_connect(gtk_builder_get_object(m_pBuilderConfigureInterface, "button_calibrate"), "pressed", G_CALLBACK(button_calibrate_pressed_cb), this);

	// Insert here the pre-configure code.
	// For example, you may want to check if a device is currently connected
	// and if more than one are connected. Then you can list in a dedicated combo-box 
	// the device currently connected so the user can choose which one he wants to acquire from.
	::GtkSpinButton* l_pConnectionPort = GTK_SPIN_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "spinbutton_port"));
	::gtk_spin_button_set_value(l_pConnectionPort, m_ui32ConnectionPort);
	::GtkEntry* l_pConnectionIp = GTK_ENTRY(gtk_builder_get_object(m_pBuilderConfigureInterface, "entry_ip"));
	::gtk_entry_set_text(l_pConnectionIp, m_sConnectionIp);

	return true;
}

bool CConfigurationEncephalan::postConfigure(void)
{
	if(m_bApplyConfiguration)
	{
		// If the user pressed the "apply" button, you need to save the changes made in the configuration.
		// For example, you can save the connection ID of the selected device:
		// m_ui32ConnectionID = <value-from-gtk-widget>
		::GtkSpinButton* l_pConnectionPort = GTK_SPIN_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "spinbutton_port"));
		m_ui32ConnectionPort = (uint32)::gtk_spin_button_get_value_as_int(l_pConnectionPort);
		::GtkEntry* l_pConnectionIp = GTK_ENTRY(gtk_builder_get_object(m_pBuilderConfigureInterface, "entry_ip"));
		m_sConnectionIp = (char*)::gtk_entry_get_text(l_pConnectionIp);
	}

	if(! CConfigurationBuilder::postConfigure()) // normal header is filled (Subject ID, Age, Gender, channels, sampling frequency), ressources are realesed
	{
		return false;
	}

	return true;
}

char* CConfigurationEncephalan::getConnectionIp()
{
	return m_sConnectionIp;
}
