/***************************************************
 * Graphical user interface (gui) created using 
 * gtkmm (a c++ wrapper of gtk)
 *
 * ************************************************/

#include <gtkmm.h>
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <string>

#include "ahdcExtractor.h"
#include "reader.h"
#include "TString.h"

class MyWindow : public Gtk::Window {
protected :
	std::string filename = "/home/touchte-codjo/Desktop/hipofiles/alert_10k.hipo";
	hipo::reader reader;
	hipo::banklist banklist;
	long unsigned int nEvent;
	// Widgets
	Gtk::Box VBox_main, VBox_header, VBox_body, VBox_footpage;

	Gtk::Notebook Pages;
	// page 0
	Gtk::Box HBox_Event;
	Gtk::ScrolledWindow Window_Event;
	Gtk::Grid Grid_Event;
	std::vector<Gtk::DrawingArea> Areas;
	std::vector<Glib::RefPtr<Gdk::Pixbuf>> Images;

	Gtk::Box HBox_footpage_buttons;
	Gtk::Box HBox_file;
	Gtk::Button Button_file;
	Gtk::Box HBox_Run;
	Gtk::Button Button_Run;
	Gtk::Box HBox_Next;
	Gtk::Button Button_Next;

	// Signals
	void on_pages_switch(Gtk::Widget * _pages, guint page_num) { // le template est imposé ! page_num == Pages.get_current_page()
		std::cout << "Switched to tab with index : " << Pages.get_current_page() << std::endl;
	}
	void on_button_file_clicked() {
		//std::cout << "File Button clicked " << std::endl;
		auto dialog = Gtk::FileDialog::create();
		
		// Add filters, so that only certain file types can be selected:
		auto filters = Gio::ListStore<Gtk::FileFilter>::create();

		auto filter_text = Gtk::FileFilter::create();
		filter_text->set_name("Text files");
		filter_text->add_mime_type("text/plain");
		filters->append(filter_text);

		auto filter_cpp = Gtk::FileFilter::create();
		filter_cpp->set_name("C/C++ files");
		filter_cpp->add_mime_type("text/x-c");
		filter_cpp->add_mime_type("text/x-c++");
		filter_cpp->add_mime_type("text/x-c-header");
		filters->append(filter_cpp);

		auto filter_any = Gtk::FileFilter::create();
		filter_any->set_name("Any files");
		filter_any->add_pattern("*");
		filters->append(filter_any);

		dialog->set_filters(filters);

		// Show the dialog and wait for a user response:
		dialog->open(sigc::bind(sigc::mem_fun(*this, &MyWindow::on_file_dialog_finish), dialog));
	}

	void on_file_dialog_finish(const Glib::RefPtr<Gio::AsyncResult>& result, const Glib::RefPtr<Gtk::FileDialog>& dialog) {
		// Handle the response:
		try
		{
			auto file = dialog->open_finish(result);

			// Notice that this is a std::string, not a Glib::ustring.
			filename = file->get_path();
			std::cout << "File selected: " <<  filename << std::endl;
		}
		catch (const Gtk::DialogError& err)
		{
			// Can be thrown by dialog->open_finish(result).
			std::cout << "No file selected. " << err.what() << std::endl;
		}
		catch (const Glib::Error& err)
		{
			std::cout << "Unexpected exception. " << err.what() << std::endl;
		}
	}

