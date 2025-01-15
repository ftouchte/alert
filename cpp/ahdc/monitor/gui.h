/**************************************
 * GUI for ALERT monitoring
 * 
 * @author ftouchte
 * @date Jan 14, 2025
 * **********************************/

#ifndef AHDC_GUI
#define AHDC_GUI

#include <gtkmm.h>
#include "reader.h"

class Window : public Gtk::Window {
protected :
	// Widgets
	Gtk::Box VBox_main, VBox_header, VBox_body, VBox_footer;
	
	/******************
         * HEADER
         * ***************/


	/******************
         * BODY
         * ***************/
	
	Gtk::Notebook Book;
	// page 0
	Gtk::Box HBox_eventViewer;
	Gtk::Grid Grid_eventViewer, Grid_waveforms; // 2x1 and NxP 

	// page 1
	Gtk::Box HBox_histograms;

	/******************
         * FOOTER
         * ***************/
	Gtk::Box HBox_footer;
	Gtk::Box HBox_prev, HBox_next, HBox_pause, HBox_run, HBox_hipo4, HBox_reset;
	Gtk::Button Button_prev, Button_next, Button_pause, Button_run, Button_hipo4, Button_reset;
	Gtk::Image img_prev, img_next, img_pause, img_run, img_hipo4, img_reset;

	/// DATA
	hipo::reader hipo_reader;
	hipo::banklist hipo_banklist;
	long unsigned int hipo_nEvent;	
		
public :
	Window();
	~Window();

	// Signals
	
	
};

#endif