	void on_button_run_clicked() {
		std::cout << "RUN ..." << std::endl;
		//hipo::reader reader(filename.c_str());
		reader.open(filename.c_str());
		//hipo::banklist banklist = reader.getBanks({"AHDC::adc","AHDC::wf:136"});
		banklist = reader.getBanks({"AHDC::adc","AHDC::wf:136"});
		//long unsigned int nEvent = 0;
		nEvent = 0;
		//while (reader.next(banklist)) {
		std::cout << "Process Event 1" << std::endl;
		if (reader.next(banklist)) {
			//if (nEvent != 0) {break;}
			for (int col = 0; col < banklist[1].getRows(); col++){ 
				int layer = banklist[1].getInt("layer",col);
				int component = banklist[1].getInt("component",col);
				std::vector<short> samples;
				for (int bin=0; bin < 136; bin++){
					std::string binName = "s" + std::__cxx11::to_string(bin+1);
					short value = banklist[1].getInt(binName.c_str(),col);
					samples.push_back(value);
				}
				ahdcExtractor T(44,0.5f,5,0.3f);
				T.adcOffset = (short) (samples[0] + samples[1] + samples[2] + samples[3] + samples[4])/5;
				std::map<std::string,double> output = T.extract(samples);
				T.Show(TString::Format("waveform_%ld_%d.png",nEvent,col));
				// Update Widget
				//Areas.clear();
				//Images.clear();
				Areas.push_back(Gtk::DrawingArea());
				try
				{
					Images.push_back(Gdk::Pixbuf::create_from_file(TString::Format("waveform_%ld_%d.png",nEvent,col).Data()));
				}
				catch (const Gio::ResourceError& ex)
				{
					std::cerr << "ResourceError: " << ex.what() << std::endl;
				}
				catch(const Gdk::PixbufError& ex)
				{
					std::cerr << "PixbufError: " << ex.what() << std::endl;
				}

				if (Images[col])
				{
					Areas[col].set_content_width(Images[col]->get_width());
					Areas[col].set_content_height(Images[col]->get_height());
				}
				
				TString::Format("waveform_%ld_%d.png",nEvent,col);
				Areas[col].set_draw_func( 
					[this, col] (const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
						this->on_draw(cr, width, height, col);
					}
				);
				Areas[col].set_expand(true);	
				Grid_Event.attach(Areas[col],1,col+1);
			}
		}
	}

	void on_button_next_clicked() {
		// Clear Grid_Event
		for (int i = banklist[1].getRows(); i>=1; i--) {
			Grid_Event.remove_row(i);
		}
		nEvent++;
		Grid_Event.queue_draw();
		Areas.clear();
		Images.clear();
		std::cout << "Process Event " << nEvent + 1 << std::endl;
		//return ;
		if (reader.next(banklist)) {
			//if (nEvent != 0) {break;}
			for (int col = 0; col < banklist[1].getRows(); col++){ 
				int layer = banklist[1].getInt("layer",col);
				int component = banklist[1].getInt("component",col);
				std::vector<short> samples;
				for (int bin=0; bin < 136; bin++){
					std::string binName = "s" + std::__cxx11::to_string(bin+1);
					short value = banklist[1].getInt(binName.c_str(),col);
					samples.push_back(value);
				}
				ahdcExtractor T(44,0.5f,5,0.3f);
				T.adcOffset = (short) (samples[0] + samples[1] + samples[2] + samples[3] + samples[4])/5;
				std::map<std::string,double> output = T.extract(samples);
				T.Show(TString::Format("waveform_%ld_%d.png",nEvent,col));
				// Update Widget
				//Areas.clear();
				//Images.clear();
				Areas.push_back(Gtk::DrawingArea());
				try
				{
					Images.push_back(Gdk::Pixbuf::create_from_file(TString::Format("waveform_%ld_%d.png",nEvent,col).Data()));
				}
				catch (const Gio::ResourceError& ex)
				{
					std::cerr << "ResourceError: " << ex.what() << std::endl;
				}
				catch(const Gdk::PixbufError& ex)
				{
					std::cerr << "PixbufError: " << ex.what() << std::endl;
				}

				if (Images[col])
				{
					Areas[col].set_content_width(Images[col]->get_width());
					Areas[col].set_content_height(Images[col]->get_height());
				}
				
				TString::Format("waveform_%ld_%d.png",nEvent,col);
				Areas[col].set_draw_func( 
					[this, col] (const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
						this->on_draw(cr, width, height, col);
					}
				);
				Areas[col].set_expand(true);	
				Grid_Event.attach(Areas[col],1,col+1);
			}
		}
	}
		
	void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height, int col) {
                if (!Images[col]) return;
                Gdk::Cairo::set_source_pixbuf(cr, Images[col], (width - Images[col]->get_width())/2, (height - Images[col]->get_height())/2);
                cr->paint();
        }

public :
	/** Default constructor */
	MyWindow () :
		//filename(""),
		// Boxes
		VBox_main(Gtk::Orientation::VERTICAL),
		VBox_header(Gtk::Orientation::VERTICAL,10),
		VBox_body(Gtk::Orientation::VERTICAL,10),
		VBox_footpage(Gtk::Orientation::VERTICAL,10),
		// Pages
		Pages(),
		HBox_Event(Gtk::Orientation::HORIZONTAL,10),
		Window_Event(),
		Grid_Event(),
		// Button
		HBox_footpage_buttons(Gtk::Orientation::HORIZONTAL,10),
		HBox_file(Gtk::Orientation::HORIZONTAL,10),
		Button_file(),
		HBox_Run(Gtk::Orientation::HORIZONTAL,10),
		Button_Run(),
		HBox_Next(Gtk::Orientation::HORIZONTAL,10),
		Button_Next()
	{
		set_title("ALERT monitoring");
		set_default_size(1378,654);
		set_child(VBox_main);

		// Header
		VBox_main.append(VBox_header);
		VBox_header.append(*Gtk::make_managed<Gtk::Label>("Header") ); // tmp // better to define a variable Gtk::Label to use set_expand()

		// Body
		VBox_main.append(VBox_body);
		VBox_body.append(Pages);
		Pages.set_margin(10);
		Pages.set_expand();
		Pages.signal_switch_page().connect(sigc::mem_fun(*this,&MyWindow::on_pages_switch) );
		// Body (page 1)
		Pages.append_page(HBox_Event, "Event Viewer");
		HBox_Event.append(Window_Event);
		Window_Event.set_child(Grid_Event);
		//Grid_Event.attach(*Gtk::make_managed<Gtk::ToggleButton>("button"),1,1);
		//Grid_Event.attach(*Gtk::make_managed<Gtk::ToggleButton>("button"),1,2);
		// Body (page 2)
		//Pages.append_page(*Gtk::make_managed<Gtk::Label>("Page 2"));
		auto picture1 = Gtk::make_managed<Gtk::Picture>("./waveform_0_0.png");
		auto picture2 = Gtk::make_managed<Gtk::Picture>("./waveform_0_0.png");
		auto grid = Gtk::make_managed<Gtk::Grid>();
		grid->attach(*picture1,1,1);
		grid->attach(*picture2,2,1);
		//picture->set_keep_aspect_ratio(true);
		Pages.append_page(*grid, "Page 2");
		// Footpage
		VBox_main.append(VBox_footpage);
		VBox_footpage.append(*Gtk::make_managed<Gtk::Label>("Footpage") ); // tmp // better to define a variable Gtk::Label to use set_expand()
		VBox_footpage.append(HBox_footpage_buttons);
		HBox_footpage_buttons.append(Button_file);
		Button_file.set_child(HBox_file);
		HBox_file.append(*Gtk::make_managed<Gtk::Image>("./folder.png") );
		HBox_file.append(*Gtk::make_managed<Gtk::Label>("Select a file") );
		Button_file.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_button_file_clicked) );
		HBox_footpage_buttons.append(Button_Run);
		Button_Run.set_child(HBox_Run);
		HBox_Run.append(*Gtk::make_managed<Gtk::Image>("./button_run.png"));
		HBox_Run.append(*Gtk::make_managed<Gtk::Label>("RUN") );
		Button_Run.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_button_run_clicked) );
		HBox_footpage_buttons.append(Button_Next);
		Button_Next.set_child(HBox_Next);
		HBox_Next.append(*Gtk::make_managed<Gtk::Image>("./button_next.jpg"));
		HBox_Next.append(*Gtk::make_managed<Gtk::Label>("Next") );
		Button_Next.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_button_next_clicked) );
	}
	/** Destructor */
	~MyWindow() {}
};

int main(int argc, char * argv[]) {
	
	std::cout << "Start GUI ..." << std::endl;

	auto app = Gtk::Application::create("org.gtkmm.example");

	return app->make_window_and_run<MyWindow>(argc, argv);

	return 0;
}
